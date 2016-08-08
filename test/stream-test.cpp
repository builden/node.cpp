#include "stdafx.h"
#include "stream-test.h"

TEST_F(StreamTest, readable) {
  auto r = new Readable();
  r->push(Buffer("hello"));
  r->push();

  r->on<const Buffer&>("data", [](const Buffer& buf) {
    cout << buf.toString() << endl;
  });

  r->on("end", []() {
    cout << "on end" << endl;
  });

  run();
}