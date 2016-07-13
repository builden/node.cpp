#include "stdafx.h"
#include "uv-thread-test.h"
#include <uv.h>

int argA = 2;
#define FIB_UNTIL 25
uv_work_t req[FIB_UNTIL];

TEST_F(UvThreadTest, create) {
  auto threadFunc = [](void* arg) {
    int a = *((int*)arg);
    EXPECT_EQ(a, 2);
    
    cout << "threadFunc end" << endl;
  };
  
  uv_thread_t threadId;
  uv_thread_create(&threadId, threadFunc, (void*)&argA);
  uv_thread_join(&threadId); // 同步暂停

  cout << "after uv_thread_join" << endl;
}

long fib_(long t) {
  if (t == 0 || t == 1)
    return 1;
  else
    return fib_(t - 1) + fib_(t - 2);
}

TEST_F(UvThreadTest, workQueue) {
  auto fib = [](uv_work_t *req) {
    int n = *(int *)req->data;
    if (2 == n) {
      for (int i = 0; i < FIB_UNTIL; i++) {
        uv_cancel((uv_req_t*)&req[i]); // 中途取消，但依然会执行after_fib
      }
    }
    long fib = fib_(n);
    fprintf(stderr, "%dth fibonacci is %lu\n", n, fib);
  };

  auto after_fib = [](uv_work_t *req, int status) {
    cout << "Done calculating " << *(int *)req->data << "th fibonacci; status " << status << endl;
    if (status == UV_ECANCELED) {
      cout << "Calculation of " << *(int *)req->data << " cancelled" << endl;
    }
  };

  int data[FIB_UNTIL] = { 0 };
  int i;
  for (i = 0; i < FIB_UNTIL; i++) {
    data[i] = i;
    req[i].data = (void *)&data[i];
    uv_queue_work(uv_default_loop(), &req[i], fib, after_fib);
  }

  run();
}

uv_async_t async;
TEST_F(UvThreadTest, progress) {
  auto fake_download = [](uv_work_t *req) {
    int size = *((int*)req->data);
    int downloaded = 0;
    double percentage;
    while (downloaded < size) {
      percentage = downloaded*100.0 / size;
      async.data = (void*)&percentage;
      uv_async_send(&async);

      Sleep(10);
      downloaded += 600; // can only download max 1000bytes/sec,
      // but at least a 200;
    }
  };

  auto after = [](uv_work_t *req, int status) {
    fprintf(stderr, "Download complete\n");
    uv_close((uv_handle_t*)&async, NULL);
  };

  auto print_progress = [](uv_async_t *handle) {
    double percentage = *((double*)handle->data);
    fprintf(stderr, "Downloaded %.2f%%\n", percentage);
  };

  uv_work_t req;
  int size = 10240;
  req.data = (void*)&size;

  uv_async_init(uv_default_loop(), &async, print_progress);
  uv_queue_work(uv_default_loop(), &req, fake_download, after);

  run();
}