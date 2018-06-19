/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: WebSocketServer.h                               */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef WebSocketServer_HEADER
#define WebSocketServer_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "WebSocketClient.h"

/* WebSocket libraries:
 * https://github.com/eidheim/Simple-WebSocket-Server
 * https://github.com/mattgodbolt/seasocks
 * https://github.com/zaphoyd/websocketpp
*/

class WebSocketServer : public AppCastingMOOSApp
{
 public:
   WebSocketServer();
   ~WebSocketServer();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();

 protected:
    void registerVariables();
    void checkRegisteredClients(std::string param, std::string value);
    void registerMailEndpoint();
    void handleInternalMessage(std::string message, std::shared_ptr<WebSocketClient> client);
    std::set<std::shared_ptr<WebSocketClient>> m_clients;
    std::string itos(double i);

 private: // Configuration variables
    bool allowSubmissions = false;
    std::string password = "";

 private: // State variables
    WsServer wsServer;
    std::map<std::string, std::string> m_recentMail;

    std::shared_ptr<WebSocketClient>
    getClientByConnection(std::shared_ptr<SimpleWeb::SocketServerBase<SimpleWeb::WS>::Connection> connection);
};

#endif 
