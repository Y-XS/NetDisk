#ifndef TIMER_H
#define TIMER_H

#include "loger.h"
#include<chrono>
#include<functional>
#include<vector>
#include<unordered_map>

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
// typedef std::chrono::seconds Seconds;
typedef Clock::time_point TimeStamp;

struct TimerNode{
    int id;                     //标记定时器
    TimeStamp expire;          //设置超时时间
    TimeoutCallBack callback;   //回调函数
    bool operator<(const TimerNode& t){
        return expire < t.expire;
    }
};
class HeapTimer{
public:
    HeapTimer(){m_heap.reserve(64);}
    ~HeapTimer(){clear();}
    //添加定时器
    void add(int id,int timeout,const TimeoutCallBack& cb);
    //处理定时器
    void adjust(int id, int newExpire);
    //触发处理函数
    void work(int id);
    void tick();
    void clear();
    void pop();
    int getNextTick();
private:
    void _del(size_t idx);
    void _siftup(size_t i);
    bool _siftdown(size_t idx, size_t n);
    void _swap(size_t i,size_t j);
    std::vector<TimerNode> m_heap;
    std::unordered_map<int,size_t> m_ref;
};



#endif
