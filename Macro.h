//---------------------------------------------------------------------------
#ifndef MacroH
#define MacroH
//---------------------------------------------------------------------------
#include <vcl.h>
#include "EncodedWriter.h"
//---------------------------------------------------------------------------
String ExecMacro(String FileName, int MaxLoop, TStringList *Modules,
  int x, int y, EncodedWriter *IO=NULL, bool IsCellMacro=false);

void StopAllMacros();

int GetRunningMacroCount();
//---------------------------------------------------------------------------
#endif

