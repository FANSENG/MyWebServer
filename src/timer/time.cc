/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-04-01 01:19:26
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-06 16:34:13
 */
#include "timer.h"

void HeapTimer::add(int id, int timeOut, const TimeoutCallback& cb){
    assert(id >= 0);
    size_t index;
    // 当前存在此 节点
    if(ref_.find(id) != ref_.end()){
        index = ref_[id];
        heap_[index].expires = Clock::now() + MS(timeOut);
        heap_[index].cb = cb;
        if(!siftdown_(index, heap_.size())) siftup_(index);
    }else{
        index = heap_.size();
        ref_[id] = index;
        heap_.push_back({id, Clock::now() + MS(timeOut), cb});
        siftup_(index);
    }
}

void HeapTimer::adjust(int id, int timeout){
    assert(ref_.find(id) != ref_.end() && !heap_.empty());
    heap_[ref_[id]].expires = Clock::now() + MS(timeout);
    siftdown_(ref_[id], heap_.size());
}

void HeapTimer::doWork(int id){
    assert(ref_.find(id) != ref_.end() && !heap_.empty());
    size_t index = ref_[id];
    heap_[index].cb();
    del_(index);
}

void HeapTimer::tick(){
    if(heap_.empty()) return;
    while(!heap_.empty()){
        TimeNode tmp = heap_.front();
        if(std::chrono::duration_cast<MS>(tmp.expires - Clock::now()).count() > 0) break;
        // 回调函数
        tmp.cb();
        pop();
    }
}

void HeapTimer::pop(){
    del_(0);
}

int HeapTimer::getNextTick(){
    tick();
    size_t res = -1;
    if(heap_.empty()) return -1;
    return std::min(0, int(std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now()).count()));
}

void HeapTimer::clear(){
    heap_.clear();
    ref_.clear();
}

void HeapTimer::swapNode_(size_t index1, size_t index2){
    assert(index1 >= 0 && index1 < heap_.size());
    assert(index2 >= 0 && index2 < heap_.size());
    std::swap(heap_[index1], heap_[index2]);
    ref_[heap_[index1].id] = index1;
    ref_[heap_[index2].id] = index2;
}

void HeapTimer::siftup_(size_t index){
    assert(index >= 0 && index < heap_.size());
    size_t tmp = (index - 1) / 2;
    while(tmp >= 0){
        if(heap_[tmp] < heap_[index]) break;
        swapNode_(tmp, index);
        index = tmp;
        tmp = (index - 1) / 2;
    }
}

bool HeapTimer::siftdown_(size_t index, size_t n){
    assert(index >= 0 && index < heap_.size());
    assert(n >= 0 && n <= heap_.size());
    size_t origin = index;
    size_t tmp = index * 2 + 1;
    while(tmp < n){
        if(tmp + 1 < n && heap_[tmp + 1] < heap_[tmp]) tmp++;
        if(heap_[index] < heap_[tmp]) break;
        swapNode_(index, tmp);
        index = tmp;
        tmp = tmp * 2 + 1;
    }
    // 返回 ture 则表明调整了结构
    // 返回 false 则表明没有调整
    return index != origin;
}

void HeapTimer::del_(size_t index){
    assert(!heap_.empty() && index >= 0 && index < heap_.size());
    size_t indexMax = heap_.size() - 1;
    assert(index <= indexMax);
    if(index != indexMax){
        swapNode_(index, indexMax);
        if(!siftdown_(index, indexMax)) siftup_(index);
    }
    ref_.erase(heap_.back().id);
    heap_.pop_back();
}