//---------------------------------------------------------------------------
#include <windows.h>
#include <process.h>
#include <string.h>
#include <vcl.h>
#pragma hdrstop
#include "Nkf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
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
TNkf::TNkf()
{
  dll = LoadDll("nkf32.dll", false);
  if(dll){
    setopt = (funcSetNkfOption *)GetProc(dll, "SetNkfOption", false);
    conv = (funcNkfConvertSafe *)GetProc(dll, "NkfConvertSafe", false);
    getkc = (funcNkfGetKanjiCode *)GetProc(dll, "NkfGetKanjiCode", false);
    ready = (setopt && conv && getkc);
  }else{
    ready = false;
  }
}
//---------------------------------------------------------------------------
TNkf::~TNkf()
{
  if(dll){
    ::FreeLibrary(dll);
  }
}
//---------------------------------------------------------------------------
int TNkf::Convert(LPSTR outStr,DWORD nOutBufferLength, LPDWORD lpBytesReturned,
                  LPCSTR inStr,DWORD nInBufferLength, LPCSTR optStr)
{
  int charcode = CHARCODE_SJIS;
  if(ready){
    setopt(optStr);
    conv(outStr, nOutBufferLength, lpBytesReturned, inStr, nInBufferLength);
    charcode = getkc();
  }else{
    *lpBytesReturned = min(nOutBufferLength, nInBufferLength);
    memcpy(outStr, inStr, *lpBytesReturned);
  }
  return charcode;
}
//---------------------------------------------------------------------------
int NkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength, LPDWORD lpBytesReturned, LPCSTR inStr,DWORD nInBufferLength, LPCSTR optStr)
{
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
