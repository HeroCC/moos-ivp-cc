/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: WebSocketServer.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "server_ws.hpp"
#include "MBUtils.h"
#include "ACTable.h"
#include "WebSocketServer.h"


typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;

using namespace std;

//---------------------------------------------------------
// Constructor

WebSocketServer::WebSocketServer()
{
}

//---------------------------------------------------------
// Destructor

WebSocketServer::~WebSocketServer()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool WebSocketServer::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "FOO") cout << "great!";

    string sendMe;
    msg.IsDouble() ? sendMe = std::to_string(msg.GetDouble()) : sendMe = msg.GetString();

    // Forward the rest to clients
    checkRegisteredClients(key, sendMe);

     //else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       //reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool WebSocketServer::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool WebSocketServer::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool WebSocketServer::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  wsServer.config.port = 9090;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    if(param == "WSPort") {
      wsServer.config.port = stoi(value);
    }
    else if(param == "BAR") {

    }

  }

  auto &echo_all = wsServer.endpoint["^/listen/?$"];
  echo_all.on_open = [this](shared_ptr<WsServer::Connection> connection) {
      reportEvent("WS: New connection from " + connection->remote_endpoint_address() + ":" + itos(connection->remote_endpoint_port()));

      shared_ptr<WebSocketClient> client (new WebSocketClient(connection));
      m_clients.insert(move(client));
  };

  echo_all.on_message = [this](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
      auto message_str = message->string();
      reportEvent("WS: Received: '" + message_str + "' from " + connection->remote_endpoint_address() + ":" + itos(connection->remote_endpoint_port()));

      m_Comms.Register(message_str, 0);
      getClientByConnection(connection)->addSubscribedMail(message_str);
  };

  echo_all.on_close = [this](shared_ptr<WsServer::Connection> connection, int status, const string &reason) {
      reportEvent("WS: Disconnected " + connection->remote_endpoint_address() + ":" + itos(connection->remote_endpoint_port()));
      m_clients.erase(m_clients.find(getClientByConnection(connection)));
  };

  registerVariables();

  reportEvent("Attempting to start WebSocket server on: " + wsServer.config.address + ":" + itos(wsServer.config.port));
  thread server_thread([this]() {
      this->wsServer.start();
  });

  server_thread.detach();
  //server_thread.join();
  return(true);
}

string WebSocketServer::itos(double ival) {
  stringstream ss;
  ss << ival;
  string str = ss.str();
  return str;
}

void WebSocketServer::checkRegisteredClients(string param, string mail) {
  for (const shared_ptr<WebSocketClient> &client : m_clients) {
    if (client->getSubscribedMail().count(param) == 0) {
      return;
    }
    sendMailToClient(client, param + "=" + mail);
  }
}

void WebSocketServer::sendMailToClient(shared_ptr<WebSocketClient> client, string mail) {
  client->sendMail(mail);
}

shared_ptr<WebSocketClient> WebSocketServer::getClientByConnection(shared_ptr<WsServer::Connection> connection) {
  for (const shared_ptr<WebSocketClient> &client : m_clients) {
    if (client->getConnection() == connection) return client;
  }
  return nullptr;
}

//---------------------------------------------------------
// Procedure: registerVariables

void WebSocketServer::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  //Register("NAV_X", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool WebSocketServer::buildReport() 
{
  //m_msgs << "============================================ \n";
  //m_msgs << "File:                                        \n";
  //m_msgs << "============================================ \n";

  ACTable actab(2);
  actab << "Location | Subscribed Messages";
  actab.addHeaderLines();
  //actab << "one" << "two" << "three" << "four";

  for (const shared_ptr<WebSocketClient> &client : m_clients) {
    actab << client->getConnection()->remote_endpoint_address() + ":" + itos(client->getConnection()->remote_endpoint_port()) << itos(client->getSubscribedMail().size());
  }
  m_msgs << actab.getFormattedString();

  return(true);
}




