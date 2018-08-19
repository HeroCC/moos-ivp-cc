/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: MumbleClient.h                                  */
/*    DATE: Summer 2018                                     */
/************************************************************/

#ifndef MumbleClient_HEADER
#define MumbleClient_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "mumlib/include/mumlib.hpp"
#include "MumbleCallbackHandler.h"
#include "SimpleRingBuffer.h"

class MumbleClient : public AppCastingMOOSApp
{
 public:
   MumbleClient();
   ~MumbleClient();
    struct AudioBuffers {
      std::shared_ptr<RingBuffer<int16_t>> recordBuffer;
      std::shared_ptr<RingBuffer<int16_t>> playBuffer;
      bool shouldRecord = false;
    };

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();
   void registerVariables();

 protected:
    void initMumbleLink();
    void initPortAudio(bool killOld);

 private: // Configuration variables
   std::string m_sendAudioKey = "SPEECH_BUTTON";
   std::string m_mumbleServerAddress = "localhost";
   int m_mumbleServerPort = 64738;
   std::string m_mumbleServerUsername; // Defaults to 'this->m_host_community'
   std::string m_mumbleServerChannelId = "-1"; // If this is a number use channel ID, else try to match by name

 private: // State variables
   mumlib::Mumlib* mum;
   PaStream* audioStream;
   AudioBuffers audioBuffers;
   MumbleCallbackHandler* cb;
   bool mumConnected = false; // Mumlib isn't super thread safe, so avoid triggering calls to it
   bool joinedDefaultChannel = true; // Starts as true to avoid issues with Iterate and Thread race conditions
   bool notifiedHearingAudio = false;
};

#endif
