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
#include <boost/circular_buffer.hpp>

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
   std::string m_sendAudioKey = "SEND_AUDIO";

 private: // State variables
   mumlib::Mumlib* mum;
   PaStream* audioStream;
   AudioBuffers audioBuffers;
};

#endif
