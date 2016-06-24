#include "ini.h"
#include "underscore.string.h"
#include "SimpleIni/SimpleIni.h"

namespace nodecpp {

  class Ini::impl {
  public:
    CSimpleIniA ini_;
  };

  Ini::Ini(): pimpl(new impl) {}

  bool Ini::loadFile(const string& path) {
    SI_Error rc = pimpl->ini_.LoadFile(path.c_str());
    return (rc >= 0);
  }

  bool Ini::loadData(const char *data, uint32_t size) {
    SI_Error rc = pimpl->ini_.LoadData(data, size);
    return (rc >= 0);
  }

  svec_t Ini::getAllSections() {
    svec_t svec;
    CSimpleIniA::TNamesDepend sections;
    pimpl->ini_.GetAllSections(sections);

    for (auto entry : sections) {
      svec.emplace_back(entry.pItem);
    }
    return svec;
  }

  svec_t Ini::getAllKeys(const string& section) {
    svec_t svec;
    CSimpleIniA::TNamesDepend keys;
    pimpl->ini_.GetAllKeys(section.c_str(), keys);

    for (auto entry : keys) {
      svec.emplace_back(entry.pItem);
    }
    return svec;
  }

  string Ini::getString(const string& section, const string& key, const string& defaultValue /*= ""*/) {
    return pimpl->ini_.GetValue(section.c_str(), key.c_str(), defaultValue.c_str());
  }

  int Ini::getInt(const string& section, const string& key, int defaultValue /*= 0*/) {
    string str = pimpl->ini_.GetValue(section.c_str(), key.c_str(), "");

    if (str.empty()) return defaultValue;
    try {
      return std::stoi(str);
    }
    catch (std::invalid_argument&) {
      return defaultValue;
    }
  }

  bool Ini::getBool(const string& section, const string& key, bool defaultValue /*= false*/) {
    string str = pimpl->ini_.GetValue(section.c_str(), key.c_str(), "");
    return str.empty() ? defaultValue : (s.toLower(str) == "true");
  }

  bool Ini::setValue(const string& section, const string& key, const string& value) {
    SI_Error rc = pimpl->ini_.SetValue(section.c_str(), key.c_str(), value.c_str());
    return (rc >= 0);
  }

  bool Ini::delSection(const string& section) {
    return pimpl->ini_.Delete(section.c_str(), NULL);
  }

  bool Ini::delKey(const string& section, const string& key) {
    return pimpl->ini_.Delete(section.c_str(), key.c_str(), true);
  }

  string Ini::save() {
    string strData;
    pimpl->ini_.Save(strData);
    return strData;
  }

  bool Ini::saveFile(const string& path) {
    SI_Error rc = pimpl->ini_.SaveFile(path.c_str());
    return (rc >= 0);
  }

}
