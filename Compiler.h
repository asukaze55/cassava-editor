//---------------------------------------------------------------------------
#ifndef CompilerH
#define CompilerH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
//---------------------------------------------------------------------------
class MacroContext {
public:
  std::vector<String> Dirs;
  std::map<String, TStream*> Modules;

  void AddDirectory(String Directory) {
    Dirs.push_back(Directory);
  }

  bool HasModule(String Name) {
    return Modules.count(Name) > 0;
  }

  ~MacroContext() {
    for (auto module : Modules) {
      delete module.second;
    }
  }
};

bool CompileMacro(String *source, String fileName, MacroContext *context,
                  bool showMessage);

bool CompileMacro(String fileName, MacroContext *context, bool showMessage);

String GetMacroModuleName(String fileName, String funcName, String argCount,
                          bool varArg);
//---------------------------------------------------------------------------
#endif
