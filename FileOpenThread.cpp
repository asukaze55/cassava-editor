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
  TCsvGrid *FGrid;
  AnsiString FFileName;
  char *buf;
  unsigned long len;
  int x;
  int y;
  int maxx;
  TStringList *nextRow;
  void __fastcall UpdateNextCell();
  void __fastcall GridRefresh();
  void __fastcall UpdateWidthHeight();
  AnsiString __fastcall NormalizeCRLF(AnsiString Val);
protected:
  void __fastcall Execute();
public:
  __fastcall FileOpenThread(bool, TCsvGrid *, AnsiString,
                            char *, unsigned long);
  __fastcall virtual ~FileOpenThread();
  __property TCsvGrid *Grid = { read=FGrid, write=FGrid };
  __property AnsiString FileName = { read=FFileName, write=FFileName };
};
//---------------------------------------------------------------------------
TThread *ThreadFileOpen(TCsvGrid *AGrid, AnsiString AFileName,
                    char *buffer, unsigned long length)
{
  return new FileOpenThread(true, AGrid, AFileName, buffer, length);
}
//---------------------------------------------------------------------------
__fastcall FileOpenThread::FileOpenThread(bool CreateSuspended,
    TCsvGrid *AGrid, AnsiString AFileName, char *buffer, unsigned long length)
  : TThread(CreateSuspended), FGrid(AGrid), FFileName(AFileName),
    buf(buffer), len(length)
{
}
//---------------------------------------------------------------------------
__fastcall FileOpenThread::~FileOpenThread()
{
  if(buf){
    delete[] buf;
  }
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
  if(Grid->DefaultViewMode == 0){
    Grid->ShowAllColumn();
    Grid->SetHeight();
  }else{
    Grid->SetWidth();
    Grid->SetHeight();
  }
}
//---------------------------------------------------------------------------
AnsiString __fastcall FileOpenThread::NormalizeCRLF(AnsiString Val)
{
  int len = Val.Length();
  for(int i=len; i > 0; i--){
    if(Val[i] == '\n'){
      if(i == 1 || Val[i-1] != '\r'){
        Val.Insert("\r", i);
      }
    }else if(Val[i] == '\r'){
      if(i == len || Val[i+1] != '\n'){
        Val.Insert("\n", i+1);
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
  AnsiString data;
  if(buf[0]=='\xFF' && buf[1]=='\xFE'){
    data = AnsiString(buf+2, len-2);
  }else if(buf[0]=='\xFF'){
    data = AnsiString(buf+1, len-1);
  }else{
    data = AnsiString(buf, len);
  }
  CsvReader *reader = new CsvReader(typeOption, data);

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
