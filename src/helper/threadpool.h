#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<list>
#include<iostream>
#include"locker.h"

template<typename T>
class ThreadPool{
public:
    ThreadPool(int thread_num=8,int max_requests=10000);
    ~ThreadPool();
    bool append(T* request);
private:
    int m_thread_num;
    int m_max_requests;
    pthread_t* m_threads;
    std::list<T*> m_work_queue;
    Locker m_queue_locker;
    Sem m_queue_stat;
    bool m_stop;
    //工作线程业务函数
    static void* _worker(void* arg);
    void _run();
};

template<typename T>
ThreadPool<T>::ThreadPool(int thread_num,int max_requests)
    :m_thread_num(thread_num),m_max_requests(max_requests),m_stop(false),m_threads(NULL){
    if((thread_num<=0)||(max_requests<=0))
        throw std::exception();
    m_threads = new pthread_t[m_thread_num];
    if(!m_threads)
        throw std::exception();
    for(int i=0;i<thread_num;++i){
        std::cout<<"create the "<<i<<"th thread."<<std::endl;
        //pthread_create第三个参数必须传静态函数，因此将this指针作为参数传进子线程以执行_run业务函数
        if(pthread_create(m_threads+i,NULL,_worker,this)!=0 && pthread_detach(m_threads[i])){
            delete[] m_threads;
            throw std::exception();
        }
    }
}
template<typename T>
ThreadPool<T>::~ThreadPool(){
    delete[] m_threads;
    m_stop = true;
}
template<typename T>
bool ThreadPool<T>::append(T* request){
    m_queue_locker.lock();
    if(m_work_queue.size()>m_max_requests){
        m_queue_locker.unlock();
        return false;
    }
    m_work_queue.push_back(request);
    m_queue_locker.unlock();
    m_queue_stat.post();
    return true;
}
template<typename T>
void* ThreadPool<T>::_worker(void* arg){
    ThreadPool* pool = (ThreadPool*)arg;
    pool->_run();
    return pool;
}
template<typename T>
void ThreadPool<T>::_run(){
    while(!m_stop){
        m_queue_stat.wait();
        m_queue_locker.lock();
        if(m_work_queue.empty()){
            m_queue_locker.unlock();
            continue;
        }
        T* request = m_work_queue.front();
        m_work_queue.pop_front();
        m_queue_locker.unlock();
        if(!request)
            continue;
        request->process();
    }
}



#endif