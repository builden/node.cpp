#include "stream-base.h"
#include "shutdown-wrap.h"
#include "write-wrap.h"

namespace nodecpp {
  bool StreamBase::IsIPCPipe() {
    return false;
  }

  int StreamBase::GetFD() {
    return -1;
  }


  int StreamBase::Shutdown() {
    ShutdownWrap* req_wrap = new ShutdownWrap(
      this,
      AfterShutdown);

    int err = DoShutdown(req_wrap);
    if (err)
      delete req_wrap;
    return err;
  }


  int StreamBase::WriteBuffer(const Buffer& buffer) {
    const char* data = buffer.data();
    size_t length = buffer.size();

    WriteWrap* req_wrap;
    uv_buf_t buf;
    buf.base = const_cast<char*>(data);
    buf.len = length;

    // Try writing immediately without allocation
    uv_buf_t* bufs = &buf;
    size_t count = 1;
    int err = DoTryWrite(&bufs, &count);
    if (err != 0)
      goto done;
    if (count == 0)
      goto done;
    CHECK_EQ(count, 1);

    // Allocate, or write rest
    req_wrap = WriteWrap::New(this, AfterWrite);

    err = DoWrite(req_wrap, bufs, count, nullptr);
    // req_wrap_obj->Set(env->async(), True(env->isolate()));

    if (err)
      req_wrap->Dispose();

  done:
    const char* msg = Error();
    if (msg != nullptr) {
      // req_wrap_obj->Set(env->error_string(), OneByteString(env->isolate(), msg));
      ClearError();
    }
    return err;
  }

  void StreamBase::AfterShutdown(ShutdownWrap* req_wrap, int /*status*/) {
/*
    StreamBase* wrap = req_wrap->wrap();
    Environment* env = req_wrap->env();

    // The wrap and request objects should still be there.
    CHECK_EQ(req_wrap->persistent().IsEmpty(), false);

    HandleScope handle_scope(env->isolate());
    Context::Scope context_scope(env->context());

    Local<Object> req_wrap_obj = req_wrap->object();
    Local<Value> argv[3] = {
      Integer::New(env->isolate(), status),
      wrap->GetObject(),
      req_wrap_obj
    };

    if (req_wrap->object()->Has(env->context(),
      env->oncomplete_string()).FromJust()) {
      req_wrap->MakeCallback(env->oncomplete_string(), arraysize(argv), argv);
    }*/

    delete req_wrap;
  }

  void StreamBase::AfterWrite(WriteWrap* req_wrap, int /*status*/) {
    StreamBase* wrap = req_wrap->wrap();

    wrap->OnAfterWrite(req_wrap);

/*
    Local<Value> argv[] = {
      Integer::New(env->isolate(), status),
      wrap->GetObject(),
      req_wrap_obj,
      Undefined(env->isolate())
    };

    const char* msg = wrap->Error();
    if (msg != nullptr) {
      argv[3] = OneByteString(env->isolate(), msg);
      wrap->ClearError();
    }

    if (req_wrap->object()->Has(env->context(),
      env->oncomplete_string()).FromJust()) {
      req_wrap->MakeCallback(env->oncomplete_string(), arraysize(argv), argv);
    }*/

    req_wrap->Dispose();
  }
}
