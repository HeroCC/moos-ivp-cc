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
#include "MumbleCallbackHandler.cpp"

class MumbleClient : public AppCastingMOOSApp
{
 public:
   MumbleClient();
   ~MumbleClient();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
   std::string m_sendAudioKey = "SEND_AUDIO";

 private: // State variables
   MumbleCallbackHandler callbackHandler;
   mumlib::Mumlib* mum;
   bool m_sendAudio = false;

};

#endif 
