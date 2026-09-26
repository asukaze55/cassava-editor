//---------------------------------------------------------------------------
#ifndef UndoH
#define UndoH
//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <vector>
//---------------------------------------------------------------------------
String EscapeMacroString(const String& str);
//---------------------------------------------------------------------------
enum TUndoCommandType
{
  UNDO_MACRO = 0,
  UNDO_SET_CELL = 1
};
//---------------------------------------------------------------------------
class TUndoCommand
{
public:
  TUndoCommandType type;
  String undoData;
  String redoData;
  String recordedData;
  int x;
  int y;
  int right;
  int bottom;

  TUndoCommand(TUndoCommandType _type, const String& _undoData,
      const String& _redoData, const String& _recordedData, int _x = 0,
      int _y = 0, int _right = 0, int _bottom = 0);
  String GetUndoMacro() const;
  String GetRedoMacro() const;
  String GetRecordedMacro() const;
};
//---------------------------------------------------------------------------
class TUndoList
{
private:
  std::vector<TUndoCommand> list;
  int current;
  int lock;
  int recording;
  std::vector<int> group;

  String GetUndoMacro(int from) const;
  String GetRedoMacro(int from) const;
  String GetRecordedMacro(int from) const;

public:
  int MaxCount;

  TUndoList();
  void AddMacro(const String& undoMacro, const String& redoMacro,
      const String& recordedMacro = "");
  void ChangeCell(int x, int y, const String& from, const String& to, int right,
      int bottom);
  bool CanUndo();
  TUndoCommand* Undo();
  bool CanRedo();
  TUndoCommand* Redo();
  void Clear();
  void StartMacroRecording();
  void StopMacroRecording();
  String GetRecordedMacro() const;
  void Lock();
  void Unlock();
  void Push();
  void Pop();
  void Pop(const String& redoAndRecordedMacro);
  void Pop(const String& redoMacro, const String& recordedMacro);
  void PopWithRecordedMacro(const String& recordedMacro);
};
//---------------------------------------------------------------------------
#endif
