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

typedef std::set<int> iset_t;
typedef std::vector<int> ivec_t;
typedef std::vector<char> cvec_t;
typedef std::vector<std::string> svec_t;
typedef std::map<int, std::string> ismap_t;
typedef std::map<std::string, std::string> ssmap_t;

using std::string;
using std::wstring;
using std::unique_ptr;
using std::shared_ptr;
using std::function;

#endif // !__NODEPP_DEF_H__