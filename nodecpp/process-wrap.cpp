#include "process-wrap.h"
#include <uv.h>
#include "error.h"
#include "util.h"
#include "nodecpp-internals.h"

namespace nodecpp {
  /*
  class ProcessWrap::impl {
  public:
    int spawn(json options) {
      char* args[3];
      args[0] = R"(D:\builden-proj\dx-cef-test\deps\node.cpp\Debug\sub-process.exe)";
      args[1] = NULL;
      args[2] = NULL;

      memset(&options, 0, sizeof(uv_process_options_t));
      uv_pipe_init(uv_default_loop(), &out, 0);
      uv_pipe_init(uv_default_loop(), &errout, 0);

      // The convention is that stdio[0] points to stdin, fd 1 is used for stdout, and fd 2 is stderr.
      uv_stdio_container_t stdio[3];
      options.stdio = stdio;
      options.stdio[0].flags = UV_IGNORE;
      options.stdio[1].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
      options.stdio[1].data.stream = (uv_stream_t*)&out;
      options.stdio[2].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
      options.stdio[2].data.stream = (uv_stream_t*)&errout;
      options.stdio_count = 3;

      options.exit_cb = on_child_exit;
      options.file = args[0];
      options.args = args;

      int rst = uv_spawn(uv_default_loop(), &child_req, &options);
      if (rst < 0) {
        fprintf(stderr, "%s\n", Error(rst).str());
        return;
      }
      uv_read_start((uv_stream_t*)&out, newBuffer, readCb);
      uv_read_start((uv_stream_t*)&errout, newBuffer, readErrCb);
    }

  private:
    static void on_child_exit(uv_process_t* req, int64_t exit_status, int term_signal) {
      fprintf(stderr, "Process exited with status %lld, signal %d\n", exit_status, term_signal);
      uv_close((uv_handle_t*)req, NULL);
    }

    static void newBuffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t *buf) {
      buf->base = new char[suggested_size];
      buf->len = suggested_size;
    }

    static void readCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
      printf("read %li bytes in a %lu byte buffer\n", nread, buf->len);
      if (uint32_t(nread + 1) > (buf->len)) return;
      buf->base[nread] = '\0'; // turn it into a cstring
      printf("read: |%s|", buf->base);
    }

    static void readErrCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
      printf("read %li bytes in a %lu byte buffer\n", nread, buf->len);
      if (uint32_t(nread + 1) > (buf->len)) return;
      buf->base[nread] = '\0'; // turn it into a cstring
      printf("read: |%s|", buf->base);
    }

  private:
    uv_process_t child_req;
    uv_process_options_t options;
    uv_pipe_t out;
    uv_pipe_t errout;
    
  };*/

  ProcessWrap::ProcessWrap() 
    : HandleWrap(reinterpret_cast<uv_handle_t*>(&process_),
      AsyncWrap::PROVIDER_PROCESSWRAP) {}
  ProcessWrap::~ProcessWrap() {}

  int ProcessWrap::spawn(json jsOptions) {
    uv_process_options_t options;
    memset(&options, 0, sizeof(uv_process_options_t));

    options.exit_cb = OnExit;

    // options.file
    options.file = jsOptions["file"].get<string>().c_str();
    
    // options.args
    int argc = jsOptions["args"].size();
    // Heap allocate to detect errors. +1 is for nullptr.
    options.args = new char*[argc + 1];
    for (int i = 0; i < argc; ++i) {
      options.args[i] = _strdup(jsOptions["args"][i].get<string>().c_str());
      CHECK_NE(options.args[i], nullptr);
    }
    options.args[argc] = nullptr;

    // options.cwd
    if (jsOptions["cwd"].is_string()) {
      options.cwd = jsOptions["cwd"].get<string>().c_str();
    }

    // options.env
    int envc = jsOptions["env"].size();
    options.env = new char*[envc + 1];  // Heap allocated to detect errors.
    for (int i = 0; i < envc; i++) {
      options.env[i] = _strdup(jsOptions["args"][i].get<string>().c_str());
      CHECK_NE(options.env[i], nullptr);
    }
    options.env[envc] = nullptr;

    // options.stdio

    // options.windows_verbatim_arguments
    if (jsOptions["windows_verbatim_arguments"].is_boolean()
      && jsOptions["windows_verbatim_arguments"].get<bool>()) {
      options.flags |= UV_PROCESS_WINDOWS_VERBATIM_ARGUMENTS;
    }

    // options.detached
    if (jsOptions["detached"].is_boolean()
      && jsOptions["detached"].get<bool>()) {
      options.flags |= UV_PROCESS_DETACHED;
    }

    int err = uv_spawn(uv_default_loop(), &process_, &options);

    if (err == 0) {
      pid = process_.pid;
    }

    if (options.args) {
      for (int i = 0; options.args[i]; i++) free(options.args[i]);
      delete[] options.args;
    }

    if (options.env) {
      for (int i = 0; options.env[i]; i++) free(options.env[i]);
      delete[] options.env;
    }

    delete[] options.stdio;
    return err;
  }


  void ProcessWrap::OnExit(uv_process_t* handle, int64_t exit_status, int term_signal) {
    ProcessWrap* wrap = static_cast<ProcessWrap*>(handle->data);
    CHECK_NE(wrap, nullptr);
    CHECK_EQ(&wrap->process_, handle);

    if (wrap->exitCb) wrap->exitCb(exit_status, signo_string(term_signal));
  }

}