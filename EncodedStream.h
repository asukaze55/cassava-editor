//---------------------------------------------------------------------------
#ifndef EncodedStreamH
#define EncodedStreamH
//---------------------------------------------------------------------------
#include <vcl.h>
#include "Nkf.h"
//---------------------------------------------------------------------------
class EncodedStream : public TStream {
private:
  TStream *Stream;
  TMemoryStream *Memory;
  TNkf *Nkf;
  int KCode;
  bool UnicodeWindowsMapping;
  bool Bom;
public:
  EncodedStream(TStream *s, int code, bool winmap);
  __fastcall virtual ~EncodedStream();
  __fastcall int Read (void * Buffer, int Count);
  __fastcall int Write (const void * Buffer, int Count);
  void SetEncode(int code);
  void SetEncode(int code, bool winmap);
  void Flush();
};
//---------------------------------------------------------------------------
#endif
