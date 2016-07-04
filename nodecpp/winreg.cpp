#include "winreg.h"
#include <windows.h>

namespace nodecpp {
  class Winreg::impl {
  public:
    impl(HiveType hive, const string& key) {
      hkey_ = hiveToHkey(hive);
      LSTATUS rst = RegOpenKeyExA(hkey_, key.c_str(), 0, KEY_ALL_ACCESS, &subKey_);
      if (rst != ERROR_SUCCESS) {

      }
      else {
        isOpen_ = true;
      }
    }

    ~impl() {
      if (subKey_ != 0) RegCloseKey(subKey_);
    }

    bool isOpen() {
      return isOpen_;
    }


    bool get(const string& name, string& value) {
      BYTE buf[MAX_PATH] = { 0 };
      DWORD type = REG_SZ;
      DWORD realBufLen = MAX_PATH;
      LSTATUS rst = RegQueryValueExA(subKey_, name.c_str(), nullptr, &type, buf, &realBufLen);
      if (rst == ERROR_SUCCESS) {
        value = (char*)buf;
        return true;
      }
      else if (rst == ERROR_MORE_DATA) {
        PBYTE dynaBuf = new BYTE[realBufLen];
        rst = RegQueryValueExA(subKey_, name.c_str(), nullptr, &type, dynaBuf, &realBufLen);
        if (rst == ERROR_SUCCESS) value = (char*)dynaBuf;
        delete [] dynaBuf;
        return (rst == ERROR_SUCCESS);
      }
      else {
        return false;
      }
    }

    bool get(const string& name, uint32_t& value) {
      DWORD type = REG_DWORD;
      DWORD realBufLen = sizeof(value);
      LSTATUS rst = RegQueryValueExA(subKey_, name.c_str(), nullptr, &type, (LPBYTE)&value, &realBufLen);
      return (rst == ERROR_SUCCESS);
    }

    bool get(const string& name, uint64_t& value) {
      DWORD type = REG_QWORD;
      DWORD realBufLen = sizeof(value);
      LSTATUS rst = RegQueryValueExA(subKey_, name.c_str(), nullptr, &type, (LPBYTE)&value, &realBufLen);
      return (rst == ERROR_SUCCESS);
    }

    bool set(const string& name, const string& value) {
      LSTATUS rst = RegSetValueExA(subKey_, name.c_str(), 0, REG_SZ, (const LPBYTE)value.data(), value.size());
      return (rst == ERROR_SUCCESS);
    }

    bool set(const string& name, uint32_t value) {
      LSTATUS rst = RegSetValueExA(subKey_, name.c_str(), 0, REG_DWORD, (const LPBYTE)&value, sizeof(value));
      return (rst == ERROR_SUCCESS);
    }

    bool set(const string& name, uint64_t value) {
      LSTATUS rst = RegSetValueExA(subKey_, name.c_str(), 0, REG_QWORD, (const LPBYTE)&value, sizeof(value));
      return (rst == ERROR_SUCCESS);
    }

  private:
    HKEY hiveToHkey(HiveType hive) {
      if (hive == HKLM) return HKEY_LOCAL_MACHINE;
      else if (hive == HKCU) return HKEY_CURRENT_USER;
      else if (hive == HKCR) return HKEY_CLASSES_ROOT;
      else if (hive == HKCC) return HKEY_CURRENT_CONFIG;
      else if (hive == HKU) return HKEY_USERS;
      else return HKEY_LOCAL_MACHINE;
    }

  private:
    HKEY hkey_ = HKEY_LOCAL_MACHINE;
    HKEY subKey_ = 0;
    bool isOpen_ = false;
  };

  Winreg::Winreg(HiveType hive, const string& key) : pimpl(new impl(hive, key)) {}
  Winreg::~Winreg() {}


  bool Winreg::get(const string& name, string& value) {
    return pimpl->get(name, value);
  }

  bool Winreg::get(const string& name, uint32_t& value) {
    return pimpl->get(name, value);
  }

  bool Winreg::get(const string& name, uint64_t& value) {
    return pimpl->get(name, value);
  }

  bool Winreg::isOpen() {
    return pimpl->isOpen();
  }

  bool Winreg::set(const string& name, const string& value) {
    return pimpl->set(name, value);
  }

  bool Winreg::set(const string& name, uint32_t value) {
    return pimpl->set(name, value);
  }

  bool Winreg::set(const string& name, uint64_t value) {
    return pimpl->set(name, value);
  }

  
}