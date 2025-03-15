//---------------------------------------------------------------------------
#pragma hdrstop
#include "EncodedWriter.h"
//---------------------------------------------------------------------------
EncodedWriter::EncodedWriter(TStream *s, TEncoding *encoding, bool bom)
  : Stream(s), Encoding(encoding), Bom(bom) {}
//---------------------------------------------------------------------------
void EncodedWriter::Write(String data)
{
  if (data.Length() > 0) {
    if (Bom) {
      if (Encoding == TEncoding::UTF8) {
        Stream->Write("\xef\xbb\xbf", 3);
      } else if (Encoding == TEncoding::Unicode) {
        Stream->Write("\xff\xfe", 2);
      } else if (Encoding == TEncoding::BigEndianUnicode) {
        Stream->Write("\xfe\xff", 2);
      }
      Bom = false;
    }
    DynamicArray<System::Byte> array = Encoding->GetBytes(data);
    Stream->Write(&(array[0]), array.Length);
  }
}
//---------------------------------------------------------------------------
void EncodedWriter::SetEncoding(TEncoding *encoding)
{
  Encoding = encoding;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
