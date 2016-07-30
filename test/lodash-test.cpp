#include "stdafx.h"
#include "lodash-test.h"

TEST_F(LodashTest, formatList) {
  int a[] = { 1, 2, 3, 4 };
  iset_t s = { 8, 9, 10, 11 };
  ivec_t v = { 9, 10, 11, 12 };
  EXPECT_EQ(_.formatList(a), "[1, 2, 3, 4]");
  EXPECT_EQ(_.formatList(s), "[8, 9, 10, 11]");
  EXPECT_EQ(_.formatList(v), "[9, 10, 11, 12]");

  ivec_t arr1 = { 1, 2, 3 };
  EXPECT_EQ(_.formatList(arr1), "[1, 2, 3]");

  ivec_t arr2 = { 1 };
  EXPECT_EQ(_.formatList(arr2), "[1]");

  ivec_t arr3;
  EXPECT_EQ(_.formatList(arr3), "[]");

  svec_t arr4 = { "abc", "bcd", "efg" };
  EXPECT_EQ(_.formatList(arr4, true), "[\"abc\", \"bcd\", \"efg\"]");
}

TEST_F(LodashTest, count) {
  int a[] = { 1, 2, 3, 4 };
  iset_t s = { 8, 9, 10, 11 };
  ivec_t v = { 9, 10, 11, 12 };

  EXPECT_EQ(_.count(a), 4);
  EXPECT_EQ(_.count(s), 4);
  EXPECT_EQ(_.count(v), 4);

  int rst1 = _.count(a, [&](int x) { return  x % 2 == 0; });
  EXPECT_EQ(rst1, 2);

  int rst2 = _.count(s, [&](int x) { return x == 9; });
  EXPECT_EQ(rst2, 1);

  int rst3 = _.count(v, [&](int x) { return x > 9; });
  EXPECT_EQ(rst3, 3);
}

TEST_F(LodashTest, map) {
  int a[] = { 1, 2, 3, 4 };
  iset_t s = { 8, 7, 6, 5 };
  ivec_t v = { 9, 10, 11, 12 };

  auto rst = _.map(s, [&](int x) { return -x; });
  EXPECT_EQ(_.formatList(rst), "[-5, -6, -7, -8]");
}

TEST_F(LodashTest, filter_reject) {
  int a[] = { 1, 2, 3, 4, 5 };
  auto rst = _.filter(a, [&](int x) { return (x % 2 == 1); });
  EXPECT_EQ(_.formatList(rst), "[1, 3, 5]");
  auto rst2 = _.reject(a, [&](int x) { return (x % 2 == 1); });
  EXPECT_EQ(_.formatList(rst2), "[2, 4]");
}

TEST_F(LodashTest, sample_shuffle) {
  int a[] = { 1, 2, 3, 4, 5 };
  auto rst = _.sample(a);
  auto rst2 = _.shuffle(a);
  std::cout << "sample: " << rst << "; shuffle " << _.formatList(rst2).c_str() << std::endl;
}

TEST_F(LodashTest, sort) {
  int a[] = { 1, 5, 2, 4, 5 };
  EXPECT_EQ(_.formatList(_.sort(a)), "[1, 2, 4, 5, 5]");
}

TEST_F(LodashTest, min_max) {
  // 包含了uv.h之后，min和max方法就无法推导出类型了
  // 还不知道为什么
  // int a[] = { 1, 2, 3, 4, 5 };
  ivec_t b = { 1, 2, 3, 4, 5 };
  EXPECT_EQ(_.min<ivec_t>(b), 1);
  EXPECT_EQ(_.max<ivec_t>(b), 5);
}

TEST_F(LodashTest, reduce) {
  int a[] = { 0, 1, 2, 3, 4 };
  auto rst = _.reduce(a, [&](int previou, int current) { return previou + current; });
  EXPECT_EQ(rst, 10);
  auto rst1 = _.reduce(a, 5, [&](int previou, int current) { return previou + current; });
  EXPECT_EQ(rst1, 15);
}

TEST_F(LodashTest, slice) {
  ivec_t ivec = { 1, 2, 3, 4, 5 };
  EXPECT_EQ(_.formatList(_.slice(ivec, 0, 1)), "[1]");
  EXPECT_EQ(_.formatList(_.slice(ivec, 1)), "[2, 3, 4, 5]");
  EXPECT_EQ(_.formatList(_.slice(ivec, 1, -1)), "[2, 3, 4]");
  EXPECT_EQ(_.formatList(_.slice(ivec, 1, -2)), "[2, 3]");
  EXPECT_EQ(_.formatList(_.slice(ivec, 3)), "[4, 5]");

  cvec_t cvec = { 'a', 'b', 'c', 'd' };
  EXPECT_EQ(_.formatList(_.slice(cvec, 3)), "[d]");
  EXPECT_EQ(_.formatList(_.slice(cvec, 2, -1)), "[c]");
  EXPECT_EQ(_.formatList(_.slice(cvec, 3, -1)), "[]");
  EXPECT_EQ(_.formatList(_.slice(cvec, 3, -2)), "[]");
  EXPECT_EQ(_.formatList(_.slice(cvec, 3, -3)), "[]");
  EXPECT_EQ(_.formatList(_.slice(cvec, 4)), "[]");
  EXPECT_EQ(_.formatList(_.slice(cvec, 5)), "[]");
}

TEST_F(LodashTest, indexOf) {
  ivec_t ivec = { 1, 2, 3, 4, 5 };
  EXPECT_EQ(_.indexOf(ivec, 2), 1);
  EXPECT_EQ(_.indexOf(ivec, 6), -1);
  EXPECT_EQ(_.indexOf(ivec, 2, 2), -1);

  svec_t svec = { "abc", "bcd", "cde" };
  EXPECT_EQ(_.indexOf(svec, "bcd"), 1);
  EXPECT_EQ(_.indexOf(svec, "bcde"), -1);
}

TEST_F(LodashTest, random) {
  for (int i = 0; i < 10; i++) {
    cout << "_.ramdon(0, 1): " << _.random() << "; _.random(1, 4): " << _.random(1, 4) << endl;
    int rand1 = _.random(-2, 2);
    EXPECT_LE(rand1, 2);
    EXPECT_GE(rand1, -2);
  } 
}