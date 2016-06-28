#include "stdafx.h"
#include "dns-test.h"

TEST_F(DnsTest, lookup) {
  dns.lookup("www.qq.com", [](const Error& err, const string& ip) {
    EXPECT_FALSE(err);
    EXPECT_FALSE(ip.empty());
    if (err) return fmt::print("lookup error: {}", err.str());
    cout << "lookup: " << ip << endl;
  });

  dns.lookup("www.errorxxxx.com", [](const Error& err, const string& ip) {
    EXPECT_TRUE(err);
    EXPECT_EQ(err.str(), "unknown node or service");
  });

  run();
}

TEST_F(DnsTest, resolve) {
  dns.resolve("www.qq.com", [](const Error& err, const svec_t& addresses) {
    EXPECT_FALSE(err);
    EXPECT_GE(addresses.size(), 0u);
    for (auto& addr : addresses) {
      cout << "resolve: " << addr << endl;
    }
  });

  run();
}

TEST_F(DnsTest, reverse) {

}