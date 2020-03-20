// C++ Wrapper for POSIX Sockets
// OPL License (attr. TLDP)
// Modified by Conlan Cesar (MIT 2019/2020)
// http://www.tldp.org/LDP/LG/issue74/misc/tougher/Socket.h.txt

#ifndef Socket_HEADER
#define Socket_HEADER

// MacOS doesn't have MSG_NOSIGNAL, explicitly declare it
#if defined(__APPLE__) && !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL 0x2000
#endif


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 1023;

class Socket {
public:
    Socket();
    virtual ~Socket();

    int close();

    // Server initialization
    bool create();
    bool bind (const std::string, const int port);
    bool listen() const;
    bool accept (Socket&) const;

    // Client initialization
    bool connect (const std::string host, const int port);

    // Data Transimission
    int send (const std::string) const;
    int recv (std::string&) const;


    void set_non_blocking (const bool);

    bool is_valid() const { return m_sock != -1; }

    sockaddr_in get_addr() const { return m_addr; }

private:
    int m_sock;
    sockaddr_in m_addr;

};


#endif