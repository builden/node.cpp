#include "stdafx.h"
#include "ini-test.h"
#include <nodecpp/ini.h>

string iniData = R"([section0]
key1=1
key2=abc
key3=true

[section1 blank]
key1=中文
key2=1
key3=true

; comment
[section2]
key1=efg
)";

TEST_F(IniTest, ini) {
  Ini ini;
  if (ini.loadData(iniData.c_str(), iniData.size())) {
    svec_t sections = ini.getAllSections();
//     for (auto section : sections) {
//       cout << section << endl;
//     }

    string testSec = "section1 blank";
    svec_t keys = ini.getAllKeys(testSec);
//     for (auto key : keys) {
//       cout << key << endl;
//     }

    
    EXPECT_EQ(ini.getString(testSec, "key1"), "中文");
    EXPECT_EQ(ini.getInt(testSec, "key2"), 1);
    EXPECT_EQ(ini.getBool(testSec, "key3"), true);

    EXPECT_EQ(ini.getString(testSec, "none"), "");
    EXPECT_EQ(ini.getInt(testSec, "key1"), 0);
    EXPECT_EQ(ini.getBool(testSec, "key1"), false);

    EXPECT_TRUE(ini.delSection("section2"));
    EXPECT_FALSE(ini.delSection("section3"));

    EXPECT_TRUE(ini.delKey("section0", "key1"));
    EXPECT_TRUE(ini.delSection(testSec));

    EXPECT_TRUE(ini.setValue("section0", "key2", "cde"));
    EXPECT_TRUE(ini.setValue("section0", "key4", "2"));

    string after = R"([section0]
key2 = cde
key3 = true
key4 = 2
)";
    after = s.replaceAll(after, "\n", "\r\n");
    EXPECT_EQ(ini.save(), after);

    // ini.saveFile("D:\\1.ini");
  }
}