//---------------------------------------------------------------------------
#ifdef CssvMacro
#ifndef MacroH
#define MacroH
//---------------------------------------------------------------------------
#include <vcl.h>

AnsiString ExecMacro(AnsiString FileName, int MaxLoop,
  TStringList *Modules, int x, int y, TStream *IO=NULL, bool IsCellMacro=false);
//---------------------------------------------------------------------------
#endif
#endif

