//
// Created by fs1n on 4/3/23.
//

#include "httpconn.h"

using namespace std;

const char* HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;
bool HttpConn::isET;

HttpConn::HttpConn(): fd_(-1), addr_({0}), isClose_(true) {}

HttpConn::~HttpConn() { close(); }

void HttpConn::init(int sockFd, const sockaddr_in &addr) {
    assert(sockFd > 0);
    userCount++;
    addr_ = addr;
    fd_ = sockFd;
    writeBuff_.readAll();
    readBuff_.readAll();
    isClose_ = false;
    LOG_INFO("Client[%d](%s:%d) in, userCount:%d", fd_, getIP(), getPort(), (int)userCount);
}

void HttpConn::close() {
    response_.unmapFile();
    if(!isClose_){
        ::close(fd_);
        isClose_ = true;
        userCount--;
        LOG_INFO("Client[%d](%s:%d) quit, UserCount:%d", fd_, getIP(), getPort(), (int)userCount);
    }
}

int HttpConn::getFd() const { return  fd_;}

struct sockaddr_in HttpConn::getAddr() const { return addr_; }

const char* HttpConn::getIP() const { return inet_ntoa(addr_.sin_addr); }

int HttpConn::getPort() const { return addr_.sin_port; }

ssize_t HttpConn::read(int *saveErrno) {
    ssize_t len = -1;
    do{
        len = readBuff_.readFd(fd_, saveErrno);
    }while(isET && len > 0);
    return len;
}

ssize_t HttpConn::write(int *saveErrno) {
    ssize_t len = -1;
    do{
        len = writev(fd_, iov_, iovCnt_);
        if(len <= 0){
            *saveErrno = errno;
            break;
        }
        if(iov_[0].iov_len + iov_[1].iov_len == 0) break;
        if(len > static_cast<ssize_t>(iov_[0].iov_len)){
            iov_[1].iov_base = (uint8_t *)iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);

            writeBuff_.readAll();
            iov_[0].iov_len = 0;
        }else{
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            writeBuff_.hasRead(len);
        }
    }while(isET || toWriteBytes() > 10240);
    return len;
}

bool HttpConn::process() {
    request_.init();
    if(readBuff_.readableBytes() <= 0) return false;
    else if(request_.parse(readBuff_)){     // 解析请求行
        LOG_DEBUG("Get request: %s", request_.path().c_str());
        response_.init(srcDir, request_.path(), request_.isKeepAlive(), 200);
    }else{
        response_.init(srcDir, request_.path(), false, 400);
    }

    response_.makeResponse(writeBuff_);
    iov_[0].iov_base = const_cast<char*>(writeBuff_.readPtrConst());
    iov_[0].iov_len = writeBuff_.readableBytes();
    iovCnt_ = 1;

    if(response_.fileLen() > 0 && response_.file()){
        iov_[1].iov_base = response_.file();
        iov_[1].iov_len = response_.fileLen();
        iovCnt_ = 2;
    }
    LOG_DEBUG("filesize:%d, %d  to %d", response_.fileLen() , iovCnt_, toWriteBytes());
    return true;
}