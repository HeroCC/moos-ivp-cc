/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include <GeomUtils.h>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class GenPath : public AppCastingMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();

 protected:
   void RegisterVariables();
    double pointDistance(double x1, double x2, double y1, double y2);

    unsigned int getClosestPointIndex(double x, double y, XYSegList p);

 private: // Configuration variables
    XYSegList points;
    int assigned_points = 0;
    bool sendShip();

 private: // State variables
    double lastX;
    double lastY;


};

#endif 
