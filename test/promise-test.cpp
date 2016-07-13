#include "stdafx.h"
#include "promise-test.h"
#include <thread>
#include <future>

TEST_F(PromiseTest, threadSync) {
  auto threadFunc = [] {
    for (int i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 线程休眠
      cout << "threadFunc: " << i << endl;
    }
  };
  std::thread t(threadFunc); // 动态参数，和线程函数一致
  cout << "thread begin" << endl; // 线程这个时候已经启动
  t.join(); // join函数会阻塞，直到threadFunc函数退出，如果线程函数有返回值，会被忽略
  // t.detach(); // 非阻塞，但会对线程失去控制
  cout << "thread end" << endl;
}

TEST_F(PromiseTest, threadMultiArgs) {
  auto threadFunc = [](int i, bool b, const string& s) {
    EXPECT_EQ(i, 1);
    EXPECT_TRUE(b);
    EXPECT_EQ(s, "test");
  };

  std::thread t(threadFunc, 1, true, "test");
  cout << "thead id: " << t.get_id() << endl; // 线程ID
  cout << "cpu number: " << t.hardware_concurrency() << endl; // CPU核数
  t.join();
}

// std::future 可以方便的获取线程函数的返回值
// std::promise 用来包装一个值，将数据和future绑定，方便线程赋值
// std::package_task 用来包装一个可调用对象，将函数和future绑定起来，以便异步调用
TEST_F(PromiseTest, future) {
  auto printInt = [](std::future<int>& fut) {
    int x = fut.get(); // 获取共享状态的值.
    std::cout << "value: " << x << '\n'; // 打印 value: 10.
  };
  std::promise<int> prom; // 生成一个 std::promise<int> 对象.
  std::future<int> fut = prom.get_future(); // 和 future 关联.
  std::thread t(printInt, std::ref(fut)); // 将 future 交给另外一个线程t.
  prom.set_value(10); // 设置共享状态的值, 此处和线程t保持同步.
  t.join();
}

TEST_F(PromiseTest, async) {
  std::future<int> f1 = std::async(std::launch::async, []() {
    return 8;
  });
  EXPECT_EQ(f1.get(), 8);

  std::future<int> f2 = std::async(std::launch::async, []() {
    cout << 8 << endl;
    return 2;
  });
  f2.wait();

  std::future<int> f3 = std::async(std::launch::async, []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    return 8;
  });

  std::future_status status;
  do {
    status = f3.wait_for(std::chrono::milliseconds(100));
    if (status == std::future_status::deferred) {
      cout << "deferred" << endl;
    }
    else if (status == std::future_status::timeout) {
      cout << "timeout" << endl;
    }
    else if (status == std::future_status::ready) {
      cout << "ready" << endl;
    }
  } while (status != std::future_status::ready);

  cout << f3.get() << endl;
}