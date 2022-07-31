//---------------------------------------------------------------------------
#ifndef MacroH
#define MacroH
//---------------------------------------------------------------------------
#include <vcl.h>
#include "EncodedWriter.h"
//---------------------------------------------------------------------------
String ExecMacro(String FileName, int MaxLoop, TStringList *Modules,
    int x, int y, EncodedWriter *IO = nullptr, bool IsCellMacro = false,
    TStringList *StringArguments = nullptr);

void StopAllMacros();

int GetRunningMacroCount();
//---------------------------------------------------------------------------
#endif

