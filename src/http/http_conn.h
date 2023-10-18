#ifndef HTTP_CONN__H
#define HTTP_CONN__H

#include<unistd.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<iostream>
#include<sys/mman.h>
#include<stdarg.h>

class http_conn{
public:
    /*所有socket上的事件都被注册到同一个epoll内核事件表中，所以将epoll文件描述符设置为静态的*/
    static int m_epollfd;
    /*统计用户数量*/
    static int m_user_count;

    http_conn();
    ~http_conn();
    /*初始化新接受的连接*/
    void init(int sockfd,const sockaddr_in &addr);
    /*关闭连接*/
    void close_conn(bool real_close=true);
    /*处理客户请求*/
    void process();
    /*非阻塞读操作*/
    bool read();
    /*非阻塞写操作*/
    bool write();

private:
    /*文件名的最大长度*/
    static const int FILENAME_LEN=200;
    /*读缓冲区的大小*/
    static const int READ_BUF_SIZE = 2048;
    /*写缓冲区的大小*/
    static const int WRITE_BUF_SIZE = 1024;
    /*HTTP请求方法，但我们仅支持GET*/
    enum METHOD{GET=0,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNETC,PATCH};
    /*解析客户请求时，主状态机所处的状态（回忆第8章）*/
    enum CHECK_STATE{STATE_REQUESTLINE=0,STATE_HEADER,STATE_CONTENT};
    /*服务器处理HTTP请求的可能结果*/
    enum HTTP_CODE{NO_REQUEST,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FORBIDDEN_REQUEST,FILE_REQUEST,INTERNAL_ERROR,CLOSED_CONNECTION};
    /*行的读取状态*/
    enum LINE_STATUS{LINE_OK=0,LINE_BAD,LINE_OPEN};
    /*该HTTP连接的socket和对方的socket地址*/
    int m_sockfd;
    sockaddr_in m_addr;
    /*读缓冲区*/
    char m_read_buf[READ_BUF_SIZE];
    /*标识读缓冲中已经读入的客户数据的最后一个字节的下一个位置*/
    int m_read_idx;
    /*当前正在分析的字符在读缓冲区中的位置*/
    int m_checked_idx;
    /*当前正在解析的行的起始位置*/
    int m_start_line;
    /*写缓冲区*/
    char m_write_buf[WRITE_BUF_SIZE];
    /*写缓冲区中待发送的字节数*/
    int m_write_idx;
    /*主状态机当前所处的状态*/
    CHECK_STATE m_check_state;
    /*请求方法*/
    METHOD m_method;
    /*客户请求的目标文件的完整路径，其内容等于doc_root+m_url，doc_root是网站根目录*/
    char m_real_file[FILENAME_LEN];
    /*客户请求的目标文件的文件名*/
    char* m_url;
    /*HTTP协议版本号，我们仅支持HTTP/1.1*/
    char* m_version;
    /*主机名*/
    char* m_host;
    /*HTTP请求的消息体的长度*/
    int m_content_length;
    /*HTTP请求是否要求保持连接*/
    bool m_linger;
    /*客户请求的目标文件被mmap到内存中的起始位置*/
    char* m_file_addr;
    /*目标文件的状态。通过它我们可以判断文件是否存在、是否为目录、是否可读，并获取文件大小等信息*/
    struct stat m_file_stat;
    /*我们将采用writev来执行写操作，所以定义下面两个成员，其中m_iv_count表示被写内存块的数量*/
    struct iovec m_iv[2];
    int m_iv_count;

    /*初始化连接*/
    void _init();
    /*解析HTTP请求*/
    HTTP_CODE _process_read();
    /*填充HTTP应答*/
    bool _process_write(HTTP_CODE ret);
    /*下面这一组函数被_ProcessRead调用以分析HTTP请求*/
    HTTP_CODE _ParseRequestLine(char *text);
    HTTP_CODE _ParseHeaders(char *text);
    HTTP_CODE _ParseContent(char *text);
    HTTP_CODE _DoRequest();
    char* _GetLine(){return m_read_buf+m_start_line;}
    LINE_STATUS _ParseLine();
    /*下面这一组函数被process_write调用以填充HTTP应答*/
    void _Unmap();
    bool _AddResponse(const char*format,...);
    bool _AddContent(const char*content);
    bool _AddStatusLine(int status,const char*title);
    bool _AddHeaders(int content_length);
    bool _AddContentLength(int content_length);
    bool _AddLinger();
    bool _AddBlankLine();
};


#endif