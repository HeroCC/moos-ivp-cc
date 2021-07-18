/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SockNinja.h                                          */
/*    DATE: Mar 25th 2020                                        */
/*****************************************************************/

#ifndef SOCK_NINJA_HEADER
#define SOCK_NINJA_HEADER

#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <list> 
#include <map> 

#define MAX_NINJA_BUF_SIZE 16384

class SockNinja {
public:
  SockNinja(std::string ntype="server", int port=29500);
  ~SockNinja() {closeSockFDs();}

  bool  setCommsType(std::string);
  bool  setIPAddr(std::string);
  bool  setPortNumber(int);
  void  setMsgFormatVerbatim();
  void  setMaxListSize(unsigned int);
  bool  setupConnection();
  void  closeSockFDs();
  
  bool                   sendSockMessage(std::string str);
  std::list<std::string> getSockMessages();
  std::string            getSockMessage();

  std::list<std::string> getWarnings();
  std::list<std::string> getRetractions();
  std::list<std::string> getEvents();

  std::list<std::string> getSummary(bool terse=false);
  std::list<std::string> getSummaryStatComms(bool terse=false);
  std::list<std::string> getSummaryStatMsgs();
  std::list<std::string> getSummaryStatErrors(bool terse=false);
  
  std::string getType()   const {return(m_ninja_type);}
  std::string getState()  const {return(m_ninja_state);}
  std::string getIPAddr() const {return(m_ip_addr);}
  std::string getFormat() const {return(m_msg_format);}
  int  getPort()          const {return(m_port);}
  bool isConnected()      const {return(m_ninja_state=="connected");}

  unsigned int getDroppedChars() const {return(m_dropped_chars);}
  unsigned int getBadSendNMEA()  const {return(m_bad_send_nmea);}
  unsigned int getBadRcvdNMEA()  const {return(m_bad_rcvd_nmea);}
  
  unsigned int getTotalMsgsSent(std::string key="");
  unsigned int getTotalMsgsRcvd(std::string key="");
  std::string  getLastMsgSent(std::string key="");
  std::string  getLastMsgRcvd(std::string key="");

  std::string  getSentKeysNMEA() const;
  std::string  getRcvdKeysNMEA() const;
  
protected:
  void  addValidMessage(std::string);
  void  addWarning(std::string);
  void  addRetraction(std::string);
  void  addEvent(std::string);
  virtual bool  isValidNMEA(std::string, bool strict=true);
  virtual bool  isCheckSumChar(char);
  bool  setupConnectionForClient();
  bool  setupConnectionForServer();
  bool  readFromSock();
  bool  setupListening();   // Only used when type=server
  
  std::string getIPAddrByName(std::string);
  std::string getStrASCII(std::string);
  
protected:
  std::string  m_ninja_type;
  std::string  m_ninja_state;
  std::string  m_msg_format;

  unsigned int m_max_list_size;
  unsigned int m_max_msg_keys;
  
  char         m_in_buffer[MAX_NINJA_BUF_SIZE];
  int          m_port;
  int          m_sockfd;
  int          m_sockfd_lis; // Only used when type=server
  std::string  m_ip_addr; 
  std::string  m_ip_addr_name; 
  
  std::list<std::string>  m_rcvd_nmea_msgs;
  std::list<std::string>  m_warnings;
  std::list<std::string>  m_retractions;
  std::list<std::string>  m_events;

  unsigned int m_dropped_chars;
  unsigned int m_bad_send_nmea;
  unsigned int m_bad_rcvd_nmea;
  
  unsigned int m_total_msgs_sent;
  unsigned int m_total_msgs_rcvd;
  std::string  m_last_msg_sent;
  std::string  m_last_msg_rcvd;

  // Key is the 6-char NMEA header, e.g. $GPRMC
  std::map<std::string, unsigned int> m_map_msg_in_count;
  std::map<std::string, std::string>  m_map_msg_in_latest;
  std::map<std::string, unsigned int> m_map_msg_in_badsum;
  std::map<std::string, unsigned int> m_map_msg_out_count;
  std::map<std::string, std::string>  m_map_msg_out_latest;
};

#endif
