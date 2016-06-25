#include "stdafx.h"
#include "moment-test.h"
#include "nodecpp/moment.h"

TEST_F(MomentTest, constructor) {
  cout << "moment: " << moment().format() << endl;
  EXPECT_EQ(Moment(1u).valueOf(), 1000);
  EXPECT_EQ(Moment(1u).unix(), 1);
  EXPECT_EQ(Moment(1u).format(), "1970-01-01 08:00:01.000");

  EXPECT_EQ(Moment(1ull).valueOf(), 1);
  EXPECT_EQ(Moment(1ull).unix(), 0);
  EXPECT_EQ(Moment(1000ull).unix(), 1);
  EXPECT_EQ(Moment(1ull).format(), "1970-01-01 08:00:00.001");

  EXPECT_EQ(Moment(1u, 1000000u).valueOf(), 1001);
  EXPECT_EQ(Moment(1u, 1000000u).format(), "1970-01-01 08:00:01.001");
}
