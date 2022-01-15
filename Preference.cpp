//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <shlobj.h>
#pragma hdrstop

#include "Preference.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#define MAX_PATH 260
//---------------------------------------------------------------------------
AnsiString GetSpecialFolderPath(int type)
{
  TCHAR szPath[MAX_PATH];
  SHGetSpecialFolderPath(NULL, szPath, type, false);
  return AnsiString(szPath);
}
//---------------------------------------------------------------------------
Preference::Preference(AnsiString cassavaPath)
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
TIniFile *Preference::GetInifile()
{
  return new TIniFile(Path + "Cassava.ini");
}
//---------------------------------------------------------------------------

