#ifndef HTTP_CONN__H
#define HTTP_CONN__H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <sys/mman.h>
#include <stdarg.h>
#include <map>
// #include <vector>
#include "http_request.h"
#include "http_response.h"
#include "../helper/epoll.h"
#include "../utils/loger.h"

class HttpConn
{
public:
  static int userCnt;
  static string rootDir;
  
  enum STATE
  {
    READ,
    WRITE
  };

  HttpConn();
  void init(int sockfd, const sockaddr_in &addr);
  ssize_t readBuf();
  ssize_t writeBuf();
  bool process();
  string doRequest();
  int doResponse(string retBody);
  void close_conn();

  int getFd() const { return m_sockFd; }
  sockaddr_in getAddr() const { return m_addr; }
  const char *getIP() const { return inet_ntoa(m_addr.sin_addr); }
  int getPort() const { return m_addr.sin_port; }
  STATE getState() const { return m_state; }
  STATE setState(STATE state) { m_state = state; }

private:
  int m_sockFd;
  struct sockaddr_in m_addr;
  STATE m_state;
  static const int BUF_SIZE = 1024;

  // std::vector<char> m_readBuf;
  // std::vector<char> m_writeBuf;
  char m_readBuf[BUF_SIZE];
  char m_writeBuf[BUF_SIZE];

  HttpRequest m_request;
  HttpResponse m_response;
};

#endif