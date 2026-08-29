//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include <dir.h>
#include <process.h>
#include <registry.hpp>

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
          (FullName + L"\nファイルが見つかりません。").c_str(),
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
int ShellOpen(const std::vector<String>& Params)
{
  return SpawnProcess(Params, /* LookupPath= */ true);
}
//---------------------------------------------------------------------------
int SpawnProcess(const std::vector<String>& Params, bool LookupPath)
{
  std::vector<String> quotedParams;
  quotedParams.reserve(Params.size());
  for (const String& param : Params) {
    bool needQuote =
        !param.IsEmpty() && param[1] != L'\"' && param.Pos(" ") > 0;
    quotedParams.push_back(needQuote ? "\"" + param + "\"" : param);
  }

  std::vector<wchar_t*> argv;
  argv.reserve(quotedParams.size() + 1);
  for (const String& quotedParam : quotedParams) {
    argv.push_back(quotedParam.c_str());
  }
  argv.push_back(nullptr);

  if (LookupPath) {
    return _wspawnvp(P_NOWAITO, Params[0].c_str(), argv.data());
  }
  return _wspawnv(P_NOWAITO, Params[0].c_str(), argv.data());
}
//---------------------------------------------------------------------------
