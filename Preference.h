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
  IniFile(String fileName) : file(std::make_unique<TIniFile>(fileName)) {}

  bool ReadBool(const String section, const String key, bool def);
  double ReadFloat(const String section, const String key, double def);
  int ReadInteger(const String section, const String key, int def);
  String ReadString(const String section, const String key, const String def);

  void WriteBool(const String section, const String key, bool value);
  void WriteFloat(const String section, const String key, double value);
  void WriteInteger(const String section, const String key, int value);
  void WriteString(const String section, const String key, const String value);

  void DeleteKey(const String section, const String key);
  bool ValueExists(const String section, const String key);
};
//---------------------------------------------------------------------------
class Preference
{
public:
  String Path;
  String SharedPath;
  String UserPath;

  Preference(String cassavaPath);
  IniFile *GetInifile();
};
//---------------------------------------------------------------------------
#endif
