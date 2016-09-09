#include "stdafx.h"
#include "events-test.h"

TEST_F(EventsTest, listeners) {
  EventEmitter emitter;

  emitter.on("message", [] {
    std::cout << "on message" << std::endl;
  });

  emitter.on("param_int", [](int i) {
    std::cout << "on param_int" << std::endl;
    EXPECT_EQ(i, 1);
  });

  emitter.on("param_int", [](int i) {
    std::cout << "on param_int2" << std::endl;
    EXPECT_EQ(i, 1);
  });

  emitter.on("param_int_str", [](int i, const std::string& str) {
    std::cout << "on param_int_str" << std::endl;
    EXPECT_EQ(i, 2);
    EXPECT_EQ(str, "test msg");
  });

  emitter.emit("message"); 
  emitter.emit<int>("param_int", 1);
  emitter.emit<int, const std::string&>("param_int_str", 2, "test msg");
  emitter.removeAllListeners("param_int_str");
  emitter.emit<int, const std::string&>("param_int_str", 3, "can not on");
}

TEST_F(EventsTest, event_inherit) {
  class Child : public EventEmitter {
  public:
    void hello() {
      emit<const std::string&>("hello", "world");
    }
  };

  Child child;
  child.on("hello", [](const std::string& str) {
    std::cout << "on hello " << str << std::endl;
  });
  child.hello();
}