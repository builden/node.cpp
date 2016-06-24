#include "stdafx.h"
#include "json11-test.h"

TEST_F(Json11Test, constructor) {
  Json j = Json::object{
    { "a", "b" }
  };

  fmt::print("{}\n", j.dump());
}