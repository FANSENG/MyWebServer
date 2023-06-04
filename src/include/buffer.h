/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-03-29 15:03:40
 * @LastEditors: fs1n
 * @LastEditTime: 2023-06-03 14:20:02
 */

/**
 * ============================
 * 1. 用于日志缓存
 * 2. 用于http连接读写缓存
 * ============================
*/
#ifndef BUFFER_H
#define BUFFER_H

#include <cstring>
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readvk
#include <atomic>
#include <assert.h>

class Buffer {
public:
    Buffer(int initBufferSize = 10240);
    ~Buffer() = default;

    /// @brief 可写字节数
    /// @return 
    size_t writeableBytes() const;

    /// @brief 可读字节数
    /// @return 
    size_t readableBytes() const;

    /// @brief 可覆盖字节数
    /// @return 
    size_t coverableBytes() const;

    /// @brief 获取可读数据首地址
    /// @return 
    char* readPtr();

    /// @brief 获取可读数据首地址
    /// @return 
    const char* readPtrConst() const;

    /// @brief 确保可写空间大于等于len 空间不足会去扩容
    /// @param len 
    void ensureWriteable(size_t len);

    /// @brief 已经写入了长度为 len 的数据，修改写指针
    /// @param len 
    void hasWrite(size_t len);

    /// @brief 读取了 len byte 数据，读指针后移
    /// @param len 
    void hasRead(size_t len);

    /// @brief 读取到了 end 处的数据，读指针后移
    /// @param end 
    void readUntil(const char* end);

    /// @brief 读取了所有数据，重置读缓冲区
    void readAll();

    /// @brief 读取所有数据并以 string 返回
    /// @return 
    std::string readAllToString();

    /// @brief 获取可写部分的首地址
    /// @return 
    const char* writePtrConst() const;
    char* writePtr();

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buff);

    /// @brief 从 fd 中读取数据，并存入读缓冲区
    /// @param fd 
    /// @param Errno 
    /// @return 
    ssize_t readFd(int fd, int* Errno);

    /// @brief 将 写缓冲区 的数据写入 fd
    /// @param fd 
    /// @param Errno 
    /// @return 
    ssize_t writeFd(int fd, int* Errno);

private:
    static size_t BUFFER_MAX;           // 最大 buffer 数量
    char* beginPtr();                   // buffer 起始地址
    const char* beginPtrConst() const;  // buffer 起始地址(const)
    void makeSpace(size_t len);         // 保证buff能够存储 len 字节数据
    
    std::vector<char> buffer;
    
    /// @brief atomic 是原子对象，不存在数据竞争，是线程安全的
    std::atomic<std::size_t> readPos;
    std::atomic<std::size_t> writePos;
};


#endif // BUFFER_H