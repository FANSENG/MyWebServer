//
// Created by fs1n on 4/2/23.
//

#include "httprequest.h"
using namespace std;

const unordered_set<string> HttpRequest::DEFAULT_HTML{
        "/index", "/register", "/login", "/welcome", "/video", "/picture"
};

const unordered_map<string, int> HttpRequest::DEFAULT_HTML_TAG{
        {"/register.html", 0},
        {"/login.html", 1}
};

int HttpRequest::converHex(char ch) {
    if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return ch;
}

void HttpRequest::init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HttpRequest::isKeepAlive() const {
    auto tmp = header_.find("Connection");
    if(tmp != header_.end()){
        return tmp->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}

bool HttpRequest::parse(Buffer &buff) {
    const char CRLF[] = "\r\n";
    if(buff.readableBytes() <= 0) return false;
    while(buff.readableBytes() && state_ != PARSE_STATE::FINISH){
        // 在 buff 中寻找以 CRLF
        const char* lineEnd = search(buff.readPtrConst(), buff.writePtrConst(), CRLF, CRLF + 2);
        std::string line(buff.readPtrConst(), lineEnd);
        switch (state_) {
            case REQUEST_LINE:
                // 确保请求行正确
                if(!parseRequestLine(line)) return false;
                parsePath();
                break;
            case HEADERS:
                parseHeader(line);
                // 读取完成，无剩余字符或剩余"\r\n"
                if(buff.readableBytes() <= 2) state_ = FINISH;
                break;
            case BODY:
                parseBody(line);
                break;
            default:
                break;
        }
        // 没有字节可读
        if(lineEnd == buff.writePtr()) break;
        buff.readUntil(lineEnd + 2);
    }
    LOG_DEBUG("[%s], [%s], [%s]", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

bool HttpRequest::parseRequestLine(const std::string &line) {
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)){
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = PARSE_STATE::HEADERS;
        return true;
    }
    LOG_ERROR("RequestLine Error");
    return false;
}

void HttpRequest::parseHeader(const std::string &line) {
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)){
        header_[subMatch[1]] = subMatch[2];
    }else{
        // 不存在 Key: Value 格式
        // header 解析完成，开始解析body
        state_ = PARSE_STATE::BODY;
    }
}

void HttpRequest::parseBody(const std::string &line) {
    body_ = line;
    parsePost();
    state_ = PARSE_STATE::FINISH;
    LOG_DEBUG("Body:%s, len:%d", line.c_str(), line.size());
}

void HttpRequest::parsePath() {
    if(path_ == "/") path_ = "/index.html";
    else if(DEFAULT_HTML.find(path_) != DEFAULT_HTML.end()) path_ += ".html";
}

void HttpRequest::parsePost() {
    if(method_ == "POST" && header_["Content-Type"] == "application/x-www-form-urlencoded"){
        parseFromUrlEncoded();
        if(DEFAULT_HTML_TAG.find(path_) != DEFAULT_HTML_TAG.end()){
            int tag = DEFAULT_HTML_TAG.find(path_)->second;
            LOG_DEBUG("Tag: %d", tag);
            if(tag == 0 || tag == 1){
                bool isLogin = (tag == 1);
                if(userVerify(post_["username"], post_["password"], isLogin))
                    path_ = "/welcome.html";
                else path_ = "/error.html";
            }
        }
    }
}

void HttpRequest::parseFromUrlEncoded() {
    if(body_.size() == 0) { return; }

    string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;

    for(; i < n; i++) {
        char ch = body_[i];
        switch (ch) {
            case '=':
                key = body_.substr(j, i - j);
                j = i + 1;
                break;
            case '+':
                body_[i] = ' ';
                break;
            case '%':
                num = converHex(body_[i + 1]) * 16 + converHex(body_[i + 2]);
                body_[i + 2] = num % 10 + '0';
                body_[i + 1] = num / 10 + '0';
                i += 2;
                break;
            case '&':
                value = body_.substr(j, i - j);
                j = i + 1;
                post_[key] = value;
                LOG_DEBUG("%s = %s", key.c_str(), value.c_str());
                break;
            default:
                break;
        }
    }
    assert(j <= i);
    if(post_.find(key) == post_.end() && j < i) {
        value = body_.substr(j, i - j);
        post_[key] = value;
    }
}

bool HttpRequest::userVerify(const std::string& name, const std::string& pwd, bool isLogin){
    if(name == "" || pwd == "") { return false; }
    LOG_INFO("Verify name:%s pwd:%s", name.c_str(), pwd.c_str());
    MYSQL** tosql;
    SqlConnRAII sqlRaII(tosql, SqlConnPool::Instance());
    MYSQL* sql = *tosql;
    assert(sql);

    bool flag = false;
    unsigned int j = 0;
    char order[256] = {0};
    MYSQL_FIELD *field = nullptr;
    MYSQL_RES *res = nullptr;
    if(!isLogin) flag = true;
    snprintf(order, 256, "SELECT password FROM user WHERE username='%s' LIMIT 1", name.c_str());
    LOG_DEBUG("Sql line: %s", order);

    if(mysql_query(sql, order)) {
        mysql_free_result(res);
        return false;
    }
    res = mysql_store_result(sql);
    j = mysql_num_fields(res);
    field = mysql_fetch_fields(res);
    while(MYSQL_ROW row = mysql_fetch_row(res)) {
        LOG_DEBUG("MYSQL ROW: %s %s", name.c_str(), row[0]);
        string password(row[0]);
        /* 注册行为 且 用户名未被使用*/
        if(isLogin) {
            if(pwd == password) { flag = true; }
            else {
                flag = false;
                LOG_DEBUG("pwd error!");
            }
        }
        else {
            flag = false;
            LOG_DEBUG("user used!");
        }
    }
    mysql_free_result(res);

    if(!isLogin && !flag){
        LOG_DEBUG("regirster!");
        bzero(order, 256);
        ::snprintf(order, 256, "INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), pwd.c_str());
        LOG_DEBUG("%s", order);
        if(mysql_query(sql, order)) {
            LOG_DEBUG( "Insert error!");
            flag = false;
        }
        flag = true;
    }
    SqlConnPool::Instance()->freeConn(sql);
    LOG_DEBUG( "UserVerify success!!");
    return flag;
}

std::string HttpRequest::path() const{
    return path_;
}

std::string& HttpRequest::path(){
    return path_;
}
std::string HttpRequest::method() const {
    return method_;
}

std::string HttpRequest::version() const {
    return version_;
}

std::string HttpRequest::getPost(const std::string& key) const {
    assert(key != "");
    if(post_.find(key) != post_.end()) return post_.find(key)->second;
    return "";
}

std::string HttpRequest::getPost(const char* key) const {
    assert(key != nullptr);
    if(post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}