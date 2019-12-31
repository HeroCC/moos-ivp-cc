//
// Created by Conlan Cesar on 11/26/19.
//

#include "TCPConnection.h"

TCPConnection::TCPConnection(asio::io_context& io_context, FrontNMEABridge* moos) : socket_(io_context) {
  m_moos = moos;
}

std::shared_ptr<TCPConnection> TCPConnection::create(asio::io_context& io_context, FrontNMEABridge* moos) {
  return std::shared_ptr<TCPConnection>(new TCPConnection(io_context, moos));
}

tcp::socket& TCPConnection::socket() {
  return socket_;
}

void TCPConnection::handleWrite(const std::error_code& error, size_t bytes_tx) {
  if (error) {
    std::string msg = "ERR Writing " + std::to_string(error.value()) + ": " + error.message();
    //m_moos->reportError(msg);
  }

}

void TCPConnection::start() {
  std::string b = "b";
  asio::async_write(socket_, asio::buffer(b), std::bind(&TCPConnection::handleWrite, shared_from_this(),
                                std::placeholders::_1,
                                std::placeholders::_2));
}