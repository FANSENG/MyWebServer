/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-02-27 20:37:51
 * @LastEditors: fs1n
 * @LastEditTime: 2023-03-30 14:10:08
 */
#ifndef SEM_H
#define SEM_H

#include <semaphore.h>

class sem{
public:
    sem();
    sem(int num);
    ~sem();
    bool wait();
    bool wait_for(int time);
    bool post();
    bool set(int num);
private:
    sem_t m_sem;
};

#endif // SEM_H