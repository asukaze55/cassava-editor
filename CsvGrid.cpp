//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <vcl\clipbrd.hpp>
#include <shellapi.h>
#include <string.h>
#include <process.h>
#include <regexp.h>
#pragma hdrstop

#include "PasteDlg.h"
#include "CsvGrid.h"
#include "AutoOpen.h"
#include "EncodedStream.h"
#include "FileOpenThread.h"

#define min(X,Y) ((X)<(Y) ? (X) : (Y))
#define max(X,Y) ((X)>(Y) ? (X) : (Y))
#define DataLeft (ShowRowCounter ? 1 : 0)
#define DataTop  (ShowColCounter ? 1 : 0)
#define soNone 0
#define soNormal 1
#define soString 2
#define soAll 3

#define RXtoAX(x)                 (x - DataLeft + 1)
#define RYtoAY(y)                 (y - DataTop  + 1)

bool SameCellClicking;
//---------------------------------------------------------------------------
static inline TCsvGrid *ValidCtrCheck()
{
    return new TCsvGrid(NULL);
}
//---------------------------------------------------------------------------
__fastcall TCsvGrid::TCsvGrid(TComponent* Owner)  //�f�t�H���g�̐ݒ�
    : TStringGrid(Owner)
{
  Options = Options << goRowSizing << goColSizing << goRowMoving << goColMoving
            << goDrawFocusSelected  << goThumbTracking << goTabs
            << goEditing << goAlwaysShowEditor << goRangeSelect;
  AlwaysShowEditor = true;
  DefaultColWidth = 64;
  MinColWidth = 32;
  Dragging = false;
  EditorMode = true;
  DragMove = true;
  TypeIndex = 0;
  TypeOption = TypeList.DefItem();
  PasteOption = -1;
  DefWay = 2;
  FShowRowCounter = true;
  FShowColCounter = true;
  UndoSetLock = 0;
  TextAlignment = cssv_taLeft;
  WheelMoveCursol = 0;
  SameCellClicking = false;
  ExecCellMacro = false;
  CalculatedCellCache = NULL;
  UsingCellMacro = NULL;
  FDataRight = 1;
  FDataBottom = 1;
  EOFMarker = new TObject();
  FileOpenThread = NULL;
}
//---------------------------------------------------------------------------
__fastcall TCsvGrid::~TCsvGrid(){
  ClearUndo();
  if(CalculatedCellCache){ delete CalculatedCellCache; }
  if(UsingCellMacro){ delete UsingCellMacro; }
  if(EOFMarker){ delete EOFMarker; }
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::ShowEditor()
{
  TStringGrid::ShowEditor();
  InplaceEditor->Brush->Color = Color;
}
//---------------------------------------------------------------------------
void TCsvGrid::SetTBMargin(int Value)
{
  FTBMargin = Value;
  UpdateDefaultRowHeight();
}
//---------------------------------------------------------------------------
void TCsvGrid::UpdateDefaultRowHeight()
{
  TEXTMETRIC tm;
  HDC hdc = ::GetDC(Handle);
  ::SelectObject(hdc, Font->Handle);
  ::GetTextMetrics(hdc, &tm);
  DefaultRowHeight = tm.tmHeight + (2 * FTBMargin);
  ::ReleaseDC(Handle, hdc);
}
//---------------------------------------------------------------------------
String FormatOmitDecimal(String str, int count)
{
  if(str == "" || count < 0) return str;
  int pos = str.Pos(".");
  if(pos > 0){
    return str.SubString(1, pos + count);
  }
  return str;
}
//---------------------------------------------------------------------------
String FormatNumComma(String Str, int Count)
{
  if(Str == "" || Count <= 0) return Str;
  int len = Str.Length();
  int L, R;
  int i = 1;
  for(; i<=len; i++){
    if(Str[i] >= _T('0') && Str[i] <= _T('9')) break;
  }
  L = i;
  for(; i<=len; i++){
    if(Str[i] < _T('0') || Str[i] > _T('9')) break;
  }
  R = i;
  for(i = R - Count; i > L; i -= Count){
    Str.Insert(",", i);
  }
  return Str;
}
//---------------------------------------------------------------------------
TRect TCsvGrid::DrawTextRect(TCanvas *Canvas, TRect Rect,
  String Str, bool Wrap, bool MeasureOnly)
{
  int x = Rect.Left;
  int y = Rect.Top;
  int textHeight = Canvas->TextHeight(Str);
  int yint = textHeight + CellLineMargin;
  int xmax = x;
  int drawIndex = 1;
  TRect R = Rect;
  for(int i=1; i<=Str.Length(); i++){
    if(y >= Rect.Bottom){
      return TRect(0,0,xmax-Rect.Left, Rect.Bottom-Rect.Top);
    }
    TCHAR c = Str[i];
    int w = Canvas->TextWidth(c);
    if(c == L'\n'){
      if(!MeasureOnly){
        R.Top = y;
        Canvas->TextRect(R, R.Left, y, Str.SubString(drawIndex, i-drawIndex));
        drawIndex = i + 1;
      }
      y += yint;
      x = Rect.Left;
    }else{
      if(Wrap && (x + w) > Rect.Right && x > Rect.Left){
        if(!MeasureOnly){
          R.Top = y;
          Canvas->TextRect(R, R.Left, y, Str.SubString(drawIndex, i-drawIndex));
          drawIndex = i;
        }
        y += yint;
        x = Rect.Left;
      }
      x += w;
      if(x > xmax){ xmax = x; }
    }
  }
  if(!MeasureOnly){
    R.Top = y;
    Canvas->TextRect(R, R.Left, y, Str.SubString(drawIndex, Str.Length()));
  }
  y += textHeight + (2 * TBMargin);
  return TRect(0,0,xmax-Rect.Left, min(y,Rect.Bottom)-Rect.Top);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DrawCell(int ACol, int ARow,
  const TRect &ARect, TGridDrawState AState)
{
  int cellType = CALC_NOTEXPR;
  String Str = Cells[ACol][ARow];
#ifdef CssvMacro
  if(ExecCellMacro){
    if(Str.Length() > 0 && Str[1] == '='){
      String CalcdStr = GetCalculatedCell(ACol+1-DataLeft, ARow+1-DataTop);
      if(!GetSelected(ACol,ARow)){
        switch(CalcdStr[1]){
        case CALC_OK:
          Canvas->Brush->Color = CalcBgColor;
          Canvas->FillRect(ARect);
          cellType = CALC_OK;
          break;
        case CALC_NG:
        case CALC_LOOP:
          Canvas->Brush->Color = CalcErrorBgColor;
          Canvas->FillRect(ARect);
          cellType = CALC_NG;
          break;
        }
      }
      Str = CalcdStr.SubString(2,CalcdStr.Length()-1);
    }
  }
#endif

  if(ACol >= FixedCols && ARow >= FixedRows &&
     (ACol > DataRight || ARow > DataBottom) &&
     (! GetSelected(ACol, ARow))){
    Canvas->Brush->Color = DummyBgColor;
    Canvas->FillRect(ARect);
  }

  TRect R;
  R.Left = ARect.Left + LRMargin; R.Top = ARect.Top + FTBMargin;
  R.Right = ARect.Right - LRMargin; R.Bottom = ARect.Bottom - FTBMargin;
  bool isnum = ((TextAlignment == cssv_taNumRight)
                || (DecimalDigits >= 0)
                || (NumberComma > 0))
             && IsNumber(Str);
  if(isnum && (DecimalDigits >= 0)){
    Str = FormatOmitDecimal(Str, DecimalDigits);
  }
  if(isnum && (NumberComma > 0)){
    Str = FormatNumComma(Str, NumberComma);
  }
  if((TextAlignment == cssv_taNumRight) && isnum){
    int L = R.Right - Canvas->TextWidth(Str);
    if(R.Left < L) R.Left = L;
  }

  if(TypeOption->DummyEof && ACol == DataRight + 1 && ARow == DataBottom + 1){
    TColor CFC = Canvas->Font->Color;
    Canvas->Font->Color = clGray;
    Canvas->TextRect(R, R.Left, R.Top, "[EOF]");
    Canvas->Font->Color = CFC;
  }else if(ShowURLBlue && isUrl(Str)){
    TFontStyles CFS = Canvas->Font->Style;
    TColor CFC = Canvas->Font->Color;
    Canvas->Font->Style = TFontStyles() << fsUnderline;
    Canvas->Font->Color = UrlColor;
    DrawTextRect(Canvas, R, Str, WordWrap);
    Canvas->Font->Style = CFS;
    Canvas->Font->Color = CFC;
  }else{
    TColor CFC = Canvas->Font->Color;
    if(cellType == CALC_OK){
      Canvas->Font->Color = CalcFgColor;
    }else if(cellType == CALC_NG){
      Canvas->Font->Color = CalcErrorFgColor;
    }else if(ACol < FixedCols || ARow < FixedRows){
      Canvas->Font->Color = FixFgColor;
    }
    DrawTextRect(Canvas, R, Str, WordWrap);
    Canvas->Font->Color = CFC;
  }

  // TextRect�łȂ����͈͂̊O�܂Ŕw�i�F���h���Ă��܂��̂ŁA
  // ���O�Ő��������Ȃ���
  TColor PenColor = Canvas->Pen->Color;
  if(ACol < FixedCols || ARow < FixedRows){
    Canvas->Pen->Color = clBlack;
  }else{
    // Canvas->Pen->Color = clGray; �� �F�̐ݒ�͂���Ȃ�
  }
  Canvas->MoveTo(ARect.Right, ARect.Top);
  Canvas->LineTo(ARect.Right, ARect.Bottom);
  Canvas->LineTo(ARect.Left, ARect.Bottom);
  Canvas->Pen->Color = PenColor;
}
//---------------------------------------------------------------------------
//  ��{���o�̓��\�b�h
//
void TCsvGrid::SetExecCellMacro(bool Value)
{
  FExecCellMacro = Value;
  ClearCalcCache();
}
//---------------------------------------------------------------------------
void TCsvGrid::ClearCalcCache()
{
  if(CalculatedCellCache){
    delete CalculatedCellCache;
    CalculatedCellCache = NULL;
  }
  if(UsingCellMacro){
    delete UsingCellMacro;
    UsingCellMacro = NULL;
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::ErrorCalcLoop()
{
  if(!CalculatedCellCache){ CalculatedCellCache = new TStringList; }
  for(int i=0; i<UsingCellMacro->Count; i++){
    AnsiString cellname = UsingCellMacro->Names[i];
    CalculatedCellCache->Values[cellname]
      = (AnsiString)CALC_LOOP + UsingCellMacro->ValueFromIndex[i];
  }
}
//---------------------------------------------------------------------------
String TCsvGrid::GetCalculatedCell(int ACol, int ARow)
{
  int c = ACol+DataLeft-1;
  int r = ARow+DataTop-1;
  if(ACol<=0 || ARow<=0 || c<0 || c>=ColCount || r<0 || r>=RowCount) {
    return CALC_NOTEXPR;
  }

  // �v�Z���łȂ����̂͂Ȃɂ����Ȃ�
  AnsiString Str = Cells[c][r];
  if(!ExecCellMacro || Str.Length() == 0 || Str[1] != '='){
    return (AnsiString)CALC_NOTEXPR + Str;
  }

  AnsiString key = (AnsiString)"[" + ACol + "," + ARow + "]";
  // ���ȎQ�Ƃ��͂���
  if(UsingCellMacro){
    int index = UsingCellMacro->IndexOfName(key);
    if(index >= 0){
      ErrorCalcLoop();
      return (AnsiString)CALC_LOOP + Str;
    }
  }
  // �L���b�V��������ΕԂ�
  if(CalculatedCellCache){
    int index = CalculatedCellCache->IndexOfName(key);
    if(index >= 0){
      AnsiString Result = CalculatedCellCache->Values[key];
      // �L���b�V�����G���[�Ȃ玩�����G���[
      if((Result[1] & CALC_NG) > 0){
        ErrorCalcLoop();
        return (AnsiString)CALC_LOOP + Str;
      }
      return Result;
    }
  }

  if(OnGetCalculatedCell){
    // ���ȎQ�ƃ`�F�b�N�̂��߂̃��X�g���X�V
    if(!UsingCellMacro){ UsingCellMacro = new TStringList; }
    UsingCellMacro->Values[key] = Str;

    // OnGetCalculatedCell�ɈϏ�
    AnsiString Result = OnGetCalculatedCell(Str, ACol, ARow);

    // �L���b�V�����X�V
    if(!CalculatedCellCache){ CalculatedCellCache = new TStringList; }
    AnsiString oldValue = CalculatedCellCache->Values[key];
    if(oldValue.Length() > 0 && oldValue[1] == CALC_LOOP){
      // ���ȎQ�ƃG���[���N�����Ă���΍X�V���Ȃ�
      Result = oldValue;
    }else{
      CalculatedCellCache->Values[key] = Result;
    }

    // ���ȎQ�ƃ`�F�b�N�̂��߂̃��X�g�����ɖ߂�
    UsingCellMacro->Delete(UsingCellMacro->IndexOfName(key));

    // ����
    return Result;
  }else{
    return (AnsiString)CALC_NOTEXPR + Str;
  }
}
//---------------------------------------------------------------------------
String TCsvGrid::GetACells(int ACol, int ARow)
{
  String Str = GetCalculatedCell(ACol, ARow);
  Str.Delete(1,1);
  int len = Str.Length();
  for(int i=len; i > 0; i--){
    if(Str[i] == '\r'){
      if(i == len || Str[i+1] != '\n'){
        Str[i] = '\n';
      }else{
        Str.Delete(i, 1);
      }
    }
  }
  return Str;
}
//---------------------------------------------------------------------------
void TCsvGrid::SetACells(int ACol, int ARow, String Val){
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
  int rx = ACol+DataLeft-1;
  int ry = ARow+DataTop-1;
  if(ColCount <= rx) { ChangeColCount(rx + 1); }
  if(RowCount <= ry) { ChangeRowCount(ry + 1); }
  Cells[rx][ry] = Val;
  UpdateDataRightBottom(rx, ry);
}
//---------------------------------------------------------------------------
void TCsvGrid::SetModified(bool Value)
{
  bool Before = FModified;
  FModified = Value;
  ClearCalcCache();
  if(FModified != Before && OnChangeModified) OnChangeModified(this);
}
//---------------------------------------------------------------------------
void TCsvGrid::Clear(int AColCount, int ARowCount, bool UpdateRightBottom)
{
  if(FileOpenThread){
    FileOpenThread->Terminate();
    FileOpenThread = NULL;
  }
  Row = FixedRows;  Col = FixedCols;
  DefaultColWidth = 64;               //��̕���64�ɖ߂�
  ColWidths[0] = 32;
  for(int i=DataTop; i <= RowCount; i++)    //���e�̃N���A
    Rows[i]->Clear();
  if(FixedCols >= AColCount){ AColCount = FixedCols + 1; }
  if(FixedRows >= ARowCount){ ARowCount = FixedRows + 1; }
  ChangeColCount(max(AColCount+1, 5));
  ChangeRowCount(max(ARowCount+1, 5));
  ClearUndo();
  Modified = false;
  TypeIndex = 0;
  TypeOption = TypeList.DefItem();
  if(UpdateRightBottom){
    FDataRight = AColCount - 1;
    FDataBottom = ARowCount - 1;
  }else{
    FDataRight = DataLeft;
    FDataBottom = DataTop;
  }
  if(TypeOption->DummyEof){
    Objects[FDataRight+1][FDataBottom+1] = EOFMarker;
  }
  ReNum();
}
//---------------------------------------------------------------------------
void TCsvGrid::ReNum()
{
  int DR = DataRight+1-DataLeft;
  int DB = DataBottom+1-DataTop;
  if(FShowRowCounter){
    for(int i=1; i <= DB; i++)
      Cells[0][i+DataTop-1] = (AnsiString)i;
    for(int i=DB+1; i <= RowCount; i++)
      Cells[0][i+DataTop-1] = "";
  }
  if(FShowColCounter){
    for(int i=1; i <= DR; i++)
      Cells[i+DataLeft-1][0] = (AnsiString)i;
    for(int i=DR+1; i <= ColCount; i++)
      Cells[i+DataLeft-1][0] = "";
  }
  // [169] �}�E�X�𗣂������ɉ�ʂ������ �̉��P�̂��ߍ폜
  // Invalidate();
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
int TCsvGrid::TextWidth(TCanvas *cnvs, String str)
{
  if((NumberComma > 0) && IsNumber(str)){
    str = FormatNumComma(str, NumberComma);
  }
  TRect MaxRect(0,0,
    (ClientWidth - ColWidths[0]), (ClientHeight - RowHeights[0]) / 2);
  TRect R = DrawTextRect(cnvs, MaxRect, str, WordWrap, true);

  return R.Right;
}

//---------------------------------------------------------------------------
void TCsvGrid::SetWidth(int i)
{
  int WMax = 0;                     //���̗�ŕ��̍ő�l
  if(i==0 && ShowRowCounter){
    if(DataBottom >= 0){
      ColWidths[i] = max(MinColWidth,
        TextWidth(Canvas, Cells[0][DataBottom]) + (2 * LRMargin));
    }else{
      ColWidths[i] = MinColWidth;
    }
    return;
  }

  int t;
  int b;
  if(CalcWidthForAllRow){
    t = DataTop;
    b = DataBottom;
  }else{
    t = TopRow;
    b = min(TopRow + VisibleRowCount, RowCount);
  }
  for(int j=t; j <= b; j++){
    AnsiString Str = GetACells(RXtoAX(i), RYtoAY(j));
    WMax = max(WMax, TextWidth(Canvas, Str));
  }
  if(WMax > 0){ WMax += 2 * LRMargin; }
  if(WMax < MinColWidth){ //�������Ȃ��悤��
    WMax = MinColWidth;
  }else if( WMax >= ClientWidth - ColWidths[0]){ //�L�����Ȃ��悤��
    WMax = ClientWidth - ColWidths[0] - 2 * GridLineWidth;
  }
  ColWidths[i] = WMax;
}
//---------------------------------------------------------------------------
void TCsvGrid::SetWidth()             //�񕝂̍œK��
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
  int r = min(LeftCol + VisibleColCount, ColCount);
  for(int i=LeftCol; i <= r; i++){
    TRect MaxRect(0, 0, ColWidths[i] - (2 * LRMargin),
      (ClientHeight - RowHeights[0]) / 2);
    AnsiString Str = GetACells(i+1-DataLeft, j+1-DataTop);
    TRect R = DrawTextRect(Canvas, MaxRect, Str, WordWrap, true);
    HMax = max(HMax, R.Bottom);
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
  int BigColSize = 0;
  int Room = WindowSize - (Minimum*CC);
  if(!Cnvs) Cnvs = Canvas;

  int RT;
  int RC;
  if(CalcWidthForAllRow){
    RT = 1;
    RC = DataBottom - DataTop + 1;
  }else{
    RT = TopRow - DataTop + 1;
    RC = min(TopRow + VisibleRowCount, RowCount) - DataTop + 1;
  }

  for(int i=1; i<=CC; i++){
    int AWMax = 0;                     //���̗�ŕ��̍ő�l
    for(int j=RT; j<=RC; j++){
      AWMax = max(AWMax, TextWidth(Cnvs, GetACells(i,j)));
    }
    if(AWMax > 0){ AWMax += (2 * LRMargin); }
    Widths[i] = max((AWMax - Minimum), 0);
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
  int WindowSize = ClientWidth - 16; // �X�N���[���o�[����

  if(ShowRowCounter){ // �J�E���^�Z��
    WMax[0] = max(MinColWidth,
      TextWidth(Canvas, Cells[0][DataBottom]) + 4);
    ColWidths[0] = WMax[0];
    WindowSize -= WMax[0];
  }

  CompactWidth(WMax, WindowSize, MinColWidth);
  for(int i=DataLeft; i<=R; i++) ColWidths[i] = WMax[i-DataLeft+1];
  delete[] WMax;
}
//---------------------------------------------------------------------------
void TCsvGrid::Cut()                  //�E�A���̗]�v�ȍ��ڂ��폜
{
  if(Col > DataRight){ Col = DataRight; }
  if(Row > DataBottom){ Row = DataBottom; }
  ChangeColCount(max(5,DataRight+2));
  ChangeRowCount(max(5,DataBottom+2));
}
//---------------------------------------------------------------------------
void TCsvGrid::UpdateDataRight()
{
  int i,j;
  bool findEof = false;
  for(i=ColCount-1; i>=DataLeft; i--) {
    for(j=RowCount-1; j>=DataTop; j--) {
      if(Cells[i][j] != "") {
        FDataRight = i;
        return;
      }else if(TypeOption->DummyEof && Objects[i][j]==EOFMarker) {
        findEof = true;
      }
    }
    if(findEof){
      FDataRight = i - 1;
      return;
    }
  }
  FDataRight = DataLeft;
}
//---------------------------------------------------------------------------
void TCsvGrid::UpdateDataBottom()
{
  int i,j;
  bool findEof = false;
  for(j=RowCount-1; j>=DataTop; j--) {
    for(i=ColCount-1; i>=DataLeft; i--) {
      if(Cells[i][j] != ""){
        FDataBottom = j;
        return;
      }else if(TypeOption->DummyEof && Objects[i][j]==EOFMarker) {
        findEof = true;
      }
    }
    if(findEof){
      FDataBottom = j - 1;
      return;
    }
  }
  FDataBottom = DataTop;
}
//---------------------------------------------------------------------------
void TCsvGrid::UpdateDataRightBottom(int modx, int mody, bool updateTableSize)
{
  int oldRight = DataRight;
  int oldBottom = DataBottom;

  if(modx <= 0 && mody <= 0){
    UpdateDataRight();
    UpdateDataBottom();
    int cc = FDataRight + 2;
    if(cc < 5){ cc = 5; }
    if(cc <= modx){ cc = modx + 1; }
    if(cc <= Col){ cc = Col + 1; }
    if(updateTableSize){ ChangeColCount(cc); }
    int rc = FDataBottom+2;
    if(rc < 5){ rc = 5; }
    if(rc <= mody){ rc = mody + 1; }
    if(rc <= Row){ rc = Row + 1; }
    if(updateTableSize){ ChangeRowCount(rc); }
  }else if(Cells[modx][mody] == ""){
    if(modx == oldRight){ UpdateDataRight(); }
    if(mody == oldBottom){ UpdateDataBottom(); }
  }else{
    if(modx > oldRight){
      UpdateDataRight();
      int cc = FDataRight + 2;
      if(cc < 5){ cc = 5; }
      if(cc <= modx){ cc = modx + 1; }
      if(cc <= Col){ cc = Col + 1; }
      if(updateTableSize){ ChangeColCount(cc); }
    }
    if(mody > oldBottom){
      UpdateDataBottom();
      int rc = FDataBottom+2;
      if(rc < 5){ rc = 5; }
      if(rc <= mody){ rc = mody + 1; }
      if(rc <= Row){ rc = Row + 1; }
      if(updateTableSize){ ChangeRowCount(rc); }
    }
  }

  if(FDataRight != oldRight || FDataBottom != oldBottom){
    if(TypeOption->DummyEof) {
      UpdateEOFMarker(oldRight, oldBottom);
    }
    ReNum();
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::UpdateEOFMarker(int oldRight, int oldBottom)
{
  if(oldRight+1 < ColCount && oldBottom+1 < RowCount &&
     Objects[oldRight+1][oldBottom+1] == EOFMarker){
    if(Col == oldRight+1 && Row == oldBottom+1 && InplaceEditor){
      // Objects�X�V���ɑI��͈͂������������̂𕜌�
      int ss = InplaceEditor->SelStart;
      int sl = InplaceEditor->SelLength;
      Objects[oldRight+1][oldBottom+1] = NULL;
      InplaceEditor->SelStart = ss;
      InplaceEditor->SelLength = sl;
    }else{
      Objects[oldRight+1][oldBottom+1] = NULL;
    }
  }
  if(FDataRight+1 < ColCount && FDataBottom+1 < RowCount){
    Objects[FDataRight+1][FDataBottom+1] = EOFMarker;
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::SetDataRightBottom(int rx, int by, bool updateTableSize)
{
  int oldRight = DataRight;
  int oldBottom = DataBottom;

  FDataRight = rx;
  FDataBottom = by;
  if(updateTableSize){
    int cc = rx + 2;
    if(cc < 5){ cc = 5; }
    if(cc <= Col){ cc = Col + 1; }
    ChangeColCount(cc);
  }
  if(updateTableSize){
    int rc = by+2;
    if(rc < 5){ rc = 5; }
    if(rc <= Row){ rc = Row + 1; }
    ChangeRowCount(rc);
  }

  if(TypeOption->DummyEof) {
    if(FDataRight != oldRight || FDataBottom != oldBottom
       || (FDataRight+1 < ColCount && FDataBottom+1 < RowCount
           && Objects[FDataRight+1][FDataBottom+1] != EOFMarker)){
      UpdateEOFMarker(oldRight, oldBottom);
      ReNum();
    }
  }else if(FDataRight != oldRight || FDataBottom != oldBottom){
    ReNum();
  }
}
//---------------------------------------------------------------------------
char GetReturnCodeAndReplaceNull(char *str, int length)
{
  int crlf = 0;
  int lf = 0;
  int cr = 0;
  char *end = str + length - 1;
  for(char *s = str; s < end; s++){
    if(*s == '\x0D'){
      if(*(s+1) == '\x0A'){
        crlf++;
        s++;
      }else{
        cr++;
      }
    }else if(*s == '\x0A'){
      lf++;
    }else if(*s == '\0'){
      *s = ' ';
    }
  }
  if(lf > crlf){
    if(lf > cr){
      return '\x0A';
    }else {
      return '\x0D';
    }
  }else if(cr > crlf){
    return '\x0D';
  }

  return LFCR;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::SetDragDropAccept(bool Accept)
{
    ::DragAcceptFiles(Handle,Accept);
    FDragDropAccept=Accept;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DropCsvFiles(TWMDropFiles inMsg)
{
  int DFiles = DragQueryFile((HDROP)inMsg.Drop, 0xffffffff, (TCHAR *) NULL,255);
  String *FileNames = new String[DFiles];
  TCHAR theFileName[255];
  for(int i=0;i<DFiles;i++){
    ::DragQueryFile((HDROP)inMsg.Drop, i, theFileName,255);
    FileNames[i]=theFileName;
  }
  if(FOnDropFiles != NULL ){
    FOnDropFiles(this, DFiles, FileNames);
  }
  delete[] FileNames;
}
//---------------------------------------------------------------------------
bool TCsvGrid::LoadFromFile(String FileName, int KCode,
  void (__closure *OnTerminate)(System::TObject* Sender))
{
  char *buf, *sjis;
  int filelength;
  unsigned long sjislen;

  TFileStream *File = new TFileStream(FileName, fmOpenRead|fmShareDenyNone);
  filelength = File->Size;
  Clear();
  buf = new char[filelength+1];
  int buflen = File->Read(buf, File->Size);
  buf[buflen] = '\0';
  delete File;

  if(!CheckKanji && KCode == CHARCODE_AUTO){ KCode = CHARCODE_SJIS; }
  if(KCode == CHARCODE_SJIS){
    sjis = buf;
  }else{
    sjis = new char[filelength+1];
  }

  switch(KCode){
  case CHARCODE_AUTO:
    KCode = NkfConvertSafe(sjis, filelength+1, &sjislen, buf, buflen, "-xs");
    break;
  case CHARCODE_SJIS:
    sjislen = filelength;
    break;
  case CHARCODE_EUC:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, buflen, "-Exs");
    break;
  case CHARCODE_JIS:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, buflen, "-Jxs");
    break;
  case CHARCODE_UTF8:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, buflen, "-Wxs");
    break;
  case CHARCODE_UTF16LE:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, buflen, "-W16Lxs");
    break;
  case CHARCODE_UTF16BE:
    NkfConvertSafe(sjis, filelength+1, &sjislen, buf, buflen, "-W16Bxs");
    break;
  }
  KanjiCode = KCode;
  if(sjis != buf) delete[] buf;

  // ���s�R�[�h�𓾂���ł�NULL�������X�y�[�X�ɒu��
  ReturnCode = GetReturnCodeAndReplaceNull(sjis, sjislen);
  AnsiString Ext = ExtractFileExt(FileName);
  if(Ext.Length() > 1 && Ext[1]=='.'){ Ext.Delete(1,1); }
  TypeIndex = TypeList.IndexOf(Ext);
  TypeOption = TypeList.Items(TypeIndex);
  Cursor = crAppStart;
  Hint = "�t�@�C����ǂݍ��ݒ��ł��B";
  ShowHint = true;

  OnFileOpenThreadTerminate = OnTerminate;
  FileOpenThread = ThreadFileOpen(this, FileName, sjis, sjislen);
  FileOpenThread->OnTerminate = FileOpenThreadTerminate;
  FileOpenThread->FreeOnTerminate = true;
  FileOpenThread->Start();
  Modified = false;
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::FileOpenThreadTerminate(System::TObject* Sender)
{
  FileOpenThread = NULL;
  Cursor = crDefault;
  Hint = "";
  ShowHint = false;
  ParentShowHint = true;
  if(OnFileOpenThreadTerminate){
    OnFileOpenThreadTerminate(Sender);
    OnFileOpenThreadTerminate = NULL;
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::PasteCSV(TStrings *List , int Left , int Top , int Way ,
			int ClipCols , int ClipRows)
{
  //Way
  // -1: �V�K�O���b�h
  //  0: �d�Ȃ��������̂�
  //  1: ����Ԃ�����
  //  2: �f�[�^�T�C�Y�㏑��
  //  3: �E�}��
  //  4: ���}��
  //  5: �e�L�X�g�Ƃ��ăZ������
  if((Way == 2 || Way == -1) && RowCount <= List->Count + Top){
    ChangeRowCount(List->Count + Top + 1);   //�s��������Ȃ���Α��₷
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
//  int MultiLine = 0;
  int colcount = 1;
  for(int i=0; i < iEnd; i++)
  {
    int lc = linecont;
    int ii= i;
    if(Way == 1 && ClipRows>0) ii = i % ClipRows;
    if(lc) List->Strings[ii] = (AnsiString)"\"" + List->Strings[ii];
    linecont = SetCsv(OneRow, List->Strings[ii]); //�J���}��؂�����^�[����؂��
    if((Way == -1 || Way > 1) && OneRow->Count + Left >= ColCount)
      ChangeColCount(OneRow->Count + Left + 1);   //�񐔂�����Ȃ���Α��₷
    if(Way == -1)
    {
       if(lc){
        Top--;
//        MultiLine++;
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
//        }else{
//          int RH = (Canvas->TextHeight("��")+4) * MultiLine;
//          if(RowHeights[ii] < RH) RowHeights[ii] = RH;
        }
      }else{
        for(int j=0; j<Left; j++) OneRow->Insert(0, (i+Top));
        if(linecont) linecont = OneRow->Count - 1;
        Rows[i+Top] = OneRow;
//        MultiLine = 0;
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
  Modified = true;

  if(Way == -1){
    if(TypeOption->DummyEof){
      if(colcount >= ColCount){
        ChangeColCount(colcount + 1);
      }
      Objects[FDataRight+1][FDataBottom+1] = NULL;
      Objects[colcount][iEnd+Top] = EOFMarker;
    }
    FDataRight = colcount - 1;
    FDataBottom = iEnd+Top - 1;
    ReNum();
  }else{
    UpdateDataRightBottom(0,0);
  }
}
//---------------------------------------------------------------------------
bool TCsvGrid::SetCsv(TStringList *Dest, String Src)
{
  int CellBegin = 1;
  bool Quoted = false;
  int Kugiri = 2; // 0:�ʏ� 1:���؂�i" "�j 2:����؂�i"," "\t"�j
  Dest->Clear();
  for(int i=1; i<=Src.Length(); i++){
   if(TypeOption->SepChars.Pos(Src[i]) > 0 ){
      if(!Quoted){
	      if(Kugiri != 1) Dest->Add(Src.SubString(CellBegin,i-CellBegin));
	      Kugiri = 2;
	      CellBegin = i+1;
//        if(Way == -1 && Src[i] == ',') CommaSeparated = true;
//        else if(Way == -1 && Src[i] == '\t') TabSeparated = true;
      }
    }else if(TypeOption->WeakSepChars.Pos(Src[i]) > 0 ){
      if(!Quoted){
      	if(Kugiri == 0){
      	  Dest->Add(Src.SubString(CellBegin,i-CellBegin));
      	  Kugiri = 1;
      	}
      	CellBegin = i+1;
      }
    }else if(TypeOption->UseQuote() &&
             TypeOption->QuoteChars.Pos(Src[i]) > 0){
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
void TCsvGrid::SaveToFile(String FileName, TTypeOption *Format,
                          bool SetModifiedFalse)
{
  TFileStream *fs = new TFileStream(FileName, fmCreate | fmShareDenyWrite);
  EncodedStream *es = new EncodedStream(fs, KanjiCode, UnicodeWindowsMapping);

  WriteGrid(es, Format);

  // ��� EncodedStream ��j�����邱��
  delete es;
  delete fs;
  if(SetModifiedFalse){
    Modified = false;
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::WriteGrid(TStream *Stream, TTypeOption *Format)
{
  if(Format == NULL) Format = TypeOption;

  TStringList* Data = new TStringList;
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
    if(ShowRowCounter) Data->Delete(0);   // �J�E���^�Z���̍폜

    AnsiString line = StringsToCSV(Data, Format);
    if(ReturnCode == LFCR){
      line = line + "\r\n";
    }else{
      line = line + ReturnCode;
    }
    Stream->Write(line.c_str(), line.Length());
  }
  delete Data;
}
//---------------------------------------------------------------------------
String TCsvGrid::StringsToCSV(TStrings* Data, TTypeOption *Format)
{
  char Sep = Format->DefSepChar();
  String Text = "";
  String Delim = Format->SepChars + Format->WeakSepChars
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
void TCsvGrid::QuotedDataToStrings(TStrings *Lines, String Text, TTypeOption *Format)
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
		OneLine->Add(GetACells(RXtoAX(j), RYtoAY(i)));
		  if(Cut == true) Cells[j][i] = "";
      }
      AnsiString ALine = StringsToCSV(OneLine, TypeOption);
      // if(*(AnsiLastChar(ALine)) == ',') ALine += "\"\"";
      Data->Add(ALine);
	}
	delete OneLine;
	String Txt = Data->Text;
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
      {            //�s�I����
        Selection.Left = DataLeft;
        ClipColCount = SelectColCount = ColCount-SLeft;
        Way = 4;
      }
      else if(RowSelected && !ColSelected)
      {            //��I����
        Selection.Top = DataTop;
        ClipRowCount = SelectRowCount = RowCount-STop;
        Way = 3;
      }
      else if(ClipRowCount != SelectRowCount || ClipColCount != SelectColCount)
      {
        TfmPasteDialog *PstDlg = new TfmPasteDialog(Application);
          PstDlg->Way->ItemIndex = DefWay;
	  PstDlg->lbMessage->Caption
	    = AnsiString("�I���T�C�Y�F ")
	    + SelectColCount + " �~ " + SelectRowCount
	    + "�@�N���b�v�{�[�h�T�C�Y�F "
	    + ClipColCount + " �~ " + ClipRowCount;
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
  else //if (EditorMode == true)
  {
    InplaceEditor->PasteFromClipboard();
    InplaceEditor->Text = TrimRight(InplaceEditor->Text);
  }
  delete Data;
  Modified = true;
  ReNum();
}
//---------------------------------------------------------------------------
bool TCsvGrid::IsNumber(String Str)
{
  try{
    if(Str == "") return false;
    Str.ToDouble();
    return true;
  }catch(...){}

  return false;
}
//---------------------------------------------------------------------------
typedef struct { double Num; void *Data; int Row; } DoubleData;
//---------------------------------------------------------------------------
int __fastcall CompareDoubleData(void *a, void *b) {
  DoubleData *dda = static_cast<DoubleData*>(a);
  DoubleData *ddb = static_cast<DoubleData*>(b);
  if(dda->Num == ddb->Num){
    if(dda->Row == ddb->Row) return 0;
    return ((dda->Row < ddb->Row) ? -1 : 1);
  }else return ((dda->Num < ddb->Num) ? -1 : 1);
}
//---------------------------------------------------------------------------
typedef struct { String Str; void *Data; int Row; } StringData;
//---------------------------------------------------------------------------
int __fastcall CompareOrderedString(void *a, void *b) {
  StringData *osa = static_cast<StringData*>(a);
  StringData *osb = static_cast<StringData*>(b);
  if(osa->Str == osb->Str){
    if(osa->Row == osb->Row) return 0;
    return ((osa->Row < osb->Row) ? -1 : 1);
  }else return ((osa->Str < osb->Str) ? -1 : 1);
}
//---------------------------------------------------------------------------
void TCsvGrid::Sort(int SLeft, int STop, int SRight, int SBottom, int SCol,
  bool Shoujun, bool NumSort, bool IgnoreCase, bool IgnoreZenhan)
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
      String str = Cells[SCol][y];
      if(IgnoreCase){
        str = str.UpperCase();
      }
      if(IgnoreZenhan){
        str = TransChar(TransKana(str, 5), 1);
      }
      SD->Str = str;
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
double TCsvGrid::GetSum(int l, int t, int r, int b, int *Count)
{
  double Sum = 0;
  int NumCount = 0;
  for(int i=l; i <= r; i++){
	for(int j=t; j <= b; j++){
	  AnsiString Str = GetACells(i,j);
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
double TCsvGrid::GetAvr(int l, int t, int r, int b)
{
  int Count;
  double Sum = GetSum(l,t,r,b,&Count);
  if(Count==0){
    return 0;
  }else{
    return (Sum / Count);
  }
}
//---------------------------------------------------------------------------
double TCsvGrid::SelectSum(int *Count)
{
  double Sum = 0;
  int NumCount = 0;
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      AnsiString Str = GetACells(RXtoAX(i),RYtoAY(j));
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
  if(wc == L' ') return L'�@';
  else if(wc < L'�' || wc > L'�') return wc;
  else if(wc >= L'�' && wc <= L'�') return L'�A' + (wc - L'�') * 2;
  else if(wc >= L'�' && wc <= L'�') return L'�J' + (wc - L'�') * 2;
  else if(wc >= L'�' && wc <= L'�') return L'�c' + (wc - L'�') * 2;
  else if(wc >= L'�' && wc <= L'�') return L'�i' + (wc - L'�');
  else if(wc >= L'�' && wc <= L'�') return L'�n' + (wc - L'�') * 3;
  else if(wc >= L'�' && wc <= L'�') return L'�}' + (wc - L'�');
  else if(wc >= L'�' && wc <= L'�') return L'��' + (wc - L'�') * 2;
  else if(wc >= L'�' && wc <= L'�') return L'��' + (wc - L'�');
  else if(wc == L'�') return L'��';
  else if(wc == L'�') return L'��';
  else if(wc == L'�') return L'��';
  else if(wc >= L'�' && wc <= L'�') return L'�@' + (wc - L'�') * 2;
  else if(wc >= L'�' && wc <= L'�') return L'��' + (wc - L'�') * 2;
  else if(wc == L'�') return L'�b';
  else if(wc == L'�') return L'�[';
  else if(wc == L'�') return L'�J';
  else if(wc == L'�') return L'�K';
  else if(wc == L'�') return L'�B';
  else if(wc == L'�') return L'�u';
  else if(wc == L'�') return L'�v';
  else if(wc == L'�') return L'�A';
  else if(wc == L'�') return L'�E';

  return wc;
}
//---------------------------------------------------------------------------
int Zenkaku2Hankaku(wchar_t wc, wchar_t *ans)
{
  if(wc == L'�@'){ *ans = L' '; return 1; }
  else if(wc == L'��'){ *ans = L'�'; return 1; }
  else if(wc == L'��'){ *ans = L'�'; return 1; }
  else if(wc == L'��'){ *ans = L'�'; return 1; }
  else if(wc == L'��'){ *ans = L'�'; *(ans+1) = L'�'; return 2; }
  else if(wc == L'�J'){ *ans = L'�'; return 1; }
  else if(wc == L'�K'){ *ans = L'�'; return 1; }
  else if(wc == L'�['){ *ans = L'�'; return 1; }
  else if(wc == L'�B'){ *ans = L'�'; return 1; }
  else if(wc == L'�u'){ *ans = L'�'; return 1; }
  else if(wc == L'�v'){ *ans = L'�'; return 1; }
  else if(wc == L'�A'){ *ans = L'�'; return 1; }
  else if(wc == L'�E'){ *ans = L'�'; return 1; }
  else if(wc < L'�@' || wc > L'��'){ *ans = wc; return 1; }
  else if(wc == L'�b'){ *ans = L'�'; return 1; }
  else if(wc >= L'�@' && wc <= L'�I'){
    int x = (wc - L'�@');
    *ans = ((x % 2) ? (L'�' + (x / 2)) : (L'�' + (x / 2)));
    return 1;
  }else if(wc >= L'�J' && wc <= L'�a'){
    int x = (wc - L'�J');
    *ans = L'�' + (x / 2);
    if(x % 2){
      *(ans+1) = L'�';
      return 2;
    }else{
      return 1;
    }
  }else if(wc >= L'�c' && wc <= L'�h'){
    int x = (wc - L'�c');
    *ans = L'�' + (x / 2);
    if(x % 2){
      *(ans+1) = L'�';
      return 2;
    }else{
      return 1;
    }
  }else if(wc >= L'�i' && wc <= L'�m'){
    *ans = L'�' + (wc - L'�i');
    return 1;
  }else if(wc >= L'�n' && wc <= L'�|'){
    int x = (wc - L'�n');
    *ans = L'�' + (x / 3);
    if((x % 3) == 1){
      *(ans+1) = L'�';
      return 2;
    }else if((x % 3) == 2){
      *(ans+1) = L'�';
      return 2;
    }else{
      return 1;
    }
  }else if(wc >= L'�}' && wc <= L'��'){
    *ans = L'�' + (wc - L'�}');
    return 1;
  }else if(wc >= L'��' && wc <= L'��'){
    int x = (wc - L'��');
    *ans = ((x % 2) ? (L'�' + (x / 2)) : (L'�' + (x / 2)));
    return 1;
  }else if(wc >= L'��' && wc <= L'��'){
    *ans = L'�' + (wc - L'��');
    return 1;
  }

  return 0;
}
//---------------------------------------------------------------------------
void TCsvGrid::TransChar(int Type)
{
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      Cells[i][j] = TransChar(Cells[i][j], Type);
    }
  }
  Modified = true;
}
//---------------------------------------------------------------------------
String TCsvGrid::TransChar(String Str, int Type)
{
  if(Type==2)      Str = Str.UpperCase();
  else if(Type==3) Str = Str.LowerCase();
  else{
    int size = Str.Length();
    wchar_t *wcfr = Str.c_str();
    wchar_t *wcto = new wchar_t[size+1];
    wchar_t *p=wcfr;
    wchar_t *q=wcto;
    wchar_t *r=wcfr + size;
    for(; p < r; p++){
      if(Type==0){
        if(*p == L'�@')      *q = L' ';
        else if(*p == L'�f') *q = L'\'';
        else if(*p == L'�h') *q = L'\"';
        else if(*p == L'��') *q = L'\\';
        else if(*p >= L'�I' && *p <= L'�`') *q = *p - (L'�`' - L'A');
        else                 *q = *p;
      }else{
        if(*p == L' ')       *q = L'�@';
        else if(*p == L'\'') *q = L'�f';
        else if(*p == L'\"') *q = L'�h';
        else if(*p == L'\\') *q = L'��';
        else if(*p >= L'!' && *p <= L'~') *q = *p + (L'�`' - L'A');
        else                 *q = *p;
      }
      q++;
    }
    *q = L'\0';
    Str = String(wcto, q - wcto);
    delete[] wcto;
  }
  return Str;
}
//---------------------------------------------------------------------------
void TCsvGrid::TransKana(int Type)
{
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      Cells[i][j] = TransKana(Cells[i][j], Type);
    }
  }
  Modified = true;
}
//---------------------------------------------------------------------------
String TCsvGrid::TransKana(String Str, int Type)
{
  if(Str == "") return Str;
  int size = Str.Length();
  wchar_t *wcfr = Str.c_str();
  wchar_t *wcto = new wchar_t[size * 2];
  wchar_t *p=wcfr;
  wchar_t *q=wcto;
  wchar_t *r=wcfr + size;
  for(; p < r; p++){
    if(Type==4){
      q += Zenkaku2Hankaku(*p, q);
    }else if(Type==5){
      *q = Hankaku2Zenkaku(*p);
      if(*(p+1) == L'�' &&
          ((*p >= L'�' && *p <= L'�') || (*p >= L'�' && *p <= L'�'))) {
        (*q)++;
        p++;
      }else if(*(p+1) == L'�' && *p >= L'�' && *p <= L'�') {
        (*q) += 2;
        p++;
      }else if(*(p+1) == L'�' && *p == L'�') {
        (*q) = L'��';
        p++;
      }
      q++;
    }
  }
  *q = L'\0';
  Str = String(wcto, q - wcto);
  delete[] wcto;
  return Str;
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
      char LastChar = *(TopStr.AnsiLastChar());
      if(TopStr.Length() >= 2 && TopStr[1] == '0'
         && TopStr[2] >= '0' && TopStr[2] <= '9'
         && LastChar>='0' && LastChar<='9'){
        IncType = 2;
      }else if(IsNumber(TopStr)){
        IncType = 1;
        TopValue = TopStr.ToDouble();
        AnsiString Snd = Cells[x][SelTop+1];
        if(IsNumber(Snd)) Increment = Snd.ToDouble() - TopValue;
      }else if(LastChar>='0' && LastChar<='9'){
        IncType = 2;
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
//  �}��
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
void TCsvGrid::InsertRow(int Top, int Bottom)
{
  SetUndoCsv();
  UndoSetLock++;
  for(int y = Top; y <= Bottom; y++){
    Rows[RowCount++]->Clear();
    TStringGrid::MoveRow(RowCount-1, y);
  }
  UndoSetLock--;
  Modified = true;
  UpdateDataRightBottom(0,0, false);
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertColumn(int Left, int Right)
{
  SetUndoCsv();
  UndoSetLock++;
  for(int x = Left; x <= Right; x++){
    Cols[ColCount++]->Clear();
    TStringGrid::MoveColumn(ColCount-1, x);
  }
  UndoSetLock--;
  Modified = true;
  UpdateDataRightBottom(0,0, false);
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteRow(int Top, int Bottom)
{
  if(Top == Bottom){ DeleteRow(Top); return; }
  SetUndoCsv();
  UndoSetLock++;
  if(RangeSelect){ Col = FixedCols; }
  if(RowCount <= FixedRows+1){
    return;
  }else if(Row >= Top && Row <= Bottom){
    Row = Top;
  }else if(Row > Bottom){
    Row = Row - (Bottom - Top + 1);
  }
  DeleteCells_Up(0, ColCount-1, Top, Bottom, true);
  UndoSetLock--;
  Modified = true;
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteColumn(int Left, int Right)
{
  if(Left == Right){ DeleteColumn(Left); return; }
  SetUndoCsv();
  UndoSetLock++;
  if(RangeSelect){ Row = FixedRows; }
  if(ColCount <= FixedCols+1){
    return;
  }else if(Col >= Left && Col <= Right){
    Col = Left;
  }else if(Col > Right){
    Col = Col - (Right - Left + 1);
  }
  DeleteCells_Left(Left, Right, 0, RowCount-1, true);
  UndoSetLock--;
  Modified = true;
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::InsertRow(int Index)
{
  InsertRow(Index, Index);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::InsertColumn(int Index)
{
  InsertColumn(Index, Index);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DeleteRow(int ARow)
{
  SetUndoCsv();
  if(RangeSelect){ Col = FixedCols; }
  if(RowCount <= FixedRows+1){
    return;
  }else if(Row == ARow){
    if(ARow == RowCount-1){
      Rows[RowCount++]->Clear();
    }
    Row = ARow + 1;
  }
  TStringGrid::DeleteRow(ARow);
  Modified = true;
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::DeleteColumn(int ACol)
{
  SetUndoCsv();
  if(RangeSelect){ Row = FixedRows; }
  if(ColCount <= FixedCols+1){
    return;
  }else if(Col == ACol){
    if(ACol == ColCount-1){
      Cols[ColCount++]->Clear();
    }
    Col = ACol + 1;
  }
  TStringGrid::DeleteColumn(ACol);
  Modified = true;
  UpdateDataRightBottom(0,0);
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
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void TCsvGrid::InsertNewLine()
{
  if(EditorMode){
    InplaceEditor->SelText = "\r\n";

    TRect MaxRect(0,0,ColWidths[Col]-4, (ClientHeight - RowHeights[0]) / 2);
    TRect R = DrawTextRect(Canvas, MaxRect, InplaceEditor->Text, WordWrap, true);
    if(RowHeights[Row] < R.Bottom){
      RowHeights[Row] = R.Bottom;
    }
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
    if(Row > DataBottom){ // �_�~�[�Z���͏������Ȃ�
      Row = DataBottom;
      Col = DataRight;
    }else if(Col > DataRight){
      Col = DataRight;
    }else if(C > FixedCols){ //�J���}�̍폜
      Cells[C-1][Row] = Cells[C-1][Row] + Cells[C][Row];
      for(int i=C; i<ColCount-1; i++)
        Cells[i][Row] = Cells[i+1][Row];
      Cells[ColCount-1][Row] = "";
      Col--;
    }else if(Row > FixedRows && Row <= DataBottom){ //���^�[���̍폜
      int i, UpColCount, ThisColCount;
      for(i=ColCount-1; i>=DataLeft && Cells[i][Row-1]==""; i--){} UpColCount = i+1;
      for(i=ColCount-1; i>=DataLeft && Cells[i][Row]  ==""; i--){} ThisColCount = i+1;
      if(UpColCount + ThisColCount  >= ColCount)  //�񐔂�����Ȃ���Α��₷
        ChangeColCount(UpColCount + ThisColCount + 1);
      for(i=DataLeft; i<=ThisColCount; i++)
        Cells[i+UpColCount-DataLeft][Row-1] = Cells[i][Row];
      Row--; Col = (UpColCount>=FixedCols) ? UpColCount : FixedCols;
      DeleteRow(Row+1);
    }
  }
  Modified = true;
  UpdateDataRightBottom(0,0);
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
  UpdateDataRightBottom(0,0);
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
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteCell_Left()
{
  if(Col > DataRight || Row > DataBottom){
    return;
  }
  SetUndoCsv();
  if(RangeSelect){
    DeleteCells_Left(SelLeft, Selection.Right, SelTop, Selection.Bottom);
  }else{
    int i;
    for(i=Col; i < ColCount; i++){
      if(Cells[i][Row] != "") break;
    }

    if(i >= ColCount){
      if(Row < DataBottom){
        // ���̍s����A��
        Row++; Col = FixedCols; ConnectCell();
      }else{
        // ��̏k�������݂�
        DeleteCells_Left(1,1,DataBottom+1,DataBottom+1);
        FDataRight--;
      }
    }else{
      // ���݂̍s���ō��ɂ߂�
      DeleteCells_Left(Col,Col,Row,Row);
    }
  }
  Modified = true;
  UpdateDataRightBottom(0,0);
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
  UpdateDataRightBottom(0,0);
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
  UpdateDataRightBottom(0,0);
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
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteCells_Left(long Left, long Right, long Top, long Bottom,
        bool UpdateWidth)
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
  if(UpdateWidth)
  {
    int delta = Right - Left + 1;
    for(int j=Left; j<ColCount - delta; j++){
      ColWidths[j] = ColWidths[j + delta];
    }
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::DeleteCells_Up(long Left, long Right, long Top, long Bottom,
        bool UpdateHeight)
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
  if(UpdateHeight)
  {
    int delta = Bottom - Top + 1;
    for(int j=Top; j<RowCount - delta; j++){
      RowHeights[j] = RowHeights[j + delta];
    }
  }
}
//---------------------------------------------------------------------------
//  �I��
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
//  �h���b�O�C�x���g�̏���
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

  if(drgRow < FixedRows && drgCol >= FixedCols) {
    int XLeft4, XRight4, YTemp;
    MouseToCell(X-4,Y,XLeft4,YTemp);
    MouseToCell(X+4,Y,XRight4,YTemp);
    ColToResize = XLeft4;
    OldWidthHeight = ColWidths[XLeft4];
    MouseDownColBorder = ((XLeft4 != drgCol) || (XRight4 != drgCol));
    if(MouseDownColBorder){ return; }
  }else{
    MouseDownColBorder = false;
  }

  if(drgCol < FixedCols && drgRow >= FixedRows) {
    int YTop4, YBottom4, XTemp;
    MouseToCell(X,Y-4,XTemp,YTop4);
    MouseToCell(X,Y+4,XTemp,YBottom4);
    RowToResize = YTop4;
    OldWidthHeight = RowHeights[YTop4];
    MouseDownRowBorder = ((YTop4 != drgRow) || (YBottom4 != drgRow));
    if(MouseDownRowBorder){ return; }
  }else{
    MouseDownRowBorder = false;
  }

  if(Button == mbRight && (IsRowSelected(drgRow) || IsColSelected(drgCol))){
    return;
  }else if(DragMove && drgCol>=0 && drgRow>=0){
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
     ! MouseDownColBorder && ! MouseDownRowBorder &&
     ((drgRow >= 0 && drgRow < FixedRows) ||
      (drgCol >= 0 && drgCol < FixedCols)) &&
     !DragMove){

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
      AnsiString cell = Cells[ACol][ARow];
      AnsiString data = cell + "|[" + ACol + "," + ARow + "]=" + cell;
      if(Hint != data){
        Application->CancelHint();
      }
      Hint = data;
      ShowHint = true;
    }else if(FileOpenThread){
      Hint = "�t�@�C����ǂݍ��ݒ��ł��B";
      ShowHint = true;
    }else{
      Hint = "";
      ShowHint = false;
      ParentShowHint = true;
    }

    if(DblClickOpenURL == 2){
      if(ACol >= FixedCols && ARow >= FixedRows && isUrl(Cells[ACol][ARow])){
        Cursor = crHandPoint;
      }else if(FileOpenThread){
        Cursor = crAppStart;
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
  Dragging = false;
  TStringGrid::MouseUp(Button,Shift,X,Y);

  if(!RangeSelect){
    if(AlwaysShowEditor || SameCellClicking){
      Options << goEditing << goAlwaysShowEditor;
      ShowEditor();
      SameCellClicking = false;
    }else{
      Options << goEditing;
    }

    if(DblClickOpenURL == 2){
      int ACol, ARow;
      MouseToCell(X,Y,ACol,ARow);
      if(ACol >= 0 && ARow >= 0 && isUrl(Cells[ACol][ARow])){
        OpenURL(Cells[ACol][ARow]);
      }
    }
  }else if(MouseDownColBorder){
    if(Selection.Left <= ColToResize && Selection.Right >= ColToResize &&
      Selection.Top == FixedRows && Selection.Bottom == RowCount-1){
      double scale = (double) ColWidths[ColToResize] / (double) OldWidthHeight;
      for(int i=Selection.Left; i<=Selection.Right; i++){
        if(i != ColToResize){
          int newWidth = ColWidths[i] * scale;
          if(newWidth >= ClientWidth - ColWidths[0]){ //�L�����Ȃ��悤��
            newWidth = ClientWidth - ColWidths[0] - 2 * GridLineWidth;
          }
          if(newWidth < 16){ newWidth = 16; } //�������Ȃ��悤��
          ColWidths[i] = newWidth;
        }
      }
    }
  }else if(MouseDownRowBorder){
    if(Selection.Top <= RowToResize && Selection.Bottom >= RowToResize &&
       Selection.Left == FixedCols && Selection.Right == ColCount-1){
      double scale = (double) RowHeights[RowToResize] / (double) OldWidthHeight;
      for(int j=Selection.Top; j<=Selection.Bottom; j++){
        if(j != RowToResize){
          int newHeight = RowHeights[j] * scale;
          if(newHeight > (ClientHeight - RowHeights[0]) / 2){ //�L�����Ȃ��悤��
            newHeight = (ClientHeight - RowHeights[0]) / 2;
          }
          if(newHeight < 16){ newHeight = 16; }  //�������Ȃ��悤��
          RowHeights[j] = newHeight;
        }
      }
    }
  }

  UpdateDataRightBottom(0,0);

  if(DragMove && (ShowColCounter || ShowRowCounter)){
    // �s�E����ړ�����Ɣԍ�������
    ReNum();
  }
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
  if(MouseDownColBorder){
    if(Selection.Left <= ColToResize && Selection.Right >= ColToResize &&
       Selection.Top == FixedRows && Selection.Bottom == RowCount-1){
      for(int i=Selection.Left; i<=Selection.Right; i++){
        SetWidth(i);
      }
    }else{
      SetWidth(ColToResize);
    }
  }else if(MouseDownRowBorder){
    if(Selection.Top <= RowToResize && Selection.Bottom >= RowToResize &&
       Selection.Left == FixedCols && Selection.Right == ColCount-1){
      for(int j=Selection.Top; j<=Selection.Bottom; j++){
        SetHeight(j);
      }
    }else{
      SetHeight(RowToResize);
    }
  }else if(Selection.Top == FixedRows &&
     Selection.Bottom == RowCount-1 &&
     Selection.Left == Selection.Right) {
    if(ColWidths[Selection.Left] > 16) {
      ColWidths[Selection.Left] = 16;
    }else{
      SetWidth(Selection.Left);
    }
  }else if(Selection.Left == FixedCols &&
     Selection.Right == ColCount-1 &&
     Selection.Top == Selection.Bottom) {
      SetHeight(Selection.Top);
  }else if(EditorMode == false){
    Options << goEditing << goAlwaysShowEditor;
    ShowEditor();
    InplaceEditor->SelLength = 0;
    InplaceEditor->SelStart = InplaceEditor->Text.Length();
  }

  if(DblClickOpenURL && isUrl(Cells[Col][Row])){
    OpenURL(Cells[Col][Row]);
  }
}
//---------------------------------------------------------------------------
void TCsvGrid::OpenURL(String FileName)
{
  if(BrowserFileName == ""){
    AutoOpen(FileName);
  }else{
	String Arg0 = (String)("\"") + BrowserFileName + "\"";
	String Arg1 = (String)("\"") + FileName + "\"";
	_wspawnl(P_NOWAITO , BrowserFileName.c_str() , Arg0.c_str() , Arg1.c_str() , NULL);
  }
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::MouseWheelUp(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled)
{
  if(WheelMoveCursol == 1){
    int r = Row - WheelScrollStep;
    if(r < FixedRows){ r = FixedRows; }
    Row = r;
  }else if(WheelMoveCursol == 2){
    int tr = TopRow - WheelScrollStep;
    if(tr < FixedRows){ tr = FixedRows; }
    int r = Row + (tr - TopRow);
    TopRow = tr;
    Row = r;
  }else{
    int tr = TopRow - WheelScrollStep;
    if(tr < FixedRows){ tr = FixedRows; }
    TopRow = tr;
  }
  Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::MouseWheelDown(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled)
{
  if(WheelMoveCursol == 1){
    int r = Row + WheelScrollStep;
    if(r > RowCount - 1){ r = RowCount - 1; }
    Row = r;
  }else if(WheelMoveCursol == 2){
    int tr = TopRow + WheelScrollStep;
    if(tr > RowCount - VisibleRowCount){ tr = RowCount - VisibleRowCount; }
    int r = Row + (tr - TopRow);
    TopRow = tr;
    Row = r;
  }else{
    int tr = TopRow + WheelScrollStep;
    if(tr > RowCount - VisibleRowCount){ tr = RowCount - VisibleRowCount; }
    TopRow = tr;
  }
  Handled = true;
}
//---------------------------------------------------------------------------
//  ����
//
#define drThisRow 0
#define drThisCol 1
#define drAll 2
//---------------------------------------------------------------------------
int FindHit(String CellText, String FindText,
        bool Case, bool Regex, bool Back, int *Length)
{
  if(!Case){
    CellText = CellText.LowerCase();
    FindText = FindText.LowerCase();
  }

  if(Regex){
    for(int i=FindText.Length() - 1; i>0; i--){
      if(FindText[i] == '\\' && FindText[i+1] == 'n'){
        FindText.Insert("\\r?", i);
      }
    }

    AnsiString AnsiCellText = (AnsiString) CellText;
    AnsiString AnsiFindText = (AnsiString) FindText;

    TRegexp regex(AnsiFindText.c_str());
    int hit = 0;
    size_t len = 0;
    if(Back){
      while(true){
        size_t lentest;
        int hittest = regex.find(AnsiCellText.c_str(), &lentest, hit) + 1;
        if(hittest > 0){
          hit = hittest;
          len = lentest;
        }else{
          break;
        }
      }
    }else{
      hit = regex.find(AnsiCellText.c_str(), &len, 0) + 1;
    }
    if(hit > 0){
      *Length = ((String)(AnsiCellText.SubString(hit, len))).Length();
      return ((String)(AnsiCellText.SubString(1, hit))).Length();;
    }else{
      return 0;
    }
  }else{
    *Length = FindText.Length();
    if(Back){
      for(int i=CellText.Length() - FindText.Length() + 1; i>0; i--){
        if(CellText.SubString(i, FindText.Length()) == FindText){
          return i;
        }
      }
      return 0;
    }else{
      return (CellText.Pos(FindText));
    }
  }
}
//---------------------------------------------------------------------------
bool TCsvGrid::Find(String FindText,
        int Range, bool Case, bool Regex, bool Word, bool Back)
{
  int Hit, Len;

  if(FindText == "") return(false);
  Options << goEditing << goAlwaysShowEditor;
  ShowEditor();
  TInplaceEdit *ipEd = InplaceEditor;
  if(!ipEd){ return(false); }

  AnsiString InCellText = "";

  if(!Word){
    if(Back){
      InCellText = ipEd->Text.SubString(1, ipEd->SelStart);
    }else{
      InCellText = ipEd->Text.SubString(ipEd->SelStart + ipEd->SelLength + 1, ipEd->Text.Length());
    }
    if(Regex){
      if(!Back && FindText[1] == '^'){ InCellText = ""; }
      else if(Back && *(FindText.LastChar()) == '$'){ InCellText = ""; }
    }
  }

  if(InCellText != ""){
    Hit = FindHit(InCellText, FindText, Case, Regex, Back, &Len);
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

  if(Regex && Word){
    if(FindText[1] != '^'){ FindText.Insert("^", 1); }
    if(*(FindText.LastChar()) != '$'){ FindText += "$"; }
  }

  int step = (Back ? -1 : 1);
  int YStart = Row;
  int YEnd = (Back ? DataTop : RowCount);
  if(Range == drThisRow){
    YEnd = Row;
  }else if(Range == drThisCol){
    if(Word && !Back && (ipEd->SelStart + ipEd->SelLength == 0)){
      YStart = Row;
    }else{
      YStart = Row+step;
    }
  }

  for(int y=YStart; y*step<=YEnd*step; y+=step)
  {
    int XStart = (Back ? ColCount : DataLeft);
    int XEnd   = (Back ? DataLeft : ColCount);
    if(Range == drThisCol){
      XStart = Col;  XEnd = Col;
    }else if(y == Row){
      if(Word && !Back && (ipEd->SelStart + ipEd->SelLength == 0)){
        XStart = Col;
      }else{
        XStart = Col+step;
      }
    }

    for(int x=XStart; x*step<=XEnd*step; x+=step)
    {
      if(Word && !Regex){
        Hit = 0;
        if(Case){
          if(Cells[x][y] == FindText){
            Hit=1; Len=FindText.Length();
          }
        }else{
          if(Cells[x][y].LowerCase() == FindText.LowerCase()){
            Hit=1; Len=FindText.Length();
          }
        }
      }else{
        Hit = FindHit(Cells[x][y], FindText, Case, Regex, Back, &Len);
        if(Word){
          // �u$�v�����s�ɂ��q�b�g����̂�h���B
          if(Len < Cells[x][y].Length()){ Hit = 0; }
        }
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
bool TCsvGrid::Replace(String FindText , String ReplaceText,
        int Range, bool Case, bool Regex, bool Word, bool Back)
{
  AnsiString FindTextAll = FindText;
  if(Regex){
    if(FindTextAll[1] != '^'){ FindTextAll.Insert("^", 1); }
    if(*(FindTextAll.AnsiLastChar()) != '$'){ FindTextAll += "$"; }

    for(int i=1; i<ReplaceText.Length(); i++){
      if(ReplaceText[i] == '\\' && ReplaceText[i+1] == 'n'){
        ReplaceText[i] = '\r';
        ReplaceText[i+1] = '\n';
      }else if(ReplaceText[i] == '\\' && ReplaceText[i+1] == 't'){
        ReplaceText[i] = '\t';
        ReplaceText.Delete(i+1, 1);
      }else if(ReplaceText[i] == '\\' && ReplaceText[i+1] == '\\'){
        ReplaceText[i] = '\\';
        ReplaceText.Delete(i+1, 1);
      }
    }
  }

  TInplaceEdit *ipEd = InplaceEditor;
  if((Regex || ReplaceText != FindText) && ipEd){
    ipEd->SetFocus();
    if(Word && !Regex){
      if((Case && (ipEd->Text == FindTextAll)) ||
         (!Case && (ipEd->Text.LowerCase() == FindTextAll.LowerCase()))){
        ipEd->Text = ReplaceText;
        Modified = true;
      }
    }else{
      if(Word){ ipEd->SelectAll(); }
      int Hit,Len;
      Hit = FindHit(ipEd->SelText, FindTextAll, Case, Regex, Back, &Len);
      if(Hit == 1 && Len == ipEd->SelText.Length()) {
        ipEd->SelText = ReplaceText;
      }
    }
  }

  return(Find(FindText,Range,Case,Regex,Word,Back));
}
//---------------------------------------------------------------------------
void TCsvGrid::AllReplace(String FindText , String ReplaceText,
        int Range, bool Case, bool Regex, bool Word, bool Back)
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
  while(Replace(FindText,ReplaceText, Range, Case, Regex, Word, Back)){}
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
//  �L�[���͂̊Ď�
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


  UpdateDataRightBottom(Col, Row);
  TStringGrid::KeyDown(Key,Shift);
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::KeyDownSub(System::TObject* Sender,
                          Word &Key, Classes::TShiftState Shift)
{
  /*
   * InplaceEditor���ł̃L�[���͂ȂǂŁA
   * �ꍇ�ɂ���Ă͒ʏ��KeyDown()�����s����Ȃ����߁A
   * OnKeyDown �ɂ��̊֐���������ŏ������܂�^^;
   */

  if(Key == VK_DELETE && EditorMode)
  {
    Modified = true;
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
      case cssv_EnterNextRow:
        if(Row == RowCount-1){ ChangeRowCount(RowCount+1); ReNum(); }
        Row++;
        Col = FixedCols;
        break;
      case cssv_EnterTabbedNextRow:
        if(Row == RowCount-1){ ChangeRowCount(RowCount+1); ReNum(); }
        if(TabStartRow == Row){
          Col = TabStartCol;
        }
        Row++;
        break;
      default:
        break;
    }
  }

  if(Key == VK_LEFT && !Shift.Contains(ssShift)){
    if(InplaceEditor->SelLength == InplaceEditor->Text.Length() &&
       Col > FixedCols && !LeftArrowInCell){
      Col--; Key = '\0';
      TabStartCol = -1;
      TabStartRow = -1;
    }
  }else if(Key == VK_RIGHT && !Shift.Contains(ssShift)){
    if(InplaceEditor->SelLength == InplaceEditor->Text.Length() ||
       (InplaceEditor->SelStart == InplaceEditor->Text.Length() &&
        InplaceEditor->SelLength == 0)){
      if(Col == ColCount-1){ ChangeColCount(ColCount+1); ReNum(); }
      Col++; Key = '\0';
      TabStartCol = -1;
      TabStartRow = -1;
    }
  }else if(Key == VK_DOWN &&
           !Shift.Contains(ssShift) && !Shift.Contains(ssCtrl)){
    if(Row == RowCount-1){ ChangeRowCount(RowCount+1); ReNum(); }
    Row++; Key = '\0';
    TabStartCol = -1;
    TabStartRow = -1;
  }else if(Key == VK_TAB && !Shift.Contains(ssShift)){
    if(Row != TabStartRow){
      TabStartCol = Col;
      TabStartRow = Row;
    }
  }
}
//---------------------------------------------------------------------------
class TCsvInplaceEdit : public TInplaceEdit
{
public:
  TCsvInplaceEdit(TCsvGrid *G) : TInplaceEdit(G) {}

  void __fastcall BoundsChanged() {
    TInplaceEdit::BoundsChanged();
    TCsvGrid *G = static_cast<TCsvGrid*>(Owner);
    RECT R;
    R.left = G->LRMargin;
    R.top = G->TBMargin;
    R.right = Width - 2;
    R.bottom = Height;
    SendMessage(Handle, EM_SETRECT, 0, (long)(&R));
  }
};
//---------------------------------------------------------------------------
TInplaceEdit* __fastcall TCsvGrid::CreateEditor()
{
  TInplaceEdit *editor = new TCsvInplaceEdit(this);
  editor->Parent = this;
  editor->Brush->Color = Color;
  return editor;
}
//---------------------------------------------------------------------------
void __fastcall TCsvGrid::SetEditText(int ACol, int ARow, String Value)
{
  String old = Cells[ACol][ARow];
  if(Value != old){
    Modified = true;
    SetUndoMacro();
  }
  UpdateDataRightBottom(Col, Row);
  TStringGrid::SetEditText(ACol, ARow, Value);
}
//---------------------------------------------------------------------------
//  Undo ����
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

  UndoCsvWidth = DataRight - DataLeft + 1;
  UndoCsvHeight = DataBottom - DataTop + 1;
  UndoCsv = new String[UndoCsvWidth * UndoCsvHeight];
  String *p = UndoCsv;
  for(int y=DataTop; y<=DataBottom; y++){
    for(int x=DataLeft; x<=DataRight; x++){
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
    Modified = true;
    UndoMacro->Delete(0);
    if(UndoMacro->Count == 0){ delete UndoMacro; UndoMacro = NULL; }
  }
  else if(UndoCsv)
  {
    SetRedoCsv();
    // EOF�}�[�J�[�̍폜
    Objects[DataLeft+DataRight][DataTop+DataBottom] = NULL;
    for(int i=1; i <= ColCount; i++)
      Cols[i]->Clear();

    int RC = DataTop+UndoCsvHeight;
    int CC = DataLeft+UndoCsvWidth;
    ChangeRowCount(RC+1);
    ChangeColCount(CC+1);
    String *p = UndoCsv;
    for(int y=DataTop; y<RC; y++){
      for(int x=DataLeft; x<CC; x++){
        Cells[x][y] = *p;
        p++;
      }
    }
    if(TypeOption->DummyEof) {
      Objects[CC][RC] = EOFMarker;
    }
    Modified = true;
    delete[] UndoCsv; UndoCsv = NULL;
    UpdateDataRightBottom(0,0);
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
  if(!RedoCsv){ delete[] RedoCsv; RedoCsv = NULL; }
  RedoCsvWidth = DataRight - DataLeft + 1;
  RedoCsvHeight = DataBottom - DataTop + 1;
  RedoCsv = new String[RedoCsvWidth * RedoCsvHeight];
  String *p = RedoCsv;
  for(int y=DataTop; y<=DataBottom; y++){
    for(int x=DataLeft; x<=DataRight; x++){
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
    // EOF�}�[�J�[�̍폜
    Objects[DataLeft+DataRight][DataTop+DataBottom] = NULL;
    for(int i=1; i <= ColCount; i++)
      Cols[i]->Clear();

    int RC = DataTop+RedoCsvHeight;
    int CC = DataLeft+RedoCsvWidth;
    ChangeRowCount(RC+1);
    ChangeColCount(CC+1);
    String *p = RedoCsv;
    for(int y=DataTop; y<RC; y++){
      for(int x=DataLeft; x<CC; x++){
        Cells[x][y] = *p;
        p++;
      }
    }
    if(TypeOption->DummyEof) {
      Objects[CC][RC] = EOFMarker;
    }
    Modified = true;
    delete[] RedoCsv; RedoCsv = NULL;
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
    Modified = true;
    RedoMacro->Delete(0);
    if(RedoMacro->Count == 0){ delete RedoMacro; RedoMacro = NULL; }
  }
  UpdateDataRightBottom(0,0);
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


