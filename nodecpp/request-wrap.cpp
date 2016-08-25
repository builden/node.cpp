#include "request-wrap.h"
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
  class RequestWrap::impl {
  public:
    impl();
    void end(RequestWrapCb_t cb) {
      cb_ = cb;
      uv_async_init(uv_default_loop(), &async_, asyncProgress);
      uv_queue_work(uv_default_loop(), &req_, runRequest, afterRequest);
    }
  private:
    static void runRequest(uv_work_t *req);
    static void afterRequest(uv_work_t *req, int status);
    static string formatHeaders(const ssmap_t& headers);
    static void asyncProgress(uv_async_t *handle);

  public:
    uv_work_t req_;
    uv_async_t async_;
    string fullUrl_;
    string errorMsg_;
    ssmap_t headers_;
    ServerResponse res_;
    RequestWrapCb_t cb_ = nullptr;
    ProgressCb_t progressCb_ = nullptr;
    string method_;
    string ctx_;
    uint32_t currByte_ = 0;
    uint32_t totalByte_ = 0;
    svec_t cookieDatas_;
  };

 RequestWrap::impl::impl() {
   req_.data = this;
   headers_["User-Agent"] = "Node.Cpp/0.0.1";
   headers_["Connection"] = "close";
   headers_["Accept-Encoding"] = "gzip, deflate";
 }

  RequestWrap::RequestWrap() : pimpl(new impl) {

  }

  void RequestWrap::request(const string& method, const string& fullUrl) {
    pimpl->method_ = method;
    pimpl->fullUrl_ = fullUrl;
    if (method == "POST") pimpl->headers_["Content-Type"] = "application/json";
  }

  RequestWrap& RequestWrap::set(const string& headerKey, const string& headerValue) {
    pimpl->headers_[headerKey] = headerValue;
    return *this;
  }

  RequestWrap& RequestWrap::setCookie(const string& data) {
    pimpl->cookieDatas_.emplace_back(data);
    return *this;
  }

  RequestWrap& RequestWrap::accept(const string& type) {
    if (type == "application/x-www-form-urlencoded") {
      pimpl->headers_["Content-Type"] = type;
    }
    return *this;
  }

  RequestWrap& RequestWrap::send(json ctx) {
    if (pimpl->headers_["Content-Type"] == "application/x-www-form-urlencoded") {
      pimpl->ctx_ = qs.stringify(ctx);
    }
    else {
      pimpl->ctx_ = iconv.strToUtf8(ctx.dump());
    }
    return *this;
  }

  RequestWrap& RequestWrap::end(RequestWrapCb_t cb) {
    pimpl->end(cb);
    return *this;
  }

  RequestWrap& RequestWrap::onProgress(ProgressCb_t cb) {
    pimpl->progressCb_ = cb;
    return *this;
  }

  void RequestWrap::impl::runRequest(uv_work_t *req) {
    auto wrap = (RequestWrap::impl*)req->data;
    auto urlInfo = url.parse(wrap->fullUrl_);
    string hostUrl = urlInfo.protocol + "//" + urlInfo.hostname;
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

    for (auto& data : wrap->cookieDatas_) {
      InternetSetCookieA(hostUrl.c_str(), nullptr, data.c_str());
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
        auto key = s.toLower(s.strLeft(rawHeaders[i], ":"));
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

    if (statusCode < 200 || statusCode >= 300) {
      wrap->errorMsg_ = statusText;
      goto end;
    }

    if (wrap->res_.headers.find("content-length") != wrap->res_.headers.end())
      wrap->totalByte_ = std::stoi(wrap->res_.headers["content-length"]);

    char cReadBuffer[4096] = { 0 };
    while (true) {
      DWORD lNumberOfBytesRead = 0;
      BOOL bret = InternetReadFile(hRequest, cReadBuffer, sizeof(cReadBuffer) - 1, &lNumberOfBytesRead);
      if (!bret || !lNumberOfBytesRead)
        break;
      cReadBuffer[lNumberOfBytesRead] = 0;
      wrap->res_.data.append(cReadBuffer, lNumberOfBytesRead);

      wrap->currByte_ += lNumberOfBytesRead;

      wrap->async_.data = wrap;
      uv_async_send(&wrap->async_);
    }

    if (wrap->res_.headers.find("content-encoding") != wrap->res_.headers.end())
      contentEncoding = wrap->res_.headers["content-encoding"];
    if (s.includes(contentEncoding, "gzip") || s.includes(contentEncoding, "deflate"))
      wrap->res_.data = zlib.unzipSync(wrap->res_.data);

  end:
    if (hInternet) InternetCloseHandle(hInternet);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hRequest) InternetCloseHandle(hRequest);
  }

  void RequestWrap::impl::afterRequest(uv_work_t *req, int status) {
    auto wrap = (RequestWrap::impl*)req->data;
    if (wrap->cb_) {
      if (!wrap->errorMsg_.empty()) {
        wrap->cb_(Error(wrap->errorMsg_), wrap->res_);
      }
      else {
        wrap->cb_(Error(), wrap->res_);
      }
    }

    uv_close((uv_handle_t*)&wrap->async_, [](uv_handle_t* handle) {
      auto wrap = (RequestWrap::impl*)handle->data;
      delete wrap;
    });
  }

  string RequestWrap::impl::formatHeaders(const ssmap_t& headers) {
    string rst;
    for (auto& header : headers) {
      rst += header.first + ": " + header.second + "\r\n";
    }
    return rst;
  }

  void RequestWrap::impl::asyncProgress(uv_async_t *handle) {
    auto wrap = (RequestWrap::impl*)handle->data;
    if (wrap->progressCb_) wrap->progressCb_(wrap->currByte_, wrap->totalByte_);
  }

}
