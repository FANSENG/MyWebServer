//
// Created by fs1n on 4/2/23.
//
/**
 * ===================================
 * 资源获取即初始化原理
 * 防止出现资源忘记释放导致的内存泄漏
 * ===================================
*/
#ifndef WEBSERVER_SQLCONNRAII_H
#define WEBSERVER_SQLCONNRAII_H

#include "sqlconnpool.h"

class SqlConnRAII{
public:
SqlConnRAII(MYSQL** s, SqlConnPool* connpool){
    /**
     * 这里要用 MYSQL** s而不是 MYSQL* s
     * 因为 getConn() 返回的是 MYSQL*
     * 如果传入的是 MYSQL* s，指针值会被覆盖
     * 只能传入指针的指针，然后去修改指针的指针的值（即指针）
     * 保证获得的 Conn 能被外部捕获到
     * */

    assert(connpool);

    // *s[MYSQL*] = MYSQL*
    // s[MYSQL**] 此值没变，变得是 s 指针指向的地址(同样也是一个指针)
    *s = connpool->getConn();
    sql = *s;
    pool = connpool;
}

~SqlConnRAII(){
    if(sql) pool->freeConn(sql);
}
private:
    MYSQL* sql;
    SqlConnPool* pool;
};

#endif //WEBSERVER_SQLCONNRAII_H
