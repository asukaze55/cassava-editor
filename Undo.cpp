//---------------------------------------------------------------------------
#pragma hdrstop
#include "Undo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
inline TUndoCommand* GetCommand(TList* list, int index)
{
  return static_cast<TUndoCommand*>(list->Items[index]);
}
//---------------------------------------------------------------------------
void SetCount(TList* list, int count)
{
  for (int i = list->Count - 1; i >= count; i--) {
    delete GetCommand(list, i);
    list->Delete(i);
  }
}
//---------------------------------------------------------------------------
wchar_t toHex(int value)
{
  return value < 10 ? (L'0' + value) : (L'a' + value - 10);
}
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
String EscapeMacroString(String str)
{
  String escaped = "";
  for (int i = 1; i <= str.Length(); i++) {
    wchar_t c = str[i];
    if (c == '\\') {
      escaped += "\\\\";
    } else if (c == '\"') {
      escaped += "\\\"";
    } else if (c == '\r') {
      continue;
    } else if (c == '\n') {
      escaped += "\\n";
    } else if (c == '\t') {
      escaped += "\\t";
    } else if (c < 32) {
      escaped += (String)"\\x" + toHex(c / 16) + toHex(c % 16);
    } else {
      escaped += c;
    }
  }
  return escaped;
}
//---------------------------------------------------------------------------
TUndoCommand::TUndoCommand(TUndoCommandType _type, String _undoData,
    String _redoData, String _recordedData, int _x, int _y, int _right,
    int _bottom)
    : type(_type), undoData(_undoData), redoData(_redoData),
      recordedData(_recordedData), x(_x), y(_y), right(_right), bottom(_bottom)
{}
//---------------------------------------------------------------------------
String TUndoCommand::GetUndoMacro()
{
  if (type == UNDO_MACRO) {
    return undoData;
  }
  if (right < x || bottom < y) {
    return (String)"Right = " + right + ";\nBottom = " + bottom + ";\n";
  }
  return (String)"[" + x + "," + y + "] = \"" + EscapeMacroString(undoData)
      + "\";";
}
//---------------------------------------------------------------------------
String TUndoCommand::GetRedoMacro()
{
  if (type == UNDO_MACRO) {
    return redoData;
  }
  return (String)"[" + x + "," + y + "] = \"" + EscapeMacroString(redoData)
      + "\";";
}
//---------------------------------------------------------------------------
String TUndoCommand::GetRecordedMacro()
{
  return recordedData != "" ? recordedData : GetRedoMacro();
}
//---------------------------------------------------------------------------
TUndoList::TUndoList()
    : list(new TList), current(0), lock(0), recording(INT_MAX), MaxCount(100) {}
//---------------------------------------------------------------------------
TUndoList::~TUndoList()
{
  SetCount(list, 0);
  delete list;
}
//---------------------------------------------------------------------------
void TUndoList::AddMacro(String undoMacro, String redoMacro,
                         String recordedMacro)
{
  if (lock > 0) {
    return;
  }
  SetCount(list, current);
  list->Add(new TUndoCommand(UNDO_MACRO, undoMacro, redoMacro,
      (recording < INT_MAX) ? recordedMacro : (String)""));
  if (group.empty() && current >= MaxCount && recording > 0) {
    list->Delete(0);
    if (recording < INT_MAX) {
      recording--;
    }
  } else {
    current++;
  }
}
//---------------------------------------------------------------------------
void TUndoList::ChangeCell(int x, int y, String from, String to, int right,
                           int bottom)
{
  if (lock > 0 || from == to) {
    return;
  }
  SetCount(list, current);
  if (current > 0 && recording != current
      && (group.empty() || group.back() != current)) {
    TUndoCommand* command = GetCommand(list, current - 1);
    if (command->type == UNDO_SET_CELL && command->x == x && command->y == y) {
      command->redoData = to;
      return;
    }
  }
  list->Add(new TUndoCommand(UNDO_SET_CELL, from, to, "", x, y, right, bottom));

  if (group.empty() && current >= MaxCount && recording > 0) {
    list->Delete(0);
    if (recording < INT_MAX) {
      recording--;
    }
  } else {
    current++;
  }
}
//---------------------------------------------------------------------------
bool TUndoList::CanUndo()
{
  return current > 0;
}
//---------------------------------------------------------------------------
TUndoCommand* TUndoList::Undo()
{
  while (current > 0) {
    TUndoCommand* command = GetCommand(list, --current);
    if (command->type == UNDO_SET_CELL || command->undoData != "") {
      return command;
    }
  }
  return nullptr;
}
//---------------------------------------------------------------------------
bool TUndoList::CanRedo()
{
  return current < list->Count;
}
//---------------------------------------------------------------------------
TUndoCommand* TUndoList::Redo()
{
  while (current < list->Count) {
    TUndoCommand* command = GetCommand(list, current++);
    if (command->type == UNDO_SET_CELL || command->redoData != "") {
      return command;
    }
  }
  return nullptr;
}
//---------------------------------------------------------------------------
void TUndoList::Clear()
{
  SetCount(list, 0);
  current = 0;
}
//---------------------------------------------------------------------------
String TUndoList::GetUndoMacro(int from) const
{
  if (from >= current) {
    return "";
  }
  String macro = GetCommand(list, current - 1)->GetUndoMacro();
  for (int i = current - 2; i >= from; i--) {
    macro += "\n" + GetCommand(list, i)->GetUndoMacro();
  }
  return macro;
}
//---------------------------------------------------------------------------
String TUndoList::GetRedoMacro(int from) const
{
  if (from >= current) {
    return "";
  }
  String macro = GetCommand(list, from)->GetRedoMacro();
  for (int i = from + 1; i < current; i++) {
    macro += GetCommand(list, i)->GetRedoMacro();
  }
  return macro;
}
//---------------------------------------------------------------------------
String TUndoList::GetRecordedMacro(int from) const
{
  if (from >= current) {
    return "";
  }
  String macro = GetCommand(list, from)->GetRecordedMacro();
  for (int i = from + 1; i < current; i++) {
    macro += "\n" + GetCommand(list, i)->GetRecordedMacro();
  }
  return macro;
}
//---------------------------------------------------------------------------
void TUndoList::StartMacroRecording()
{
  recording = current;
}
//---------------------------------------------------------------------------
void TUndoList::StopMacroRecording()
{
  recording = INT_MAX;
}
//---------------------------------------------------------------------------
String TUndoList::GetRecordedMacro() const
{
  return GetRecordedMacro(recording);
}
//---------------------------------------------------------------------------
void TUndoList::Lock()
{
  lock++;
}
//---------------------------------------------------------------------------
void TUndoList::Unlock()
{
  lock--;
}
//---------------------------------------------------------------------------
void TUndoList::Push()
{
  group.push_back(current);
}
//---------------------------------------------------------------------------
void TUndoList::Pop()
{
  int prev = group.back();
  Pop(GetRedoMacro(prev),
      GetRecordedMacro(recording > prev ? recording : prev));
}
//---------------------------------------------------------------------------
void TUndoList::Pop(String redoAndRecordedMacro)
{
  Pop(redoAndRecordedMacro, redoAndRecordedMacro);
}
//---------------------------------------------------------------------------
void TUndoList::Pop(String redoMacro, String recordedMacro)
{
  int prev = group.back();
  group.pop_back();

  String undoMacro = GetUndoMacro(prev);
  if (recording > prev && recording < INT_MAX) {
    recording = prev;
  }
  if (prev >= current && (recording == INT_MAX || recordedMacro == "")) {
    return;
  }
  current = prev;
  AddMacro(undoMacro, redoMacro, recordedMacro);
}
//---------------------------------------------------------------------------
void TUndoList::PopWithRecordedMacro(String recordedMacro)
{
  Pop(GetRedoMacro(group.back()), recordedMacro);
}
//---------------------------------------------------------------------------
