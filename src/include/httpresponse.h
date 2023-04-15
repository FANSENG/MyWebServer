/**
 * ===========================
 * Http回复对象，包括功能如下：
 *      1. 构造回复的状态行、header、content
 *      2. 读取请求的文件到内存中
 *      3. 根据状态码判断是否需要返回 error 对应的 html页面
 *      4. 释放内存中的请求文件
 * ===========================
*/
#ifndef WEBSERVER_HTTPRESPONSE_H
#define WEBSERVER_HTTPRESPONSE_H

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap

#include "buffer.h"
#include "log.h"

class HttpResponse{
public:
    HttpResponse();
    ~HttpResponse();

    /// @brief 
    /// @param srcDir 资源文件目录
    /// @param path 请求的文件名
    /// @param isKeepAlive 是否长连接
    /// @param code 状态码(200/400/403/404)
    void init(const std::string &srcDir, std::string &path, bool isKeepAlive = false, int code = -1);
    
    /// @brief 构造response，并写入 buff 中
    /// @param buff 
    void makeResponse(Buffer &buff);
    
    /// @brief 释放请求的文件在内存中的映射
    void unmapFile();

    char* file();

    /// @brief 获取文件长度
    /// @return 
    size_t fileLen() const;

    /// @brief 构造 error 返回内容， 写入 buff 中
    /// @param buff 
    /// @param message 
    void errorContent(Buffer &buff, std::string message);

    /// @brief 获取 状态码
    /// @return 
    int code() const{ return code_; }

private:
    /// @brief 添加状态行(version code status)
    /// @param buff 
    void addStateLine_(Buffer &buff);

    /// @brief 添加 response header
    /// @param buff 
    void addHeader_(Buffer &buff);

    /// @brief 添加 response content
    /// @param buff 
    void addContent_(Buffer &buff);

    /// @brief 根据 code 读取 对应的html信息到 mmFileStat_
    void errorHtml_();

    /// @brief 根据 SUFFIX_TYPE 获取文件的类型
    /// @return 
    std::string getFileType_();

    int code_;                  // 状态码
    bool isKeepAlive_;          // 长连接

    std::string path_;          // 请求资源名
    std::string srcDir_;        // 资源目录

    char* mmFile_;              // 请求文件在内存中的地址
    struct stat mmFileStat_;    // 内存中文件的状态

    /// @brief 文件后缀对应在 html 中的类型
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;

    /// @brief 不同 code 对应的 message
    static const std::unordered_map<int, std::string> CODE_STATUS;

    /// @brief 不同 code 对应的 html 文件
    static const std::unordered_map<int, std::string> CODE_PATH;
};

#endif //WEBSERVER_HTTPRESPONSE_H
