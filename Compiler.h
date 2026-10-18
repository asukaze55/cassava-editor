//---------------------------------------------------------------------------
#ifndef CompilerH
#define CompilerH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
//---------------------------------------------------------------------------
enum class TMacroOperator : char {
  Unknown = '\0',
  IfThen = '?',  // Used in if, while.
  Goto = 'g',  // Clears stack. Used in if, while.
  Jump = 'j',  // Keeps stack. Used in ?:.
  Return = '}',  // return
  Cell = ']',  // [x,y]
  Not = '!',
  Minus = 'm',  // -
  Inc = 'i',  // ++
  Dec = 'd',  // --
  Eq = 'Q',  // ==
  Add = 'A',  // +=
  Subtract = 'S',  // -=
  Multiply = 'M',  // *=
  Divide = 'D',  // /=
  NEq = 'N',   // !=
  LEq = 'L',  // <=
  GEq = 'G',  // >=
  In = 'n',  // in
  ObjKey = 'k',  // :
  DefParam = 'p',  // Jump if the stack size >= specified size.
  VarArg = 'v'  // Converts stack to an ArrayLike object.
};
//---------------------------------------------------------------------------
class TByteVector {
private:
  std::vector<std::byte> Data;

public:
  size_t Size() const {
    return Data.size();
  }
  char ReadChar(size_t& Index) const {
    return static_cast<char>(Data[Index++]);
  }
  double ReadDouble(size_t& Index) const;
  int ReadInteger(size_t& Index) const;
  TMacroOperator ReadOperator(size_t& Index) const {
    return static_cast<TMacroOperator>(Data[Index++]);
  }
  String ReadString(size_t& Index) const;
  void Write(const TByteVector& Bytes);
  void WriteChar(char Value) {
    Data.push_back(static_cast<std::byte>(Value));
  }
  void WriteDouble(double Value);
  void WriteInteger(int Value);
  void WriteInteger(int Value, size_t Index) {
    std::memcpy(Data.data() + Index, &Value, sizeof(int));
  }
  void WriteString(const String& Value);
};
//---------------------------------------------------------------------------
class TMacroContext {
public:
  std::vector<String> Dirs;
  std::map<String, TByteVector> Modules;

  void AddDirectory(const String& Directory) {
    Dirs.push_back(Directory);
  }

  bool HasModule(const String& Name) const {
    return Modules.count(Name) > 0;
  }
};
//---------------------------------------------------------------------------
bool CompileMacro(const String* source, const String& fileName,
    TMacroContext *context, bool showMessage);

bool CompileMacro(const String& fileName, TMacroContext *context,
    bool showMessage);

String GetMacroModuleName(const String& fileName, const String& funcName,
    const String& argCount, bool varArg);
//---------------------------------------------------------------------------
#endif
