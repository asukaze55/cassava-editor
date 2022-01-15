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

  AnsiString FileName = Format;

  int p;
  while((p = FileName.AnsiPos("%f")) > 0){
    FileName.Delete(p, 2);
    FileName.Insert(Body2, p);
  }
  while((p = FileName.AnsiPos("%x")) > 0){
    FileName.Delete(p, 2);
    FileName.Insert(Ext2, p);
  }
  if((Ext2.Length() < 2) ||
     ((Format[1]!='\\') && (Format[2]!=':') && (Format[1]!='/'))) {
    FileName = Path2 + FileName;
  }
  return FileName;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

