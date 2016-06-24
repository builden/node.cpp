#include "stdafx.h"
#include "core-math-test.h"

TEST_F(MathTest, min_max) {
  // 包含了uv.h头文件后，min和max方法就无法推导出类型了
  EXPECT_EQ(Math.min<int>(1, 2), 1);
  EXPECT_EQ(Math.min<int>(1, 1), 1);
  EXPECT_EQ(Math.max<int>(1, 2), 2);
  EXPECT_EQ(Math.max<int>(1, 1), 1);
}

TEST_F(MathTest, floor_ceil_round) {
  EXPECT_EQ(Math.floor(1.1), 1);
  EXPECT_EQ(Math.floor(1.9), 1);
  EXPECT_EQ(Math.ceil(1.1), 2);
  EXPECT_EQ(Math.ceil(1.9), 2);
  EXPECT_EQ(Math.round(1.1), 1);
  EXPECT_EQ(Math.round(1.499), 1);
  EXPECT_EQ(Math.round(1.501), 2);
  EXPECT_EQ(Math.round(1.9), 2);

  EXPECT_EQ(Math.ceil(0), 0);
  EXPECT_EQ(Math.floor(0), 0);
}
