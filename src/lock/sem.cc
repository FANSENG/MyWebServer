/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-03-30 14:04:57
 * @LastEditors: fs1n
 * @LastEditTime: 2023-03-30 14:29:06
 */
#include "sem.h"

sem::sem(){
    sem_init(&m_sem, 0, 0);
}

sem::sem(int num){
    sem_init(&m_sem, 0, num);
}

sem::~sem(){
    sem_destroy(&m_sem);
}

bool sem::wait(){
    sem_wait(&m_sem);
}

bool sem::wait_for(int time){
    
}

bool sem::post(){
    sem_post(&m_sem);
}

bool sem::set(int num){
    int now;
    sem_getvalue(&m_sem, &now);
    // assert(num > now);
    now -= num;
    while(now < 0){
        post();
        now++;
    }
}