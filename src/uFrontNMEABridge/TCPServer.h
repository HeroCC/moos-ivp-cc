//
// Created by Conlan Cesar on 11/26/19.
//

#ifndef IVP_EXTEND_TCPSERVER_H
#define IVP_EXTEND_TCPSERVER_H

#include <asio.hpp>
#include "FrontNMEABridge.h"
#include "TCPConnection.h"

class TCPServer {

public:
    TCPServer(asio::io_context&, FrontNMEABridge*);

private:
    void startAccept();
    void handleAccept(std::shared_ptr<TCPConnection>, const std::error_code&);
    asio::ip::tcp::acceptor acceptor_;
    FrontNMEABridge* m_moos;


};


#endif //IVP_EXTEND_TCPSERVER_H
