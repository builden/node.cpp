#include "super-agent.h"
#include "process.h"
#include "url.h"
#include "buffer.h"
#include "fs.h"
#include "fmt/format.h"
#include <uv.h>
#include <windows.h>
#include <WinInet.h>
#pragma comment(lib, "wininet.lib")

namespace nodecpp {

  class RequestWrap {
  public:
    RequestWrap() {
      req_.data = this;
    }

    void get(const string& fullUrl) {
      fullUrl_ = fullUrl;
    }

    void set(const string& headerKey, const string& headerValue) {
      headers_[headerKey] = headerValue;
    }

    void end(SuperAgentCb_t cb) {
      cb_ = cb;
      uv_queue_work(uv_default_loop(), &req_, runRequest, afterRequest);
    }

    static void runRequest(uv_work_t *req) {
      auto wrap = (RequestWrap*)req->data;
      auto urlInfo = url.parse(wrap->fullUrl_);
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
      BOOL rst = HttpSendRequestA(hRequest, nullptr, 0, nullptr, 0);
      if (!rst) {
        wrap->errorMsg_ = fmt::format("RequestWrap HttpSendRequestA Error {}", GetLastError());
        goto end;
      }

      char tmpBuf[MAX_PATH] = { 0 };
      DWORD bufLen = MAX_PATH;
      if (!HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE, &tmpBuf, &bufLen, 0)) {
        int lastError = GetLastError();
      }

      DWORD ctxLen = 0, ctxLenSize = 4;
      if (!HttpQueryInfoA(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &ctxLen, &ctxLenSize, nullptr)) {
        int lastError = GetLastError();
      }

      char cReadBuffer[4096] = { 0 };
      while (true) {
        DWORD lNumberOfBytesRead = 0;
        BOOL bret = InternetReadFile(hRequest, cReadBuffer, sizeof(cReadBuffer) - 1, &lNumberOfBytesRead);
        if (!bret || !lNumberOfBytesRead)
          break;
        cReadBuffer[lNumberOfBytesRead] = 0;
        wrap->res_.data.append(cReadBuffer, lNumberOfBytesRead);
      }

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

  private:
    uv_work_t req_;
    string fullUrl_;
    string errorMsg_;
    ssmap_t headers_;
    ServerResponse res_;
    SuperAgentCb_t cb_ = nullptr;
  };

  class SuperAgent::impl {
  public:
    impl() {}
    void get(const string& fullUrl) {
      wrap_ = new RequestWrap();
      wrap_->get(fullUrl);
    }

    void set(const string& headerKey, const string& headerValue) {
      if (wrap_) wrap_->set(headerKey, headerValue);
    }

    void end(SuperAgentCb_t cb) {
      if (wrap_) wrap_->end(cb);
    }

    RequestWrap* wrap_ = nullptr;
  };

  SuperAgent::SuperAgent() : pimpl(new impl()) {
    
  }

  SuperAgent& SuperAgent::get(const string& fullUrl) {
    pimpl->get(fullUrl);
    return *this;
  }

  SuperAgent& SuperAgent::set(const string& headerKey, const string& headerValue) {
    return *this;
  }

  void SuperAgent::end(SuperAgentCb_t cb) {
    pimpl->end(cb);
  }

  SuperAgent& sa = SuperAgent::instance();
}