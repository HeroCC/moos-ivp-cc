//
// Created by Conlan Cesar on 9/4/17.
//

#ifndef IVP_EXTEND_WEBSOCKETCLIENT_H
#define IVP_EXTEND_WEBSOCKETCLIENT_H

#include <vector>
#include <string>
#include <set>
#include <server_ws.hpp>

typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;

class WebSocketClient {
private:

protected:

    mutable std::set<std::string> m_subscribedMail;
    std::shared_ptr<WsServer::Connection> m_connection;

public:
    WebSocketClient(std::shared_ptr<WsServer::Connection> connection);
    std::set<std::string> getSubscribedMail();
    void addSubscribedMail(std::string key);

    bool sendMail(std::string mail);

    std::shared_ptr<SimpleWeb::SocketServerBase<SimpleWeb::WS>::Connection> getConnection();
    friend bool operator< (const WebSocketClient &left, const WebSocketClient &right);

};


#endif //IVP_EXTEND_WEBSOCKETCLIENT_H
