/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-01 01:19:26
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-01 22:48:57
 */
#include "timer.h"

void HeapTimer::add(int id, int timeOut, const TimeoutCallback& cb){
    assert(id >= 0);
    size_t index;
    // 当前存在此 节点
    if(ref.find(id) != ref.end()){
        index = ref[id];
        heap[index].expires = Clock::now() + MS(timeOut);
        heap[index].cb = cb;
        if(!siftdown_(index, heap.size())) siftup_(index);
    }else{
        index = heap.size();
        ref[id] = index;
        heap.push_back({id, Clock::now() + MS(timeOut), cb});
        siftup_(index);
    }
}

void HeapTimer::adjust(int id, int timeout){
    assert(ref.find(id) != ref.end() && !heap.empty());
    heap[ref[id]].expires = Clock::now() + MS(timeout);
    siftdown_(ref[id], heap.size());
}

void HeapTimer::doWork(int id){
    assert(ref.find(id) != ref.end() && !heap.empty());
    size_t index = ref[id];
    heap[index].cb();
    this->del_(index);
}

void HeapTimer::tick(){
    if(heap.empty()) return;
    while(!heap.empty()){
        TimeNode tmp = heap.front();
        if(std::chrono::duration_cast<MS>(tmp.expires - Clock::now()).count() > 0) break;
        // 回调函数
        tmp.cb();
        this->pop();
    }
}

void HeapTimer::pop(){
    TimeNode tmp = heap.front();
    ref.erase(tmp.id);
    del_(0);
}

void HeapTimer::clear(){
    heap.clear();
    ref.clear();
}

void HeapTimer::swapNode_(size_t index1, size_t index2){
    assert(index1 >= 0 && index1 < heap.size());
    assert(index2 >= 0 && index2 < heap.size());
    std::swap(heap[index1], heap[index2]);
    ref[heap[index1].id] = index1;
    ref[heap[index2].id] = index2;
}

void HeapTimer::siftup_(size_t index){
    assert(index >= 0 && index < heap.size());
    size_t tmp = (index - 1) / 2;
    while(tmp >= 0){
        if(heap[tmp] >= heap[index]){
            swapNode_(tmp, index);
        }
        index = tmp;
        tmp = (index - 1) / 2;
    }
}

bool HeapTimer::siftdown_(size_t index, size_t n){
    assert(index >= 0 && index < heap.size());
    assert(n >= 0 && n <= heap.size());
    size_t origin = index;
    size_t tmp = index * 2 + 1;
    while(tmp < n){
        if(tmp + 1 < n && heap[tmp + 1] < heap[tmp]) tmp++;
        if(heap[index] < heap[tmp]) break;
        swapNode_(index, tmp);
        index = tmp;
        tmp = tmp * 2 + 1;
    }
    // 返回 ture 则表明调整了结构
    // 返回 false 则表明没有调整
    return index != origin;
}

void HeapTimer::del_(size_t index){
    assert(!heap.empty() && index >= 0 && index < heap.size());
    size_t indexMax = heap.size() - 1;
    assert(index <= indexMax);
    // ?这部分理解一下
    if(index != indexMax){
        swapNode_(index, indexMax);
        if(!siftdown_(index, indexMax)) siftup_(index);
    }
    ref.erase(heap.back().id);
    heap.pop_back();
}