/*
 * @Author: fs1n
 * @Email: fs1n@qq.com
 * @Description: {To be filled in}
 * @Date: 2023-03-29 15:03:33
 * @LastEditors: fs1n
 * @LastEditTime: 2023-04-01 14:07:49
 */

#include "buffer.h"

// buffer 最大 128kb
size_t Buffer::BUFFER_MAX = 65535 * 2;

Buffer::Buffer(int initBufferSize): buffer(initBufferSize), capacity(initBufferSize), readPos(0), writePos(0){}

size_t Buffer::writeableBytes() const{
    // 获得缓冲区中能直接写的字节数
    // 这里不包含已经读过的空间
    return buffer.size() - writePos;
}

size_t Buffer::readableBytes() const{
    return writePos - readPos;
}

size_t Buffer::coverableBytes() const{
    return readPos;
}

const char* Buffer::readPtr() const{
    return beginPtrConst() + readPos;
}

void Buffer::ensureWriteable(size_t len){
    if(len > writeableBytes()){
        makeSpace(len);
    }
    assert(writeableBytes() >= len);
}

void Buffer::hasWrite(size_t len){
    assert(len <= writeableBytes());
    writePos += len;
}

void Buffer::hasRead(size_t len){
    assert(len <= readableBytes());
    readPos += len;
}

void Buffer::readUntil(const char* end){
    assert((end - beginPtr()) <= readableBytes());
    hasRead(end - beginPtr());
}

void Buffer::readAll(){
    // 省去置为零的操作，之后直接覆盖即可
    // 读写指针可以保证不会读到之前的数据
    // bzero((void*)(&buffer), capacity);
    readPos = 0;
    writePos = 0;
}

std::string Buffer::readAllToString(){
    std::string str(readPtr(), readableBytes());
    readAll();
    return str;
}

const char* Buffer::writePtrConst() const{
    return beginPtrConst() + writePos;
}

char* Buffer::writePtr(){
    return beginPtr() + writePos;
}

void Buffer::Append(const std::string &str){
    Append(str.data(), str.size());
}

void Buffer::Append(const char* str, size_t len){
    assert(str);
    ensureWriteable(len);
    std::move(str, str + len, writePtrConst());
    hasWrite(len);
}

void Buffer::Append(const void* data, size_t len){
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const Buffer& buff){
    Append(buff.readPtr(), buff.readableBytes());
}

ssize_t Buffer::readFd(int fd, int* Errno){
    char buff[65535];
    struct iovec iov[2];
    const size_t writeable = writeableBytes();

    iov[0].iov_base = writePtr();
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0){
        *Errno = errno;
    }else if(static_cast<size_t>(len) < writeable){
        hasWrite(len);
        
    }else{
        writePos = buffer.size();
        Append(buff, len - writeable);
    }
    return len;
}

ssize_t Buffer::writeFd(int fd, int* Errno){
    ssize_t len = write(fd, readPtr(), readableBytes());
    if(len < 0){
        *Errno = errno;
        return len;
    }
    hasRead(len);
    return len;
}

char* Buffer::beginPtr(){
    return &(*buffer.begin());
}

const char* Buffer::beginPtrConst() const{
    return &(*buffer.begin());
}

void Buffer::makeSpace(size_t len){
    // 确保缓冲区可以存储此数据
    assert(len <= (BUFFER_MAX - readableBytes()));
    if(writeableBytes() + coverableBytes() >= len){
        std::move(readPtr(), writePtrConst() - 1, beginPtrConst());
        writePos = readableBytes();
        readPos = 0;
    }else{
        while(capacity <= len && capacity < BUFFER_MAX) capacity = capacity * 2;
        buffer.resize(capacity);
    }
}