#ifndef HTTP_CONN__H
#define HTTP_CONN__H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include<sys/sendfile.h>
#include <stdlib.h>
#include <iostream>
#include <sys/mman.h>
#include <stdarg.h>
#include <map>
#include <set>
#include <sys/uio.h>
#include "http_request.h"
#include "http_response.h"
#include "../helper/epoll.h"
#include "../utils/loger.h"
#include "../helper/sql_conn_pool.h"

class HttpConn
{
public:
  static int userCnt;
  static string rootDir;
  static const int FILENAME_LEN = 200;
  // static std::set<std::string> m_file_set;

  enum STATE
  {
    READ,
    WRITE
  };

  HttpConn();
  void init(int sockfd, const sockaddr_in &addr);
  ssize_t readBuf();
  bool process();
  int doRequest();
  int doGet();
  int doPost();
  bool doResponse();
  bool writeBuf();
  void unmap();
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
  static const int BUF_SIZE = 4096;
  static const int HEADER_SIZE = 1024;

  char m_readBuf[BUF_SIZE];
  char m_writeBuf[BUF_SIZE];
  int m_writeInfo_len;
  char m_resp_header[HEADER_SIZE];

  char m_file_name[FILENAME_LEN];
  char* m_file_address;
  struct stat m_file_stat;
  int m_iovCnt;
  struct iovec m_iov[2];
  int bytes_to_send;
  int bytes_have_send;

  HttpRequest m_request;
  HttpResponse m_response;

  void init();
  void getFileList();
  inline int getToWriteBytes(){return m_iov[0].iov_len+m_iov[1].iov_len;}
};

#endif