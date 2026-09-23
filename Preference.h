//---------------------------------------------------------------------------
#ifndef PreferenceH
#define PreferenceH
//---------------------------------------------------------------------------
#include <System.IniFiles.hpp>
//---------------------------------------------------------------------------
class IniFile
{
private:
  std::unique_ptr<TIniFile> file;
public:
  IniFile(const String& fileName)
      : file(std::make_unique<TIniFile>(fileName)) {}

  bool ReadBool(const String& section, const String& key, bool def);
  double ReadFloat(const String& section, const String& key, double def);
  int ReadInteger(const String& section, const String& key, int def);
  String ReadString(const String& section, const String& key,
      const String& def);

  void WriteBool(const String& section, const String& key, bool value);
  void WriteFloat(const String& section, const String& key, double value);
  void WriteInteger(const String& section, const String& key, int value);
  void WriteString(const String& section, const String& key,
      const String& value);

  void DeleteKey(const String& section, const String& key);
  bool ValueExists(const String& section, const String& key);

  template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
  T ReadEnum(const String& section, const String& key, T def) {
    return static_cast<T>(
        file->ReadInteger(section, key, static_cast<int>(def)));
  }

  template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
  void WriteEnum(const String& section, const String& key, T value) {
    file->WriteInteger(section, key, static_cast<int>(value));
  }
};
//---------------------------------------------------------------------------
class Preference
{
public:
  String Path;
  String SharedPath;
  String UserPath;

  Preference(const String& cassavaPath);
  IniFile GetInifile();
};
//---------------------------------------------------------------------------
#endif
