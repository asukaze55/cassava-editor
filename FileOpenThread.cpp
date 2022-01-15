//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FileOpenThread.h"
#include "CsvReader.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ���ӁF�قȂ�X���b�h�����L���� VCL �̃��\�b�h/�֐�/�v���p�e�B��
//       �ʂ̃X���b�h�̒����爵���ꍇ�C�r�������̖�肪�������܂��B
//
//       ���C���X���b�h�̏��L����I�u�W�F�N�g�ɑ΂��Ă� Synchronize
//       ���\�b�h���g�������ł��܂��B���̃I�u�W�F�N�g���Q�Ƃ��邽��
//       �̃��\�b�h���X���b�h�N���X�ɒǉ����CSynchronize ���\�b�h��
//       �����Ƃ��ēn���܂��B
//
//       ���Ƃ��� UpdateCaption ���ȉ��̂悤�ɒ�`���A
//
//      void __fastcall FileOpenThread::UpdateCaption()
//      {
//        Form1->Caption = "�X���b�h���珑�������܂���";
//      }
//
//       Execute ���\�b�h�̒��� Synchronize ���\�b�h�ɓn�����ƂŃ��C
//       ���X���b�h�����L���� Form1 �� Caption �v���p�e�B�����S�ɕ�
//       �X�ł��܂��B
//
//      Synchronize(&UpdateCaption);
//---------------------------------------------------------------------------
class FileOpenThread : public TThread
{
private:
  TMainGrid *FGrid;
  String FFileName;
  String Data;
  int x;
  int y;
  int maxx;
  TStringList *nextRow;
  void __fastcall UpdateNextCell();
  void __fastcall GridRefresh();
  void __fastcall UpdateWidthHeight();
  String __fastcall NormalizeCRLF(String Val);
protected:
  void __fastcall Execute();
public:
  __fastcall FileOpenThread(bool, TMainGrid *, String, String);
  __property TMainGrid *Grid = { read=FGrid, write=FGrid };
  __property String FileName = { read=FFileName, write=FFileName };
};
//---------------------------------------------------------------------------
TThread *ThreadFileOpen(TMainGrid *AGrid, String AFileName, String AData)
{
  return new FileOpenThread(true, AGrid, AFileName, AData);
}
//---------------------------------------------------------------------------
__fastcall FileOpenThread::FileOpenThread(bool CreateSuspended,
    TMainGrid *AGrid, String AFileName, String AData)
  : TThread(CreateSuspended), FGrid(AGrid), FFileName(AFileName), Data(AData)
{
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::UpdateNextCell()
{
  if(Terminated){
    return;
  }
  ::SendMessage(Grid->Handle, WM_SETREDRAW, 0, 0);
  if(x >= Grid->ColCount){
    Grid->ColCount = x + 1;
  }
  if(y >= Grid->RowCount){
    Grid->RowCount = y + 1;
  }
  Grid->Rows[y] = nextRow;
  ::SendMessage(Grid->Handle, WM_SETREDRAW, 1, 0);
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::GridRefresh()
{
  if(Terminated){
    return;
  }
  Grid->SetDataRightBottom(maxx, y, false);
  ::SendMessage(Grid->Handle, WM_SETREDRAW, 1, 0);
  Grid->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::UpdateWidthHeight()
{
  if(Terminated){
    return;
  }
  Grid->SetDataRightBottom(maxx, y, true);
  ::SendMessage(Grid->Handle, WM_SETREDRAW, 1, 0);
  Grid->SetWidth();
  Grid->SetHeight();
  Grid->Invalidate();
}
//---------------------------------------------------------------------------
String __fastcall FileOpenThread::NormalizeCRLF(String Val)
{
  int len = Val.Length();
  for(int i=len; i > 0; i--){
    if(Val[i] == TEXT('\n')){
      if(i == 1 || Val[i-1] != TEXT('\r')){
        Val.Insert(TEXT("\r"), i);
      }
    }else if(Val[i] == TEXT('\r')){
      if(i == len || Val[i+1] != TEXT('\n')){
        Val.Insert(TEXT("\n"), i+1);
      }
    }
  }
  return Val;
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::Execute()
{
  TTypeOption *typeOption = Grid->TypeOption;
  int dt = Grid->DataTop;
  int dl = Grid->DataLeft;
  if (Data.Length() > 0) {
    if (Data[1] == TEXT('\xFEFF') || Data[1] == TEXT('\xFFFE')) {
      Data.Delete(1, 1);
    }
  }
  CsvReader *reader = new CsvReader(typeOption, Data);

  y = dt;
  x = dl;
  maxx = 1;
  nextRow = new TStringList();
  if(dl){ nextRow->Add(""); }
  while (true) {
    int type = reader->GetNextType();
    if (type == NEXT_TYPE_END_OF_FILE) {
      if(nextRow->Count > dl + 1 ||
         (nextRow->Count == dl + 1 && nextRow->Strings[dl] != "")){
        if(x - 1 > maxx){ maxx = x - 1; }
        Synchronize(&UpdateNextCell);
        y++;
      }
      break;
    } else if (type == NEXT_TYPE_HAS_MORE_ROW) {
      if(Terminated){
        break;
      }
      if(x - 1 > maxx){ maxx = x - 1; }
      Synchronize(&UpdateNextCell);
      nextRow->Clear();
      if(dl){ nextRow->Add(""); }
      if (y == Grid->TopRow + Grid->VisibleRowCount){
        Synchronize(&GridRefresh);
      } else if (!(y & 0x07ff)) {
        Synchronize(&GridRefresh);
      }
      y++;
      x = dl;
    }
    nextRow->Add(NormalizeCRLF(reader->Next()));
    x++;
  }
  delete nextRow;
  delete reader;
  y--;
  Synchronize(&UpdateWidthHeight);
}
//---------------------------------------------------------------------------
