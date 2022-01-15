//---------------------------------------------------------------------------
#pragma hdrstop
#include "TypeOption.h"
//---------------------------------------------------------------------------
void TTypeOption::SetExts(AnsiString ExtString)
{
  Exts->Clear();
  int pos;
  while((pos = ExtString.AnsiPos(";")) > 0){
    AnsiString ext = ExtString.SubString(1,pos-1);
    if(ext != "") { Exts->Add(ext); }
    ExtString.Delete(1,pos);
  }
  if(ExtString != "") Exts->Add(ExtString);
}
//---------------------------------------------------------------------------
AnsiString TTypeOption::GetExtsStr(int From)
{
  AnsiString Temp = "";
  for(int i=From; i<Exts->Count; i++){
    if(i > From) Temp += ";";
    Temp += Exts->Strings[i];
  }
  return Temp;
}
//---------------------------------------------------------------------------
AnsiString Ascii2Ctrl(AnsiString str){
  AnsiString r;
  for(int i=1; i<=str.Length(); i++){
    if(str[i] == '\\' && i < str.Length()){
      i++;
      switch(str[i]){
        case 'n':  r += "\n";   break;
        case 't':  r += "\t";   break;
        case '_':  r += " ";    break;
        default:   r += str[i]; break;
      }
    }else{
      r += str[i];
    }
  }
  return r;
}
//---------------------------------------------------------------------------
AnsiString Ctrl2Ascii(AnsiString str){
  AnsiString r;
  for(int i=1; i<=str.Length(); i++){
    switch(str[i]){
      case '\n': r += "\\n"; break;
      case '\t': r += "\\t"; break;
      case ' ':  r += "\\_"; break;
      case '\\': r += "\\\\"; break;
      default:   r += str[i]; break;
    }
  }
  return r;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
