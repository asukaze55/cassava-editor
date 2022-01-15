//---------------------------------------------------------------------------
#ifdef CssvMacro
#ifndef CompilerH
#define CompilerH
//---------------------------------------------------------------------------
bool MacroCompile(TStream *stream, TStringList *inpaths,
                  AnsiString inpath, AnsiString inname, AnsiString ext,
                  TStringList *modules, bool showMessage);

bool MacroCompile(AnsiString infile, TStringList *inpaths,
                  TStringList *modules, bool showMessage);
//---------------------------------------------------------------------------
#endif
#endif
