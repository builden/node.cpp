#include "stdafx.h"
#include "stream-test.h"

TEST_F(StreamTest, readable) {
  auto r = new Readable();
  r->push(Buffer("hello"));
  r->push(Buffer("world!"));
  r->push();

  r->on("data", [](const Buffer& buf) {
    cout << buf.toString() << endl;
  });

  r->on("end", []() {
    cout << "on end" << endl;
  });

  run();
}