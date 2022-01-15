//---------------------------------------------------------------------------
#ifdef CssvMacro
#ifndef CompilerH
#define CompilerH
//---------------------------------------------------------------------------
bool MacroCompile(String *source, String fileName, TStringList *macroDirs,
                  TStringList *modules, bool showMessage);

bool MacroCompile(String fileName, TStringList *macroDirs, TStringList *modules,
                  bool showMessage);

String GetMacroModuleName(String fileName, String funcName, String argCount);
//---------------------------------------------------------------------------
#endif
#endif
