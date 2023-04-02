#include "condition.h"

condition::condition(){
    m_cond = new std::condition_variable_any();
    time_slice = 100ms;
}

condition::~condition(){
    delete m_cond;
}

void condition::wait(std::shared_mutex *m_mutex){
    m_cond->wait(*m_mutex);
}

bool condition::wait_for(std::shared_mutex *m_mutex, int time){
    return m_cond->wait_for(*m_mutex, time * time_slice) == std::cv_status::no_timeout;
}

void condition::notify_one(){
    m_cond->notify_one();
}

void condition::notify_all(){
    m_cond->notify_all();
}