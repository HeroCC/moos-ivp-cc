//
// Created by Conlan Cesar on 11/26/19.
//

#ifndef IVP_EXTEND_TCPCONNECTION_H
#define IVP_EXTEND_TCPCONNECTION_H

#include "FrontNMEABridge.h"
#include <asio.hpp>

using asio::ip::tcp;

class TCPConnection : public std::enable_shared_from_this<TCPConnection> {

public:
    static std::shared_ptr<TCPConnection> create(asio::io_context& io_context, FrontNMEABridge*);
    tcp::socket& socket();
    void start();

private:
    TCPConnection(asio::io_context& io_context, FrontNMEABridge*);
    void handleWrite(const std::error_code&, size_t);

    tcp::socket socket_;
    FrontNMEABridge* m_moos;
};


#endif //IVP_EXTEND_TCPCONNECTION_H
