#include "stdafx.h"
#include "promise-test.h"
#include <thread>
#include <future>

TEST_F(PromiseTest, threadSync) {
  auto threadFunc = [] {
    for (int i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); // �߳�����
      cout << "threadFunc: " << i << endl;
    }
  };
  std::thread t(threadFunc); // ��̬���������̺߳���һ��
  cout << "thread begin" << endl; // �߳����ʱ���Ѿ�����
  t.join(); // join������������ֱ��threadFunc�����˳�������̺߳����з���ֵ���ᱻ����
  // t.detach(); // ��������������߳�ʧȥ����
  cout << "thread end" << endl;
}

TEST_F(PromiseTest, threadMultiArgs) {
  auto threadFunc = [](int i, bool b, const string& s) {
    EXPECT_EQ(i, 1);
    EXPECT_TRUE(b);
    EXPECT_EQ(s, "test");
  };

  std::thread t(threadFunc, 1, true, "test");
  cout << "thead id: " << t.get_id() << endl; // �߳�ID
  cout << "cpu number: " << t.hardware_concurrency() << endl; // CPU����
  t.join();
}

// std::future ���Է���Ļ�ȡ�̺߳����ķ���ֵ
// std::promise ������װһ��ֵ�������ݺ�future�󶨣������̸߳�ֵ
// std::package_task ������װһ���ɵ��ö��󣬽�������future���������Ա��첽����
TEST_F(PromiseTest, future) {
  auto printInt = [](std::future<int>& fut) {
    int x = fut.get(); // ��ȡ����״̬��ֵ.
    std::cout << "value: " << x << '\n'; // ��ӡ value: 10.
  };
  std::promise<int> prom; // ����һ�� std::promise<int> ����.
  std::future<int> fut = prom.get_future(); // �� future ����.
  std::thread t(printInt, std::ref(fut)); // �� future ��������һ���߳�t.
  prom.set_value(10); // ���ù���״̬��ֵ, �˴����߳�t����ͬ��.
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