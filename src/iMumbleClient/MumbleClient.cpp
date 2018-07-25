/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: MumbleClient.cpp                                */
/*    DATE: Summer 2018                                     */
/*    Attributions: Hunter522's mumpi, Slomkowski's mumlib, */
/*                  The Mumble team, and Misha              */
/************************************************************/

#include <thread>
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
                       FRAMES_PER_BUFFER,
                       paCallback,
                       &audioBuffers);

  auto err = Pa_StartStream(audioStream);
  if (err) {
    string errText = "Error starting audio engine: ";
    errText.append(Pa_GetErrorText(err));
    reportRunWarning(errText);
  }
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

    if(key == m_sendAudioKey) {
      m_Comms.Notify("AUDIO_TX", msg.GetAsString());
      audioBuffers.shouldRecord = toupper(msg.GetAsString()) == "TRUE";
    }

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
  if (!joinedDefaultChannel &&
      this->m_mumbleServerChannelId != "-1" &&
      this->mum->getConnectionState() == mumlib::ConnectionState::CONNECTED) {
    if (isInteger(this->m_mumbleServerChannelId)) {
      this->mum->joinChannel(stoi(this->m_mumbleServerChannelId));
    } else {
      string channelId = tokStringParse(this->cb->channelList, this->m_mumbleServerChannelId, ',', '=');
      reportEvent("Found channel ID " + channelId + " matching " + m_mumbleServerChannelId);
      this->mum->joinChannel(stoi(channelId));
    }
    this->joinedDefaultChannel = true;
  }

  // Tell the DB we are hearing things
  if (this->audioBuffers.playBuffer->isEmpty() && this->notifiedHearingAudio) {
    // Not hearing anything, need to notify
    Notify("HEARING_VOIP_AUDIO", "status=FALSE,inChan=" + this->m_mumbleServerChannelId);
    this->notifiedHearingAudio = false;
  } else if (!this->audioBuffers.playBuffer->isEmpty() && !this->notifiedHearingAudio) {
    // Hearing things, need to notify
    Notify("HEARING_VOIP_AUDIO", "status=TRUE,inChan=" + this->m_mumbleServerChannelId);
    this->notifiedHearingAudio = true;
  }
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MumbleClient::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  this->m_mumbleServerUsername = this->m_host_community;

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
    if(param == "AUDIO_TX_KEY") {
      m_sendAudioKey = value;
      Register(value);
    } else if(param == "SERVER_IP") {
      m_mumbleServerAddress = value;
    } else if(param == "SERVER_PORT") {
      m_mumbleServerPort = stoi(value);
    } else if (param == "CLIENT_USERNAME") {
      m_mumbleServerUsername = value;
    } else if (param == "CHANNEL_ID") {
      m_mumbleServerChannelId = value;
    }
    else { handled = false; }

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
    this->cb = new MumbleCallbackHandler(this->audioBuffers.playBuffer);
    mumlib::MumlibConfiguration conf;
    //conf.opusEncoderBitrate = 48000; // Higher = better quality, more bandwidth
    this->mum = new mumlib::Mumlib(*cb, conf);

    // Attempt to maintain a connection forever
    while (this->mum->getConnectionState() != mumlib::ConnectionState::CONNECTED) {
      this->joinedDefaultChannel = false;
      try {
        this->mum->connect(this->m_mumbleServerAddress, this->m_mumbleServerPort, this->m_mumbleServerUsername, "");
        this->mum->run();
      } catch (mumlib::MumlibException &e) {
        string errMessage = "There was an issue trying to connect Murmur: ";
        errMessage.append(e.what());
        this->reportRunWarning(errMessage);
        this->mum->disconnect(); // After recovering from a bad connection, mumlib fails to reset the connection status
        std::this_thread::sleep_for(std::chrono::seconds(3)); // How long to wait until retrying
      }
    }
  });
  server_thread.detach();


  // Now that mumlib is initialized, add a listener for sending audio data
  thread sendRecordedAudioThread([this]() {
      auto *out_buf = new int16_t[MAX_SAMPLES];
      bool notifiedSendingAudio = false;
      while (true) {
        if (!this->audioBuffers.recordBuffer->isEmpty() && this->audioBuffers.recordBuffer->getRemaining() >= OPUS_FRAME_SIZE) {
          if (!notifiedSendingAudio) {
            Notify("SENDING_VOIP_AUDIO", "status=TRUE,inChan=" + this->m_mumbleServerChannelId);
            notifiedSendingAudio = true;
          }
          this->audioBuffers.recordBuffer->top(out_buf, 0, OPUS_FRAME_SIZE);
          if (this->mum != nullptr && this->mum->getConnectionState() == mumlib::ConnectionState::CONNECTED) {
            this->mum->sendAudioData(out_buf, OPUS_FRAME_SIZE);
          }
        } else {
          if (notifiedSendingAudio && !this->audioBuffers.shouldRecord) {
            Notify("SENDING_VOIP_AUDIO", "status=FALSE,inChan=" + this->m_mumbleServerChannelId);
            notifiedSendingAudio = false;
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
      }
  });
  sendRecordedAudioThread.detach();
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
  m_msgs << "Speaking:   " << boolToString(this->audioBuffers.shouldRecord) << endl;
  m_msgs << "Trigger:    " << this->m_sendAudioKey << endl;
  m_msgs << endl;
  m_msgs << "Connected:  " << boolToString(this->mum->getConnectionState() == mumlib::ConnectionState::CONNECTED) << endl;
  // TODO This data is what the values are desired to be, consult with mumlib::userState for real information
  m_msgs << "Username:   " << this->m_mumbleServerUsername << endl;
  m_msgs << "Server:     " << this->m_mumbleServerAddress << ":" << intToString(this->m_mumbleServerPort) << endl;
  m_msgs << "Channel ID: " << this->m_mumbleServerChannelId << endl;
  m_msgs << endl;
  m_msgs << "Channels:   " << this->cb->channelList << endl;

  return(true);
}

bool MumbleClient::isInteger(const std::string& s) {
  // Thanks https://stackoverflow.com/a/2845275/1709894
  if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

  char* p;
  strtol(s.c_str(), &p, 10);

  return (*p == 0);
}