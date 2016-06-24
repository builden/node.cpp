#include "stdafx.h"
#include "moment-test.h"
#include "nodecpp/moment.h"

TEST_F(MomentTest, constructor) {
  std::cout << "moment: " << moment().format() << std::endl;
}
