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
  TEncoding *FEncoding;
  bool FIsDetectedEncoding;
  TList *allCells;
  int updatedRows;
  int maxCol;
  void __fastcall UpdateGrid();
  void __fastcall ShowError();
  String __fastcall NormalizeCRLF(String Val);
  bool __fastcall ExecuteOnce();
protected:
  void __fastcall Execute();
public:
  __fastcall FileOpenThread(TMainGrid *AGrid, String AFileName,
      TEncoding *AEncoding, bool AIsDetectedEncoding);
  __property TMainGrid *Grid = { read=FGrid, write=FGrid };
  __property String FileName = { read=FFileName, write=FFileName };
};
//---------------------------------------------------------------------------
TThread *ThreadFileOpen(TMainGrid *Grid, String FileName, TEncoding *Encoding,
    bool IsDetectedEncoding)
{
  return new FileOpenThread(Grid, FileName, Encoding, IsDetectedEncoding);
}
//---------------------------------------------------------------------------
__fastcall FileOpenThread::FileOpenThread(TMainGrid *AGrid, String AFileName,
    TEncoding *AEncoding, bool AIsDetectedEncoding)
  : TThread(true), FGrid(AGrid), FFileName(AFileName), FEncoding(AEncoding),
    FIsDetectedEncoding(AIsDetectedEncoding)
{
}
//---------------------------------------------------------------------------
String __fastcall FileOpenThread::NormalizeCRLF(String Val)
{
  int len = Val.Length();
  for (int i = len; i > 0; i--) {
    if (Val[i] == L'\n') {
      if (i == 1 || Val[i - 1] != L'\r') {
        Val.Insert(L"\r", i);
      }
    } else if (Val[i] == L'\r') {
      if (i == len || Val[i + 1] != L'\n') {
        Val.Insert(L"\n", i + 1);
      }
    }
  }
  return Val;
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::UpdateGrid()
{
  if (maxCol >= Grid->ColCount) {
    Grid->ChangeColCount(maxCol + 1);
  }
  int dt = updatedRows + Grid->DataTop;
  int maxRow = allCells->Count + dt - 1;
  if (maxRow >= Grid->RowCount) {
    Grid->RowCount = maxRow + 1;
  }
  Grid->SetDataRightBottom(maxCol, maxRow, true);
  for (int i = 0; i < allCells->Count; i++) {
    TStringList *row = static_cast<TStringList*>(allCells->Items[i]);
    int count = row->Count;
    for (int x = count; x <= Grid->ColCount; x++) {
      row->Add("");
    }
    Grid->Rows[i + dt] = row;
    Grid->SetRowDataRight(i + dt, count - 1);
    delete row;
  }
  Grid->SetWidth();
  Grid->SetHeight();

  updatedRows += allCells->Count;
  allCells->Clear();
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::ShowError()
{
  Application->MessageBox(
      L"ファイルの読み込みに失敗しました。", CASSAVA_TITLE, 0);
  Grid->Clear();
}
//---------------------------------------------------------------------------
bool __fastcall FileOpenThread::ExecuteOnce()
{
  int dl = Grid->DataLeft;
  CsvReader reader(Grid->TypeOption, FFileName, FEncoding);
  try {
    allCells = new TList();
    maxCol = 1;
    updatedRows = 0;
    TStringList *nextRow = new TStringList();
    if (dl) { nextRow->Add(""); }
    int x = dl;
    while (true) {
      int type = reader.GetNextType();
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
        if ((updatedRows == 0 && allCells->Count == 100)
            || allCells->Count % 1000000 == 0) {
          Synchronize(&UpdateGrid);
        }
      }
      nextRow->Add(NormalizeCRLF(reader.Next()));
      x++;
    }
  } catch (...) {
    delete allCells;
    return false;
  }
  Synchronize(&UpdateGrid);
  delete allCells;
  return true;
}
//---------------------------------------------------------------------------
void __fastcall FileOpenThread::Execute()
{
  bool ok = ExecuteOnce();
  if (!ok && FIsDetectedEncoding &&
      FEncoding->CodePage != TEncoding::Default->CodePage) {
    FEncoding = TEncoding::Default;
    Grid->Encoding = TEncoding::Default;
    Grid->AddBom = false;
    ok = ExecuteOnce();
  }
  if (!ok) {
    Synchronize(&ShowError);
  }
}
//---------------------------------------------------------------------------
