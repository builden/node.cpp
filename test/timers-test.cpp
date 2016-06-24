#include "stdafx.h"
#include "timers-test.h"
#include "nodecpp/timers.h"

TEST_F(TimersTest, setTimeout) {
  int a = 10;
  setTimeout([&] {
    std::cout << "on setTimeout callback" << std::endl;
    EXPECT_EQ(a, 10);
  }, 10);

  run();
}

TEST_F(TimersTest, clearTimeout) {
  int timer = setTimeout([] {
    std::cout << "on clearTimeout callback" << std::endl;
  }, 10);

  clearTimeout(timer);
  run();
}

TEST_F(TimersTest, setInterval) {
  static int count = 0;
  int timer = setInterval([&] {
    std::cout << "on setInterval callback " << count << std::endl;
    if (count++ == 5) {
      clearInterval(timer);
    }
  }, 10);
  run();
}
