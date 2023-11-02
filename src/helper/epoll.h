#pragma once
#include<sys/epoll.h>
#include<iostream>
#include<vector>
#include<fcntl.h>

class Epoll{
public:
    static Epoll* getInstance(){return m_instance;}

    Epoll(int maxEvent=1024);
    Epoll(Epoll*);
    ~Epoll();
    int getEpfd(){return this->m_epfd;}

    void addFd(int fd, uint32_t events, bool one_shot=false);
    void modFd(int fd, uint32_t events);
    void delFd(int fd);
    int wait(int timeout_ms);
    inline uint32_t getEvents(int i){return m_events[i].events;}
    inline int getEventFd(int i){return m_events[i].data.fd;}

private:
    static Epoll* m_instance;
    int m_epfd;
    static const int OPEN_MAX = 1024;
    std::vector<struct epoll_event> m_events;

    void _setNonBlocking(int fd);
};
