//---------------------------------------------------------------------------
#include <vcl.h>
#include <shlobj.h>
#pragma hdrstop

#include "Preference.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool IniFile::ReadBool(const String section, const String key, bool def)
{
  return file->ReadBool(section, key, def);
}
//---------------------------------------------------------------------------
double IniFile::ReadFloat(const String section, const String key, double def)
{
  return file->ReadFloat(section, key, def);
}
//---------------------------------------------------------------------------
int IniFile::ReadInteger(const String section, const String key, int def)
{
  return file->ReadInteger(section, key, def);
}
//---------------------------------------------------------------------------
String IniFile::ReadString(const String section, const String key,
                           const String def)
{
  String raw = file->ReadString(section, key, def);
  String result = "";
  for (int i = 1; i <= raw.Length(); i++) {
    if (i > raw.Length() - 2 || raw[i] != '?' || raw[i + 1] != '#'
        || (raw[i + 2] < '1' && raw[i + 2] > '9')) {
      result += raw[i];
      continue;
    }

    i++;
    int code = 0;
    while (i < raw.Length() && raw[i + 1] >= '0' && raw[i + 1] <= '9') {
      code = code * 10 + (raw[++i] - '0');
    }
    result += (wchar_t)code;
    if (i < raw.Length() && raw[i + 1] == ';') {
      i++;
    }
  }
  return result;
}
//---------------------------------------------------------------------------
void IniFile::WriteBool(const String section, const String key, bool value)
{
  file->WriteBool(section, key, value);
}
//---------------------------------------------------------------------------
void IniFile::WriteFloat(const String section, const String key, double value)
{
  return file->WriteFloat(section, key, value);
}
//---------------------------------------------------------------------------
void IniFile::WriteInteger(const String section, const String key, int value)
{
  file->WriteInteger(section, key, value);
}
//---------------------------------------------------------------------------
void IniFile::WriteString(const String section, const String key,
                          const String value)
{
  AnsiString ansiStr = "";
  for (int i = 1; i <= value.Length(); i++) {
    wchar_t c = value[i];
    if ((AnsiString)c != "?") {
      ansiStr += c;
    } else {
      ansiStr += (AnsiString)"?#" + (int)c + ";";
    }
  }
  file->WriteString(section, key, ansiStr);
}
//---------------------------------------------------------------------------
void IniFile::DeleteKey(const String section, const String key)
{
  file->DeleteKey(section, key);
}
//---------------------------------------------------------------------------
bool IniFile::ValueExists(const String section, const String key)
{
  return file->ValueExists(section, key);
}
//---------------------------------------------------------------------------
#define MAX_PATH 260
//---------------------------------------------------------------------------
String GetSpecialFolderPath(int type)
{
  TCHAR szPath[MAX_PATH];
  SHGetSpecialFolderPath(nullptr, szPath, type, false);
  return String(szPath);
}
//---------------------------------------------------------------------------
Preference::Preference(String cassavaPath)
{
  SharedPath = cassavaPath;
  UserPath = GetSpecialFolderPath(CSIDL_APPDATA) + "\\Asukaze\\Cassava\\";
  if(FileExists(SharedPath + "Cassava.ini")){
    Path = SharedPath;
  }else{
    Path = UserPath;
    if(!DirectoryExists(Path)){
      ForceDirectories(Path);
    }
  }
}
//---------------------------------------------------------------------------
IniFile *Preference::GetInifile()
{
  return new IniFile(Path + "Cassava.ini");
}
//---------------------------------------------------------------------------

