//---------------------------------------------------------------------------
#ifndef UndoH
#define UndoH
//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <vector>
//---------------------------------------------------------------------------
String EscapeMacroString(String str);
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

  TUndoCommand(TUndoCommandType _type, String _undoData, String _redoData,
      String _recordedData, int _x = 0, int _y = 0, int _right = 0,
      int _bottom = 0);
  String GetUndoMacro();
  String GetRedoMacro();
  String GetRecordedMacro();
};
//---------------------------------------------------------------------------
class TUndoList
{
private:
  TList* list;
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
  ~TUndoList();
  void AddMacro(String undoMacro, String redoMacro, String recordedMacro = "");
  void ChangeCell(int x, int y, String from, String to, int right, int bottom);
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
  void Pop(String redoAndRecordedMacro);
  void Pop(String redoMacro, String recordedMacro);
  void PopWithRecordedMacro(String recordedMacro);
};
//---------------------------------------------------------------------------
#endif
