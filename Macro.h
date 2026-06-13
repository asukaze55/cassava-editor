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

TMacroValue RunMacro(String FileName, int MaxLoop, const TMacroContext &Context,
    int x, int y, bool ReadOnly = false, EncodedWriter *IO = nullptr,
    TStringList *StringArguments = nullptr);

void StopAllMacros();

int GetRunningMacroCount();
//---------------------------------------------------------------------------
#endif

