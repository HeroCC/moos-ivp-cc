/*****************************************************************/
/*    NAME: Michael Benjamin (deriv from Alon Yaari)             */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SockNinja.cpp                                        */
/*    DATE: Mar 25th 2020                                        */
/*****************************************************************/

#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include "MBUtils.h"
#include "SockNinja.h"

using namespace std;

#define BUF_SIZE      4096
#define MAX_BUF_SIZE  4096

//---------------------------------------------------------
// Constructor

SockNinja::SockNinja(string ninja_type, int port)
{
  // legal type values are client or server
  m_ninja_type  = "server";
  if(ninja_type == "client")
    m_ninja_type = "client";
 
  memset(m_in_buffer, 0, MAX_NINJA_BUF_SIZE);

  m_msg_format  = "nmea";
  m_port        = port;

  m_ninja_state = "unconnected";
  m_sockfd_lis  = 0;
  m_sockfd      = 0;

  // To guard against users who never retrieve warnings, events,
  // or retractions, a max list size is enforced.
  m_max_list_size = 100;

  // To guard against unbouned types of NMEA message keys, and 
  // thus unbound map sizes, a max number of keys is enforced.
  m_max_msg_keys = 100;
  
  // For clients, this is the server we're trying to connect to.
  // For servers, this will hold the IP of the connected client.
  m_ip_addr = "127.0.0.1";

  m_dropped_chars = 0;
  m_bad_send_nmea = 0;
  m_bad_rcvd_nmea = 0;

  m_total_msgs_sent = 0;
  m_total_msgs_rcvd = 0;
}

//---------------------------------------------------------
// Procedure: setCommsType()

bool  SockNinja::setCommsType(string str)
{
  // If this does not represent a change, do nothing
  if(str == m_ninja_type)
    return(true);

  // Changes to comms type only allowed if still disconnected
  if(m_ninja_state != "unconnected")
    return(false);

  // Only possible comms types are client or server
  if((str != "client") && (str != "server"))
    return(false);

  m_ninja_type = str;

  // If an ip address had been configured when this type was previously
  // a server, retract the warning that may have been generated.
  if(m_ninja_type == "client")
    addRetraction("IP Address can only be set for client comms type");
  
  return(true);
}

//---------------------------------------------------------
// Procedure: setIPAddr()
//      Note: Intended to be called by user soon after instantiation

bool SockNinja::setIPAddr(string ip_addr)
{
  // Check basic structure for IPv4 format N.N.N.N,  0<=N<=256
  if(!isValidIPAddress(ip_addr)) {
    string resolved_ip_addr = getIPAddrByName(ip_addr);
    if(!isValidIPAddress(resolved_ip_addr))
      return(false);
    m_ip_addr_name = ip_addr;
    ip_addr = resolved_ip_addr;
  }
  
  // If this is a server, ip address is discovered when the client
  // connects, so we warn that the ip_addr cannot be directly set.
  if(m_ninja_type == "server")
    addWarning("IP Address can only be set for client comms type");
  
  // Once connected, further changes to ipaddr are not allowed.
  if(m_ninja_state != "unconnected")
    return(false);
  
  m_ip_addr = ip_addr;
  return(true);
}

//---------------------------------------------------------
// Procedure: setPortNumber()

bool SockNinja::setPortNumber(int port)
{
  // Once connected, further changes to port number are not allowed.
  if(m_ninja_state != "unconnected")
    return(false);
  
  if((port <= 0) || (port > 65334)) {
    addWarning("Invalid port number requested:" + intToString(port));
      return(false);
  }

  m_port = port;
  return(true);
}

//---------------------------------------------------------
// Procedure: setMsgFormatVerbatim()
//            In NMEA mode incoming messages are parsed based on
//            an expected NMEA format. In verbatim mode, no such
//            checks are made.
//      Note: Currently only formats supported: nmea or verbatim

void SockNinja::setMsgFormatVerbatim()
{
  m_msg_format = "verbatim";  
}

//---------------------------------------------------------
// Procedure: setMaxListSize()
//      Note: To guard against users who never retrieve warnings,
//            events, or retractions, a max list size is enforced.
//      Note: The default size is 100, but may be increased to 1000.


void SockNinja::setMaxListSize(unsigned int amt)
{
  if(amt > 1000)
    amt = 1000;
  m_max_list_size = amt;
}

//---------------------------------------------------------
// Procedure: setupConnection()

bool SockNinja::setupConnection()
{
  if(m_ninja_type == "connected")
    return(true);
  
  if(m_ninja_type == "server") {
    if(m_ninja_state == "unconnected")
      setupListening();
    if(m_ninja_state == "listening") 
      return(setupConnectionForServer());
    return(false);
  }
  
  return(setupConnectionForClient());
}

//---------------------------------------------------------
// Procedure: closeSockFDs()
//   Purpose: Close any open file descriptors. Applications 
//            that use SockNinja are encouraged to call this 
//            prior to exiting. 

void SockNinja::closeSockFDs()
{
  if(m_sockfd > 0) 
    close(m_sockfd);
  if(m_sockfd_lis > 0)
    close(m_sockfd_lis);

  m_sockfd = 0;
  m_sockfd_lis = 0;
  m_ninja_state = "unconnected";
}

//---------------------------------------------------------
// Procedure: setupConnectionForClient()
//      Note: Only relevant when ninja_type is client

bool SockNinja::setupConnectionForClient()
{
  if(m_ninja_state != "unconnected")
    return(false);

  // Part 1: Create the Socket
  if((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    string whyfail(strerror(errno));
    addWarning("Socket creation error: " + whyfail);
    return(false);
  }
  string event_strx = "Socket created (m_sockfd:" + intToString(m_sockfd) + ")";
  addEvent(event_strx);
      
  // Part 2: Fill out the sockaddr_in structure preparing for connect
  sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(m_port);
  
  // Convert IPv4 and IPv6 addresses from text to binary form  
  if(inet_pton(AF_INET, m_ip_addr.c_str(), &serv_addr.sin_addr)<=0) {
    addWarning("Invalid address or Address not supported");
    return(false);
  }
  
  // Part 3: Try to connect
  if(connect(m_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    string whyfail(strerror(errno));
    addWarning("Looking for Server at " + m_ip_addr + ", but none detected.");
    close(m_sockfd);
    return(false);
  }

  string event_str = "Socket connected on port: " + intToString(m_port);
  event_str += ", (m_sockfd:" + intToString(m_sockfd) + ")";
  addEvent(event_str);

  m_ninja_state = "connected";
  addRetraction("Disconnected");
  addRetraction("Looking for Server at " + m_ip_addr + ", but none detected.");
  return(true);
}

//---------------------------------------------------------
// Procedure: setupConnectionForServer()
//      Note: Only relevant when ninja_type is server

bool SockNinja::setupConnectionForServer()
{
  if(m_ninja_state != "listening")
    return(false);

  sockaddr_in cli_addr;
  // Look for incoming connections to accept
  socklen_t client_len = sizeof(cli_addr);
  m_sockfd = accept(m_sockfd_lis, (struct sockaddr*) &cli_addr, &client_len);
  string whyfail(strerror(errno));


  if(m_sockfd >= 0) {
    m_ninja_state = "connected";
    addRetraction("Disconnected");
    addRetraction("Listening for Client, but none detected.");
  }
  else {
    //addWarning("Error accepting client to TCP socket: " + whyfail);
    addWarning("Listening for Client, but none detected.");
    return(false);
  }


  char ipaddr[INET_ADDRSTRLEN];
  // Convert binary form to IPv4 and IPv6 addresses text
  if(inet_ntop(AF_INET, &(cli_addr.sin_addr), ipaddr, INET_ADDRSTRLEN) == 0) {
    string whyfail(strerror(errno));
    addWarning("Invalid client address: " + whyfail);
  }
  else
    m_ip_addr = ipaddr;
  
  addRetraction("Disconnected");
  string event_str = "Accepted connection on port: " + intToString(m_port);
  event_str += ", (m_sockfd:" + intToString(m_sockfd) + ")";
  addEvent(event_str);

  return(true);
}

//---------------------------------------------------------
// Procedure: readFromSock()

bool SockNinja::readFromSock()
{
  if(m_ninja_state != "connected")
    return(false);

  // ==========================================================
  // Part 1: Grab any chars that may have arrived at the socket
  // ==========================================================
  char incoming[BUF_SIZE];

  // Ensure we don't block if no data available to read
  int flags = fcntl(m_sockfd, F_GETFL, 0);
  fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);  
  
  ssize_t num_bytes = read(m_sockfd, incoming, BUF_SIZE);
  string whyfail(strerror(errno));
  if(num_bytes == 0) // Normal connection, just no data 
    return(false);

  if((num_bytes < 0) && (errno != EWOULDBLOCK)) {
    if(m_ninja_type == "server") {
      m_ninja_state = "listening";
      addWarning("Disconnected");
      //close(m_sockfd);
    }
    else
      m_ninja_state = "unconnected";
    addEvent("Error in reading from socket: " + whyfail);
    return(false);
  }

  // If the new data fits, move onto the input buffer
  if(strlen(m_in_buffer) + num_bytes < MAX_BUF_SIZE)
    strncat(m_in_buffer, incoming, num_bytes);

  string inbuff = m_in_buffer;

  // ==========================================================
  // Part 2A: Handle Input buffer in Verbatim Mode
  // ==========================================================
  if(m_msg_format == "verbatim") {
    // Parse string into a vector of lines
    findReplace(inbuff, "\r\n", "\n");
    vector<string> lines = parseString(inbuff, '\n');
    for(unsigned int i=0; i<lines.size(); i++) 
      m_rcvd_nmea_msgs.push_front(lines[i]);
    memset(m_in_buffer, 0, MAX_BUF_SIZE);
    return(true);
  }
  
  // ==========================================================
  // Part 2B: Handle and Parse Input buffer in NMEA Mode
  // ==========================================================

  // If there is no $ char, then no NMEA to parse. Leave the buffer
  // alone and wait for more content to arrive.
  size_t dpos = inbuff.find('$');
  if(dpos == string::npos)
    return(true);

  // Clear the character array buffer
  memset(m_in_buffer, 0, MAX_BUF_SIZE);

  // Remove any content preceding the first $ char
  inbuff = inbuff.substr(dpos);
  m_dropped_chars += (int)(dpos);
  

  vector<string> lines;
  while(inbuff.length() > 0) {
    size_t found = inbuff.find("\r\n");
    if(found == string::npos) {
      strcpy(m_in_buffer, inbuff.c_str());
      inbuff = "";
    }
    else {
      string line = inbuff.substr(0, found+2);
      inbuff = inbuff.substr(found+2);
      lines.push_back(line);
    }
  }

  if(lines.size() == 0)
    return(true);

  // handle lines that are valid NMEA messages w.r.t. checksum
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];
    if(isValidNMEA(line, true)) {
      addValidMessage(line);
    }
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: sendSockMessage()
//   Purpose: Sends a string over TCP to the attached client.
//   Returns: true if successful or if blank string (which is not sent)
//            false if error on writing to the TCP port

bool SockNinja::sendSockMessage(string str)
{
  if(m_ninja_state != "connected")
    return(true);

  if(str.length() == 0) {
    addWarning("Outgoing empty msg detected");
    return(true);
  }

  if((m_msg_format == "nmea") && !isValidNMEA(str)) {
    m_bad_send_nmea++;
    addWarning("Outgoing msg blocked - invalid NMEA format");
    return(false);
  }
  
  int num_bytes = write(m_sockfd, str.c_str(), str.length());
  string whyfail(strerror(errno));
  if(num_bytes == 0) {
    addWarning("Zero bytes written of non-zero length msg");
    return(false);
  }
  if(num_bytes < 0) {
    if(m_ninja_type == "server") {
      m_ninja_state = "listening";
      addWarning("Disconnected");
      close(m_sockfd);
    }
    else {
      m_ninja_state = "unconnected";
      addWarning("Disconnected");
    }
    addEvent("Error writing to socket: " + whyfail);
    return(false);
  }

  m_total_msgs_sent++;
  m_last_msg_sent = str;


  // If m_msg_format==nmea, them update map of messages keyed on the
  // NMEA header. If unbounded header types are received, eventually
  // this map will not be updated when the size is greater than
  // m_max_msg_keys. As these maps are a bookkeeping service, this
  // should not affect ninja I/O
  if(m_msg_format == "nmea") {
    string key = str.substr(0,6);
    if(m_map_msg_out_count.size() < m_max_msg_keys) {
      m_map_msg_out_count[key]++;
      m_map_msg_out_latest[key] = str;
    }
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: addValidMessage()

void SockNinja::addValidMessage(string msg)
{
  // Part 1: Add the message to the list for retrieval. This is
  // always done regardless of list size, but if user is not
  // retrieving messages, then old messages will be dropped off
  // eventually when the list exceeds m_max_list_size
  m_rcvd_nmea_msgs.push_front(msg);
  if(m_rcvd_nmea_msgs.size() > m_max_list_size)
    m_rcvd_nmea_msgs.pop_back();

  // Part 2: Update the total messages received and the latest
  // message string. Always done since this does not grow memory
  m_total_msgs_rcvd++;
  m_last_msg_rcvd = msg;

  // Part 3: If m_msg_format==nmea, them update map of messages
  // keyed on the NMEA header. If unbounded header types are
  // received, eventually this map will not be updated when the
  // size is greater than m_max_msg_keys. As these maps are a
  // bookkeeping service, this should not affect ninja I/O
  if(m_msg_format == "nmea") {
    string key = msg.substr(0,6);
    if(m_map_msg_in_count.size() < m_max_msg_keys) {
      m_map_msg_in_count[key]++;
      m_map_msg_in_latest[key] = msg;
    }
  }
}

//---------------------------------------------------------
// Procedure: addWarning()

void SockNinja::addWarning(string warning)
{
  m_warnings.push_front(warning);
  if(m_warnings.size() > m_max_list_size)
    m_warnings.pop_back();
}

//---------------------------------------------------------
// Procedure: addRetraction()

void SockNinja::addRetraction(string retraction)
{
  m_retractions.push_front(retraction);
  if(m_retractions.size() > m_max_list_size)
    m_retractions.pop_back();
}

//---------------------------------------------------------
// Procedure: addEvent()

void SockNinja::addEvent(string event)
{
  m_events.push_front(event);
  if(m_events.size() > m_max_list_size)
    m_events.pop_back();
}

//---------------------------------------------------------
// Procedure: getSockMessages()

list<string> SockNinja::getSockMessages()
{
  list<string> messages;
  if(!readFromSock())
    return(messages);
  
  messages = m_rcvd_nmea_msgs;
  m_rcvd_nmea_msgs.clear();

  return(messages);  
}

//---------------------------------------------------------
// Procedure: getSockMessage()

string SockNinja::getSockMessage()
{
  if(!readFromSock())
    return("fail");
  
  string most_recent_message = m_rcvd_nmea_msgs.back();
  m_rcvd_nmea_msgs.clear();

  return(most_recent_message);  
}

//---------------------------------------------------------
// Procedure: getWarnings()

list<string> SockNinja::getWarnings()
{
  list<string> warnings = m_warnings;
  m_warnings.clear();

  return(warnings);  
}

//---------------------------------------------------------
// Procedure: getRetractions()

list<string> SockNinja::getRetractions()
{
  list<string> retractions = m_retractions;
  m_retractions.clear();

  return(retractions);  
}

//---------------------------------------------------------
// Procedure: getEvents()

list<string> SockNinja::getEvents()
{
  list<string> events = m_events;
  m_events.clear();

  return(events);  
}

//---------------------------------------------------------
// Procedure: getTotalMsgsSent()

unsigned int SockNinja::getTotalMsgsSent(string key)
{
  if(key == "")
    return(m_total_msgs_sent);

  if(!m_map_msg_in_count.count(key))
    return(0);
  return(m_map_msg_in_count[key]);  
}

//---------------------------------------------------------
// Procedure: getTotalMsgsRcvd()

unsigned int SockNinja::getTotalMsgsRcvd(string key)
{
  if(key == "")
    return(m_total_msgs_rcvd);

  if(!m_map_msg_out_count.count(key))
    return(0);
  return(m_map_msg_out_count[key]);  
}


//---------------------------------------------------------
// Procedure: getLastMsgSent()

string SockNinja::getLastMsgSent(string key)
{
  if(key == "")
    return(m_last_msg_sent);

  if(!m_map_msg_out_latest.count(key))
    return("");
  return(m_map_msg_out_latest[key]);  
}

//---------------------------------------------------------
// Procedure: getLastMsgRcvd()

string SockNinja::getLastMsgRcvd(string key)
{
  if(key == "")
    return(m_last_msg_rcvd);

  if(!m_map_msg_in_latest.count(key))
    return("");
  return(m_map_msg_in_latest[key]);  
}



//---------------------------------------------------------
// Procedure: getSummary()

list<string> SockNinja::getSummary(bool terse)
{
  list<string> lines;
  list<string> lines_stat_comms = getSummaryStatComms(terse);
  list<string> lines_stat_msgs  = getSummaryStatMsgs();

  lines = lines_stat_comms;
  lines.push_back("---------------------------");
  lines.splice(lines.end(), lines_stat_msgs);
    
  return(lines);
}


//---------------------------------------------------------
// Procedure: getSummaryStatComms()

list<string> SockNinja::getSummaryStatComms(bool terse)
{
  string address;
  if(m_ninja_state == "connected") {
    address = m_ip_addr;
    if(m_ip_addr_name != "")
      address += " (a.k.a. " + m_ip_addr_name + ")";
    if(m_ninja_type == "server")
      address += " (of client)";
    else
      address += " (of server)";
  }

  list<string> lines;
  if(terse) {
    string line = getType() + "/" + getFormat() + "/" + getState();
    line += "/" + intToString(m_port) + "/" + address;
    lines.push_back(line);
  }
  else {
    lines.push_back("Comms Status: ");
    lines.push_back("  Type:   " + getType());
    lines.push_back("  Format: " + getFormat());
    lines.push_back("  State:  " + getState());
    lines.push_back("  Port:   " + intToString(m_port));
    lines.push_back("  IPAddr: " + address);
  }
  return(lines);  
}


//---------------------------------------------------------
// Procedure: getSummaryStatMsgs()
//   Purpose: Get a summary of all messages sent and received. For
//            each message type, show just the most recent message.
//            Also indicate if it was received or sent, and how
//            many have been received or sent.
//   Example: 
// <--R   27  $PYDIR,45,55*57
//  S-->  27  $CPNVG,134213.686,00.00,N,000.00,W,1,,,0,,,134213.686*64
//  S-->  27  $CPRBS,134213.686,1,15.2,15.1,15.3,0*5B
//  S-->  27  $GPRMC,134213.686,A,00.00,N,000.00,W,0,0,291263,0,E*67

list<string> SockNinja::getSummaryStatMsgs()
{
  list<string> lines;

  lines.push_back("NMEA sentences:            ");

  map<string, unsigned int>::iterator p;
  for(p=m_map_msg_in_count.begin(); p!=m_map_msg_in_count.end(); p++) {
    string key = p->first;
    unsigned int amt = p->second;
    string str_amt = uintToString(amt);
    str_amt = padString(str_amt, 6);
    string line = "<--R  " + str_amt + "  ";
    line += m_map_msg_in_latest[key];
    line = biteString(line, '\r');
    lines.push_back(line);
  }

  for(p=m_map_msg_out_count.begin(); p!=m_map_msg_out_count.end(); p++) {
    string key = p->first;
    unsigned int amt = p->second;
    string str_amt = uintToString(amt);
    str_amt = padString(str_amt, 6);
    string line = " S--> " + str_amt + "  ";
    line += m_map_msg_out_latest[key];
    line = biteString(line, '\r');
    lines.push_back(line);
  }

  return(lines);  
}

//---------------------------------------------------------
// Procedure: getSummaryStatErrors()

list<string> SockNinja::getSummaryStatErrors(bool terse)
{
  list<string> lines;

  if(m_msg_format == "verbatim") {
    lines.push_back("Errors: No Syntax error checking in verbatim mode");
    return(lines);
  }

  if(terse) {
    string line = "Errors: ";
    line += "DroppedChars:" + uintToString(m_dropped_chars);
    line += ",BadSendNMEA"  + intToString(m_bad_send_nmea);
    line += ",BadRcvdNMEA"  + intToString(m_bad_rcvd_nmea);
    lines.push_back(line);
  }
  else {
    lines.push_back("Errors:                 ");
    lines.push_back("  Dropped Chars: " + uintToString(m_dropped_chars));
    lines.push_back("  Bad Send NMEA: " + intToString(m_bad_send_nmea));
    lines.push_back("  Bad Rcvd NMEA: " + intToString(m_bad_rcvd_nmea));
  }
  return(lines);  
}



//---------------------------------------------------------
// Procedure: getSentKeysNMEA()
//   Purpose: Return a string of all unique NMEA keys ever sent.
//            Allows the user to potentially report out to the
//            terminal or log file if desired, as an easy way of
//            determining if a certain message type ever sent
//   Example: "$PYDIR,$PYABC,$MKERQ"

string SockNinja::getSentKeysNMEA() const
{
  string summary;
  map<string, unsigned int>::const_iterator p;
  for(p=m_map_msg_out_count.begin(); p!=m_map_msg_out_count.end(); p++) {
    string key = p->first;
    if(summary != "")
      summary += ",";
    summary += key;
  }

  return(summary);  
}

//---------------------------------------------------------
// Procedure: getRcvdKeysNMEA()
//   Purpose: Return a string of all unique NMEA keys ever received.
//            Allows the user to potentially report out to the
//            terminal or log file if desired, as an easy way of
//            determining if a certain message type ever received.
//   Example: "$CPNVG,$CPBAT,$GPRMC"

string SockNinja::getRcvdKeysNMEA() const
{
  string summary;
  
  map<string, unsigned int>::const_iterator p;
  for(p=m_map_msg_in_count.begin(); p!=m_map_msg_in_count.end(); p++) {
    string key = p->first;
    if(summary != "")
      summary += ",";
    summary += key;
  }

  return(summary);  
}


//---------------------------------------------------------
// Procedure: isValidNMEA()
//      Note: Minimum string with one empty field: $ABCDE,*HH\r\n
//            So a string with length less than 12 can be rejected
//      Note: A minimally valid NMEA satisfies:
//            1. Minimum length 12 characters
//            2. Ends with CRLF (\r\n)
//            3. $ at str[0]
//            4. , at str[6]
//            5. * at str[len-5]
//            6. Uppercase Hex at str[len-4], str[len-3]
//            7. Uppercase char at str[1..6]
//      Note: A strictly valid NMEA has a valid checksum

bool SockNinja::isValidNMEA(string str, bool strict)
{
  unsigned int len = str.length();
  if(len < 12) {
    addWarning("NMEA bad len");	      
    return(false);
  }
  if(str[len-1] != '\n')  {
    addWarning("NMEA bad nend");	      
    return(false);
  }
  if(str[len-2] != '\r') {
    int c = (int)(str[len-2]);
    addWarning("NMEA bad rend: " + intToString(c));	      
    return(false);
  }
  if((str[0] != '$') || (str[6] != ',') || (str[len-5] != '*')) {
    addWarning("NMEA bad $,*");	      
    return(false);
  }
  if(!isCheckSumChar(str[len-4]) || !isCheckSumChar(str[len-3])) {
    addWarning("NMEA HH");	      
    return(false);
  }
  string key;
  for(unsigned int i=1; i<6; i++) {
    if((str[i] < 65) || (str[i] > 90)) {
      addWarning("NMEA bad key");	      
      return(false);
    }
    key = str.substr(0,6);
  }
  
  if(!strict)
    return(true);
  
  str = str.substr(1); // chop off leading dollar sign

  string tail = rbiteString(str, '*');
  if(tail.length() < 2) {
    addWarning("NMEA bad tail");
    return(false);
  }
  string hexstr1 = tail.substr(0,2);
  string hexstr2 = checksumHexStr(str);

  if(hexstr1 != hexstr2) {
    if(m_map_msg_in_badsum.size() < m_max_msg_keys) {
      m_map_msg_in_badsum[key]++;
      addWarning("NMEA bad checksum: " + key);
    }
    return(false);
  }
  
  return(true);
}


//---------------------------------------------------------
// Procedure: isCheckSumChar()
//   Purpose: Determine if given char is [0..9] or [A..F]

bool SockNinja::isCheckSumChar(char c)
{
  if(((c>47) && (c<58)) || ((c>64)&&(c<71)))
    return(true);
  return(false);
}
    
//---------------------------------------------------------
// Procedure: setupListening()
//      Note: Only used when the ninja_type is a server 

bool SockNinja::setupListening()
{
  if(m_ninja_type != "server")
    return(false);
  
  // Part 1: Create the Socket
  m_sockfd_lis = socket(AF_INET, SOCK_STREAM, 0);
  string whyfail(strerror(errno));

  int good_sock = (m_sockfd_lis >= 0);
  if(!good_sock) {
    addWarning("Error creating TCP port: " + whyfail);
    return(false);
  }
  string event_str = "Socket created on port: " + intToString(m_port);
  event_str += ", (m_sockfd_lis:" + intToString(m_sockfd_lis) + ")";
  addEvent(event_str);

  // Make this socket non-blocking
  int res = fcntl(m_sockfd_lis, F_SETFL,
		  fcntl(m_sockfd_lis, F_GETFL, 0) | O_NONBLOCK);
  if(res == -1) {
    addWarning("Error in calling fcntl on m_sockfd_lis");
    return(false);
  }
  
  sockaddr_in serv_addr;  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(m_port);
  
  // Part 2: Bind the Socket
  int bind_res = bind(m_sockfd_lis, (sockaddr*) &serv_addr, sizeof(serv_addr));
  good_sock = (bind_res >= 0);
  whyfail.assign(strerror(errno));

  if(!good_sock) {
    // If socket in use, we handle as special case, allowing for retraction
    // when the conflict is resolved. Create our own warning message so we
    // we can exactly match it in a later possible retraction
    if(errno == EADDRINUSE)
      addWarning("Error binding TCP socket: Already in use.");
    else
      addWarning("Error binding TCP socket: " + whyfail);
    return(false);
  }
  else
    addRetraction("Error binding TCP socket: Already in use.");
  
  
  // Part 3: set up listening
  int heard = listen(m_sockfd_lis, 2);
  whyfail.assign(strerror(errno));

  if(heard) {
    addEvent("Error configuring listening on TCP socket. " + whyfail);
    return(false);
  }

  addEvent("Listening for connection on port: " + intToString(m_port));

  m_ninja_state = "listening";
  
  return(true);
}

//---------------------------------------------------------
// Procedure: getIPAddrByName()
//   Purpose: Attempt to resolve the given hostname to an IPv4
//            IP address. Typically will look in the local
//            /etc/hosts file first, and then use the DNS if
//            nothing found in /etc/hosts.

string SockNinja::getIPAddrByName(string host)
{
  struct addrinfo* result;

  int error = getaddrinfo(host.c_str(), 0, 0, &result);
  if(error) {
    string errstr = gai_strerror(error);
    addWarning("Failed to resolve hostname: " + errstr);
    return("");
  }
  else {
    char ipchar[INET_ADDRSTRLEN];
    if(getnameinfo(result->ai_addr, result->ai_addrlen, ipchar,
		   sizeof(ipchar), 0, 0, NI_NUMERICHOST) == 0) {
      host = string(ipchar);
    }
    else 
      addWarning("Unable to parse given host, using " + host);
  }
  return(host);
}


//---------------------------------------------------------
// Procedure: getStrASCII()

string SockNinja::getStrASCII(string str)
{
  string rstr;
  unsigned int len = str.length();
  for(unsigned int i=0; i<len; i++) {
    int c = (int)(str[i]);
    if(rstr != "")
      rstr += ",";
    rstr += intToString(c);
  }
  return(rstr);  
}
    
