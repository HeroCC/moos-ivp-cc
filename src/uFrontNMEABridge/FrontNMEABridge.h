/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: FrontNMEABridge.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef FrontNMEABridge_HEADER
#define FrontNMEABridge_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <iostream>
#include <asio.hpp>

class TCPServer;

class FrontNMEABridge : public AppCastingMOOSApp
{
 public:
   FrontNMEABridge();
   ~FrontNMEABridge();

    std::string genNMEAString();
    unsigned short m_port = 10110;

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
    void registerVariables();
    void startAccept(asio::io_service& svc, asio::ip::tcp::acceptor& acc);
    void handleWrite(const std::error_code&, size_t);

 private: // Configuration variables

 private: // State variables
    std::shared_ptr<asio::streambuf> data = std::make_shared<asio::streambuf>();
    asio::io_context m_svc;
    std::shared_ptr<asio::ip::tcp::acceptor> m_acc;

    time_t m_last_updated_time = -1;
    double m_latest_heading = 0;
    double m_latest_speed = 0;
    double m_latest_depth = 0;
    double m_latest_long = -1;
    double m_latest_alt = -1;
    double m_latest_lat = -1;


};

#endif 
