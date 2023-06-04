/**
 * ================================
 * webserver 对象
 *      1. 通过监听端口监听 client 的连接
 *      2. 存储 client 连接相关信息
 *      3. 重置 client 连接时间
 *      4. 处理 client 读
 *      5. 处理 client 写
 *      6. 解析处理 client 的request，得出 response
 * ================================
*/

#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H
#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <cassert>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "log.h"
#include "timer.h"
#include "sqlconnpool.h"
#include "threadpool.h"
#include "sqlconnRAII.h"
#include "httpconn.h"
#include "config.h"

class WebServer {
public:
    WebServer(
            int port, int trigMode, int timeoutMS, bool OptLinger,
            int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum, int threadNum,
            bool openLog, LogLevel logLevel, int logQueSize);

    ~WebServer();
    void start();

private:
    /// @brief 初始化 socket
    /// @return 
    bool initSocket_();

    /// @brief 设置 监听 和 client 在epoll中的触发模式(ET/LT)
    /// @param trigMode 
    void initEventMode_(int trigMode);

    /// @brief 新增客户端连接，添加到 user_ 和 epoll 中
    /// @param fd 
    /// @param addr 
    void addClient_(int fd, sockaddr_in addr);

    /// @brief 处理监听端口，将申请连接的 client 添加到 user_ 和 epoll 中
    void listenProcessor_();

    /// @brief 通过线程池单独分配一个线程去处理 写 client 的任务
    /// @param client 
    void writeProcessor_(HttpConn* client);

    /// @brief 通过线程池单独分配一个线程去处理 读 client 的任务
    /// @param client 
    void readProcessor_(HttpConn* client);

    /// @brief 发送 info 到 fd，发送信息为错误信息，发送完成后会关闭连接
    /// @param fd 
    /// @param info 
    void sendError_(int fd, const char*info);

    /// @brief 重置 client 连接过期时间
    /// @param client 
    void extentTime_(HttpConn* client);

    /// @brief 关闭 client 连接
    /// @param client 
    void closeConn_(HttpConn* client);

    /// @brief 从 client 读取数据，读取完成后通过 onProcess 处理数据
    /// @param client 
    void onRead_(HttpConn* client);

    /// @brief 向 client 发送数据，具体发送内容由 http 解析部分决定
    /// @param client 
    void onWrite_(HttpConn* client);

    /// @brief 执行解析&response流程，执行完成后监听输出事件
    ///         如果没有可处理数据，则监听输入事件
    /// @param client 
    void onProcess(HttpConn* client);

    static const int MAX_FD = 65536;

    /// @brief 设置 fd 非阻塞
    /// @param fd 
    /// @return 
    static int setFdNonblock(int fd);

    int port_;
    bool openLinger_;
    int timeoutMS_;  /* 毫秒MS */
    bool isClose_;
    int listenFd_;
    char* srcDir_;

    uint32_t listenEvent_;
    uint32_t connEvent_;

    std::unique_ptr<HeapTimer> timer_;
    std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<Epoller> epoller_;

    // fd -> HttpConn
    std::unordered_map<int, HttpConn> users_;
};
#endif //WEBSERVER_WEBSERVER_H
