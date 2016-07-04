#pragma once
#include "nodecpp-def.h"

namespace nodecpp {
  class Winreg {
  public:
    enum HiveType {
      HKLM,  // HKEY_LOCAL_MACHINE
      HKCU,  // HKEY_CURRENT_USER
      HKCR,  // HKEY_CLASSES_ROOT
      HKCC,  // HKEY_CURRENT_CONFIG
      HKU    // HKEY_USERS
    };

  public:
    Winreg(HiveType hive, const string& key);
    ~Winreg();

    void values();
    void keys();

    bool get(const string& name, string& value);       // REG_SZ
    bool get(const string& name, uint32_t& value);     // REG_DWORD
    bool get(const string& name, uint64_t& value);     // REG_QWORD

    // if not exist, will create
    bool set(const string& name, const string& value); // REG_SZ
    bool set(const string& name, uint32_t value);      // REG_DWORD
    bool set(const string& name, uint64_t value);      // REG_QWORD

    bool remove(const string& name);
    void create();
    void erase();

    bool keyExists();
    bool valueExists(const string& name);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}