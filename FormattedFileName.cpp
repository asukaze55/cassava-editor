//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "FormattedFileName.h"
//---------------------------------------------------------------------------
String FormattedFileName(String Format, String BaseFileName)
{
  String Path2 = ExtractFilePath(BaseFileName);
  String FileName2 = ExtractFileName(BaseFileName);
  String Ext2 = ExtractFileExt(FileName2);
  String Body2 = ChangeFileExt(FileName2,"");
  if(Ext2.Length() >= 1 && Ext2[1]=='.') Ext2.Delete(1,1);
  unsigned short year, month, day;
  Date().DecodeDate(&year, &month, &day);
  unsigned short hours, minutes, seconds, msec;
  Time().DecodeTime(&hours, &minutes, &seconds, &msec);

  bool nExists = false;
  String FileName = "";
  for(int i=1; i<=Format.Length(); i++){
    if(Format[i] == '%' && i < Format.Length()){
      i++;
      TCHAR ch = Format[i];
      switch(ch){
      case 'f':
        FileName += Body2;
        break;
      case 'x':
        FileName += Ext2;
        break;
      case 'y':
        FileName += (String)year;
        break;
      case 'M':
        FileName += (char)('0' + (month / 10));
        FileName += (char)('0' + (month % 10));
        break;
      case 'd':
        FileName += (char)('0' + (day / 10));
        FileName += (char)('0' + (day % 10));
        break;
      case 'H':
        FileName += (char)('0' + (hours / 10));
        FileName += (char)('0' + (hours % 10));
        break;
      case 'm':
        FileName += (char)('0' + (minutes / 10));
        FileName += (char)('0' + (minutes % 10));
        break;
      case 's':
        FileName += (char)('0' + (seconds / 10));
        FileName += (char)('0' + (seconds % 10));
        break;
      case 'n':
        FileName += "<n>"; // <> は Windows ではファイル名に使えない
        nExists = true;
        break;
      case '%':
        FileName += '%';
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
  if(nExists){
    for(int n = 1;; n++){
      String FileNameTest = FileName;
      int p;
      while((p = FileNameTest.Pos("<n>")) > 0){
        FileNameTest.Delete(p, 3);
        FileNameTest.Insert((String)n, p);
      }
      if(! FileExists(FileNameTest)){
        return FileNameTest;
      }
    }
  }else{
    return FileName;
  }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)

