#ifndef __KEYBOAR_CODE_TRANS_H__
#define __KEYBOAR_CODE_TRANS_H__

#include "nodecpp-def.h"
#include "keyboard-codes.h"

namespace nodecpp {
  using std::string;
  using std::vector;
  using KeyboardCodes = vector<KeyboardCode>;

  KeyboardCode keyboardCodeFromStr(const string& str, bool* shifted);
  KeyboardCodes keyboardCodesFromStr(const string& str, bool* shifted);

}

#endif // !__KEYBOAR_CODE_TRANS_H__
