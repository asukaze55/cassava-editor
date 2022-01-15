//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
#include "FormattedFileName.h"
//---------------------------------------------------------------------------
AnsiString FormattedFileName(AnsiString Format, AnsiString BaseFileName)
{
  AnsiString Path2 = ExtractFilePath(BaseFileName);
  AnsiString FileName2 = ExtractFileName(BaseFileName);
  AnsiString Ext2 = ExtractFileExt(FileName2);
  AnsiString Body2 = ChangeFileExt(FileName2,"");
  if(Ext2.Length() >= 1 && Ext2[1]=='.') Ext2.Delete(1,1);

  AnsiString FileName = "";
  for(int i=1; i<=Format.Length(); i++){
    if(Format[i] == '%' && i < Format.Length()){
      i++;
      char ch = Format[i];
      switch(ch){
      case 'f':
        FileName += Body2;
        break;
      case 'x':
        FileName += Ext2;
        break;
      default:
        FileName += '%';
        FileName += ch;
        break;
      }
    }else{
      FileName += Format[i];
    }
  }
  if((Format.Length() < 2) ||
     ((Format[1]!='\\') && (Format[2]!=':') && (Format[1]!='/'))) {
    FileName = Path2 + FileName;
  }
  return FileName;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

