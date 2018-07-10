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

 private: // Configuration variables
   std::string m_sendAudioKey = "SPEECH_BUTTON";
   std::string m_mumbleServerAddress = "localhost";
   int m_mumbleServerPort = 64738;
   std::string m_mumbleServerUsername; // Defaults to 'this->m_host_community'
   int m_mumbleServerChannelId = -1;

 private: // State variables
   mumlib::Mumlib* mum;
   PaStream* audioStream;
   AudioBuffers audioBuffers;
   MumbleCallbackHandler* cb;
   bool joinedDefaultChannel = false;
};

#endif
