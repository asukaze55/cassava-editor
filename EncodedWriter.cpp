//---------------------------------------------------------------------------
#pragma hdrstop
#include "EncodedWriter.h"
//---------------------------------------------------------------------------
EncodedWriter::EncodedWriter(TStream *s, int code)
  : Stream(s), Bom(true)
{
  SetEncode(code);
}
//---------------------------------------------------------------------------
void EncodedWriter::Write(String data)
{
  if(data.Length() > 0){
    if(Bom){
      if(Encoding == TEncoding::Unicode){
        Stream->Write("\xff\xfe", 2);
      }else if(Encoding == TEncoding::BigEndianUnicode){
        Stream->Write("\xfe\xff", 2);
      }
      Bom = false;
    }
    DynamicArray<System::Byte> array = Encoding->GetBytes(data);
    Stream->Write(&(array[0]), array.Length);
  }
}
//---------------------------------------------------------------------------
void EncodedWriter::SetEncode(int code)
{
  switch(code){
    case CHARCODE_SJIS:    Encoding = TEncoding::GetEncoding(932); return;
    case CHARCODE_EUC:     Encoding = TEncoding::GetEncoding(20932); return;
    case CHARCODE_JIS:     Encoding = TEncoding::GetEncoding(50221); return;
    case CHARCODE_UTF8:    Encoding = TEncoding::UTF8; return;
    case CHARCODE_UTF16LE: Encoding = TEncoding::Unicode; return;
    case CHARCODE_UTF16BE: Encoding = TEncoding::BigEndianUnicode; return;
  }
  Encoding = TEncoding::Default;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
