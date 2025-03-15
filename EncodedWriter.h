//---------------------------------------------------------------------------
#ifndef EncodedWriterH
#define EncodedWriterH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
class EncodedWriter {
private:
  TStream *Stream;
  TEncoding *Encoding;
  bool Bom;
public:
  EncodedWriter(TStream *s, TEncoding *encoding, bool bom);
  void Write(String data);
  void SetEncoding(TEncoding *encoding);
};
//---------------------------------------------------------------------------
#endif
