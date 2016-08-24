#include "super-agent.h"
#include "process.h"
#include "url.h"
#include "buffer.h"
#include "fs.h"
#include "fmt/format.h"
#include "node-zlib.h"
#include "underscore.string.h"
#include "iconv.h"
#include "querystring.h"
#include <uv.h>
#include <windows.h>
#include <WinInet.h>
#pragma comment(lib, "wininet.lib")

namespace nodecpp {

  class RequestWrap {
  public:
    RequestWrap() {
      req_.data = this;
      headers_["User-Agent"] = "Node.Cpp/0.0.1";
      headers_["Connection"] = "close";
      headers_["Accept-Encoding"] = "gzip, deflate";
    }

    void request(const string& method, const string& fullUrl) {
      method_ = method;
      fullUrl_ = fullUrl;
      if (method == "POST") headers_["Content-Type"] = "application/json";
    }

    void set(const string& headerKey, const string& headerValue) {
      headers_[headerKey] = headerValue;
    }

    void accept(const string& type) {
      if (type == "application/x-www-form-urlencoded") {
        headers_["Content-Type"] = type;
      }
    }

    void send(json ctx) {
      if (headers_["Content-Type"] == "application/x-www-form-urlencoded") {
        ctx_ = qs.stringify(ctx);
      }
      else {
        ctx_ = iconv.strToUtf8(ctx.dump());
      }
    }

    void end(SuperAgentCb_t cb) {
      cb_ = cb;
      uv_queue_work(uv_default_loop(), &req_, runRequest, afterRequest);
    }

    static void runRequest(uv_work_t *req) {
      auto wrap = (RequestWrap*)req->data;
      auto urlInfo = url.parse(wrap->fullUrl_);
      auto headers = wrap->formatHeaders(wrap->headers_);
      svec_t rawHeaders;
      string contentEncoding;

      HINTERNET hInternet = NULL, hConnect = NULL, hRequest = NULL;
      Buffer buf;
      hInternet = InternetOpenA(nullptr, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
      if (!hInternet) {
        wrap->errorMsg_ = fmt::format("RequestWrap InternetOpenA Error {}", GetLastError());
        goto end;
      }

      int port = 80;
      if (!urlInfo.port.empty()) port = std::stoi(urlInfo.port);
      hConnect = InternetConnectA(hInternet, urlInfo.hostname.c_str(), port, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
      if (!hConnect) {
        wrap->errorMsg_ = fmt::format("RequestWrap InternetConnectA Error {}", GetLastError());
        goto end;
      }

      DWORD dwFlag = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD;
      hRequest = HttpOpenRequestA(hConnect, "GET", urlInfo.path.c_str(), "HTTP/1.1", nullptr, nullptr, dwFlag, 0);
      if (!hRequest) {
        wrap->errorMsg_ = fmt::format("RequestWrap HttpOpenRequestA Error {}", GetLastError());
        goto end;
      }

      // 第二个参数设置消息头
      // 第四个参数设置接收的文件类型, post数据
      BOOL rst = HttpSendRequestA(hRequest, headers.c_str(), headers.length(), (LPVOID)wrap->ctx_.c_str(), wrap->ctx_.length());
      if (!rst) {
        wrap->errorMsg_ = fmt::format("RequestWrap HttpSendRequestA Error {}", GetLastError());
        goto end;
      }

      char headerBuf[2048] = { 0 };
      DWORD headerBufLen = 2048;
      if (!HttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, &headerBuf, &headerBufLen, 0)) {
        wrap->errorMsg_ = fmt::format("RequestWrap HttpQueryInfoA Error {}", GetLastError());
        goto end;
      }
      rawHeaders = s.split(headerBuf, "\r\n");
      for (size_t i = 1; i < rawHeaders.size(); ++i) {
        if (!rawHeaders.empty()) {
          auto key = s.strLeft(rawHeaders[i], ":");
          if (!key.empty()) {
            wrap->res_.headers[key] = s.trim(s.strRight(rawHeaders[i], ":"));
          }
        }
      }

      DWORD statusCode = 0, statusCodeSize = 4;
      if (!HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, nullptr)) {
        wrap->errorMsg_ = fmt::format("RequestWrap HttpQueryInfoA HTTP_QUERY_STATUS_CODE Error {}", GetLastError());
        goto end;
      }

      wrap->res_.statusCode = statusCode;

      char statusText[MAX_PATH] = { 0 };
      DWORD statusTextLen = MAX_PATH;
      if (!HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_TEXT, &statusText, &statusTextLen, 0)) {
        wrap->errorMsg_ = fmt::format("RequestWrap HttpQueryInfoA HTTP_QUERY_STATUS_TEXT Error {}", GetLastError());
      }
      wrap->res_.statusMessage = statusText;

      char cReadBuffer[4096] = { 0 };
      while (true) {
        DWORD lNumberOfBytesRead = 0;
        BOOL bret = InternetReadFile(hRequest, cReadBuffer, sizeof(cReadBuffer) - 1, &lNumberOfBytesRead);
        if (!bret || !lNumberOfBytesRead)
          break;
        cReadBuffer[lNumberOfBytesRead] = 0;
        wrap->res_.data.append(cReadBuffer, lNumberOfBytesRead);
      }

      if (wrap->res_.headers.find("Content-Encoding") != wrap->res_.headers.end())
        contentEncoding = wrap->res_.headers["Content-Encoding"];
      if (s.includes(contentEncoding, "gzip") || s.includes(contentEncoding, "deflate"))
        wrap->res_.data = zlib.unzipSync(wrap->res_.data);
    
    end:
      if (hInternet) InternetCloseHandle(hInternet);
      if (hConnect) InternetCloseHandle(hConnect);
      if (hRequest) InternetCloseHandle(hRequest);
    }

    static void afterRequest(uv_work_t *req, int status) {
      auto wrap = (RequestWrap*)req->data;
      if (wrap->cb_) {
        if (!wrap->errorMsg_.empty()) {
          wrap->cb_(Error(wrap->errorMsg_), wrap->res_);
        }
        else {
          wrap->cb_(Error(), wrap->res_);
        }
      }
        
      delete wrap;
    }

    static string formatHeaders(const ssmap_t& headers) {
      string rst;
      for (auto& header : headers) {
        rst += header.first + ": " + header.second + "\r\n";
      }
      return rst;
    }

  private:
    uv_work_t req_;
    string fullUrl_;
    string errorMsg_;
    ssmap_t headers_;
    ServerResponse res_;
    SuperAgentCb_t cb_ = nullptr;
    string method_;
    string ctx_;
  };

  class SuperAgent::impl {
  public:
    impl() {}
    void request(const string& method, const string& fullUrl) {
      wrap_ = new RequestWrap();
      wrap_->request(method, fullUrl);
    }

    void set(const string& headerKey, const string& headerValue) {
      if (wrap_) wrap_->set(headerKey, headerValue);
    }

    void accept(const string& type) {
      if (wrap_) wrap_->accept(type);
    }

    void send(json ctx) {
      if (wrap_) wrap_->send(ctx);
    }

    void end(SuperAgentCb_t cb) {
      if (wrap_) wrap_->end(cb);
    }

    RequestWrap* wrap_ = nullptr;
  };

  SuperAgent::SuperAgent() : pimpl(new impl()) {
    
  }

  SuperAgent& SuperAgent::request(const string& method, const string& fullUrl) {
    pimpl->request(method, fullUrl);
    return *this;
  }

  SuperAgent& SuperAgent::get(const string& fullUrl) {
    return request("GET", fullUrl);
  }

  SuperAgent& SuperAgent::post(const string& fullUrl) {
    return request("POST", fullUrl);
  }

  SuperAgent& SuperAgent::set(const string& headerKey, const string& headerValue) {
    pimpl->set(headerKey, headerValue);
    return *this;
  }

  SuperAgent& SuperAgent::accept(const string& type) {
    pimpl->accept(type);
    return *this;
  }

  SuperAgent& SuperAgent::send(json ctx) {
    pimpl->send(ctx);
    return *this;
  }

  void SuperAgent::end(SuperAgentCb_t cb) {
    pimpl->end(cb);
  }

  SuperAgent& sa = SuperAgent::instance();
}