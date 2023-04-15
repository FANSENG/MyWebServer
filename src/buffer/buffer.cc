#include "buffer.h"

// buffer 最大 128kb
size_t Buffer::BUFFER_MAX = 65535 * 2;

Buffer::Buffer(int initBufferSize): buffer(initBufferSize), readPos(0), writePos(0){}

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

char* Buffer::readPtr(){
    return beginPtr() + readPos;
}

const char* Buffer::readPtrConst() const{
    return beginPtrConst() + readPos;
}

void Buffer::ensureWriteable(size_t len){
    if(len > writeableBytes()){
        makeSpace(len);
    }
    assert(writeableBytes() >= len);
}

void Buffer::hasWrite(size_t len){
    // assert(len <= writeableBytes());
    writePos += len;
}

void Buffer::hasRead(size_t len){
    assert(len <= readableBytes());
    readPos += len;
}

void Buffer::readUntil(const char* end){
    assert(readPtr() < end);
    hasRead(end - readPtr());
}

void Buffer::readAll(){
    // 省去置为零的操作，之后直接覆盖即可
    // 读写指针可以保证不会读到之前的数据
    bzero(&buffer[0], buffer.size());
    readPos = 0;
    writePos = 0;
}

std::string Buffer::readAllToString(){
    std::string str(readPtrConst(), readableBytes());
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

void Buffer::Append(const void* data, size_t len){
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const Buffer& buff){
    Append(buff.readPtrConst(), buff.readableBytes());
}

void Buffer::Append(const char* str, size_t len){
    assert(str);
    ensureWriteable(len);
    std::copy(str, str + len, writePtr());
    hasWrite(len);
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
    }else if(static_cast<size_t>(len) <= writeable){
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
    return &*buffer.begin();
}

const char* Buffer::beginPtrConst() const{
    return &*buffer.begin();
}

void Buffer::makeSpace(size_t len){
    // 确保缓冲区可以存储此数据
    assert(len <= (BUFFER_MAX - readableBytes()));
    if(writeableBytes() + coverableBytes() >= len){
        std::copy(readPtr(), writePtr(), beginPtr());
        writePos = readableBytes();
        readPos = 0;
    }else{
        int capacity = buffer.size();
        while(capacity <= (len + writePos + 1) && capacity < BUFFER_MAX) capacity <<= 1;
        buffer.resize(capacity);
        // buffer.resize(writePos + len + 1);
    }
}