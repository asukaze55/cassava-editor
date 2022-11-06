//---------------------------------------------------------------------------
#ifndef MacroH
#define MacroH
//---------------------------------------------------------------------------
#include <map>
#include <vcl.h>
#include "EncodedWriter.h"
//---------------------------------------------------------------------------
struct TMacroValue {
  String string;
  std::map<String, String> object;

  TMacroValue() {}
  TMacroValue(String s, std::map<String, String> o) : string(s), object(o) {}
};

TMacroValue ExecMacro(String FileName, int MaxLoop, TStringList *Modules,
    int x, int y, EncodedWriter *IO = nullptr, bool IsCellMacro = false,
    TStringList *StringArguments = nullptr);

void StopAllMacros();

int GetRunningMacroCount();
//---------------------------------------------------------------------------
#endif

