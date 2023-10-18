#pragma once
#include<sys/epoll.h>
#include<iostream>
#include<vector>
#include<fcntl.h>

class Epoll{
public:
    Epoll(int maxEvent=1024);
    ~Epoll();

    void add_fd(int fd, uint32_t events);
    void mod_fd(int fd, uint32_t events);
    void del_fd(int fd);
    int wait(int timeout_ms);
    inline uint32_t get_events(int i){return m_events[i].events;}
    inline int get_event_fd(int i){return m_events[i].data.fd;}

private:
    static const int OPEN_MAX = 1024;
    int m_epfd;
    std::vector<struct epoll_event> m_events;

    void _set_nonblocking(int fd);
};