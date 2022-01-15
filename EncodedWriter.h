//---------------------------------------------------------------------------
#ifndef EncodedWriterH
#define EncodedWriterH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
#define CHARCODE_AUTO   (-1)
#define CHARCODE_SJIS    (0)
#define CHARCODE_EUC     (1)
#define CHARCODE_JIS     (2)
#define CHARCODE_UTF8    (3)
#define CHARCODE_UTF16LE (4)
#define CHARCODE_UTF16BE (5)
//---------------------------------------------------------------------------
class EncodedWriter {
private:
  TStream *Stream;
  TEncoding *Encoding;
  bool Bom;
public:
  EncodedWriter(TStream *s, int code, bool bom);
  void Write(String data);
  void SetEncode(int code);
};
//---------------------------------------------------------------------------
#endif
