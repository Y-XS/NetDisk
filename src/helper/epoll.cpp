#include"epoll.h"

Epoll::Epoll(int maxEvent):events_(maxEvent){
    epfd_ = epoll_create(OPEN_MAX);
}

Epoll::~Epoll(){
}

void Epoll::AddFD(int fd, uint32_t events){
    struct epoll_event event;
    event.events = events|EPOLLET;
    event.data.fd = fd;
    int ret = epoll_ctl(epfd_,EPOLL_CTL_ADD,fd,&event);
    if(ret != 0){
        //log here
    }
    _SetNonBlocking(fd);
}
void Epoll::ModFD(int fd, uint32_t events){
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;
    int ret = epoll_ctl(epfd_,EPOLL_CTL_MOD,fd,&event);
    if(ret != 0){
        //log here
    }
}
void Epoll::DelFD(int fd){
    int ret = epoll_ctl(epfd_,EPOLL_CTL_DEL,fd,NULL);
    if(ret != 0){
        //log here
    }
}

int Epoll::Wait(int timeout_ms){
    return epoll_wait(epfd_,&events_[0],events_.size(),timeout_ms);
}

void Epoll::_SetNonBlocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, old_option|O_NONBLOCK);
}
