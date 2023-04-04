/**
 * =========================================
 * 请求处理对象，包含以下功能
 *      1. 对请求行的解析，得出 method url version
 *      2. 解析请求头，得出 是否长连接
 *      3. 解析请求体，适用于 POST 方法
 *      4. 根据 url 解析出请求的 html 文件
 *      5. 用户登录 & 注册
 * =========================================
*/

#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <cerrno>
#include <mysql/mysql.h>  //mysql

#include "buffer.h"
#include "log.h"
#include "sqlconnpool.h"
#include "sqlconnRAII.h"

class HttpRequest{
public:
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH
    };

    enum HEEP_CODE{
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    HttpRequest() { init(); }
    ~HttpRequest() = default;

    void init();
    bool parse(Buffer& buff);

    /// @brief 获取 URL
    /// @return 
    std::string path() const;
    std::string& path();

    /// @brief 获取 request method
    /// @return 
    std::string method() const;

    /// @brief 获取 HTTP version. Default HTTP 1.1
    /// @return 
    std::string version() const;

    /// @brief 功能基本和 post_[key] 等价，如果不存在 key 则返回空字符串
    /// @param key 
    /// @return 
    std::string getPost(const std::string& key) const;
    std::string getPost(const char* key) const;

    /// @brief 是否长连接
    /// @return 
    bool isKeepAlive() const;

private:
    /// @brief 解析请求行
    /// @param line 
    /// @return 
    bool parseRequestLine(const std::string& line);

    /// @brief 解析请求头，解析结果存入header_
    /// @param line 
    void parseHeader(const std::string& line);

    /// @brief 解析请求体，解析结果存入post_
    /// @param line 
    void parseBody(const std::string& line);

    /// @brief 解析 path, 将path解析为对应的 html 文件
    void parsePath();

    /// @brief 解析 path 中传递的参数, 判断返回的 html 页面
    void parsePost();
    
    void parseFromUrlEncoded();

    /// @brief 登录&注册函数
    /// @param name 用户名
    /// @param pwd 密码
    /// @param isLogin 是否为登录(false 表示 注册)
    /// @return 
    static bool userVerify(const std::string& name, const std::string& pwd, bool isLogin);

    // 当前解析状态(REQUEST_LINE/BODY/HEADER/FINISH)
    PARSE_STATE state_;
    // REQUEST_LINE 解析出的内容 和 请求体
    std::string method_, path_, version_, body_;
    // 请求头 解析出的内容
    std::unordered_map<std::string, std::string> header_;
    // post 解析出的内容
    std::unordered_map<std::string, std::string> post_;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;
    
    // ch 为十六进制数的某一位，转化为十进制后返回int。
    // 如 'A' => 10
    static int converHex(char ch);

};

#endif //WEBSERVER_HTTPREQUEST_H
