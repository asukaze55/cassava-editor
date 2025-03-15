//---------------------------------------------------------------------------
#ifndef EncodingDetectorH
#define EncodingDetectorH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
enum TCharCode {
  CHARCODE_AUTO = -1,
  CHARCODE_SJIS = 0,
  CHARCODE_EUC = 1,
  CHARCODE_JIS = 2,
  CHARCODE_UTF8 = 3,
  CHARCODE_UTF16LE = 4,
  CHARCODE_UTF16BE = 5
};
//---------------------------------------------------------------------------
enum TCodePage {
  CODE_PAGE_SJIS = 932,
  CODE_PAGE_EUC = 20932,
  CODE_PAGE_JIS = 50221,
  CODE_PAGE_UTF8 = 65001,
  CODE_PAGE_UTF16LE = 1200,
  CODE_PAGE_UTF16BE = 1201
};
//---------------------------------------------------------------------------
class TEncodingDetector {
public:
  bool Enabled;
  TEncoding *DefaultEncoding;

  TEncodingDetector() : Enabled(true), DefaultEncoding(TEncoding::UTF8) {}
  TEncoding *Detect(String fileName);
};
//---------------------------------------------------------------------------
TEncoding *ToEncoding(int charCode);
TCharCode ToCharCode(TEncoding * encoding);
//---------------------------------------------------------------------------
#endif
