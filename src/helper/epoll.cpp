#include"epoll.h"

Epoll::Epoll(int maxEvent):m_events(maxEvent){
    m_epfd = epoll_create(OPEN_MAX);
}

Epoll::~Epoll(){
}

void Epoll::add_fd(int fd, uint32_t events){
    struct epoll_event event;
    event.events = events|EPOLLET;
    event.data.fd = fd;
    int ret = epoll_ctl(m_epfd,EPOLL_CTL_ADD,fd,&event);
    if(ret != 0){
        //log here
    }
    _set_nonblocking(fd);
}
void Epoll::mod_fd(int fd, uint32_t events){
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;
    int ret = epoll_ctl(m_epfd,EPOLL_CTL_MOD,fd,&event);
    if(ret != 0){
        //log here
    }
}
void Epoll::del_fd(int fd){
    int ret = epoll_ctl(m_epfd,EPOLL_CTL_DEL,fd,NULL);
    if(ret != 0){
        //log here
    }
}

int Epoll::wait(int timeout_ms){
    return epoll_wait(m_epfd,&m_events[0],m_events.size(),timeout_ms);
}

void Epoll::_set_nonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, old_option|O_NONBLOCK);
}
