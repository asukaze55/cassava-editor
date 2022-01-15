//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FileOpenThread.h"
#include "CsvReader.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// 注意：異なるスレッドが所有する VCL のメソッド/関数/プロパティを
//       別のスレッドの中から扱う場合，排他処理の問題が発生します。
//
//       メインスレッドの所有するオブジェクトに対しては Synchronize
//       メソッドを使う事ができます。他のオブジェクトを参照するため
//       のメソッドをスレッドクラスに追加し，Synchronize メソッドの
//       引数として渡します。
//
//       たとえば UpdateCaption を以下のように定義し、
//
//      void __fastcall FileOpenThread::UpdateCaption()
//      {
//        Form1->Caption = "スレッドから書き換えました";
//      }
//
//       Execute メソッドの中で Synchronize メソッドに渡すことでメイ
//       ンスレッドが所有する Form1 の Caption プロパティを安全に変
//       更できます。
//
//      Synchronize(&UpdateCaption);
//---------------------------------------------------------------------------
class FileOpenThread : public TThread
{
private:
  TMainGrid *FGrid;
  String FFileName;
  String Data;
  TList *allCells;
  int maxCol;
  void __fastcall UpdateGrid();
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
void __fastcall FileOpenThread::UpdateGrid()
{
  if (maxCol >= Grid->ColCount) {
    Grid->ColCount = maxCol + 1;
  }
  int dt = Grid->DataTop;
  int maxRow = allCells->Count + dt - 1;
  if (maxRow >= Grid->RowCount) {
    Grid->RowCount = maxRow + 1;
  }
  Grid->SetDataRightBottom(maxCol, maxRow, true);
  for (int i = 0; i < allCells->Count; i++) {
    Grid->Rows[i + dt] = static_cast<TStringList*>(allCells->Items[i]);
  }
  Grid->SetWidth();
  Grid->SetHeight();
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::Execute()
{
  TTypeOption *typeOption = Grid->TypeOption;
  int dl = Grid->DataLeft;
  if (Data.Length() > 0) {
    bool bom = (Data[1] == TEXT('\xFEFF') || Data[1] == TEXT('\xFFFE'));
    if (bom) {
      Data.Delete(1, 1);
    }
    Grid->AddBom = bom;
  }
  CsvReader *reader = new CsvReader(typeOption, Data);

  allCells = new TList();
  maxCol = 1;
  TStringList *nextRow = new TStringList();
  if (dl) { nextRow->Add(""); }
  int x = dl;
  while (true) {
    int type = reader->GetNextType();
    if (type == NEXT_TYPE_END_OF_FILE) {
      if (nextRow->Count > dl + 1 ||
          (nextRow->Count == dl + 1 && nextRow->Strings[dl] != "")) {
        if (x - 1 > maxCol) { maxCol = x - 1; }
        allCells->Add(nextRow);
      }
      break;
    } else if (type == NEXT_TYPE_HAS_MORE_ROW) {
      if (Terminated) {
        break;
      }
      if (x - 1 > maxCol) { maxCol = x - 1; }
      allCells->Add(nextRow);
      nextRow = new TStringList();
      if (dl) { nextRow->Add(""); }
      x = dl;
      if (allCells->Count == 100) {
        Synchronize(&UpdateGrid);
      }
    }
    nextRow->Add(NormalizeCRLF(reader->Next()));
    x++;
  }
  delete reader;
  Synchronize(&UpdateGrid);
  for (int i = 0; i < allCells->Count; i++) {
    delete static_cast<TStringList*>(allCells->Items[i]);
  }
  delete allCells;
}
//---------------------------------------------------------------------------
