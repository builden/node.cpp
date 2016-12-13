#ifndef __NODEPP_DEF_H__
#define __NODEPP_DEF_H__

#include <stdint.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <iostream>

#include "nlohmann/json.hpp"
using nlohmann::json;

typedef std::set<int> iset_t;
typedef std::vector<int> ivec_t;
typedef std::vector<char> cvec_t;
typedef std::vector<uint8_t> ucvec_t;
typedef std::vector<std::string> svec_t;
typedef std::map<int, std::string> ismap_t;
typedef std::map<std::string, std::string> ssmap_t;

using std::string;
using std::wstring;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::function;
using std::cout;
using std::wcout;
using std::endl;
using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;
using std::placeholders::_6;
using std::placeholders::_7;
using std::placeholders::_8;
using std::placeholders::_9;

#endif // !__NODEPP_DEF_H__