//
// Created by Conlan Cesar on 9/4/17.
//

#include <server_ws.hpp>
#include "WebSocketClient.h"

typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;

using namespace std;

WebSocketClient::WebSocketClient(shared_ptr<WsServer::Connection> connection) {
  m_connection = std::move(connection);
}

set<string> WebSocketClient::getSubscribedMail() {
  return(m_subscribedMail);
}

std::shared_ptr<WsServer::Connection> WebSocketClient::getConnection() {
  return(m_connection);
}

void WebSocketClient::addSubscribedMail(string key) {
  m_subscribedMail.insert(key);
}

bool WebSocketClient::sendMail(string mail) {
  auto send_stream = make_shared<WsServer::SendStream>();
  //cout << "Server: Sending message '" + mail + "' to " << m_connection.get() << endl;
  *send_stream << mail;
  // connection->send is an asynchronous function
  m_connection->send(send_stream, [](const SimpleWeb::error_code &ec) {
      if(ec) {
        cout << "Server: Error sending message. " <<
             // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
             "Error: " << ec << ", error message: " << ec.message() << endl;
        return false;
      }
      return true;
  });
}

bool operator< (const WebSocketClient &left, const WebSocketClient &right) {
  return left.m_connection->remote_endpoint_port() > right.m_connection->remote_endpoint_port();
  // TODO Actual logic here, for some reason unordered_set freaks out things, and we don't care about client order, so sort method is by port
}


