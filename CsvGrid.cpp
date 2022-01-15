//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <vcl\clipbrd.hpp>
#include <shellapi.h>
#include <string.h>
#include <process.h>
#pragma hdrstop

#include "PasteDlg.h"
#pragma link "PasteDlg.obj"
#include "CsvGrid.h"
#include "AutoOpen.h"

#define min(X,Y) ((X)<(Y) ? (X) : (Y))
#define max(X,Y) ((X)>(Y) ? (X) : (Y))
#define DataLeft (ShowRowCounter ? 1 : 0)
#define DataTop  (ShowColCounter ? 1 : 0)
#define soNone 0
#define soNormal 1
#define soString 2
#define soAll 3
#define STRDEOF "\t[EOF]"

bool SameCellClicking;
//---------------------------------------------------------------------------
static inline TCsvGrid *ValidCtrCheck()
{
    return new TCsvGrid(NULL);
}
//---------------------------------------------------------------------------
__fastcall TCsvGrid::TCsvGrid(TComponent* Owner)  //デフォルトの設定
    : TStringGrid(Owner)
{
  Options = Options << goRowSizing << goColSizing << goRowMoving << goColMoving
		    << goDrawFocusSelected  << goThumbTracking << goTabs
		    << goEditing << goAlwaysShowEditor << goRangeSelect;
  AlwaysShowEditor = true;
  DefaultColWidth = 64;
  DefaultRowHeight = 24;
  Dragging = false;
  EditorMode = true;
  DragMove = true;
  TypeIndex = 0;
  TypeOption = TypeList.DefItem();
//  SepChars = ",\t";
//  WeakSepChars = " ";
//  QuoteChars = "\"";
//  SaveOption = soNormal;
  PasteOption = -1;
  DefWay = 2;
  FShowRowCounter = true;
  FShowColCounter = true;
  UndoSetLock = 0;
  TextAlignment = cssv_taLeft;
  WheelMoveCursol = false;
  SameCellClicking = false;
}
//---------------------------------------------------------------------------
AnsiString FormatNumComma(AnsiString Str, int Count)
{
  if(Str == "" || Count <= 0) return Str;
  int len = Str.Length();
  int L, R;
  int i = 1;
  for(; i<=len; i++){
    if(Str[i] >= '0' && Str[i] <= '9') break;
  }
  L = i;
  for(; i<=len; i++){
    if(Str[i] < '0' || Str[i] > '9') break;
  }
  R = i;
  for(i = R - Count; i > L; i -= Count){
    Str.Insert(",", i);
  }
  return Str;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DrawCell(int ACol, int ARow,
  const TRect &ARect, TGridDrawState AState)
{
//  TStringGrid::DrawCell(ACol, ARow, ARect, AState);
  AnsiString Str = Cells[ACol][ARow];
  bool isnum = ((TextAlignment == cssv_taNumRight) || (NumberComma > 0))
             && IsNumber(Str);
  if(isnum && (NumberComma > 0)){
    Str = FormatNumComma(Str, NumberComma);
  }
  int nl, t=ARect.Top + 2, tint = Canvas->TextHeight(Str) + 4;
  TRect R;
  R.Left = ARect.Left + 2;
  R.Right = ARect.Right - 2; R.Bottom = ARect.Bottom - 2;
  if((TextAlignment == cssv_taNumRight) && isnum){
    int L = R.Right - Canvas->TextWidth(Str);
    if(R.Left < L) R.Left = L;
  }

  while((nl = Str.AnsiPos("\n")) > 0){
    R.Top = t;
    Canvas->TextRect(R, R.Left, t, Str.SubString(1, nl-1));
    Str.Delete(1, nl);
    t += tint;
    if(t >= R.Bottom) return;
  }
  R.Top = t;
  if(TypeOption->DummyEof && Str == STRDEOF){
    TColor CFC = Canvas->Font->Color;
    Canvas->Font->Color = clGray;
    Canvas->TextRect(R, R.Left, t, "[EOF]");
    Canvas->Font->Color = CFC;
  }else if(ShowURLBlue && Str.SubString(1,7) == "http://"){
    TFontStyles CFS = Canvas->Font->Style;
    TColor CFC = Canvas->Font->Color;
    Canvas->Font->Style = TFontStyles() << fsUnderline;
    Canvas->Font->Color = clBlue;
    Canvas->TextRect(R, R.Left, t, Str);
    Canvas->Font->Style = CFS;
    Canvas->Font->Color = CFC;
  }else{
    Canvas->TextRect(R, R.Left, t, Str);
  }
}
//---------------------------------------------------------------------------
//  基本入出力メソッド
//
AnsiString TCsvGrid::GetACells(int ACol, int ARow){
  int c = ACol+DataLeft-1;
  int r = ARow+DataTop-1;
  if(ACol<=0 || ARow<=0 || c<0 || c>=ColCount || r<0 || r>=RowCount)
    return "";
  return Cells[c][r];
}
//---------------------------------------------------------------------------
void TCsvGrid::Clear(int AColCount, int ARowCount)
{
  Row = FixedRows;  Col = FixedCols;
  DefaultColWidth = 64;               //列の幅を64に戻す
  ColWidths[0] = 32;
  DefaultRowHeight = Font->Size * Screen->PixelsPerInch / 48;  //行の高さは文字サイズの２倍
  for(int i=DataTop; i <= RowCount; i++)    //内容のクリア
    Rows[i]->Clear();
  ChangeColCount(AColCount+1);  ChangeRowCount(ARowCount+1);
  ClearUndo();
  Modified = false;
  ReNum();
  TypeIndex = 0;
  TypeOption = TypeList.DefItem();
}
//---------------------------------------------------------------------------
void TCsvGrid::ReNum()
{
  int DR = DataRight+1-DataLeft;
  int DB = DataBottom+1-DataTop;
  if(FShowRowCounter){
    for(int i=1; i <= DB; i++)
      ACells[0][i] = (AnsiString)i;
    for(int i=DB+1; i <= RowCount; i++)
      ACells[0][i] = "";
  }
  if(FShowColCounter){
    for(int i=1; i <= DR; i++)
      ACells[i][0] = (AnsiString)i;
    for(int i=DR+1; i <= ColCount; i++)
      ACells[i][0] = "";
  }
  Refresh();
}
//---------------------------------------------------------------------------
void TCsvGrid::SetShowRowCounter(bool Value)
{
  if(Value != FShowRowCounter){
    FShowRowCounter = Value;
    bool M = Modified;

    int C, R;
    if(RangeSelect){
      C = Selection.Left;
      R = Selection.Top;
      Col=C; Row=R;
    } else { C=Col; R=Row;}
    if(AlwaysShowEditor){
      Options << goEditing << goAlwaysShowEditor;
    }else{
      Options << goEditing;
    }

    if(Value){
      FixedCols = 1;
      Col=C; Row=R;
      InsertColumn(0); ColWidths[0] = 32;
    } else {
      if(Col <= FixedCols) Col = FixedCols + 1;
      if(ColCount <= FixedCols+1) FShowRowCounter = true;
      else  DeleteColumn(0);
    }
    Modified = M;
    if(FShowRowCounter || ShowColCounter) Cells[0][0] = "";
    ReNum();
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::SetShowColCounter(bool Value)
{
  if(Value != FShowColCounter){
    FShowColCounter = Value;
    bool M = Modified;

    int C, R;
    if(RangeSelect){
      C = Selection.Left;
      R = Selection.Top;
      Col=C; Row=R;
    } else { C=Col; R=Row;}
    if(AlwaysShowEditor){
      Options << goEditing << goAlwaysShowEditor;
    }else{
      Options << goEditing;
    }

    if(Value){
      FixedRows = 1;
      Col=C; Row=R;
      InsertRow(0);
    } else {
      if(Row <= FixedRows)        Row = FixedRows + 1;
      if(RowCount <= FixedRows+1) FShowColCounter = true;
      else  DeleteRow(0);
    }
    Modified = M;
    if(FShowRowCounter || ShowColCounter) Cells[0][0] = "";
    ReNum();
  }
}
//---------------------------------------------------------------------------
int TCsvGrid::TextWidth(TCanvas *cnvs, AnsiString str)
{
  if((NumberComma > 0) && IsNumber(str)){
    str = FormatNumComma(str, NumberComma);
  }
  return cnvs->TextWidth(str);
}

//---------------------------------------------------------------------------
void TCsvGrid::SetWidth(int i)
{
  int WMax = 0;                     //その列で幅の最大値
  if(i==0 && ShowRowCounter){
    ColWidths[i] =
      max(32, TextWidth(Canvas, Cells[0][DataBottom]) + 4);
    return;
  }
  for(int j=DataTop; j < RowCount; j++)
    WMax = max(WMax, TextWidth(Canvas, Cells[i][j]));
  WMax += 4;
  if( WMax < 32 ) WMax = 32;        //狭すぎないように
  else if( WMax >= ClientWidth - ColWidths[0])         //広すぎないように
    WMax = ClientWidth - ColWidths[0] - 2 * GridLineWidth;
  ColWidths[i] = WMax;
}
//---------------------------------------------------------------------------
void TCsvGrid::SetWidth()             //列幅の最適化
{
  for(int i=0; i < ColCount; i++)
  {
    SetWidth(i);
  }

  DefaultViewMode = cssv_viewalltext;
}
//---------------------------------------------------------------------------
void TCsvGrid::SetHeight(int j)
{
  int HMax = DefaultRowHeight;
  if(j==0 && ShowColCounter){
    RowHeights[j] = DefaultRowHeight;
    return;
  }
  for(int i=DataLeft; i < ColCount; i++){
    int MultiLine = 1;
    for(int k=1; k<Cells[i][j].Length(); k++){
      if(Cells[i][j][k] == '\n'){ MultiLine++; }
    }
    HMax = max(HMax, (Canvas->TextHeight("あ")+4) * MultiLine);
  }
  if(HMax >= (ClientHeight - RowHeights[0]) / 2){
    HMax = (ClientHeight - RowHeights[0]) / 2;
  }
  RowHeights[j] = HMax;
}
//---------------------------------------------------------------------------
void TCsvGrid::SetHeight()
{
  for(int i=0; i < RowCount; i++)
  {
    SetHeight(i);
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::CompactWidth(int *Widths, int WindowSize, int Minimum,
                            TCanvas *Cnvs)
{
  int CC = DataRight - DataLeft + 1;
  int RC = DataBottom - DataTop + 1;
  int BigColSize = 0;
  int Room = WindowSize - (Minimum*CC);
  if(!Cnvs) Cnvs = Canvas;

  for(int i=1; i<=CC; i++){
    int AWMax = 0;                     //その列で幅の最大値
    for(int j=1; j<=RC; j++)
      AWMax = max(AWMax, TextWidth(Cnvs, ACells[i][j]));
    Widths[i] = max((AWMax + 4 - Minimum), 0);
    BigColSize += Widths[i];
  }

  if(BigColSize > Room){
    if(Room <= Minimum){
      for(int i=1; i<=CC; i++) Widths[i] = Minimum;
    }else {
      double Zoom = (double)Room / BigColSize;
      for(int i=1; i<=CC; i++){
        Widths[i] = (int)(Zoom * Widths[i]) + Minimum;
      }
    }
  }else{
    for(int i=1; i<=CC; i++) Widths[i] += Minimum;
  }

  DefaultViewMode = cssv_viewallcolumn;
}
//---------------------------------------------------------------------------
void TCsvGrid::ShowAllColumn()
{
  int R = DataRight;
  int *WMax = new int[ColCount+2];
  int WindowSize = ClientWidth - 16; // スクロールバー分ね

  if(ShowRowCounter){ // カウンタセル
    WMax[0] = max(32, TextWidth(Canvas, Cells[0][DataBottom]) + 4);
    ColWidths[0] = WMax[0];
    WindowSize -= WMax[0];
  }

  CompactWidth(WMax, WindowSize, 32);
  for(int i=DataLeft; i<=R; i++) ColWidths[i] = WMax[i-DataLeft+1];
  delete[] WMax;
}
//---------------------------------------------------------------------------
int TCsvGrid::GetDataRight()
{
  int i,j;
  for(i=ColCount-1; i>=DataLeft; i--)
    for(j=RowCount-1; j>=DataTop; j--){
      if(TypeOption->DummyEof && Cells[i][j]==STRDEOF){
         return (i-1);
      }
      if(Cells[i][j] != "") { return i; }
    }
  return DataLeft;
}
//---------------------------------------------------------------------------
int TCsvGrid::GetDataBottom()
{
  int i,j;
  for(j=RowCount-1; j>=DataTop; j--)
    for(i=ColCount-1; i>=DataLeft; i--){
      if(TypeOption->DummyEof && Cells[i][j]==STRDEOF){
         return (j-1);
      }
      if(Cells[i][j] != ""){ return j; }
    }
  return DataTop;
}
//---------------------------------------------------------------------------
void TCsvGrid::Cut()                  //右、下の余計な項目を削除
{
  ChangeColCount(max(5,DataRight+2));
  ChangeRowCount(max(5,DataBottom+2));
}
//---------------------------------------------------------------------------
void TCsvGrid::SpreadRightBottom()
{
  if(Col == ColCount-1 && Cells[Col][Row] != "")
  { ChangeColCount(ColCount+1); ReNum(); }
  if(Row == RowCount-1 && Cells[Col][Row] != "")
  { ChangeRowCount(RowCount+1); ReNum(); }
}
//---------------------------------------------------------------------------
char GetReturnCode(char *s)
{
  while(*s != '\x0D' && *s != '\x0A' && *s != '\0') s++;
  if(*s == '\x0D' && *(s+1) == '\x0A') return LFCR;
  else if(*s == '\x0D' || *s == '\x0A') return *s;
  return LFCR;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::SetDragDropAccept(bool Accept){
    ::DragAcceptFiles(Handle,Accept);
    FDragDropAccept=Accept;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DropCsvFiles(TWMDropFiles inMsg)
{
  int DFiles = DragQueryFile((HDROP)inMsg.Drop, 0xffffffff, (LPSTR) NULL,255);
  AnsiString *FileNames;
  FileNames = new AnsiString[DFiles];
  char theFileName[255];
  for(int i=0;i<DFiles;i++){
    ::DragQueryFile((HDROP)inMsg.Drop, i, theFileName,255);
    FileNames[i]=theFileName;
  }
  if(FOnDropFiles != NULL ){
    FOnDropFiles( this,DFiles,FileNames);
  }
  delete FileNames;
}
//---------------------------------------------------------------------------
bool TCsvGrid::LoadFromFile(AnsiString FileName, int KCode)
{
  char *buf, *sjis;
  int filelength;
  unsigned long sjislen;
  TFileStream *File = new TFileStream(FileName, fmOpenRead|fmShareDenyNone);
  filelength = File->Size;
  if(filelength > 1048576){
    if(Application->MessageBox(
      ((AnsiString)"ファイルサイズが " + filelength / 1048576
       +" MB あります。\n処理を続行しますか？").c_str(),
      ExtractFileName(FileName).c_str(),
      MB_YESNO) == IDNO){
        return false;
    }
  }
  Clear();
  buf = new char[filelength+1];
  int r = File->Read(buf, File->Size);
  buf[r] = '\0';
  delete File;

  if(!CheckKanji && KCode == CHARCODE_AUTO){ KCode = CHARCODE_SJIS; }
  if(KCode == CHARCODE_SJIS){
    sjis = buf;
  }else{
    sjis = new char[filelength+1];
  }

  switch(KCode){
  case CHARCODE_AUTO:
    KCode = NkfConvertSafe(sjis, filelength+1, &sjislen, buf, filelength+1, "-xs");
    break;
  case CHARCODE_SJIS:
    sjislen = filelength;
    break;
  case CHARCODE_EUC:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, filelength+1, "-Exs");
    break;
  case CHARCODE_JIS:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, filelength+1, "-Jxs");
    break;
  case CHARCODE_UTF8:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, filelength+1, "-Wxs");
    break;
  case CHARCODE_UTF16LE:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, filelength+1, "-W16Lxs");
    break;
  case CHARCODE_UTF16BE:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, filelength+1, "-W16Bxs");
    break;
  }
  KanjiCode = KCode;
  if(sjis != buf) delete[] buf;
  ReturnCode = GetReturnCode(sjis);
  TStringList *List = new TStringList;
  if(sjis[0]=='\xFF' && sjis[1]=='\xFE'){
    List->Text = AnsiString(sjis+2, sjislen-2);
  }else if(sjis[0]=='\xFF'){
    List->Text = AnsiString(sjis+1, sjislen-1);
  }else{
    List->Text = AnsiString(sjis, sjislen);
  }
  delete[] sjis;
  AnsiString Ext = ExtractFileExt(FileName);
  if(Ext.Length() > 1 && Ext[1]=='.'){ Ext.Delete(1,1); }
  TypeIndex = TypeList.IndexOf(Ext);
  TypeOption = TypeList.Items(TypeIndex);
//  CommaSeparated = false; TabSeparated = false;
  PasteCSV(List, DataLeft, DataTop, -1, 0, 0);
  delete List;
//  if(!CommaSeparated && TabSeparated) SaveFormat = sfTSV;
//  else                                SaveFormat = sfCSV;
  if(DefaultViewMode == 0){
    ShowAllColumn();
  }else{
    SetWidth();
  }
  Modified = false;
  return true;
}
//---------------------------------------------------------------------------
void TCsvGrid::PasteCSV(TStrings *List , int Left , int Top , int Way ,
			int ClipCols , int ClipRows)
{
  //Way
  // -1: 新規グリッド
  //  0: 重なった部分のみ
  //  1: くり返し処理
  //  2: データサイズ上書き
  //  3: 右挿入
  //  4: 下挿入
  //  5: テキストとしてセル内に
  if((Way == 2 || Way == -1) && RowCount <= List->Count + Top){
    ChangeRowCount(List->Count + Top + 1);   //行数が足りなければ増やす
  }else if(Way == 3){
    if(RowCount <= ClipRows + Top)  ChangeRowCount(ClipRows + Top + 1);
    InsertCells_Right(Left, Left + ClipCols - 1, Top, Top + ClipRows - 1);
  }else if(Way == 4){
    if(ColCount <= ClipCols + Left) ChangeColCount(ClipCols + Left + 1);
    InsertCells_Down(Left, Left + ClipCols - 1, Top, Top + ClipRows - 1);
  }else if(Way == 5){
    ShowEditor();
    InplaceEditor->PasteFromClipboard();
    InplaceEditor->Text = TrimRight(InplaceEditor->Text);
    return;
  }
  TStringList *OneRow = new TStringList;
  int iEnd = List->Count;
  if(Way == 0) iEnd = min((int)Selection.Bottom - Top + 1 , iEnd);
  else if(Way == 1) iEnd = Selection.Bottom - Top + 1;
  int linecont = 0;
  int MultiLine = 0;
  int colcount = 0;
  for(int i=0; i < iEnd; i++)
  {
    int lc = linecont;
    int ii= i;
    if(Way == 1 && ClipRows>0) ii = i % ClipRows;
    if(lc) List->Strings[ii] = (AnsiString)"\"" + List->Strings[ii];
    linecont = SetCsv(OneRow, List->Strings[ii]); //カンマ区切りをリターン区切りに
    if((Way == -1 || Way > 1) && OneRow->Count + Left >= ColCount)
      ChangeColCount(OneRow->Count + Left + 1);   //列数が足りなければ増やす
    if(Way == -1)
    {
       if(lc){
        Top--;
        MultiLine++;
        ii = i + Top;
        Cells[lc][ii] = Cells[lc][ii] + "\r\n";
        if(OneRow->Count + lc >= ColCount){
          ChangeColCount(OneRow->Count + lc + 1);
        }
        colcount = max(colcount, OneRow->Count + lc);
        for(int j=0; j<OneRow->Count; j++){
          if(j==0) Cells[lc][ii] = Cells[lc][ii] + OneRow->Strings[0];
          else Cells[lc+j][ii] = OneRow->Strings[j];
        }
        if(linecont){
          if(OneRow->Count != 0) linecont = lc + OneRow->Count - 1;
          else linecont = lc;
        }else{
          int RH = (Canvas->TextHeight("あ")+4) * MultiLine;
          if(RowHeights[ii] < RH) RowHeights[ii] = RH;
        }
      }else{
        for(int j=0; j<Left; j++) OneRow->Insert(0,"");
        if(linecont) linecont = OneRow->Count - 1;
        Rows[i+Top] = OneRow;
        MultiLine = 0;
        colcount = max(colcount, OneRow->Count);
      }
    }
    else
    {
      int jEnd = OneRow->Count;
      if(Way == 0) jEnd = min((int)Selection.Right - Left + 1 , jEnd);
      else if(Way == 1) jEnd = Selection.Right - Left + 1;
      for(int j=0; j<jEnd; j++)
      {
	      int jj = j;
	      if(Way == 1 && ClipCols>0) jj = j % ClipCols;
	      if(jj < OneRow->Count)
          Cells[j+Left][i+Top] = OneRow->Strings[jj];
      }
    }
  }
  delete OneRow;
  if(Way == -1 && TypeOption->DummyEof){
    if(colcount >= ColCount){
      ChangeColCount(colcount + 1);
    }
    Cells[colcount][iEnd+Top] = STRDEOF;
  }
  Modified = true;
  Cut();
  ReNum();
}
//---------------------------------------------------------------------------
bool TCsvGrid::SetCsv(TStringList *Dest, AnsiString Src)
{
  int CellBegin = 1;
  bool Quoted = false;
  int Kugiri = 2; // 0:通常 1:弱区切り（" "） 2:強区切り（"," "\t"）
  Dest->Clear();
  for(int i=1; i<=Src.Length(); i++){
    if(Src.IsTrailByte(i)){
      continue;
    }else if(TypeOption->SepChars.AnsiPos(Src[i]) > 0 ){
      if(!Quoted){
	      if(Kugiri != 1) Dest->Add(Src.SubString(CellBegin,i-CellBegin));
	      Kugiri = 2;
	      CellBegin = i+1;
//        if(Way == -1 && Src[i] == ',') CommaSeparated = true;
//        else if(Way == -1 && Src[i] == '\t') TabSeparated = true;
      }
    }else if(TypeOption->WeakSepChars.AnsiPos(Src[i]) > 0 ){
      if(!Quoted){
      	if(Kugiri == 0){
      	  Dest->Add(Src.SubString(CellBegin,i-CellBegin));
      	  Kugiri = 1;
      	}
      	CellBegin = i+1;
      }
    }else if(TypeOption->UseQuote() &&
             TypeOption->QuoteChars.AnsiPos(Src[i]) > 0){
      if(Quoted){
      	if(i<Src.Length() && Src[i+1]=='\"'){
      	  Src.Delete(i,1);
      	}else{
      	  Dest->Add(Src.SubString(CellBegin,i-CellBegin));
	        Kugiri = 1;
	        CellBegin = i+1;
      	  Quoted = false;
      	}
      }else{
      	if(Kugiri > 0){
      	  Quoted = true;
      	  Kugiri = 0;
      	  CellBegin = i+1;
      	}
      }
    }else{
      Kugiri = 0;
    }
  }
  if(CellBegin <= Src.Length()){
    Dest->Add(Src.SubString(CellBegin,Src.Length()-CellBegin+1));
  }else if(Kugiri == 2 || Quoted){
    Dest->Add("");
  }

  return Quoted;
}
//---------------------------------------------------------------------------
void TCsvGrid::SaveToNotSjisFile(AnsiString FileName, char *sjis, int length)
{
  char *buf = new char[length*2+1];
  unsigned long buflen;

  switch(KanjiCode){
  case CHARCODE_EUC:
    NkfConvertSafe(buf, length*2+1, &buflen, sjis, length, "-Se");
    break;
  case CHARCODE_JIS:
    NkfConvertSafe(buf, length*2+1, &buflen, sjis, length, "-Sj");
    break;
  case CHARCODE_UTF8:
    NkfConvertSafe(buf, length*2+1, &buflen, sjis, length, "-Sw");
    break;
  case CHARCODE_UTF16LE:
    NkfConvertSafe(buf, length*2+1, &buflen, sjis, length, "-Sw16L");
    break;
  case CHARCODE_UTF16BE:
    NkfConvertSafe(buf, length*2+1, &buflen, sjis, length, "-Sw16B");
    break;
  }

  TFileStream *File = new TFileStream(FileName, fmCreate | fmShareExclusive);
  File->Write(buf, buflen);
  delete File;
  delete[] buf;
  return;
}
//---------------------------------------------------------------------------
void TCsvGrid::SaveToFile(AnsiString FileName, TTypeOption *Format, bool html)
{
  TStringList *Lines = new TStringList;

  if(html){
    GridToHtml(Lines);
  }else{
    GridToCSV(Lines, Format);
  }

  if(KanjiCode == CHARCODE_SJIS || KanjiCode == CHARCODE_AUTO){
    if(ReturnCode==LFCR) Lines->SaveToFile(FileName);
    else{
      TFileStream *File = new TFileStream(FileName, fmCreate|fmShareExclusive);
      for(int i=0; i<Lines->Count; i++){
        File->Write(Lines->Strings[i].c_str(), Lines->Strings[i].Length());
        File->Write(&ReturnCode,1);
      }
      delete File;
    }
  }else{
    AnsiString Text;
    if(ReturnCode==LFCR){
      Text = Lines->Text;
    }else{
      for(int i=0; i<Lines->Count; i++) Text += Lines->Strings[i] + ReturnCode;
    }
    SaveToNotSjisFile(FileName, Text.c_str(), Text.Length());
  }
  delete Lines;
  Modified = false;
}
//---------------------------------------------------------------------------
AnsiString TCsvGrid::StringsToCSV(TStrings* Data, TTypeOption *Format)
{
  char Sep = Format->DefSepChar();
  AnsiString Text = "";
  if(Format->QuoteOption == soNormal && Sep == ',' && ReturnCode == LFCR){
    return Data->CommaText;
  }
  AnsiString Delim = Format->SepChars + Format->WeakSepChars
    + Format->QuoteChars + "\r\n";

  for(int i=0; i<Data->Count; i++){
    AnsiString Str = Data->Strings[i];

    if(Format->QuoteOption != soNone){
      for(int j=Str.Length(); j>0; j--){
        if(Str[j] == '\"'){
          Str.Insert("\"", j);
        }else if(Str[j] == '\r' && ReturnCode == '\n'){
          Str.Delete(j, 1);
        }else if(Str[j] == '\n' && ReturnCode == '\r'){
          Str.Delete(j, 1);
        }
      }
    }

    if(Format->QuoteOption==soNone ||
       (Format->QuoteOption==soString && IsNumber(Str))){
      Text += Str;
    }else if(Format->QuoteOption==soNormal){
      if(Str.LastDelimiter(Delim) > 0){
        Text += (AnsiString)("\"") + Str + "\"";
      }else{
        Text += Str;
      }
    }else{
      Text += (AnsiString)("\"") + Str + "\"";
    }
    if(i < Data->Count-1) Text += Sep;
  }
  return Text;
}
//---------------------------------------------------------------------------
void TCsvGrid::QuotedDataToStrings(TStrings *Lines, AnsiString Text, TTypeOption *Format)
{
  Lines->Text = Text;
  if(Format->QuoteOption==soNone){ return; }
  int i=0;
  while(i<Lines->Count){
    int qc = 0;
    AnsiString str = Lines->Strings[i];
    for(int j=1; j<=str.Length(); j++){
      if(str.IsDelimiter(Format->QuoteChars, j)){ qc++; }
    }
    if((qc % 2) && i+1 <Lines->Count){
      AnsiString rc = ((ReturnCode == LFCR) ?
        (AnsiString)"\r\n" : (AnsiString)ReturnCode);
      Lines->Strings[i] = str + rc + Lines->Strings[i+1];
      Lines->Delete(i+1);
    }else{
      i++;
    }
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::GridToCSV(TStrings *Lines, TTypeOption *Format)
{
  if(Format == NULL) Format = TypeOption;

  TStringList* Data = new TStringList;
  Lines->Clear();
  int R = ((Format->OmitComma) ? 0 : DataRight);
  for(int i=DataTop; i<=DataBottom; i++)
  {
    Data->Assign(Rows[i]);

    if(Format->OmitComma){
      for(int i = Data->Count-1; i>0 && Data->Strings[i]==""; i--)
        Data->Delete(i);
    }else{
      for(int i = Data->Count-1; i>R; i--)
        Data->Delete(i);
    }
    if(ShowRowCounter) Data->Delete(0);   // カウンタセルの削除

    Lines->Add(StringsToCSV(Data, Format));
  }
  delete Data;
}
//---------------------------------------------------------------------------
void TCsvGrid::GridToHtml(TStrings *Lines)
{
/*
  Lines->Clear();
  Lines->Add("<html>");
  Lines->Add("");
  Lines->Add("<table border=1>");

  int db = DataBottom, dr = DataRight;
  for(int y=DataTop; y<=db; y++)
  {
    Lines->Add("<tr>");
    for(int x=DataLeft; x<=dr; x++)
    {
      char dh = (x<FixedCols || y<FixedRows) ? 'h' : 'd';
      Lines->Add((AnsiString)("  <t") + dh + ">"
                + Cells[x][y] + "</t" + dh + ">");
    }
    Lines->Add("</tr>");
  }

  Lines->Add("</table>");
  Lines->Add("");
  Lines->Add("</html>");
*/
}
//---------------------------------------------------------------------------
void TCsvGrid::CopyToClipboard(bool Cut)
{
  if(Cut) SetUndoCsv();
  if(! EditorMode)
  {
    int SLeft = SelLeft;
    int STop = SelTop;
    int SRight = Selection.Right;
    int SBottom = Selection.Bottom;

    TStringList *Data = new TStringList;
    TStringList *OneLine = new TStringList;
    for(int i = STop; i <= SBottom; i++)
    {
      OneLine->Clear();
      for(int j = SLeft; j <= SRight; j++)
      {
        OneLine->Add(Cells[j][i]);
	      if(Cut == true) Cells[j][i] = "";
      }
      AnsiString ALine = StringsToCSV(OneLine, TypeOption);
      // if(*(AnsiLastChar(ALine)) == ',') ALine += "\"\"";
      Data->Add(ALine);
    }
    delete OneLine;
    AnsiString Txt = Data->Text;
    Txt.SetLength(Txt.Length()-2);
    TClipboard *Clip = new TClipboard;
    Clip->SetTextBuf(Txt.c_str());
    delete Clip;
    delete Data;
  }
  else
  {
    if(Cut == false)
      InplaceEditor->CopyToClipboard();
    else
      InplaceEditor->CutToClipboard();
  }
  if(Cut) Modified = true;
}
//---------------------------------------------------------------------------
void TCsvGrid::CutToClipboard()
{
  CopyToClipboard(true);
}
//---------------------------------------------------------------------------
void TCsvGrid::PasteFromClipboard()
{

  TStringList *Data = new TStringList;
  TClipboard *Clip = new TClipboard;
  QuotedDataToStrings(Data, Clip->AsText, TypeOption);
  delete Clip;

  if(! EditorMode || Data->Count > 1)
  {
    SetUndoCsv();
    int STop = SelTop;
    int SLeft = SelLeft;
    int SBottom = Selection.Bottom;
    int SRight = Selection.Right;
    int SelectRowCount = SBottom - STop + 1;
    int SelectColCount = SRight - SLeft + 1;
    bool RowSelected =
      (Selection.Bottom == RowCount-1 && Selection.Top == FixedRows);
    bool ColSelected =
      (Selection.Right == ColCount-1 && Selection.Left == FixedCols);

    int ClipRowCount = Data->Count;
    int ClipColCount = 0;
    TStringList *ARow = new TStringList;
    for(int i=0; i < ClipRowCount; i++)
    {
      SetCsv(ARow, Data->Strings[i]);
      ClipColCount = max(ARow->Count,ClipColCount);
    }
    delete ARow;
    int Way;
    if(PasteOption < 0){
      if(ColSelected && !RowSelected)
      {            //行選択時
        Selection.Left = DataLeft;
        ClipColCount = SelectColCount = ColCount-SLeft;
        Way = 4;
      }
      else if(RowSelected && !ColSelected)
      {            //列選択時
        Selection.Top = DataTop;
        ClipRowCount = SelectRowCount = RowCount-STop;
        Way = 3;
      }
      else if(ClipRowCount != SelectRowCount || ClipColCount != SelectColCount)
      {
        TfmPasteDialog *PstDlg = new TfmPasteDialog(Application);
          PstDlg->Way->ItemIndex = DefWay;
	  PstDlg->lbMessage->Caption
	    = AnsiString("選択サイズ： ")
	    + SelectColCount + " × " + SelectRowCount
	    + "　クリップボードサイズ： "
	    + ClipColCount + " × " + ClipRowCount;
          if(PstDlg->ShowModal() != IDOK){ delete Data; return; }
	  Way = DefWay = PstDlg->Way->ItemIndex;
        delete PstDlg;
      }
      else
      {
        Way = 2;
      }
    }else{
      Way = PasteOption;
    }
    PasteCSV(Data, SLeft, STop ,Way, ClipColCount, ClipRowCount);

    if(Way <= 1){
      SetSelection(SLeft, SLeft + SelectColCount - 1,
                   STop , STop  + SelectRowCount - 1);
    }else if(Way <= 4){
      SetSelection(SLeft, SLeft + ClipColCount - 1,
                   STop , STop  + ClipRowCount - 1);
    }
  }
  else //if(EditorMode == true){
  {
    SetUndoMacro();
    InplaceEditor->PasteFromClipboard();
    InplaceEditor->Text = TrimRight(InplaceEditor->Text);
  }
  delete Data;
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
bool TCsvGrid::IsNumber(AnsiString Str)
{
  try{
    if(Str == "") return false;
    Str.ToDouble();
    return true;
  }catch(...){}

  return false;
}
//---------------------------------------------------------------------------
void TCsvGrid::Sort(int SLeft, int STop, int SRight, int SBottom,
                    int SCol, bool Shoujun, bool NumSort)
{
  TList *StrList = new TList;
  TList *NumList = new TList;

  if(SLeft   < 0) SLeft   = 0;
  if(SRight  < 0) SRight  = 0;
  if(STop    < 0) STop    = 0;
  if(SBottom < 0) SBottom = 0;

  for(int y=STop; y<=SBottom; y++){
    TStringList *L = new TStringList;
    for(int x=SLeft; x<=SRight; x++){
      L->Add(Cells[x][y]);
    }
    if(NumSort && IsNumber(Cells[SCol][y])){
      DoubleData *DD = new DoubleData;
      DD->Num = Cells[SCol][y].ToDouble();
      DD->Data = L;
      DD->Row = y;
      NumList->Add(DD);
    }else{
      StringData *SD = new StringData;
      SD->Str = Cells[SCol][y];
      SD->Data = L;
      SD->Row = y;
      StrList->Add(SD);
    }
  }
  StrList->Sort(CompareOrderedString);
  NumList->Sort(CompareDoubleData);

  for(int y=STop; y<=SBottom; y++){
    int Idx = Shoujun ? y-STop : SBottom-y;
    TStringList *L;
    if(Idx <NumList->Count){
      L = static_cast<TStringList*>(
            static_cast<DoubleData*>(NumList->Items[Idx])->Data);
      delete(NumList->Items[Idx]);
    }else{
      int idx2 = Idx - NumList->Count;
      L = static_cast<TStringList*>(
            static_cast<StringData*>(StrList->Items[idx2])->Data);
    }
    for(int x=SLeft; x<=SRight; x++){
      Cells[x][y] = L->Strings[x-SLeft];
    }
    delete L;
  }

  delete StrList;
  delete NumList;
  Modified = true;
}
//---------------------------------------------------------------------------
double TCsvGrid::SelectSum(int *Count)
{
  double Sum = 0;
  int NumCount = 0;
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      AnsiString Str = Cells[i][j];
      if(Str != ""){
        try{
          Sum += Str.ToDouble();
          NumCount++;
        }catch(...){}
      }
    }
  }

  if(Count) *Count = NumCount;
  return Sum;
}
//---------------------------------------------------------------------------
void TCsvGrid::CopySum()
{
  TClipboard *Clip = new TClipboard;
    Clip->AsText = (AnsiString)SelectSum(NULL);
  delete Clip;
}
//---------------------------------------------------------------------------
void TCsvGrid::CopyAvr()
{
  int Count;
  double Sum = SelectSum(&Count);

  TClipboard *Clip = new TClipboard;
    if(Count==0)
      Clip->AsText = "0";
    else
      Clip->AsText = (AnsiString)(Sum / Count);
  delete Clip;
}
//---------------------------------------------------------------------------
wchar_t Hankaku2Zenkaku(wchar_t wc)
{
  if(wc == L' ') return L'　';
  else if(wc < L'ｦ' || wc > L'ﾟ') return wc;
  else if(wc >= L'ｱ' && wc <= L'ｵ') return L'ア' + (wc - L'ｱ') * 2;
  else if(wc >= L'ｶ' && wc <= L'ﾁ') return L'カ' + (wc - L'ｶ') * 2;
  else if(wc >= L'ﾂ' && wc <= L'ﾄ') return L'ツ' + (wc - L'ﾂ') * 2;
  else if(wc >= L'ﾅ' && wc <= L'ﾉ') return L'ナ' + (wc - L'ﾅ');
  else if(wc >= L'ﾊ' && wc <= L'ﾎ') return L'ハ' + (wc - L'ﾊ') * 3;
  else if(wc >= L'ﾏ' && wc <= L'ﾓ') return L'マ' + (wc - L'ﾏ');
  else if(wc >= L'ﾔ' && wc <= L'ﾖ') return L'ヤ' + (wc - L'ﾔ') * 2;
  else if(wc >= L'ﾗ' && wc <= L'ﾛ') return L'ラ' + (wc - L'ﾗ');
  else if(wc == L'ﾜ') return L'ワ';
  else if(wc == L'ｦ') return L'ヲ';
  else if(wc == L'ﾝ') return L'ン';
  else if(wc >= L'ｧ' && wc <= L'ｫ') return L'ァ' + (wc - L'ｧ') * 2;
  else if(wc >= L'ｬ' && wc <= L'ｮ') return L'ャ' + (wc - L'ｬ') * 2;
  else if(wc == L'ｯ') return L'ッ';
  else if(wc == L'ｰ') return L'ー';
  else if(wc == L'ﾞ') return L'゛';
  else if(wc == L'ﾟ') return L'゜';

  return wc;
}
//---------------------------------------------------------------------------
int Zenkaku2Hankaku(wchar_t wc, wchar_t *ans)
{
  if(wc == L'　'){ *ans = L' '; return 1; }
  else if(wc == L'ワ'){ *ans = L'ﾜ'; return 1; }
  else if(wc == L'ヲ'){ *ans = L'ｦ'; return 1; }
  else if(wc == L'ン'){ *ans = L'ﾝ'; return 1; }
  else if(wc == L'゛'){ *ans = L'ﾞ'; return 1; }
  else if(wc == L'゜'){ *ans = L'ﾟ'; return 1; }
  else if(wc == L'ー'){ *ans = L'ｰ'; return 1; }
  else if(wc < L'ァ' || wc > L'ロ'){ *ans = wc; return 1; }
  else if(wc == L'ッ'){ *ans = L'ｯ'; return 1; }
  else if(wc >= L'ァ' && wc <= L'オ'){
    int x = (wc - L'ァ');
    *ans = ((x % 2) ? (L'ｱ' + (x / 2)) : (L'ｧ' + (x / 2)));
    return 1;
  }else if(wc >= L'カ' && wc <= L'チ'){
    int x = (wc - L'カ');
    *ans = L'ｶ' + (x / 2);
    if(x % 2){
      *(ans+1) = L'ﾞ';
      return 2;
    }else{
      return 1;
    }
  }else if(wc >= L'ツ' && wc <= L'ト'){
    int x = (wc - L'ツ');
    *ans = L'ﾂ' + (x / 2);
    if(x % 2){
      *(ans+1) = L'ﾞ';
      return 2;
    }else{
      return 1;
    }
  }else if(wc >= L'ナ' && wc <= L'ノ'){
    *ans = L'ﾅ' + (wc - L'ナ');
    return 1;
  }else if(wc >= L'ハ' && wc <= L'ホ'){
    int x = (wc - L'ハ');
    *ans = L'ﾊ' + (x / 3);
    if((x % 3) == 1){
      *(ans+1) = L'ﾞ';
      return 2;
    }else if((x % 3) == 2){
      *(ans+1) = L'ﾟ';
      return 2;
    }else{
      return 1;
    }
  }else if(wc >= L'マ' && wc <= L'モ'){
    *ans = L'ﾏ' + (wc - L'マ');
    return 1;
  }else if(wc >= L'ャ' && wc <= L'ヨ'){
    int x = (wc - L'ャ');
    *ans = ((x % 2) ? (L'ﾔ' + (x / 2)) : (L'ｬ' + (x / 2)));
    return 1;
  }else if(wc >= L'ラ' && wc <= L'ロ'){
    *ans = L'ﾗ' + (wc - L'ラ');
    return 1;
  }

  return 0;
}
//---------------------------------------------------------------------------
void TCsvGrid::TransChar(int Type)
{
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      AnsiString Str = Cells[i][j];
      if(Type==2)      Str = Str.UpperCase();
      else if(Type==3) Str = Str.LowerCase();
      else{
        int size = Str.WideCharBufSize();
        wchar_t *wc = new wchar_t[size];
        Str.WideChar(wc, size);
        wchar_t *q = wc+size;
        for(wchar_t *p=wc; p<q; p++){
          if(Type==0){
            if(*p == L'　')      *p = L' ';
            else if(*p == L'’') *p = L'\'';
            else if(*p == L'”') *p = L'\"';
            else if(*p == L'￥') *p = L'\\';
            else if(*p >= L'！' && *p <= L'～') *p -= (L'Ａ' - L'A');
          }else{
            if(*p == L' ')       *p = L'　';
            else if(*p == L'\'') *p = L'’';
            else if(*p == L'\"') *p = L'”';
            else if(*p == L'\\') *p = L'￥';
            else if(*p >= L'!' && *p <= L'~') *p += (L'Ａ' - L'A');
          }
        }
        Str = (AnsiString)wc;
        delete[] wc;
      }
      Cells[i][j] = Str;
    }
  }
  Modified = true;
}
//---------------------------------------------------------------------------
void TCsvGrid::TransKana(int Type)
{
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      AnsiString Str = Cells[i][j];
      if(Str == "") continue;
      int size = Str.Length() * 2;
      wchar_t *wcfr = new wchar_t[size];
      wchar_t *wcto = new wchar_t[size];
      Str.WideChar(wcfr, size);
      wchar_t *p=wcfr;
      wchar_t *q=wcto;
      for(; *p != L'\0'; p++){
        if(Type==4){
          q += Zenkaku2Hankaku(*p, q);
        }else if(Type==5){
          *q = Hankaku2Zenkaku(*p);
          if(*(p+1) == L'ﾞ' &&
            ((*p >= L'ｶ' && *p <= L'ﾄ') || (*p >= L'ﾊ' && *p <= L'ﾎ'))) {
            (*q)++;
            p++;
          }else if(*(p+1) == L'ﾟ' && *p >= L'ﾊ' && *p <= L'ﾎ') {
            (*q) += 2;
            p++;
          }
          q++;
        }
      }
      *q = L'\0';
      Str = (AnsiString)wcto;
      delete[] wcfr;
      delete[] wcto;
      Cells[i][j] = Str;
    }
  }
  Modified = true;
}
//---------------------------------------------------------------------------
void TCsvGrid::Sequence(bool Inc)
{
  for(int x=SelLeft; x<=Selection.Right; x++){
    int IncType = 0;
    double Increment = 1.0;
    double TopValue = 0.0;
    AnsiString TopStr = Cells[x][SelTop];

    if(Inc && TopStr != ""){
      if(IsNumber(TopStr)){
        IncType = 1;
        TopValue = TopStr.ToDouble();
        AnsiString Snd = Cells[x][SelTop+1];
        if(IsNumber(Snd)) Increment = Snd.ToDouble() - TopValue;
      }else{
        char LastChar = *(TopStr.AnsiLastChar());
        if(LastChar>='0' && LastChar<='9') IncType = 2;
      }
    }
    for(int y=SelTop+1; y<=Selection.Bottom; y++){
      switch(IncType){
        case 0:
          Cells[x][y] = TopStr; break;
        case 1:
          Cells[x][y] = (AnsiString)(TopValue + Increment*(y-SelTop));
          break;
        case 2:{
          int p = TopStr.Length();
          while(p>0 && TopStr[p]=='9'){ TopStr[p]='0'; p--; }
          if(p==0) TopStr = (AnsiString)"1" + TopStr;
          else if(TopStr[p]>='0' && TopStr[p]<'9') TopStr[p]++;
          else TopStr.Insert("1",p+1);
          Cells[x][y] = TopStr;
        } break;
      }
    }
  }
  Modified = true;
}
//---------------------------------------------------------------------------
//  挿入
//
void TCsvGrid::ChangeRowCount(int Count)
{
  if(Count > RowCount){
    int R = RowCount;
    RowCount = Count;
    for(int i=R; i<Count; i++) Rows[i]->Clear();
  }else if(Count < RowCount) RowCount = Count;
}
//---------------------------------------------------------------------------
void TCsvGrid::ChangeColCount(int Count)
{
  if(Count > ColCount){
    int R = ColCount;
    ColCount = Count;
    for(int i=R; i<Count; i++) Cols[i]->Clear();
  }else if(Count < ColCount) ColCount = Count;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::InsertRow(int Index)
{
  SetUndoCsv();
  Rows[RowCount++]->Clear();
  TStringGrid::MoveRow(RowCount-1,Index);
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::InsertColumn(int Index)
{
  SetUndoCsv();
  Cols[ColCount++]->Clear();
  TStringGrid::MoveColumn(ColCount-1,Index);
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DeleteRow(int ARow)
{
  SetUndoCsv();
  if(RangeSelect) Col = 1;
  if(RowCount <= FixedRows+1) return;
  else if(Row == ARow && ARow == RowCount-1) Row--;
  else if(Row == ARow) Row++;
  TStringGrid::DeleteRow(ARow);
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DeleteColumn(int ACol)
{
  SetUndoCsv();
  if(RangeSelect) Row = 1;
  if(ColCount <= FixedCols+1) return;
  else if(Col == ACol && ACol == ColCount-1) Col--;
  else if(Col == ACol) Col++;
  TStringGrid::DeleteColumn(ACol);
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertEnter()
{
  long X = SelLeft, Y = Selection.Top, L = DataLeft;
  int SelLen = Selection.Right - SelLeft;

  SetUndoCsv();
  InsertRow(Y+1);
  bool OneCell = EditorMode;
  if(OneCell){
    SelLen = InplaceEditor->SelLength;
    Cells[L][Y+1] = InplaceEditor->Text.SubString
		     (InplaceEditor->SelStart+1,InplaceEditor->Text.Length());
    Cells[X][Y] = InplaceEditor->Text.SubString(1,InplaceEditor->SelStart);
  }else{
    Cells[L][Y+1] = Cells[X][Y];
    Cells[X][Y] = "";
  }

  for(int i=1; i+X < ColCount; i++)
  {
    Cells[i+L][Y+1] = Cells[i+X][Y];
    Cells[i+X][Y] = "";
  }
  if(OneCell){
    Row=Y+1; Col=FixedCols;
    if(SelLen>=0 && Col==DataLeft) InplaceEditor->SelLength = SelLen;
  }else{
    SetSelection(L, L+SelLen, Y+1, Y+1);
  }
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertNewLine()
{
  if(EditorMode){
    SetUndoMacro();
    InplaceEditor->SelText = "\r\n";
    Modified = true;
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::ConnectCell()
{
  SetUndoCsv();
  if(RangeSelect){
    AnsiString str = "";
    for(int j=Selection.Top; j <= Selection.Bottom; j++){
      for(int i=Selection.Left; i <= Selection.Right; i++){
        str += Cells[i][j];
        Cells[i][j] = "";
      }
    }
    Cells[Selection.Left][Selection.Top] = str;
  }else{
    int C = SelLeft;
    if(C > FixedCols)                //カンマの削除
    {
      Cells[C-1][Row] = Cells[C-1][Row] + Cells[C][Row];
      for(int i=C; i<ColCount-1; i++)
        Cells[i][Row] = Cells[i+1][Row];
      Cells[ColCount-1][Row] = "";
      Col--;
    }
    else if(Row > FixedRows)            //リターンの削除
    {
      int i, UpColCount, ThisColCount;
      for(i=ColCount-1; i>=DataLeft && Cells[i][Row-1]==""; i--){} UpColCount = i+1;
      for(i=ColCount-1; i>=DataLeft && Cells[i][Row]  ==""; i--){} ThisColCount = i+1;
      if(UpColCount + ThisColCount  >= ColCount)  //列数が足りなければ増やす
        ChangeColCount(UpColCount + ThisColCount + 1);
      for(i=DataLeft; i<=ThisColCount; i++)
        Cells[i+UpColCount-DataLeft][Row-1] = Cells[i][Row];
      Row--; Col = (UpColCount>=FixedCols) ? UpColCount : FixedCols;
      DeleteRow(Row+1);
    }
  }
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertCell_Right()
{
  SetUndoCsv();
  if(RangeSelect)
    InsertCells_Right(SelLeft, Selection.Right, SelTop, Selection.Bottom);
  else
    InsertCells_Right(Col,Col,Row,Row);
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertCell_Down()
{
  SetUndoCsv();
  if(RangeSelect)
    InsertCells_Down(SelLeft, Selection.Right, SelTop, Selection.Bottom);
  else
    InsertCells_Down(Col,Col,Row,Row);
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteCell_Left()
{
  SetUndoCsv();
  if(RangeSelect)
    DeleteCells_Left(SelLeft, Selection.Right, SelTop, Selection.Bottom);
  else
  {
    int i;
    for(i=Col; i < ColCount; i++)
      if(Cells[i][Row] != "") break;

    if(i >= ColCount && Row != RowCount-1)
    { Row++; Col = FixedCols; ConnectCell(); }
    else
      DeleteCells_Left(Col,Col,Row,Row);
  }
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteCell_Up()
{
  SetUndoCsv();
  if(RangeSelect)
    DeleteCells_Up(SelLeft, Selection.Right, SelTop,Selection.Bottom);
  else
    DeleteCells_Up(Col,Col,Row,Row);

  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertCells_Right(long Left, long Right, long Top, long Bottom)
{
  ChangeColCount(ColCount + Right - Left  + 1);
  TStringList *Temp = new TStringList;
  for(int i=Top; i<=Bottom; i++)
  {
    Temp->Assign(Rows[i]);
    for(int j=Left; j<=Right; j++)
    {
      Temp->Insert(Left,"");
      Temp->Delete(Temp->Count-1);
    }
    Rows[i]->Assign(Temp);
  }
  delete Temp;
  SetSelection(Left, Right, Top, Bottom);
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertCells_Down(long Left, long Right, long Top, long Bottom)
{
  ChangeRowCount(RowCount + Bottom - Top + 1);
  TStringList *Temp = new TStringList;
  for(int i=Left; i<=Right; i++)
  {
    Temp->Assign(Cols[i]);
    for(int j=Top; j<=Bottom; j++)
    {
      Temp->Insert(Top,"");
      Temp->Delete(Temp->Count-1);
    }
    Cols[i]->Assign(Temp);
  }
  delete Temp;
  SetSelection(Left, Right, Top, Bottom);
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteCells_Left(long Left, long Right, long Top, long Bottom)
{
  TStringList *Temp = new TStringList;
  for(int i=Top; i<=Bottom; i++)
  {
    Temp->Assign(Rows[i]);
    for(int j=Left; j<=Right; j++)
    {
      Temp->Delete(Left);
      Temp->Add("");
    }
    Rows[i]->Assign(Temp);
  }
  delete Temp;
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteCells_Up(long Left, long Right, long Top, long Bottom)
{
  TStringList *Temp = new TStringList;
  for(int i=Left; i<=Right; i++)
  {
    Temp->Assign(Cols[i]);
    for(int j=Top; j<=Bottom; j++)
    {
      Temp->Delete(Top);
      Temp->Add("");
    }
    Cols[i]->Assign(Temp);
  }
  delete Temp;
}
//---------------------------------------------------------------------------
//  選択
//
void TCsvGrid::SetSelection(long Left, long Right, long Top, long Bottom)
{
  if(Left   < FixedCols) Left   = FixedCols;
  if(Right  < FixedCols) Right  = FixedCols;
  if(Top    < FixedRows) Top    = FixedRows;
  if(Bottom < FixedRows) Bottom = FixedRows;

  if(Left   >= ColCount) Left   = ColCount-1;
  if(Right  >= ColCount) Right  = ColCount-1;
  if(Top    >= RowCount) Top    = RowCount-1;
  if(Bottom >= RowCount) Bottom = RowCount-1;

  if(Left==Right && Top==Bottom && AlwaysShowEditor){
    Options << goEditing << goAlwaysShowEditor;
    Row = Top; Col = Left;
    ShowEditor();
  }else{
    TGridRect theRect;
    theRect.Left   = Left;
    theRect.Right  = Right;
    theRect.Top    = Top;
    theRect.Bottom = Bottom;

    EditorMode = false;
    Options >> goAlwaysShowEditor; // >> goEditing;
    Selection = theRect;
  }
}
//---------------------------------------------------------------------------
//  ドラッグイベントの処理
//
void __fastcall TCsvGrid::MouseDown(Controls::TMouseButton Button,
    Classes::TShiftState Shift, int X, int Y)
{
  MouseToCell(X,Y,drgCol,drgRow);

  if(EditorMode == false && Col == drgCol && Row == drgRow && !RangeSelect){
    SameCellClicking = true;
  }
  EditorMode = false;
  Options >> goAlwaysShowEditor >> goEditing;
  Dragging = true;
  TStringGrid::MouseDown(Button,Shift,X,Y);


  if(drgRow < FixedRows) {
    int XLeft4, XRight4, YTemp;
    MouseToCell(X-4,Y,XLeft4,YTemp);
    MouseToCell(X+4,Y,XRight4,YTemp);
    ColToResize = XLeft4;
    MouseDownCellBorder = ((XLeft4 != drgCol) || (XRight4 != drgCol));
  }else{
    MouseDownCellBorder = false;
  }

  if(DragMove && drgCol>=0 && drgRow>=0){
    if(drgCol < FixedCols && drgRow < FixedRows) {
      SelectAll();
    }else if(drgCol < FixedCols) {
      if(Shift.Contains(ssShift)){
        SelectRows(drgRow, Row);
      }else{
        SelectRow(drgRow);
      }
    }else if(drgRow < FixedRows) {
      if(Shift.Contains(ssShift)){
        SelectCols(drgCol, Col);
      }else{
        SelectColumn(drgCol);
      }
    }
  }else if(!DragMove){
    if(Shift.Contains(ssShift)){
      if(drgRow >=0 && drgRow < FixedRows && drgCol >= FixedCols)
        drgCol = Col;
      else if(drgCol >=0 && drgCol < FixedCols && drgRow >= FixedRows)
        drgRow = Row;
    }
    MouseMove(Shift, X, Y);
  }
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::MouseMove(Classes::TShiftState Shift, int X, int Y)
{
  int ACol, ARow;
  MouseToCell(X,Y,ACol,ARow);

  if(Dragging &&
     ((drgRow >= 0 && drgRow < FixedRows) ||
      (drgCol >= 0 && drgCol < FixedCols)) &&
     !DragMove){

//    int ACol, ARow;
//    MouseToCell(X,Y,ACol,ARow);

    if(drgCol < FixedCols && drgRow < FixedRows) {

      SelectAll();

    } else if(drgCol < FixedCols) {

      if(ARow < 0){
        ARow = TopRow + VisibleRowCount - 1;
      }else if(ARow < TopRow) {
        if(TopRow > FixedRows) TopRow--;
        ARow = TopRow;
      } else if(ARow >= TopRow + VisibleRowCount) {
        if(TopRow + VisibleRowCount < RowCount) TopRow++;
        ARow = TopRow + VisibleRowCount - 1;
      }
      SelectRows(ARow, drgRow);

    } else if(drgRow < FixedRows) {

      if(ACol < 0){
        ACol = LeftCol + VisibleColCount - 1;
      }else if(ACol < LeftCol) {
        if(LeftCol > FixedCols) LeftCol--;
        ACol = LeftCol;
      } else if(ACol >= LeftCol + VisibleColCount) {
        if(LeftCol + VisibleColCount < ColCount) LeftCol++;
        ACol = LeftCol + VisibleColCount - 1;
      }
      SelectCols(ACol, drgCol);

    }
  }

  if(ARow >= 0 && ACol >= 0) {
    if(Canvas->TextWidth(Cells[ACol][ARow]) + 4 > ColWidths[ACol]){
      Hint = Cells[ACol][ARow];
      ShowHint = true;
    }else{
      ShowHint = false;
      ParentShowHint = true;
    }

    if(DblClickOpenURL == 2){
      if(Cells[ACol][ARow].SubString(1,7) == "http://"){
        Cursor = crHandPoint;
      }else{
        Cursor = crDefault;
      }
    }
  }

  TStringGrid::MouseMove(Shift,X,Y);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::MouseUp(Controls::TMouseButton Button,
    Classes::TShiftState Shift, int X, int Y)
{
  int ACol, ARow;
  MouseToCell(X,Y,ACol,ARow);

  TStringGrid::MouseUp(Button,Shift,X,Y);

  if(DragMove && ACol>=0 && ARow>=0){
    if(ACol < FixedCols && ARow < FixedRows) {
      SelectAll();
    }else if(ACol < FixedCols && drgCol < FixedCols && ARow == drgRow) {
      if(Shift.Contains(ssShift)) SelectRows(ARow, Row);
      else SelectRow(ARow);
    }else if(ARow < FixedRows && drgRow < FixedRows && ACol == drgCol) {
      if(Shift.Contains(ssShift)) SelectCols(ACol, Col);
      else SelectColumn(ACol);
    }
  }

  if(!RangeSelect){
    if(AlwaysShowEditor || SameCellClicking){
      Options << goEditing << goAlwaysShowEditor;
      ShowEditor();
      SameCellClicking = false;
    }else{
      Options << goEditing;
    }

    if((DblClickOpenURL == 2) && Cells[ACol][ARow].SubString(1,7) == "http://"){
      OpenURL(Cells[ACol][ARow]);
    }
  }

  Dragging = false;
  ReNum();
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::RowMoved(int FromIndex, int ToIndex)
{
  SetUndoCsv();
  TStringGrid::RowMoved(FromIndex,ToIndex);
  Modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::ColumnMoved(int FromIndex, int ToIndex)
{
  SetUndoCsv();
  TStringGrid::ColumnMoved(FromIndex,ToIndex);
  Modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DblClick(void)
{
  if(MouseDownCellBorder){
     SetWidth(ColToResize);
  }else if(Selection.Top == FixedRows &&
     Selection.Bottom == RowCount-1 &&
     Selection.Left == Selection.Right)
  {
    if(ColWidths[Selection.Left] > 16) {
      ColWidths[Selection.Left] = 16;
    }else{
      SetWidth(Selection.Left);
    }
  }else if(EditorMode == false){
    Options << goEditing << goAlwaysShowEditor;
    ShowEditor();
    InplaceEditor->SelLength = 0;
    InplaceEditor->SelStart = InplaceEditor->Text.Length();
  }

  if(DblClickOpenURL && Cells[Col][Row].SubString(1,7) == "http://"){
    OpenURL(Cells[Col][Row]);
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::OpenURL(AnsiString FileName)
{
  if(BrowserFileName == ""){
    AutoOpen(FileName);
  }else{
    AnsiString Arg0 = (AnsiString)("\"") + BrowserFileName + "\"";
    AnsiString Arg1 = (AnsiString)("\"") + FileName + "\"";
    spawnl(P_NOWAITO , BrowserFileName.c_str() , Arg0.c_str() , Arg1.c_str() , NULL);
  }
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::MouseWheelUp(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled)
{
  if(WheelMoveCursol){
    if(Row > FixedRows){ Row--; }
  }else{
    if(TopRow > FixedRows){ TopRow--; }
  }
  Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::MouseWheelDown(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled)
{
  if(WheelMoveCursol){
    if(Row < RowCount - 1){ Row++; }
  }else{
    if(TopRow < RowCount - VisibleRowCount){ TopRow++; }
  }
  Handled = true;
}
//---------------------------------------------------------------------------
//  検索
//
#define drThisRow 0
#define drThisCol 1
#define drAll 2
//---------------------------------------------------------------------------
int FindHit(AnsiString CellText,AnsiString FindText,bool Asterisk, bool Back, int *Length)
{
  *Length = FindText.Length();
  if(Back){
    for(int i=CellText.Length() - FindText.Length() + 1; i>0; i--){
      if(CellText.SubString(i, FindText.Length()) == FindText){
        return i;
      }
    }
    return 0;
  }else{
    return (CellText.AnsiPos(FindText));
  }
}
//---------------------------------------------------------------------------
bool TCsvGrid::Find(AnsiString FindText, int Range, bool Asterisk, bool Word, bool Back)
{
  int Hit, Len;

  if(FindText == "") return(false);
  Options << goEditing << goAlwaysShowEditor;
  ShowEditor();
  TInplaceEdit *ipEd = InplaceEditor;
  if(!ipEd){ return(false); }

  AnsiString InCellText = "";
  if(Back){
    InCellText = ipEd->Text.SubString(1, ipEd->SelStart);
  }else if(!Word || (ipEd->SelStart + ipEd->SelLength == 0)){
    InCellText = ipEd->Text.SubString(ipEd->SelStart + ipEd->SelLength + 1, ipEd->Text.Length());
  }

  if(InCellText != ""){
    Hit = FindHit(InCellText, FindText, Asterisk, Back, &Len);
    if(Hit>0){
      if(Back){
        ipEd->SelStart = Hit - 1;
      }else{
        ipEd->SelStart += ipEd->SelLength + Hit - 1;
      }
      ipEd->SelLength = Len;
      ipEd->SetFocus();
      return(true);
    }
  }

  int step = (Back ? -1 : 1);
  int YStart = Row;
  int YEnd = (Back ? DataTop : RowCount);
  if(Range == drThisRow) YEnd = Row;
  else if(Range == drThisCol) YStart = Row+step;

  for(int y=YStart; y*step<=YEnd*step; y+=step)
  {
    int XStart = (Back ? ColCount : DataLeft);
    int XEnd   = (Back ? DataLeft : ColCount);
    if(Range == drThisCol){ XStart = Col;  XEnd = Col; }
    else if(y == Row){ XStart = Col+step; }

    for(int x=XStart; x*step<=XEnd*step; x+=step)
    {
      if(Word){
        if(Cells[x][y] == FindText){ Hit=1; Len=FindText.Length(); }
        else Hit = 0;
      }else{
        Hit = FindHit(Cells[x][y], FindText, Asterisk, Back, &Len);
      }
      if(Hit>0){
        if(x >= FixedCols){
  	  Row=y; Col=x;
	  ipEd->SelStart = Hit - 1;
	  ipEd->SelLength = Len;
        }else{
          Row=y; Col=FixedCols;
          EditorMode = true;
          ipEd->SelStart = 0;
          ipEd->SelLength = 0;
        }
        ipEd->SetFocus();
	return(true);
      }
    }
  }
  return(false);
}
//---------------------------------------------------------------------------
bool TCsvGrid::Replace(AnsiString FindText , AnsiString ReplaceText,
                       int Range, bool Asterisk, bool Word, bool Back)
{
  TInplaceEdit *ipEd = InplaceEditor;
  if(ReplaceText != FindText && ipEd){
    if(Word){
      if(ipEd->Text == FindText){
        SetUndoMacro();
        ipEd->Text = ReplaceText;
        Modified = true;
      }
    }else{
      int Hit,Len;
      Hit = FindHit(ipEd->SelText, FindText, Asterisk, Back, &Len);
      if(Hit == 1 && Len == ipEd->SelText.Length()) {
        SetUndoMacro();
        ipEd->SelText = ReplaceText;
        Modified = true;
      }
    }
  }

  return(Find(FindText,Range,Asterisk,Word,Back));
}
//---------------------------------------------------------------------------
void TCsvGrid::AllReplace(AnsiString FindText , AnsiString ReplaceText,
                          int Range, bool Asterisk, bool Word, bool Back)
{
  int C = Col, R = Row;
  Options << goEditing << goAlwaysShowEditor;
  ShowEditor();
  if(!Back){
    if(Range != drThisCol) Col = FixedCols;
    if(Range != drThisRow) Row = FixedRows;
    TInplaceEdit *ipEd = InplaceEditor;
    if(ipEd){
      ipEd->SelLength = 0; ipEd->SelStart = 0;
    }
  }else{
    if(Range != drThisCol) Col = DataRight;
    if(Range != drThisRow) Row = DataBottom;
    TInplaceEdit *ipEd = InplaceEditor;
    if(ipEd){
      ipEd->SelLength = 0; ipEd->SelStart = ipEd->Text.Length();
    }
  }
  while(Replace(FindText,ReplaceText, Range, Asterisk, Word, Back)){}
  Col = C; Row = R;
}
//---------------------------------------------------------------------------
bool TCsvGrid::NumFind(double *Min, double *Max, int Range, bool Back)
{
  SetFocus();
  int step = (Back ? -1 : 1);
  int YStart = Row;
  int YEnd = (Back ? DataTop : RowCount);
  if(Range == drThisRow) YEnd = Row;
  else if(Range == drThisCol){
    TInplaceEdit *ipEd = InplaceEditor;
    if(ipEd && (ipEd->SelStart + ipEd->SelLength == 0)){
      YStart = Row;
    }else{
      YStart = Row+step;
    }
  }

  for(int y=YStart; y*step<=YEnd*step; y+=step){
    int XStart = (Back ? ColCount : DataLeft);
    int XEnd   = (Back ? DataLeft : ColCount);
    if(Range == drThisCol){ XStart = Col;  XEnd = Col; }
    else if(y == Row){
      TInplaceEdit *ipEd = InplaceEditor;
      if(ipEd && (ipEd->SelStart + ipEd->SelLength == 0)){
        XStart = Col;
      }else{
        XStart = Col+step;
      }
    }

    for(int x=XStart; x*step<=XEnd*step; x+=step){
      try{
        double Val = Cells[x][y].ToDouble();
        if((Min == NULL || Val >= *Min) &&
           (Max == NULL || Val <= *Max) )
        {
          if(x >= FixedCols){
            Row=y; Col=x;
          }else{
            Row=y; Col=FixedCols;
          }
          TInplaceEdit *ipEd = InplaceEditor;
          if(ipEd){ ipEd->SelectAll(); }
	  return(true);
        }
      }catch(...){}
    }
  }
  return(false);
}
//---------------------------------------------------------------------------
//  キー入力の監視
//
void __fastcall TCsvGrid::KeyDown(Word &Key, Classes::TShiftState Shift)
{
  if(Key == VK_DELETE && EditorMode == false)
  {
    SetUndoCsv();
    for(int i=SelTop; i<=Selection.Bottom; i++)
      for(int j=SelLeft; j<= Selection.Right; j++)
	      Cells[j][i] = "";
    Modified = true;
  }else if(Key == VK_DELETE){
    Modified = true;
  }else if(Key == VK_RIGHT || Key == VK_UP ||
           Key == VK_LEFT  || Key == VK_DOWN){
    if(Shift.Contains(ssShift)){
      EditorMode = false;
      Options >> goAlwaysShowEditor; // >> goEditing;
    }else if(AlwaysShowEditor){
      Options << goEditing << goAlwaysShowEditor;
      ShowEditor();
    }
  }else if(Key == VK_RETURN && EditorMode == false){
    Options << goEditing << goAlwaysShowEditor;
    ShowEditor();
    InplaceEditor->SelLength = 0;
    InplaceEditor->SelStart = InplaceEditor->Text.Length();
  }


  SpreadRightBottom();
  TStringGrid::KeyDown(Key,Shift);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::KeyDownSub(System::TObject* Sender,
                          Word &Key, Classes::TShiftState Shift)
{
  /*
   * InplaceEditor内でのキー入力などで、
   * 場合によっては通常のKeyDown()が実行されないため、
   * OnKeyDown にこの関数をつっこんで処理します^^;
   */

  if(Key == VK_DELETE && EditorMode)
  {
    Modified = true;
    SetUndoMacro();
  }
  else if(Key == VK_RETURN && EditorMode)
  {
    switch(EnterMove){
      case cssv_EnterDown:
        if(Row == RowCount-1){ ChangeRowCount(RowCount+1); ReNum(); }
        Row++;
        break;
      case cssv_EnterRight:
        if(Col == ColCount-1){ ChangeColCount(ColCount+1); ReNum(); }
        Col++;
        break;
      default:
        break;
    }
  }
  else if(!Shift.Contains(ssShift) &&
     InplaceEditor->SelLength == InplaceEditor->Text.Length())
  {
    if(Key == VK_LEFT && !LeftArrowInCell && Col > FixedCols){
      Col--; Key = '\0';
    }else if(Key == VK_RIGHT && Col < ColCount-1){
      Col++; Key = '\0';
    }
  }
  else if(Key == VK_RIGHT && !Shift.Contains(ssShift) &&
     InplaceEditor->SelStart == InplaceEditor->Text.Length() &&
     InplaceEditor->SelLength == 0 &&
     Col < ColCount-1)
  {
    Col++; Key = '\0';
  }
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::KeyPress(char &Key)
{
  if(!RangeSelect){
    if(Key != '\n' && Key != '\r' && Key != '\t' && Key != '\x1B'){
      Modified = true; SetUndoMacro();
    }
    SpreadRightBottom();
    TStringGrid::KeyPress(Key);
  }
}
//---------------------------------------------------------------------------
//  Undo 処理
//
void TCsvGrid::SetUndoMacro(bool inRedo)
{
  if(UndoSetLock > 0) return;

  if(!UndoMacro) UndoMacro = new TStringList;

  AnsiString Tmp = (AnsiString)int(Col - DataLeft) + ","
                 + (AnsiString)int(Row - DataTop);
  if(UndoMacro->Count == 0 || UndoMacro->Names[0] != Tmp)
  {
    Tmp += (AnsiString)"=" + Cells[Col][Row];
    UndoMacro->Insert(0,Tmp);
  }

  if(!inRedo){
    if(RedoMacro){ delete RedoMacro; RedoMacro = NULL; }
    if(RedoCsv){ delete[] RedoCsv; RedoCsv = NULL; }
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::SetUndoCsv(bool inRedo)
{
  if(UndoSetLock > 0) return;

  if(UndoMacro){ delete UndoMacro; UndoMacro = NULL; }
  if(!UndoCsv){ delete[] UndoCsv; }

  UndoCsvWidth = ColCount - DataLeft;
  UndoCsvHeight = RowCount - DataTop;
  UndoCsv = new AnsiString[UndoCsvWidth * UndoCsvHeight];
  AnsiString *p = UndoCsv;
  for(int y=DataTop; y<RowCount; y++){
    for(int x=DataLeft; x<ColCount; x++){
      *p = Cells[x][y];
      p++;
    }
  }

  if(!inRedo){
    if(RedoMacro){ delete RedoMacro; RedoMacro = NULL; }
    if(RedoCsv){ delete[] RedoCsv; RedoCsv = NULL; }
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::ClearUndo()
{
  if(UndoMacro){ delete UndoMacro; UndoMacro = NULL; }
  if(UndoCsv){ delete[] UndoCsv; UndoCsv = NULL; }
  if(RedoMacro){ delete RedoMacro; RedoMacro = NULL; }
  if(RedoCsv){ delete[] RedoCsv; RedoCsv = NULL; }
}
//---------------------------------------------------------------------------
void TCsvGrid::Undo()
{
  if(UndoMacro)
  {
    AnsiString Macro = UndoMacro->Strings[0];
    int Sep = Macro.Pos(",");
    AnsiString Dat = Macro.SubString(1,Sep-1);
    Macro.Delete(1,Sep);
    Col = Dat.ToInt() + DataLeft;
    Sep = Macro.Pos("=");
    Dat = Macro.SubString(1,Sep-1);
    Macro.Delete(1,Sep);
    Row = Dat.ToInt() + DataTop;
    SetRedoMacro();
    Cells[Col][Row] = Macro;
    UndoMacro->Delete(0);
    if(UndoMacro->Count == 0){ delete UndoMacro; UndoMacro = NULL; }
  }
  else if(UndoCsv)
  {
    SetRedoCsv();
    for(int i=1; i <= ColCount; i++)
      Cols[i]->Clear();

    int RC = DataTop+UndoCsvHeight;
    int CC = DataLeft+UndoCsvWidth;
    ChangeRowCount(RC);
    ChangeColCount(CC);
    AnsiString *p = UndoCsv;
    for(int y=DataTop; y<RC; y++){
      for(int x=DataLeft; x<CC; x++){
        Cells[x][y] = *p;
        p++;
      }
    }
    delete[] UndoCsv; UndoCsv = NULL;
    ReNum();
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::SetRedoMacro()
{
  if(RedoCsv){ delete[] RedoCsv; RedoCsv = NULL; }
  if(!RedoMacro) RedoMacro = new TStringList;

  AnsiString Tmp = (AnsiString)int(Col - DataLeft) + ","
                 + (AnsiString)int(Row - DataTop);
  if(RedoMacro->Count == 0 || RedoMacro->Names[0] != Tmp)
  {
    Tmp += (AnsiString)"=" + Cells[Col][Row];
    RedoMacro->Insert(0,Tmp);
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::SetRedoCsv()
{
  if(!RedoCsv){ delete[] RedoCsv; }
  RedoCsvWidth = ColCount - DataLeft;
  RedoCsvHeight = RowCount - DataTop;
  RedoCsv = new AnsiString[RedoCsvWidth * RedoCsvHeight];
  AnsiString *p = RedoCsv;
  for(int y=DataTop; y<RowCount; y++){
    for(int x=DataLeft; x<ColCount; x++){
      *p = Cells[x][y];
      p++;
    }
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::Redo()
{
  if(RedoCsv){   
    SetUndoCsv(true);
    for(int i=1; i <= ColCount; i++)
      Cols[i]->Clear();

    int RC = DataTop+RedoCsvHeight;
    int CC = DataLeft+RedoCsvWidth;
    ChangeRowCount(RC);
    ChangeColCount(CC);
    AnsiString *p = RedoCsv;
    for(int y=DataTop; y<RC; y++){
      for(int x=DataLeft; x<CC; x++){
        Cells[x][y] = *p;
        p++;
      }
    }
    delete[] RedoCsv; RedoCsv = NULL;
    ReNum();
  } else if(RedoMacro) {
    AnsiString Macro = RedoMacro->Strings[0];
    int Sep = Macro.Pos(",");
    AnsiString Dat = Macro.SubString(1,Sep-1);
    Macro.Delete(1,Sep);
    Col = Dat.ToInt() + DataLeft;
    Sep = Macro.Pos("=");
    Dat = Macro.SubString(1,Sep-1);
    Macro.Delete(1,Sep);
    Row = Dat.ToInt() + DataTop;
    SetUndoMacro(true);
    Cells[Col][Row] = Macro;
    RedoMacro->Delete(0);
    if(RedoMacro->Count == 0){ delete RedoMacro; RedoMacro = NULL; }
  }
}
//---------------------------------------------------------------------------
namespace Csvgrid
{
    void __fastcall Register()
    {
        TComponentClass classes[1] = {__classid(TCsvGrid)};
        RegisterComponents("Samples", classes, 0);
    }
}
//---------------------------------------------------------------------------


