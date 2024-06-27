#include "time.h"
#include "timer.h"


void HeapTimer::_swap(size_t i,size_t j){
    std::swap(m_heap[i], m_heap[j]);
    m_ref[m_heap[i].id] = i;
    m_ref[m_heap[j].id] = j;
}



void HeapTimer::_siftup(size_t i)
{
    size_t j = (i - 1) / 2;
    while(j >= 0) {
        if(m_heap[j] < m_heap[i]) { break; }
        swap(i, j);
        i = j;
        j = (i - 1) / 2;
    }
}
bool  HeapTimer::_siftdown(size_t idx, size_t n){
    size_t i = idx;
    size_t j = i * 2 + 1;
    while(j < n) {
        if(j + 1 < n && m_heap[j + 1] < m_heap[j]) j++;
        if(m_heap[i] < m_heap[j]) break;
        swap(i, j);
        i = j;
        j = i * 2 + 1;
    }
    return i > idx;
}


void HeapTimer::add(int id, int timeout, const TimeoutCallBack &callback)
{
    if(id<0)
        return;
    size_t i;
    if(m_ref.count(id) == 0){
        /* 新节点：堆尾插入，调整堆 */
        i = m_heap.size();
        m_ref[id] = i;
        m_heap.push_back({id, Clock::now() + MS(timeout), callback});
        _siftup(i);
    }
    else{
        /* 已有结点：调整堆 */
        i = m_ref[id];
        m_heap[i].expire = Clock::now() + MS(timeout);
        m_heap[i].callback = callback;
        if(!_siftdown(i, m_heap.size())) {
            _siftup(i);
        }
    }
}

void HeapTimer::_del(size_t idx)
{
    /* 删除指定位置的结点 */
    // assert(!heap_.empty() && idx >= 0 && idx < heap_.size());
    /* 将要删除的结点换到队尾，然后调整堆 */
    size_t i = idx;
    size_t n = m_heap.size() - 1;
    // assert(i <= n);
    if(i < n) {
        _swap(i, n);
        if(!_siftdown(i, n)) {
            _siftup(i);
        }
    }
    /* 队尾元素删除 */
    m_ref.erase(m_heap.back().id);
    m_heap.pop_back();
}

void HeapTimer::work(int id)
{
    /* 删除指定id结点，并触发回调函数 */
    if(m_heap.empty() || m_ref.count(id) == 0) {
        return;
    }
    size_t i = m_ref[id];
    TimerNode node = m_heap[i];
    node.callback();
    _del(i);
}
void HeapTimer::adjust(int id, int newExpire){
    if(m_heap.empty()&&m_ref.count(id)<=0)
        return;
    m_heap[m_ref[id]].expire = Clock::now() + MS(newExpire);;
    _siftdown(m_ref[id], m_heap.size());
}
void HeapTimer::tick(){
    if(m_heap.empty())
        return;
    while(!m_heap.empty()){
        TimerNode node = m_heap.front();
        if(std::chrono::duration_cast<MS>(node.expire - Clock::now()).count()>0){
            break;
        }
        node.callback();
        pop();
    }
}


void HeapTimer::clear()
{
    m_ref.clear();
    m_heap.clear();
}

void HeapTimer::pop()
{
    if(m_heap.empty())
        return;
    _del(0);
}

int HeapTimer::getNextTick() {
    tick();
    size_t res = -1;
    if(!m_heap.empty()) {
        res = std::chrono::duration_cast<MS>(m_heap.front().expire - Clock::now()).count();
        if(res < 0) { res = 0; }
    }
    return res;
}


