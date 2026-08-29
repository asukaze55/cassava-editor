//---------------------------------------------------------------------------
#ifndef CompilerH
#define CompilerH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
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
  void WriteString(String Value);
};
//---------------------------------------------------------------------------
class TMacroContext {
public:
  std::vector<String> Dirs;
  std::map<String, TByteVector> Modules;

  void AddDirectory(String Directory) {
    Dirs.push_back(Directory);
  }

  bool HasModule(String Name) const {
    return Modules.count(Name) > 0;
  }
};
//---------------------------------------------------------------------------
bool CompileMacro(String *source, String fileName, TMacroContext *context,
                  bool showMessage);

bool CompileMacro(String fileName, TMacroContext *context, bool showMessage);

String GetMacroModuleName(String fileName, String funcName, String argCount,
                          bool varArg);
//---------------------------------------------------------------------------
#endif
