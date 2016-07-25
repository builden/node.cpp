#include "fs-req-wrap.h"

namespace nodecpp {

  FSReqWrap::FSReqWrap(const char* syscall, const char* data, enum encoding encoding)
    : ReqWrap(AsyncWrap::PROVIDER_FSREQWRAP),
      encoding_(encoding),
      syscall_(syscall),
      data_(data) {

  }

  void FSReqWrap::Dispose() {
    this->~FSReqWrap();
    delete[] reinterpret_cast<char*>(this);
  }

  void After(uv_fs_t *req) {
    FSReqWrap* req_wrap = static_cast<FSReqWrap*>(req->data);
    CHECK_EQ(&req_wrap->req_, req);
    req_wrap->ReleaseEarly();  // Free memory that's no longer used now.

    // there is always at least one argument. "error"
    int argc = 1;

    // Allocate space for two args. We may only use one depending on the case.
    // (Feel free to increase this if you need more)
    // Local<Value> argv[2];
    // Local<Value> link;

    if (req->result < 0) {
      // An error happened.
    }
    else {
      // error value is empty or null for non-error.
      // argv[0] = Null(env->isolate());

      // All have at least two args now.
      argc = 2;

      switch (req->fs_type) {
        // These all have no data to pass.
      case UV_FS_ACCESS:
      case UV_FS_CLOSE:
      case UV_FS_RENAME:
      case UV_FS_UNLINK:
      case UV_FS_RMDIR:
      case UV_FS_MKDIR:
      case UV_FS_FTRUNCATE:
      case UV_FS_FSYNC:
      case UV_FS_FDATASYNC:
      case UV_FS_LINK:
      case UV_FS_SYMLINK:
      case UV_FS_CHMOD:
      case UV_FS_FCHMOD:
      case UV_FS_CHOWN:
      case UV_FS_FCHOWN:
        // These, however, don't.
        argc = 1;
        break;

      case UV_FS_UTIME:
      case UV_FS_FUTIME:
        argc = 0;
        break;

      case UV_FS_OPEN:
        // argv[1] = Integer::New(env->isolate(), req->result);
        break;

      case UV_FS_WRITE:
        // argv[1] = Integer::New(env->isolate(), req->result);
        break;

      case UV_FS_STAT:
      case UV_FS_LSTAT:
      case UV_FS_FSTAT:
/*
        argv[1] = BuildStatsObject(env,
          static_cast<const uv_stat_t*>(req->ptr));*/
        break;

      case UV_FS_MKDTEMP:
/*
        link = StringBytes::Encode(env->isolate(),
          static_cast<const char*>(req->path),
          req_wrap->encoding_);
        if (link.IsEmpty()) {
          argv[0] = UVException(env->isolate(),
            UV_EINVAL,
            req_wrap->syscall(),
            "Invalid character encoding for filename",
            req->path,
            req_wrap->data());
        }
        else {
          argv[1] = link;
        }*/
        break;

      case UV_FS_READLINK:
/*
        link = StringBytes::Encode(env->isolate(),
          static_cast<const char*>(req->ptr),
          req_wrap->encoding_);
        if (link.IsEmpty()) {
          argv[0] = UVException(env->isolate(),
            UV_EINVAL,
            req_wrap->syscall(),
            "Invalid character encoding for link",
            req->path,
            req_wrap->data());
        }
        else {
          argv[1] = link;
        }*/
        break;

      case UV_FS_REALPATH:
/*
        link = StringBytes::Encode(env->isolate(),
          static_cast<const char*>(req->ptr),
          req_wrap->encoding_);
        if (link.IsEmpty()) {
          argv[0] = UVException(env->isolate(),
            UV_EINVAL,
            req_wrap->syscall(),
            "Invalid character encoding for link",
            req->path,
            req_wrap->data());
        }
        else {
          argv[1] = link;
        }*/
        break;

      case UV_FS_READ:
        // Buffer interface
        // argv[1] = Integer::New(env->isolate(), req->result);
        break;

      case UV_FS_SCANDIR:
      {
/*
        int r;
        Local<Array> names = Array::New(env->isolate(), 0);
        Local<Function> fn = env->push_values_to_array_function();
        Local<Value> name_argv[NODE_PUSH_VAL_TO_ARRAY_MAX];
        size_t name_idx = 0;

        for (int i = 0; ; i++) {
          uv_dirent_t ent;

          r = uv_fs_scandir_next(req, &ent);
          if (r == UV_EOF)
            break;
          if (r != 0) {
            argv[0] = UVException(r,
              nullptr,
              req_wrap->syscall(),
              static_cast<const char*>(req->path));
            break;
          }

          Local<Value> filename = StringBytes::Encode(env->isolate(),
            ent.name,
            req_wrap->encoding_);
          if (filename.IsEmpty()) {
            argv[0] = UVException(env->isolate(),
              UV_EINVAL,
              req_wrap->syscall(),
              "Invalid character encoding for filename",
              req->path,
              req_wrap->data());
            break;
          }
          name_argv[name_idx++] = filename;

          if (name_idx >= arraysize(name_argv)) {
            fn->Call(env->context(), names, name_idx, name_argv)
              .ToLocalChecked();
            name_idx = 0;
          }
        }

        if (name_idx > 0) {
          fn->Call(env->context(), names, name_idx, name_argv)
            .ToLocalChecked();
        }

        argv[1] = names;*/
      }
      break;

      default:
        // CHECK(0 && "Unhandled eio response");
        break;
      }
    }

    // req_wrap->MakeCallback(env->oncomplete_string(), argc, argv);
    if (req_wrap->onComplete1) req_wrap->onComplete1(Error(req->result));

    uv_fs_req_cleanup(&req_wrap->req_);
    req_wrap->Dispose();
  }

  void Access(const string& path, int mode, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap != nullptr) {
      ASYNC_CALL(access, reqWrap, UTF8, path.c_str(), mode);
    }
    else {
      SYNC_CALL(access, path.c_str(), path.c_str(), mode);
    }
  }

}