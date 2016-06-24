#ifndef __INI_H__
#define __INI_H__
#include "nodecpp-def.h"

#include "SimpleIni.h"

namespace nodecpp {
  using std::string;

  class Ini {
  public:
    bool loadFile(const string& path);
    bool loadData(const char *data, uint32_t size);

    svec_t getAllSections();
    svec_t getAllKeys(const string& section);

    string getString(const string& section, const string& key, const string& defaultValue = "");
    int getInt(const string& section, const string& key, int defaultValue = 0);
    bool getBool(const string& section, const string& key, bool defaultValue = false);

    bool setValue(const string& section, const string& key, const string& value);

    bool delSection(const string& section);
    bool delKey(const string& section, const string& key);

    string save();
    bool saveFile(const string& path);

  private:
    CSimpleIniA ini_;
  };
}

#endif // !__INI_H__