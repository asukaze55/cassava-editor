//---------------------------------------------------------------------------
#ifndef MacroH
#define MacroH
//---------------------------------------------------------------------------
#include <map>
#include <vcl.h>
#include "Compiler.h"
#include "EncodedWriter.h"
//---------------------------------------------------------------------------
struct TMacroValue {
  String string;
  std::map<String, String> object;

  TMacroValue() {}
  TMacroValue(String s, std::map<String, String> o) : string(s), object(o) {}
};

TMacroValue RunMacro(String FileName, int MaxLoop, MacroContext *Context,
    int x, int y, EncodedWriter *IO = nullptr, bool IsCellMacro = false,
    TStringList *StringArguments = nullptr);

void StopAllMacros();

int GetRunningMacroCount();
//---------------------------------------------------------------------------
#endif

