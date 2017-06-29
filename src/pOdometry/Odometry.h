/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.h                                      */
/*    DATE: 6/27/17                                         */
/************************************************************/

#ifndef Odometry_HEADER
#define Odometry_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class Odometry : public AppCastingMOOSApp
{
 public:
   Odometry();
   ~Odometry();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();

 protected:
   void RegisterVariables();

 protected:
   bool   m_first_reading;
   double m_current_x;
   double m_current_y;
   double m_previous_x;
   double m_total_distance;

 private: // Configuration variables

 private: // State variables
};

#endif 
