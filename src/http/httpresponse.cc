//
// Created by fs1n on 4/3/23.
//
#include "httpresponse.h"

using namespace std;

const unordered_map<string, string> HttpResponse::SUFFIX_TYPE = {
        { ".html",  "text/html" },
        { ".xml",   "text/xml" },
        { ".xhtml", "application/xhtml+xml" },
        { ".txt",   "text/plain" },
        { ".rtf",   "application/rtf" },
        { ".pdf",   "application/pdf" },
        { ".word",  "application/nsword" },
        { ".png",   "image/png" },
        { ".gif",   "image/gif" },
        { ".jpg",   "image/jpeg" },
        { ".jpeg",  "image/jpeg" },
        { ".au",    "audio/basic" },
        { ".mpeg",  "video/mpeg" },
        { ".mpg",   "video/mpeg" },
        { ".avi",   "video/x-msvideo" },
        { ".gz",    "application/x-gzip" },
        { ".tar",   "application/x-tar" },
        { ".css",   "text/css" },
        { ".js",    "text/javascript" },
};

const unordered_map<int, string> HttpResponse::CODE_STATUS = {
        { 200, "OK" },
        { 400, "Bad Request" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
};

const unordered_map<int, string> HttpResponse::CODE_PATH = {
        { 400, "/400.html" },
        { 403, "/403.html" },
        { 404, "/404.html" },
};

HttpResponse::HttpResponse(): code_(-1), isKeepAlive_(false), mmFile_(nullptr), mmFileStat_({0}) {}

HttpResponse::~HttpResponse() {
    unmapFile();
}

void HttpResponse::init(const std::string &srcDir, std::string &path, bool isKeepAlive, int code) {
    assert(!srcDir.empty());
    if(mmFile_) unmapFile();
    srcDir_ = srcDir;
    path_ = path;
    isKeepAlive_ = isKeepAlive;
    code_ = code;
    mmFile_ = nullptr;
    mmFileStat_ = {0};
}

void HttpResponse::makeResponse(Buffer &buff) {
    if(stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)){
        code_ = 404;
    }
    else if(!(mmFileStat_.st_mode & S_IROTH)) {
        code_ = 403;
    }
    else if(code_ == -1) {
        code_ = 200;
    }
    errorHtml_();
    addStateLine_(buff);
    addHeader_(buff);
    addContent_(buff);
}

char* HttpResponse::file() {
    return mmFile_;
}

size_t HttpResponse::fileLen() const {
    return mmFileStat_.st_size;
}

void HttpResponse::errorHtml_() {
    if(CODE_PATH.find(code_) != CODE_PATH.end()){
        path_ = CODE_PATH.find(code_)->second;
        stat((srcDir_ + path_).data(), &mmFileStat_);
    }
}

void HttpResponse::addStateLine_(Buffer& buff) {
    string status;
    if(CODE_STATUS.find(code_) != CODE_STATUS.end()) {
        status = CODE_STATUS.find(code_)->second;
    }
    else {
        code_ = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buff.Append("HTTP/1.1 " + to_string(code_) + " " + status + "\r\n");
}

void HttpResponse::addHeader_(Buffer& buff) {
    buff.Append("Connection: ");
    if(isKeepAlive_) {
        buff.Append("keep-alive\r\n");
        buff.Append("keep-alive: max=6, timeout=120\r\n");
    } else{
        buff.Append("close\r\n");
    }
    buff.Append("Content-type: " + getFileType_() + "\r\n");
}

void HttpResponse::addContent_(Buffer &buff) {
    int srcFd = open((srcDir_ + path_).data(), O_RDONLY);
    if(srcFd < 0){
        errorContent(buff, "File NotFound!");
        return;
    }

    LOG_DEBUG("file path: %s", (srcDir_ + path_).data());
    // 将文件读取到内存中
    // PORT_READ : 可以被读取
    // MAP_PRIVATE : 建设一个写时复制的私有映射空间
    int *mmRet = (int*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if(*mmRet == -1){
        errorContent(buff, "File NotFound!");
        return;
    }
    mmFile_ = (char*)mmRet;
    close(srcFd);
    buff.Append("Content-length: " + to_string(mmFileStat_.st_size) + "\r\n\r\n");
}

void HttpResponse::unmapFile() {
    if(mmFile_){
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
}

string HttpResponse::getFileType_() {
    string::size_type idx = path_.find_last_of('.');
    // path_ 中没有找到 . 说明不是文件
    if(idx == string::npos)
        return "text/plain";
    string suffix = path_.substr(idx);
    return SUFFIX_TYPE.find(suffix) == SUFFIX_TYPE.end() ? "text/plain" : SUFFIX_TYPE.find(suffix)->second;
}

void HttpResponse::errorContent(Buffer &buff, string message) {
    string body;
    string status;
    status = CODE_STATUS.find(code_) == CODE_STATUS.end() ? "Bad Request" : CODE_STATUS.find(code_)->second;

    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    body += to_string(code_) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";

    buff.Append("Content-length: " + to_string(body.size()) + "\r\n\r\n");
    buff.Append(body);
}