//---------------------------------------------------------------------------
#include <windows.h>
#include <process.h>
#include <string.h>
#pragma hdrstop
#include "Nkf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
typedef int CALLBACK funcSetNkfOption(LPCSTR optStr);
typedef BOOL WINAPI funcNkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/, LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
typedef int CALLBACK funcNkfGetKanjiCode();
//---------------------------------------------------------------------------
#define min(x,y) ((x) < (y) ? (x) : (y))
//---------------------------------------------------------------------------
HINSTANCE LoadDll(const char *DllName, bool warn)
{
  HINSTANCE inst = ::LoadLibrary(DllName);
  if(inst) return inst;
  if(warn){ ::MessageBoxA(NULL, DllName, "DLL Not Found", 0); }
  return NULL;
}
//---------------------------------------------------------------------------
void *GetProc(HINSTANCE Dll, const char *ProcName, bool warn)
{
  void *proc = ::GetProcAddress(Dll, ProcName);
  if(proc) return proc;
  if(warn){ ::MessageBoxA(NULL, ProcName, "DLL Function Not Found", 0); }
  return NULL;
}
//---------------------------------------------------------------------------
int NkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength, LPDWORD lpBytesReturned, LPCSTR inStr,DWORD nInBufferLength, LPCSTR optStr){
  int charcode = CHARCODE_SJIS;
  HINSTANCE dll = LoadDll("nkf32.dll", false);
  if(dll){
    funcSetNkfOption *setopt = (funcSetNkfOption *)GetProc(dll, "SetNkfOption", false);
    funcNkfConvertSafe *conv = (funcNkfConvertSafe *)GetProc(dll, "NkfConvertSafe", false);
    funcNkfGetKanjiCode *getkc = (funcNkfGetKanjiCode *)GetProc(dll, "NkfGetKanjiCode", false);
    if(setopt && conv && getkc){
      setopt(optStr);
      conv(outStr, nOutBufferLength, lpBytesReturned, inStr, nInBufferLength);
      charcode = getkc();
    }else{
      *lpBytesReturned = min(nOutBufferLength, nInBufferLength);
      memcpy(outStr, inStr, *lpBytesReturned);
    }
    ::FreeLibrary(dll);
  }else{
    *lpBytesReturned = min(nOutBufferLength, nInBufferLength);
    memcpy(outStr, inStr, *lpBytesReturned);
  }
  return charcode;
}
//---------------------------------------------------------------------------
