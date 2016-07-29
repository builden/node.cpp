#include "keyboard-code-trans.h"
#include "underscore.string.h"
#include "fmt/format.h"

namespace nodecpp {
  // Return key code of the char, and also determine whether the SHIFT key is
  // pressed.
  KeyboardCode KeyboardCodeFromCharCode(char c, bool* shifted) {
    c = char(tolower(c));
    *shifted = false;
    switch (c) {
    case 0x08: return VKEY_BACK;
    case 0x7F: return VKEY_DELETE;
    case 0x09: return VKEY_TAB;
    case 0x0D: return VKEY_RETURN;
    case 0x1B: return VKEY_ESCAPE;
    case ' ': return VKEY_SPACE;

    case 'a': return VKEY_A;
    case 'b': return VKEY_B;
    case 'c': return VKEY_C;
    case 'd': return VKEY_D;
    case 'e': return VKEY_E;
    case 'f': return VKEY_F;
    case 'g': return VKEY_G;
    case 'h': return VKEY_H;
    case 'i': return VKEY_I;
    case 'j': return VKEY_J;
    case 'k': return VKEY_K;
    case 'l': return VKEY_L;
    case 'm': return VKEY_M;
    case 'n': return VKEY_N;
    case 'o': return VKEY_O;
    case 'p': return VKEY_P;
    case 'q': return VKEY_Q;
    case 'r': return VKEY_R;
    case 's': return VKEY_S;
    case 't': return VKEY_T;
    case 'u': return VKEY_U;
    case 'v': return VKEY_V;
    case 'w': return VKEY_W;
    case 'x': return VKEY_X;
    case 'y': return VKEY_Y;
    case 'z': return VKEY_Z;

    case ')': *shifted = true; case '0': return VKEY_0;
    case '!': *shifted = true; case '1': return VKEY_1;
    case '@': *shifted = true; case '2': return VKEY_2;
    case '#': *shifted = true; case '3': return VKEY_3;
    case '$': *shifted = true; case '4': return VKEY_4;
    case '%': *shifted = true; case '5': return VKEY_5;
    case '^': *shifted = true; case '6': return VKEY_6;
    case '&': *shifted = true; case '7': return VKEY_7;
    case '*': *shifted = true; case '8': return VKEY_8;
    case '(': *shifted = true; case '9': return VKEY_9;

    case ':': *shifted = true; case ';': return VKEY_OEM_1;
    case '+': *shifted = true; case '=': return VKEY_OEM_PLUS;
    case '<': *shifted = true; case ',': return VKEY_OEM_COMMA;
    case '_': *shifted = true; case '-': return VKEY_OEM_MINUS;
    case '>': *shifted = true; case '.': return VKEY_OEM_PERIOD;
    case '?': *shifted = true; case '/': return VKEY_OEM_2;
    case '~': *shifted = true; case '`': return VKEY_OEM_3;
    case '{': *shifted = true; case '[': return VKEY_OEM_4;
    case '|': *shifted = true; case '\\': return VKEY_OEM_5;
    case '}': *shifted = true; case ']': return VKEY_OEM_6;
    case '"': *shifted = true; case '\'': return VKEY_OEM_7;

    default: return VKEY_UNKNOWN;
    }
  }

  // Return key code represented by |str|.
  KeyboardCode KeyboardCodeFromKeyIdentifier(const std::string& ident,
    bool* shifted) {
    std::string str = s.toLower(ident);
    if (str == "ctrl" || str == "control") {
      return VKEY_CONTROL;
    }
    else if (str == "super" || str == "cmd" || str == "command" ||
      str == "meta") {
      return VKEY_COMMAND;
    }
    else if (str == "commandorcontrol" || str == "cmdorctrl") {
#if defined(OS_MACOSX)
      return VKEY_COMMAND;
#else
      return VKEY_CONTROL;
#endif
    }
    else if (str == "alt" || str == "option") {
      return VKEY_MENU;
    }
    else if (str == "shift") {
      return VKEY_SHIFT;
    }
//     else if (str == "altgr") {
//       return VKEY_ALTGR;
//     }
    else if (str == "plus") {
      *shifted = true;
      return VKEY_OEM_PLUS;
    }
    else if (str == "tab") {
      return VKEY_TAB;
    }
    else if (str == "space") {
      return VKEY_SPACE;
    }
    else if (str == "backspace") {
      return VKEY_BACK;
    }
    else if (str == "delete") {
      return VKEY_DELETE;
    }
    else if (str == "insert") {
      return VKEY_INSERT;
    }
    else if (str == "enter" || str == "return") {
      return VKEY_RETURN;
    }
    else if (str == "up") {
      return VKEY_UP;
    }
    else if (str == "down") {
      return VKEY_DOWN;
    }
    else if (str == "left") {
      return VKEY_LEFT;
    }
    else if (str == "right") {
      return VKEY_RIGHT;
    }
    else if (str == "home") {
      return VKEY_HOME;
    }
    else if (str == "end") {
      return VKEY_END;
    }
    else if (str == "pageup") {
      return VKEY_PRIOR;
    }
    else if (str == "pagedown") {
      return VKEY_NEXT;
    }
    else if (str == "esc" || str == "escape") {
      return VKEY_ESCAPE;
    }
    else if (str == "volumemute") {
      return VKEY_VOLUME_MUTE;
    }
    else if (str == "volumeup") {
      return VKEY_VOLUME_UP;
    }
    else if (str == "volumedown") {
      return VKEY_VOLUME_DOWN;
    }
    else if (str == "medianexttrack") {
      return VKEY_MEDIA_NEXT_TRACK;
    }
    else if (str == "mediaprevioustrack") {
      return VKEY_MEDIA_PREV_TRACK;
    }
    else if (str == "mediastop") {
      return VKEY_MEDIA_STOP;
    }
    else if (str == "mediaplaypause") {
      return VKEY_MEDIA_PLAY_PAUSE;
    }
    else if (str == "printscreen") {
      return VKEY_SNAPSHOT;
    }
    else if (str.size() > 1 && str[0] == 'f') {
      // F1 - F24.
      try {
        int n = std::stoi(str.c_str() + 1);
        if (n > 0 && n < 25) {
          return static_cast<KeyboardCode>(VKEY_F1 + n - 1);
        }
        else {
          // LOG(WARNING) << str << "is not available on keyboard";
          return VKEY_UNKNOWN;
        }
      }
      catch (std::invalid_argument&) {
        return VKEY_UNKNOWN;
      }
    }
    else {
      // LOG(WARNING) << "Invalid accelerator token: " << str;
      return VKEY_UNKNOWN;
    }
  }

  KeyboardCode keyboardCodeFromStr(const string& str, bool* shifted) {
    if (str.size() == 1)
      return KeyboardCodeFromCharCode(str[0], shifted);
    else
      return KeyboardCodeFromKeyIdentifier(str, shifted);
  }

  KeyboardCodes keyboardCodesFromStr(const string& str, bool* shifted) {
    svec_t svec = s.split(str, "+");
    KeyboardCodes codes;
    for (auto token : svec) {
      token = s.trim(token);
      bool oneShifted = false;
      if (!token.empty()) {
        codes.emplace_back(keyboardCodeFromStr(token, &oneShifted));
        if (oneShifted) *shifted = oneShifted;
      }
    }
    return codes;
  }

  string virtKeyToStr(int virkCode) {
    switch (virkCode) {
    case VK_LBUTTON:  return "VKEY_LBUTTON";
    case VK_RBUTTON:  return "VKEY_RBUTTON";
    case VK_CANCEL:   return "VKEY_CANCEL";
    case VK_MBUTTON:  return "VKEY_MBUTTON";
    case VK_XBUTTON1: return "VKEY_XBUTTON1";
    case VK_XBUTTON2: return "VKEY_XBUTTON2";
    case 0x07:        return "unassigned 0x07";
    case VK_BACK:     return "VKEY_BACK";
    case VK_TAB:      return "VKEY_TAB";
    case 0x0A:        return "reserved 0x0A";
    case 0x0B:        return "reserved 0x0B";
    case VK_CLEAR:    return "VKEY_CLEAR";
    case VK_RETURN:   return "VKEY_RETURN";
    case 0x0E:        return "reserved 0x0E";
    case 0x0F:        return "reserved 0x0F";
    case VK_SHIFT:    return "VKEY_SHIFT";
    case VK_CONTROL:  return "VKEY_CONTROL";
    case VK_MENU:     return "VKEY_MENU";
    case VK_PAUSE:    return "VKEY_PAUSE";
    case VK_CAPITAL:  return "VKEY_CAPITAL";
    case VK_KANA:     return "VKEY_KANA";
    case VK_JUNJA:    return "VKEY_JUNJA";
    case VK_FINAL:    return "VKEY_FINAL";
    case VK_KANJI:    return "VKEY_KANJI";
    case 0x1A:        return "reserved 0x1A";
    case VK_ESCAPE:   return "VKEY_ESCAPE";
    case VK_CONVERT:  return "VKEY_CONVERT";
    case VK_NONCONVERT: return "VEKY_NONCONVERT";
    case VK_ACCEPT:   return "VKEY_ACCEPT";
    case VK_MODECHANGE: return "VKEY_MODECHANGE";
    case VK_SPACE:    return "VKEY_SPACE";
    case VK_PRIOR:    return "VKEY_PRIOR";
    case VK_NEXT:     return "VKEY_NEXT";
    case VK_END:      return "VKEY_END";
    case VK_HOME:     return "VKEY_HOME";
    case VK_LEFT:     return "VKEY_LEFT";
    case VK_UP:       return "VKEY_UP";
    case VK_RIGHT:    return "VKEY_RIGHT";
    case VK_DOWN:     return "VKEY_DOWN";
    case VK_SELECT:   return "VKEY_SELECT";
    case VK_PRINT:    return "VKEY_PRINT";
    case VK_SNAPSHOT: return "VKEY_SNAPSHOT";
    case VK_INSERT:   return "VKEY_INSERT";
    case VK_DELETE:   return "VKEY_DELETE";
    case VK_HELP:     return "VKEY_HELP";
    case 0x30:        return "VKEY_0";
    case 0x31:        return "VKEY_1";
    case 0x32:        return "VKEY_2";
    case 0x33:        return "VKEY_3";
    case 0x34:        return "VKEY_4";
    case 0x35:        return "VKEY_5";
    case 0x36:        return "VKEY_6";
    case 0x37:        return "VKEY_7";
    case 0x38:        return "VKEY_8";
    case 0x39:        return "VKEY_9";
    case 0x3A:        return "reserved 0x3A";
    case 0x3B:        return "reserved 0x3B";
    case 0x3C:        return "reserved 0x3C";
    case 0x3D:        return "reserved 0x3D";
    case 0x3E:        return "reserved 0x3E";
    case 0x3F:        return "reserved 0x3F";
    case 0x40:        return "unassigned 0x40";
    case 0x41:        return "VKEY_A";
    case 0x42:        return "VKEY_B";
    case 0x43:        return "VKEY_C";
    case 0x44:        return "VKEY_D";
    case 0x45:        return "VKEY_E";
    case 0x46:        return "VKEY_F";
    case 0x47:        return "VKEY_G";
    case 0x48:        return "VKEY_H";
    case 0x49:        return "VKEY_I";
    case 0x4A:        return "VKEY_J";
    case 0x4B:        return "VKEY_K";
    case 0x4C:        return "VKEY_L";
    case 0x4D:        return "VKEY_M";
    case 0x4E:        return "VKEY_N";
    case 0x4F:        return "VKEY_O";
    case 0x50:        return "VKEY_P";
    case 0x51:        return "VKEY_Q";
    case 0x52:        return "VKEY_R";
    case 0x53:        return "VKEY_S";
    case 0x54:        return "VKEY_T";
    case 0x55:        return "VKEY_U";
    case 0x56:        return "VKEY_V";
    case 0x57:        return "VKEY_W";
    case 0x58:        return "VKEY_X";
    case 0x59:        return "VKEY_Y";
    case 0x5A:        return "VKEY_Z";
    case VK_LWIN:     return "VKEY_LWIN";
    case VK_RWIN:     return "VKEY_RWIN";
    case VK_APPS:     return "VKEY_APPS";
    case 0x5E:        return "reserved 0x5E";
    case VK_SLEEP:    return "VKEY_SLEEP";
    case VK_NUMPAD0:  return "VKEY_NUMPAD0";
    case VK_NUMPAD1:  return "VKEY_NUMPAD1";
    case VK_NUMPAD2:  return "VKEY_NUMPAD2";
    case VK_NUMPAD3:  return "VKEY_NUMPAD3";
    case VK_NUMPAD4:  return "VKEY_NUMPAD4";
    case VK_NUMPAD5:  return "VKEY_NUMPAD5";
    case VK_NUMPAD6:  return "VKEY_NUMPAD6";
    case VK_NUMPAD7:  return "VKEY_NUMPAD7";
    case VK_NUMPAD8:  return "VKEY_NUMPAD8";
    case VK_NUMPAD9:  return "VKEY_NUMPAD9";
    case VK_MULTIPLY: return "VKEY_MULTIPLY";
    case VK_ADD:      return "VKEY_ADD";
    case VK_SEPARATOR:return "VKEY_SEPARATOR";
    case VK_SUBTRACT: return "VKEY_SUBTRACT";
    case VK_DECIMAL:  return "VKEY_DECIMAL";
    case VK_DIVIDE:   return "VKEY_DIVIDE";
    case VK_F1:       return "VKEY_F1";
    case VK_F2:       return "VKEY_F2";
    case VK_F3:       return "VKEY_F3";
    case VK_F4:       return "VKEY_F4";
    case VK_F5:       return "VKEY_F5";
    case VK_F6:       return "VKEY_F6";
    case VK_F7:       return "VKEY_F7";
    case VK_F8:       return "VKEY_F8";
    case VK_F9:       return "VKEY_F9";
    case VK_F10:      return "VKEY_F10";
    case VK_F11:      return "VKEY_F11";
    case VK_F12:      return "VKEY_F12";
    case VK_F13:      return "VKEY_F13";
    case VK_F14:      return "VKEY_F14";
    case VK_F15:      return "VKEY_F15";
    case VK_F16:      return "VKEY_F16";
    case VK_F17:      return "VKEY_F17";
    case VK_F18:      return "VKEY_F18";
    case VK_F19:      return "VKEY_F19";
    case VK_F20:      return "VKEY_F20";
    case VK_F21:      return "VKEY_F21";
    case VK_F22:      return "VKEY_F22";
    case VK_F23:      return "VKEY_F23";
    case VK_F24:      return "VKEY_F24";
    case 0x88:        return "unassigned 0X88";
    case 0x89:        return "unassigned 0X89";
    case 0x8A:        return "unassigned 0X8A";
    case 0x8B:        return "unassigned 0X8B";
    case 0x8C:        return "unassigned 0X8C";
    case 0x8D:        return "unassigned 0X8D";
    case 0x8E:        return "unassigned 0X8E";
    case 0x8F:        return "unassigned 0X8F";
    case VK_NUMLOCK:  return "VKEY_NUMLOCK";
    case VK_SCROLL:   return "VKEY_SCROLL";
    case VK_OEM_NEC_EQUAL:  return "VKEY_OEM_NEC_EQUAL";
    case VK_OEM_FJ_MASSHOU: return "VKEY_OEM_FJ_MASSHOU";
    case VK_OEM_FJ_TOUROKU: return "VKEY_OEM_FJ_TOUROKU";
    case VK_OEM_FJ_LOYA:    return "VKEY_OEM_FJ_LOYA";
    case VK_OEM_FJ_ROYA:    return "VKEY_OEM_FJ_ROYA";
    case 0x97:        return "unassigned 0x97";
    case 0x98:        return "unassigned 0x98";
    case 0x99:        return "unassigned 0x99";
    case 0x9A:        return "unassigned 0x9A";
    case 0x9B:        return "unassigned 0x9B";
    case 0x9C:        return "unassigned 0x9C";
    case 0x9D:        return "unassigned 0x9D";
    case 0x9E:        return "unassigned 0x9E";
    case 0x9F:        return "unassigned 0x9F";
    case VK_LSHIFT:   return "VKEY_LSHIFT";
    case VK_RSHIFT:   return "VKEY_RSHIFT";
    case VK_LCONTROL: return "VKEY_LCONTROL";
    case VK_RCONTROL: return "VKEY_RCONTROL";
    case VK_LMENU:    return "VKEY_LMENU";
    case VK_RMENU:    return "VKEY_RMENU";
    case VK_BROWSER_BACK:     return "VKEY_BROWSER_BACK";
    case VK_BROWSER_FORWARD:  return "VKEY_BROWSER_FORWARD";
    case VK_BROWSER_REFRESH:  return "VKEY_BROWSER_REFRESH";
    case VK_BROWSER_STOP:     return "VKEY_BROWSER_STOP";
    case VK_BROWSER_SEARCH:   return "VKEY_BROWSER_SEARCH";
    case VK_BROWSER_FAVORITES:return "VKEY_BROWSER_FAVORITES";
    case VK_BROWSER_HOME:     return "VKEY_BROWSER_HOME";

    case VK_VOLUME_MUTE:      return "VKEY_VOLUME_MUTE";
    case VK_VOLUME_DOWN:      return "VKEY_VOLUME_DOWN";
    case VK_VOLUME_UP:        return "VKEY_VOLUME_UP";
    case VK_MEDIA_NEXT_TRACK: return "VKEY_MEDIA_NEXT_TRACK";
    case VK_MEDIA_STOP:       return "VKEY_MEDIA_STOP";
    case VK_MEDIA_PLAY_PAUSE: return "VKEY_MEDIA_PLAY_PAUSE";
    case VK_LAUNCH_MAIL:      return "VKEY_LAUNCH_MAIL";
    case VK_LAUNCH_MEDIA_SELECT:  return "VKEY_LAUNCH_MEDIA_SELECT";
    case VK_LAUNCH_APP1:      return "VKEY_LAUNCH_APP1";
    case VK_LAUNCH_APP2:      return "VKEY_LAUNCH_APP2";
    case 0xB8:                return "reserved 0xB8";
    case 0xB9:                return "reserved 0xB9";
    case VK_OEM_1:            return "VKEY_OEM_1";
    case VK_OEM_PLUS:         return "VKEY_OEM_PLUS";
    case VK_OEM_COMMA:        return "VKEY_OEM_COMMA";
    case VK_OEM_MINUS:        return "VKEY_OEM_MINUS";
    case VK_OEM_PERIOD:       return "VKEY_OEM_PERIOD";
    case VK_OEM_2:            return "VKEY_OEM_2";
    case VK_OEM_3:            return "VKEY_OEM_3";
    case 0xC1:                return "reserved 0xC1";
    case 0xC2:                return "reserved 0xC2";
    case 0xC3:                return "reserved 0xC3";
    case 0xC4:                return "reserved 0xC4";
    case 0xC5:                return "reserved 0xC5";
    case 0xC6:                return "reserved 0xC6";
    case 0xC7:                return "reserved 0xC7";
    case 0xC8:                return "reserved 0xC8";
    case 0xC9:                return "reserved 0xC9";
    case 0xCA:                return "reserved 0xCA";
    case 0xCB:                return "reserved 0xCB";
    case 0xCC:                return "reserved 0xCC";
    case 0xCD:                return "reserved 0xCD";
    case 0xCE:                return "reserved 0xCE";
    case 0xCF:                return "reserved 0xCF";
    case 0xD0:                return "reserved 0xD0";
    case 0xD1:                return "reserved 0xD1";
    case 0xD2:                return "reserved 0xD2";
    case 0xD3:                return "reserved 0xD3";
    case 0xD4:                return "reserved 0xD4";
    case 0xD5:                return "reserved 0xD5";
    case 0xD6:                return "reserved 0xD6";
    case 0xD7:                return "reserved 0xD7";
    case 0xD8:                return "unassigned 0xD8";
    case 0xD9:                return "unassigned 0xD9";
    case 0xDA:                return "unassigned 0xDA";
    case VK_OEM_4:            return "VKEY_OEM_4";
    case VK_OEM_5:            return "VKEY_OEM_5";
    case VK_OEM_6:            return "VKEY_OEM_6";
    case VK_OEM_7:            return "VKEY_OEM_7";
    case VK_OEM_8:            return "VKEY_OEM_8";
    case 0xE0:                return "reserved 0xE0";
    case VK_OEM_AX:           return "VKEY_OEM_AX";
    case VK_OEM_102:          return "VKEY_OEM_102";
    case VK_ICO_HELP:         return "VKEY_ICO_HELP";
    case VK_ICO_00:           return "VKEY_ICO_00";
    case VK_PROCESSKEY:       return "VKEY_PROCESSKEY";
    case VK_ICO_CLEAR:        return "VKEY_ICO_CLEAR";
    case VK_PACKET:           return "VKEY_PACKET";
    case 0xE8:                return "unassigned 0xE8";
    case VK_OEM_RESET:        return "VKEY_OEM_RESET";
    case VK_OEM_JUMP:         return "VKEY_OEM_JUMP";
    case VK_OEM_PA1:          return "VKEY_OEM_PA1";
    case VK_OEM_PA2:          return "VKEY_OEM_PA2";
    case VK_OEM_PA3:          return "VKEY_OEM_PA3";
    case VK_OEM_WSCTRL:       return "VKEY_OEM_WSCTRL";
    case VK_OEM_CUSEL:        return "VKEY_OEM_CUSEL";
    case VK_OEM_ATTN:         return "VKEY_OEM_ATTN";
    case VK_OEM_FINISH:       return "VKEY_OEM_FINISH";
    case VK_OEM_COPY:         return "VKEY_OEM_COPY";
    case VK_OEM_AUTO:         return "VKEY_OEM_AUTO";
    case VK_OEM_ENLW:         return "VKEY_OEM_ENLW";
    case VK_OEM_BACKTAB:      return "VKEY_OEM_BACKTAB";
    case VK_ATTN:             return "VKEY_ATTN";
    case VK_CRSEL:            return "VKEY_CRSEL";
    case VK_EXSEL:            return "VKEY_EXSEL";
    case VK_EREOF:            return "VKEY_EREOF";
    case VK_PLAY:             return "VKEY_PLAY";
    case VK_ZOOM:             return "VKEY_ZOOM";
    case VK_NONAME:           return "VKEY_NONAME";
    case VK_PA1:              return "VKEY_PA1";
    case VK_OEM_CLEAR:        return "VKEY_OEM_CLEAR";
    case 0xFF:                return "reserved 0xFF";
    default:                  return fmt::format("unknown {:0x}", virkCode);
    }
  }

}
