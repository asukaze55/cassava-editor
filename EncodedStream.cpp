//---------------------------------------------------------------------------
#pragma hdrstop
#include "EncodedStream.h"
//---------------------------------------------------------------------------
EncodedStream::EncodedStream(TStream *s, int code, bool winmap)
  : Stream(s), KCode(code), UnicodeWindowsMapping(winmap)
{
  Memory = new TMemoryStream();
  Nkf = new TNkf();
  Bom = true;
}
//---------------------------------------------------------------------------
__fastcall EncodedStream::~EncodedStream()
{
  Flush();
  delete Memory;
  delete Nkf;
}
//---------------------------------------------------------------------------
int __fastcall EncodedStream::Read (void * Buffer, int Count)
{
  return Stream->Read(Buffer, Count);
}
//---------------------------------------------------------------------------
int __fastcall EncodedStream::Write(const void * Buffer, int Count)
{
  if(KCode == CHARCODE_SJIS || KCode == CHARCODE_AUTO){
    return Stream->Write(Buffer, Count);
  }else{
    int count = Memory->Write(Buffer, Count);
    if(Memory->Size > 1000){
      Flush();
    }
    return count;
  }
}
//---------------------------------------------------------------------------
void EncodedStream::SetEncode(int code)
{
  SetEncode(code, UnicodeWindowsMapping);
}
//---------------------------------------------------------------------------
void EncodedStream::SetEncode(int code, bool winmap)
{
  if(code != KCode || winmap != UnicodeWindowsMapping){
    Flush();
    KCode = code;
    UnicodeWindowsMapping = winmap;
  }
}
//---------------------------------------------------------------------------
void EncodedStream::Flush()
{
  const char *sjis = (char *) Memory->Memory;
  int length = Memory->Size;

  if(length == 0){
    return;
  }else if(KCode == CHARCODE_SJIS || KCode == CHARCODE_AUTO){
    Stream->Write(sjis, length);
    Memory->Clear();
    return;
  }

  unsigned long bufsize = length*2+1;
  char *buf = new char[bufsize];
  unsigned long convlen;
  const char *convOpt;

  switch(KCode){
  case CHARCODE_EUC:
    convOpt = "-Sxe";
    break;
  case CHARCODE_JIS:
    convOpt = "-Sxj";
    break;
  case CHARCODE_UTF8:
    convOpt = (UnicodeWindowsMapping ?  "-Sxw --cp932" : "-Sxw");
    break;
  case CHARCODE_UTF16LE:
    if(Bom){
      convOpt = (UnicodeWindowsMapping ?  "-Sxw16L --cp932" : "-Sxw16L");
    }else{
      convOpt = (UnicodeWindowsMapping ?  "-Sxw16L0 --cp932" : "-Sxw16L0");
    }
    break;
  case CHARCODE_UTF16BE:
    if(Bom){
      convOpt = (UnicodeWindowsMapping ?  "-Sxw16B --cp932" : "-Sxw16B");
    }else{
      convOpt = (UnicodeWindowsMapping ?  "-Sxw16B0 --cp932" : "-Sxw16B0");
    }
    break;
  }

  NkfConvertSafe(buf, bufsize, &convlen, sjis, length, convOpt);
  if(convlen >= bufsize){
    delete[] buf;
    bufsize = convlen + 1;
    buf = new char[bufsize];
    NkfConvertSafe(buf, bufsize, &convlen, sjis, length, convOpt);
  }

  Stream->Write(buf, convlen);
  delete[] buf;
  Bom = false;
  Memory->Clear();
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
