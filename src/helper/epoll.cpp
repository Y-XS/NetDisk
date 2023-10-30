#include"epoll.h"

Epoll::Epoll(int maxEvent){
    m_epfd = epoll_create(maxEvent);
    m_instance = nullptr;
}

Epoll::~Epoll(){
}

void Epoll::addFd(int fd, uint32_t events, bool one_shot){
    struct epoll_event event;
    event.events = events|EPOLLET;
    event.data.fd = fd;
    if(one_shot){
        event.events |= EPOLLONESHOT;
    }
    int ret = epoll_ctl(m_epfd,EPOLL_CTL_ADD,fd,&event);
    if(ret != 0){
        //log here
    }
    _setNonBlocking(fd);
}
void Epoll::modFd(int fd, uint32_t events){
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;
    int ret = epoll_ctl(m_epfd,EPOLL_CTL_MOD,fd,&event);
    if(ret != 0){
        //log here
    }
}
void Epoll::delFd(int fd){
    int ret = epoll_ctl(m_epfd,EPOLL_CTL_DEL,fd,NULL);
    if(ret != 0){
        //log here
    }
}

int Epoll::wait(int timeout_ms){
    return epoll_wait(m_epfd,&m_events[0],m_events.size(),timeout_ms);
}

void Epoll::_setNonBlocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, old_option|O_NONBLOCK);
}

Epoll* Epoll::m_instance = new Epoll(1024);
