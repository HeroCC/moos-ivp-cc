/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: MumbleClient.cpp                                */
/*    DATE: Summer 2018                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MumbleClient.h"
#include "mumlib/include/mumlib.hpp"
#include "portaudio.h"

using namespace std;

const int SAMPLE_RATE = 48000;
const int NUM_CHANNELS = 1;
const int FRAMES_PER_BUFFER = 512;
const int OPUS_FRAME_SIZE = (SAMPLE_RATE / 1000.0)*20.0;
const size_t MAX_SAMPLES = 0.5 * SAMPLE_RATE * NUM_CHANNELS;

static int paCallback(const void *_inputBuffer,
                      void *_outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* /*timeInfo*/,
                      PaStreamCallbackFlags /*statusFlags*/,
                      void *userData) {
  // Recast the bits lost in transport
  const auto *paData = (const MumbleClient::AudioBuffers*) userData;
  auto *inputBuffer = (int16_t*) _inputBuffer;
  auto *outputBuffer = (int16_t*) _outputBuffer;

  // Dump the input into the send buffer
  if(inputBuffer != nullptr && paData->shouldRecord) {
    // Send the samples into the buffer
    paData->recordBuffer->push(inputBuffer, 0, framesPerBuffer * NUM_CHANNELS);
  }

  // Do the same for the playback buffer
  const size_t requested_samples = (framesPerBuffer * NUM_CHANNELS);
  const size_t available_samples = paData->playBuffer->getRemaining();
  if(requested_samples > available_samples) {
    paData->playBuffer->top(outputBuffer, 0, available_samples);
    for(size_t i = available_samples; i < requested_samples - available_samples; i++) {
      outputBuffer[i] = 0;
    }
  } else {
    paData->playBuffer->top(outputBuffer, 0, requested_samples);
  }

  return paContinue;
}

//---------------------------------------------------------
// Constructor

MumbleClient::MumbleClient() {
  audioBuffers.recordBuffer = std::make_shared<RingBuffer<int16_t>>(MAX_SAMPLES);
  audioBuffers.playBuffer = std::make_shared<RingBuffer<int16_t>>(MAX_SAMPLES);

  Pa_Initialize();

  Pa_OpenDefaultStream(&audioStream,
                       NUM_CHANNELS,
                       NUM_CHANNELS,
                       paInt16,
                       SAMPLE_RATE,
                       OPUS_FRAME_SIZE,
                       paCallback,
                       &audioBuffers);

  auto err = Pa_StartStream(audioStream);
  std::cout << Pa_GetErrorText(err) << std::endl;
}

//---------------------------------------------------------
// Destructor

MumbleClient::~MumbleClient()
{
  Pa_Terminate();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MumbleClient::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == m_sendAudioKey)
      audioBuffers.shouldRecord = msg.GetAsString() == "true";

    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MumbleClient::OnConnectToServer() {
  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MumbleClient::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MumbleClient::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = true;
    if(param == "START_MAIL_NAME") {
      m_sendAudioKey = value;
      Register(value);
    }
    else if(param == "BAR") {

    } else { handled = false; }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  initMumbleLink();

  registerVariables();

  return(true);
}

void MumbleClient::initMumbleLink() {
  // Begin with everything
  thread server_thread([this]() {

    // Configure Mumble
    MumbleCallbackHandler cb(this->audioBuffers.playBuffer);
    mumlib::MumlibConfiguration conf;
    //conf.opusEncoderBitrate = 48000; // Higher = better quality, more bandwidth
    this->mum = new mumlib::Mumlib(cb, conf);

      // Now that mumlib is initialized, add a listener for sending audio data
      thread sendRecordedAudioThread([this]() {
          auto *out_buf = new int16_t[MAX_SAMPLES];
          // TODO This block makes sent audio slightly choppy, it needs to be tweaked
          while (true) {
            this->audioBuffers.recordBuffer->top(out_buf, 0, OPUS_FRAME_SIZE);
            if (!this->audioBuffers.recordBuffer->isEmpty() && this->audioBuffers.recordBuffer->getRemaining() >= OPUS_FRAME_SIZE) {
              if (this->mum->getConnectionState() == mumlib::ConnectionState::CONNECTED) {
                this->mum->sendAudioData(out_buf, OPUS_FRAME_SIZE);
              }
            } else {
              std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
          }
      });
      sendRecordedAudioThread.detach();

    // Attempt to maintain a connection forever
    while (this->mum->getConnectionState() != mumlib::ConnectionState::CONNECTED) {
      try {
        this->mum->connect("localhost", 64738, this->m_host_community, "");
        this->mum->run();
      } catch (std::exception &e) {
        this->reportRunWarning("There was an issue trying to connect Murmur: " + *e.what());
        std::this_thread::sleep_for(std::chrono::seconds(3)); // How long to wait until retrying
      }
    }
  });
  server_thread.detach();
}

//---------------------------------------------------------
// Procedure: registerVariables

void MumbleClient::registerVariables() {
  AppCastingMOOSApp::RegisterVariables();
  Register(m_sendAudioKey, .1); // Add a bit of a delay for a debounce
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MumbleClient::buildReport() {
  m_msgs << "============================================ \n";
  m_msgs << "Mumble Client                                \n";
  m_msgs << "============================================ \n";
  m_msgs << "                                             \n";

  return(true);
}



