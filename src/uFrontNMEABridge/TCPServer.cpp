/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: FrontNMEABridge.cpp                             */
/*    DATE: Winter 2019                                     */
/*    Bits and pieces taken from ASIO's Example Code        */
/************************************************************/

#include "TCPServer.h"
#include <memory>
#include <functional>

using asio::ip::tcp;

TCPServer::TCPServer(asio::io_context& io_context, FrontNMEABridge* moos) : acceptor_(io_context, tcp::endpoint(tcp::v4(), moos->m_port)) {
  startAccept();
}

void TCPServer::startAccept() {
  std::shared_ptr<TCPConnection> newConnection = TCPConnection::create(acceptor_.get_executor().context(), m_moos);

  acceptor_.async_accept(newConnection->socket(), std::bind(&TCPServer::handleAccept, this, newConnection, std::placeholders::_1));
}

void TCPServer::handleAccept(std::shared_ptr<TCPConnection> newConnection, const std::error_code& error) {
  if (!error) {
    newConnection->start();
  }

  startAccept();
}