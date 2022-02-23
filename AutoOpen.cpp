//---------------------------------------------------------------------------
#include <vcl.h>
#include <dir.h>
#include <registry.hpp>
#include <process.h>
#pragma hdrstop

#include "AutoOpen.h"
//---------------------------------------------------------------------------
bool isUrl(String FileName)
{
  return (FileName.SubString(1,7) == "http://" ||
          FileName.SubString(1,8) == "https://");
}
//---------------------------------------------------------------------------
bool AutoOpen(String FileName, String BasePath)
{
  String FullName;
  if(isUrl(FileName)){
    FullName = FileName;
  }else{
    if (BasePath == "" || FileName[1] == '\\' || FileName[2] == ':') {
      FullName = ExpandFileName(FileName);
    } else if(*(BasePath.LastChar()) == '\\') {
      FullName = BasePath + FileName;
    } else {
      FullName = BasePath + "\\" + FileName;
    }
    if(!FileExists(FullName)){
      if(FullName != ""){
        Application->MessageBox(
          (FullName + "\nファイルが見つかりません。").c_str(),
          L"Not Found", 0);
      }
      return false;
    }
  }
  ::ShellExecute(nullptr, nullptr, FullName.c_str(), nullptr, nullptr,
                 SW_SHOWDEFAULT);
  return true;
}
//---------------------------------------------------------------------------
