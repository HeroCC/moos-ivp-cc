// This is the CMOOSApp example from the MOOS website documentation
// Included here for convenience
//
// Feb 10th, 2013

#include "MOOS/libMOOS/App/MOOSApp.h"

class ExampleApp : public CMOOSApp {

  bool OnNewMail (MOOSMSG_LIST & Mail) 
  {
    // process it
    MOOSMSG_LIST::iterator q;
    for(q=Mail.begin(); q!=Mail.end(); q++) {
      q->Trace();
    }
    return(true);
  }
  
  bool OnConnectToServer () {
    return(Register("X", 0.0));
  } 
  
  bool Iterate ( ) {
    std :: vector<unsigned char> X(100) ; 
    Notify("X" ,X) ;
    return true ;
  }
};

