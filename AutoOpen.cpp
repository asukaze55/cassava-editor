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
bool AutoOpen(String FileName)
{
  String FullName;
  if(isUrl(FileName)){
	FullName = FileName;
  }else{
	FullName = ExpandFileName(FileName);
	if(!FileExists(FullName)){
	  if(FullName != ""){
		Application->MessageBox(
		  (FullName + "\nファイルが見つかりません。").c_str(),
		  TEXT("Not Found"),0);
	  }
	  return false;
	}
  }
  ::ShellExecute(NULL, NULL, FullName.c_str(), NULL, NULL, SW_SHOWDEFAULT);
  return true;
}
//---------------------------------------------------------------------------
