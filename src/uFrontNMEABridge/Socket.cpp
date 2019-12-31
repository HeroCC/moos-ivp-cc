// C++ Wrapper for POSIX Sockets
// OPL License (attr. TLDP)
// Modified by Conlan Cesar (MIT 2019/2020)
// http://www.tldp.org/LDP/LG/issue74/misc/tougher/Socket.cpp.txt

#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>


Socket::Socket() : m_sock(-1) {
  memset(&m_addr, 0, sizeof(m_addr));
}

Socket::~Socket() {
  if (is_valid()) {
    ::close(m_sock);
  }
}

bool Socket::create() {
  m_sock = socket(AF_INET, SOCK_STREAM, 0);

  if (!is_valid()) {
    return false;
  }


  // TIME_WAIT - argh
  int on = 1;
  return setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on)) != -1;
}


bool Socket::bind(const int port) {
  if (!is_valid()) {
    return false;
  }


  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = INADDR_ANY;
  m_addr.sin_port = htons(port);

  int bind_return = ::bind(m_sock, (struct sockaddr *) &m_addr, sizeof(m_addr));


  return bind_return != -1;
}


bool Socket::listen() const {
  if (!is_valid()) {
    return false;
  }

  int listen_return = ::listen(m_sock, MAXCONNECTIONS);


  return listen_return != -1;
}


bool Socket::accept(Socket &new_socket) const {
  errno = 0;
  int addr_length = sizeof(m_addr);
  new_socket.m_sock = ::accept(m_sock, (sockaddr *) &m_addr, (socklen_t *) &addr_length);

  return new_socket.m_sock > 0;
  // WOULDBLOCK and AGAIN are successful when nonblocking
  //return (errno == EWOULDBLOCK) || (errno == EAGAIN) || new_socket.m_sock > 0;
}


int Socket::send(const std::string s) const {
  errno = 0;
  int status = ::send(m_sock, s.c_str(), s.size(), MSG_NOSIGNAL);
  //return status != -1;
  return (status != -1) ? 0 : errno;
}


int Socket::recv(std::string &s) const {
  char buf[MAXRECV + 1];

  s = "";

  memset(buf, 0, MAXRECV + 1);

  errno = 0;

  int status = ::recv(m_sock, buf, MAXRECV, 0);

  if (status == -1) {
    if (errno != EAGAIN) std::cerr << "status == -1   errno == " << strerror(errno) << "  in Socket::recv\n";
    return 0;
  } else if (status == 0) {
    return 0;
  } else {
    s = buf;
    return status;
  }
}


bool Socket::connect(const std::string host, const int port) {
  if (!is_valid()) return false;

  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons (port);

  int status = inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr);

  if (errno == EAFNOSUPPORT) return false;

  status = ::connect(m_sock, (sockaddr *) &m_addr, sizeof(m_addr));

  return status == 0;
}

void Socket::set_non_blocking(const bool b) {
  int opts;

  opts = fcntl(m_sock, F_GETFL);

  if (opts < 0) {
    return;
  }

  if (b) {
    opts = (opts | O_NONBLOCK);
  } else {
    opts = (opts & ~O_NONBLOCK);
  }

  fcntl(m_sock, F_SETFL, opts);
}