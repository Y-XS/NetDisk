#pragma once
#include<sys/epoll.h>
#include<iostream>
#include<vector>
#include<fcntl.h>

class Epoll{
public:
    Epoll(int maxEvent=1024);
    ~Epoll();

    void AddFD(int fd, uint32_t events);
    void ModFD(int fd, uint32_t events);
    void DelFD(int fd);
    int Wait(int timeout_ms);
    inline uint32_t GetEvents(int i){return events_[i].events;}
    inline int GetEventFd(int i){return events_[i].data.fd;}

private:
    static const int OPEN_MAX = 1024;
    int epfd_;
    std::vector<struct epoll_event> events_;

    void _SetNonBlocking(int fd);
};