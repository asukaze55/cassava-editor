//---------------------------------------------------------------------------
#include <vcl.h>
#include <dir.h>
#include <registry.hpp>
#include <process.h>
#pragma hdrstop

#include "AutoOpen.h"
//---------------------------------------------------------------------------
AnsiString Action(AnsiString Ext)
{
  const AnsiString YY = "\\";
  AnsiString FileType = "";
  AnsiString ActionType = "";
  AnsiString ActionStr = "";

  TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CLASSES_ROOT;
    if(Reg->OpenKeyReadOnly(YY + Ext)){
      FileType = Reg->ReadString("");
      Reg->CloseKey();
    }
    if(FileType != ""){
      if(Reg->OpenKeyReadOnly(YY + FileType + "\\shell")){
        ActionType = Reg->ReadString("");
        Reg->CloseKey();
      }
      if(ActionType == "") ActionType = "open";

      AnsiString ComPath = YY+FileType+"\\shell\\"+ActionType+"\\command";
      if(Reg->OpenKeyReadOnly(ComPath)){
        ActionStr = Reg->ReadString("");
        Reg->CloseKey();
      }
    }
  delete Reg;

  return ActionStr;
};
//---------------------------------------------------------------------------
bool AutoOpen(AnsiString FileName)
{
  AnsiString FullName = ExpandFileName(FileName);
  AnsiString Ext = ExtractFileExt(FileName);
  if(FileName.SubString(1,7) == "http://"){
    FullName = FileName;
    Ext=".html";
  }else if(!FileExists(FullName)){
    if(FullName != ""){
      Application->MessageBox(
        (FullName + "\nファイルが見つかりません。").c_str(),
        "Not Found",0);
    }
    return false;
  }
  if(Ext == "") Ext = ".txt";


  AnsiString Act = Action(Ext);
  if(Act == "") Act = Action(".txt");
  if(Act == "") return false;

  TStringList *Line = new TStringList;
    Line->CommaText = Act;
    AnsiString Exe  = Line->Strings[0];
    AnsiString Arg0 = (AnsiString)("\"") + Exe + "\"";
    AnsiString Arg1 = (AnsiString)("\"") + FullName + "\"";
  delete Line;

  if(Exe == "%1"){
    spawnl(P_NOWAITO , FullName.c_str() , Arg1.c_str() , NULL);
  }else{
    spawnl(P_NOWAITO , Exe.c_str() , Arg0.c_str() , Arg1.c_str() , NULL);
  }

  return true;
}
//---------------------------------------------------------------------------
