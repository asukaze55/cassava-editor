//---------------------------------------------------------------------------
#ifndef NkfH
#define NkfH
//---------------------------------------------------------------------------
#define CHARCODE_AUTO   (-1)
#define CHARCODE_SJIS    (0)
#define CHARCODE_EUC     (1)
#define CHARCODE_JIS     (2)
#define CHARCODE_UTF8    (3)
#define CHARCODE_UTF16LE (4)
#define CHARCODE_UTF16BE (5)
//---------------------------------------------------------------------------
typedef int CALLBACK funcSetNkfOption(LPCSTR optStr);
typedef BOOL WINAPI funcNkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/, LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
typedef int CALLBACK funcNkfGetKanjiCode();
//---------------------------------------------------------------------------
class TNkf {
private:
  HINSTANCE dll;
  funcSetNkfOption *setopt;
  funcNkfConvertSafe *conv;
  funcNkfGetKanjiCode *getkc;
  bool ready; 
public:
  TNkf();
  virtual ~TNkf();
  int Convert(LPSTR outStr,DWORD nOutBufferLength, LPDWORD lpBytesReturned,
              LPCSTR inStr,DWORD nInBufferLength, LPCSTR optStr);
};
//---------------------------------------------------------------------------
int NkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength, LPDWORD lpBytesReturned,
                   LPCSTR inStr,DWORD nInBufferLength, LPCSTR optStr);
//---------------------------------------------------------------------------
#endif
