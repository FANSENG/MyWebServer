/**
 * ==================================
 * server 和 client 连接的对象
 * 提供接口: 
 *      1. 读 client 的信息到 readBuff 中
 *      2. 将 writeBuff 的信息写入 client 中
 *      3. process函数，处理 readBuff 中的信息(request)，将结果写入 writeBuff(response)
 *      4. 获取 client 相关信息的 getFd、getPort、getIp、getAddr 等函数
 *      5. 初始化接口 init
 * ==================================
*/
#ifndef WEBSERVER_HTTPCONN_H
#define WEBSERVER_HTTPCONN_H

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <cstdlib>      // atoi()
#include <cerrno>

#include "log.h"
#include "sqlconnRAII.h"
#include "buffer.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpConn {
public:
    HttpConn();

    ~HttpConn();

    /**
     * @brief 根据 socket fd 和 地址 初始化连接
     * @param {int} sockFd
     * @param {sockaddr_in&} addr
     * @return {*}
     */
    void init(int sockFd, const sockaddr_in& addr);

    /**
     * @brief 从 socket fd 读取数据， 存储到 readBuff_ 中
     * @param {int*} saveErrno
     * @return {*}
     */
    ssize_t read(int* saveErrno);

    /**
     * @brief 将 writeBuff_ 中的数据写入 socket 的发送缓冲区
     * @param {int*} saveErrno
     * @return {*}
     */
    ssize_t write(int* saveErrno);

    /**
     * @brief 关闭连接
     * @return {*}
     */
    void close();

    /**
     * @brief 获取 当前描述符
     * @return {*}
     */
    int getFd() const;

    /**
     * @brief 获取 端口号
     * @return {*}
     */
    int getPort() const;

    /**
     * @brief 获取 IP 地址
     * @return {*}
     */
    const char* getIP() const;

    /**
     * @brief 获取 地址
     * @return {*}
     */
    sockaddr_in getAddr() const;

    /**
     * @brief 处理连接
     * @return {*}
     */
    bool process();

    /**
     * @brief 等待写入的数据量
     * @return {*}
     */
    int toWriteBytes() {
        return iov_[0].iov_len + iov_[1].iov_len;
    }

    /**
     * @brief 是否保持连接
     * @return {*}
     */
    bool isKeepAlive() const {
        return request_.isKeepAlive();
    }

    static bool isET;                   // 是否使用 ET 模式
    static const char* srcDir;          // 资源目录
    static std::atomic<int> userCount;  // 用户数量

private:

    int fd_;                        // socket fd
    struct  sockaddr_in addr_;      // 地址

    bool isClose_;                  // 连接是否关闭

    int iovCnt_{};                    // iovec 数量
    // iov_[0] 存储响应头的内容
    // iov_[1] 存储响应体的内容
    struct iovec iov_[2]{};           // iovec 数组

    Buffer readBuff_;               // 读缓冲区
    Buffer writeBuff_;              // 写缓冲区

    HttpRequest request_;           // 请求对象
    HttpResponse response_;         // 响应对象
};

#endif //WEBSERVER_HTTPCONN_H
