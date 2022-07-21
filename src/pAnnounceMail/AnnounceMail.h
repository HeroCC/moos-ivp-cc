/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AnnounceMail.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AnnounceMail_HEADER
#define AnnounceMail_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class AnnounceMail : public AppCastingMOOSApp
{
 public:
   AnnounceMail();
   ~AnnounceMail();

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

 private: // State variables
  std::set<std::string> m_broadcast_mails;
};

#endif
