//---------------------------------------------------------------------------
#include <vcl.h>
#include <Vcl.clipbrd.hpp>
#include <shellapi.h>
#include <string.h>
#include <process.h>
#include <boost\regex.hpp>
#pragma hdrstop

#include "AutoOpen.h"
#include "Compiler.h"
#include "EncodedWriter.h"
#include "EncodingDetector.h"
#include "FileOpenThread.h"
#include "Find.h"
#include "Macro.h"
#include "MainForm.h"
#include "MainGrid.h"
#include "PasteDlg.h"

#define DataLeft (ShowRowCounter ? 1 : 0)
#define DataTop  (ShowColCounter ? 1 : 0)
#define soNone 0
#define soNormal 1
#define soString 2
#define soAll 3

#define RXtoAX(x) ((x) - DataLeft + 1)
#define RYtoAY(y) ((y) - DataTop  + 1)
#define AXtoRX(x) ((x) + DataLeft - 1)
#define AYtoRY(y) ((y) + DataTop  - 1)
//---------------------------------------------------------------------------
static inline int min(int a, int b)
{
  return (a < b ? a : b);
}
//---------------------------------------------------------------------------
static inline int max(int a, int b)
{
  return (a > b ? a : b);
}
//---------------------------------------------------------------------------
static inline String boolToStr(bool value)
{
  return (value ? "true" : "false");
}
//---------------------------------------------------------------------------
__fastcall TMainGrid::TMainGrid(TComponent* Owner)  //デフォルトの設定
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
  DragBehavior = dbMoveIfSelected;
  PasteOption = -1;
  DefWay = 2;
  FShowRowCounter = true;
  FShowColCounter = true;
  ReturnCode = CRLF;
  InCellReturnCode = CRLF;
  TextAlignment = cssv_taLeft;
  WheelMoveCursol = 0;
  SameCellClicking = false;
  ExecCellMacro = false;
  FDataRight = 1;
  FDataBottom = 1;
  EOFMarker = new TObject();
  EOLMarker = new TObject();
  FileOpenThread = nullptr;
  DefaultDrawing = false;
  LastMatch = new TStringList();
  FUndoList = new TUndoList();
}
//---------------------------------------------------------------------------
__fastcall TMainGrid::~TMainGrid(){
  UndoList->Clear();
  if (EOFMarker) { delete EOFMarker; }
  if (EOLMarker) { delete EOLMarker; }
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::ShowEditor()
{
  TStringGrid::ShowEditor();
  InplaceEditor->Brush->Color = Color;
}
//---------------------------------------------------------------------------
void TMainGrid::SetTBMargin(int Value)
{
  FTBMargin = Value;
  UpdateDefaultRowHeight();
}
//---------------------------------------------------------------------------
void TMainGrid::UpdateDefaultRowHeight()
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
TRect TMainGrid::DrawTextRect(TCanvas *Canvas, TRect Rect,
  String Str, bool Wrap, bool MeasureOnly)
{
  int textHeight = Canvas->TextHeight(Str);
  int yint = textHeight + CellLineMargin;
  int wrapWidth = Rect.Right - Rect.Left;

  int maxWidth = 0;
  int y = Rect.Top;
  int drawIndex = 1;
  TRect R = Rect;
  for (int i = 1; i <= Str.Length(); i++) {
    if (y >= Rect.Bottom) {
      return TRect(0, 0, maxWidth, Rect.Bottom - Rect.Top);
    }
    if (Str[i] == L'\n') {
      if (!MeasureOnly && i > drawIndex) {
        R.Top = y;
        Canvas->TextRect(R, R.Left, y, Str.SubString(drawIndex, i - drawIndex));
      }
      drawIndex = i + 1;
      y += yint;
      continue;
    }
    if (i == drawIndex || Str.IsLeadSurrogate(i)
        || !(Wrap || i == Str.Length() || Str[i + 1] == L'\n')) {
      continue;
    }
    int drawLength = i - drawIndex + 1;
    int drawWidth = Canvas->TextWidth(Str.SubString(drawIndex, drawLength));
    if (Wrap && drawWidth > wrapWidth) {
      // Wrap at the previous character.
      drawLength -= (Str.IsLeadSurrogate(i - 1) ? 2 : 1);
      if (drawLength == 0) {
        continue;
      }
      if (!MeasureOnly) {
        R.Top = y;
        Canvas->TextRect(R, R.Left, y, Str.SubString(drawIndex, drawLength));
      }
      drawIndex += drawLength;
      y += yint;
      continue;
    }
    if (drawWidth > maxWidth) {
      maxWidth = drawWidth;
    }
  }
  if (!MeasureOnly) {
    R.Top = y;
    Canvas->TextRect(R, R.Left, y, Str.SubString(drawIndex, Str.Length()));
  }
  y += textHeight + (2 * TBMargin);
  return TRect(0, 0, maxWidth, min(y, Rect.Bottom) - Rect.Top);
}
//---------------------------------------------------------------------------
static bool FindHit(String CellText, int x, int y);
//---------------------------------------------------------------------------
void __fastcall TMainGrid::DrawCell(int ACol, int ARow,
  const TRect &ARect, TGridDrawState AState)
{
  bool isSelected = GetSelected(ACol, ARow);
  TFormattedCell cell = GetCellToDraw(ACol, ARow);
  TFontStyles styles;

  if (isSelected) {
    cell.bgColor = clHighlight;
  } else if (FoundBgColor != Color && FindHit(Cells[ACol][ARow], ACol, ARow)) {
    cell.bgColor = FoundBgColor;
  }

  if (TypeOption->DummyEof && ACol == DataRight + 1 && ARow == DataBottom + 1) {
    cell.fgColor = clGray;
    cell.text = "[EOF]";
  } else if (TypeOption->DummyEol && ARow <= DataBottom && ARow >= FixedRows &&
             ACol == GetRowDataRight(ARow) + 1) {
    cell.fgColor = clGray;
    cell.text = L"\u21b5";
  } else if (ShowURLBlue && isUrl(Cells[ACol][ARow])) {
    styles << fsUnderline;
    cell.fgColor = UrlColor;
  } else if (isSelected) {
    cell.fgColor = clHighlightText;
  }

  Canvas->Brush->Color = cell.bgColor;
  Canvas->FillRect(ARect);

  TRect textRect;
  textRect.Left = ARect.Left + LRMargin;
  textRect.Top = ARect.Top + FTBMargin;
  textRect.Right = ARect.Right - LRMargin;
  textRect.Bottom = ARect.Bottom - FTBMargin;
  if (cell.alignment == taRightJustify) {
    int left = textRect.Right - Canvas->TextWidth(cell.text);
    if (textRect.Left < left) {
      textRect.Left = left;
    }
  } else if (cell.alignment == taCenter) {
    int left =
        (textRect.Left + textRect.Right - Canvas->TextWidth(cell.text)) / 2;
    if (textRect.Left < left) {
      textRect.Left = left;
    }
  }

  Canvas->Font->Color = cell.fgColor;
  Canvas->Font->Style = styles;
  DrawTextRect(Canvas, textRect, cell.text, WordWrap);

  TColor PenColor = Canvas->Pen->Color;
  if(ACol < FixedCols || ARow < FixedRows){
    Canvas->Pen->Color = clBlack;
  }
  Canvas->MoveTo(ARect.Right, ARect.Top);
  Canvas->LineTo(ARect.Right, ARect.Bottom);
  Canvas->LineTo(ARect.Left - 1, ARect.Bottom);
  Canvas->Pen->Color = PenColor;
}
//---------------------------------------------------------------------------
//  基本入出力メソッド
//
void TMainGrid::SetExecCellMacro(bool Value)
{
  FExecCellMacro = Value;
  ClearCalcCache();
}
//---------------------------------------------------------------------------
void TMainGrid::ClearCalcCache()
{
  CalculatedCellCache.clear();
  FormattedCellCache.clear();
  UsingCellMacro.clear();
}
//---------------------------------------------------------------------------
void TMainGrid::ErrorCalcLoop()
{
  for (const auto& [key, value] : UsingCellMacro) {
    CalculatedCellCache[key] = TCalculatedCell(value, ctError);
  }
}
//---------------------------------------------------------------------------
TCalculatedCell TMainGrid::GetCalculatedCell(int AX, int AY)
{
  int rx = AXtoRX(AX);
  int ry = AYtoRY(AY);
  if (AX <= 0 || AY <= 0 ||
      rx < 0 || rx >= ColCount || ry < 0 || ry >= RowCount) {
    return TCalculatedCell("", ctNotExpr);
  }

  // 計算式でないものはなにもしない
  String Str = Cells[rx][ry];
  if(!ExecCellMacro || Str.Length() == 0 || Str[1] != '='){
    return TCalculatedCell(Str, ctNotExpr);
  }

  String key = (String)"[" + AX + "," + AY + "]";
  // 自己参照をはじく
  if (UsingCellMacro.count(key) > 0){
    ErrorCalcLoop();
    return TCalculatedCell(Str, ctError);
  }
  // キャッシュがあれば返す
  if (CalculatedCellCache.count(key) > 0){
    const TCalculatedCell& cache = CalculatedCellCache[key];
    if (cache.calcType == ctError) {
      ErrorCalcLoop();
    }
    return cache;
  }

  if(OnGetCalculatedCell){
    // 自己参照チェックのためのリストを更新
    UsingCellMacro[key] = Str;

    // OnGetCalculatedCellに委譲
    TCalculatedCell result = OnGetCalculatedCell(Str, AX, AY);

    // キャッシュを更新
    const TCalculatedCell& cache = CalculatedCellCache[key];
    if(CalculatedCellCache[key].calcType == ctError){
      // 自己参照エラーが起こっていれば更新しない
      result = cache;
    }else{
      CalculatedCellCache[key] = result;
    }

    // 自己参照チェックのためのリストを元に戻す
    UsingCellMacro.erase(key);

    // 成功
    return result;
  }else{
    return TCalculatedCell(Str, ctNotExpr);
  }
}
//---------------------------------------------------------------------------
TFormattedCell TMainGrid::GetFormattedCell(TCalculatedCell Cell, int AX, int AY)
{
  int rx = AXtoRX(AX);
  int ry = AYtoRY(AY);
  if (rx < 0 || rx > DataRight || ry < 0 || ry > DataBottom) {
    return GetStyledCell(Cell, AX, AY);
  }
  String key = (String)AX + "," + AY;

  if (FormattedCellCache.count(key) > 0) {
    return FormattedCellCache[key];
  }
  if (!OnGetFormattedCell) {
    return GetStyledCell(Cell, AX, AY);
  }
  TFormattedCell result = OnGetFormattedCell(Cell, AX, AY);
  FormattedCellCache[key] = result;
  return result;
}
//---------------------------------------------------------------------------
TFormattedCell TMainGrid::GetStyledCell(TCalculatedCell Cell, int AX, int AY)
{
  int rx = AXtoRX(AX);
  int ry = AYtoRY(AY);
  String str = Cell.text;

  bool isNum = IsNumber(str);
  if (isNum && DecimalDigits >= 0) {
    str = FormatOmitDecimal(str, DecimalDigits);
  }
  if (isNum && NumberComma > 0) {
    str = FormatNumComma(str, NumberComma);
  }
  TAlignment alignment =
      (isNum && TextAlignment == cssv_taNumRight) ? taRightJustify
                                                  : taLeftJustify;
  TColor fgColor;
  TColor bgColor;
  if (Cell.calcType == ctOk) {
    fgColor = CalcFgColor;
    bgColor = CalcBgColor;
  } else if (Cell.calcType == ctError) {
    fgColor = CalcErrorFgColor;
    bgColor = CalcErrorBgColor;
  } else if (rx < FixedCols || ry < FixedRows) {
    fgColor = FixFgColor;
    bgColor = FixedColor;
  } else {
    fgColor = Font->Color;
    if (ry == Row && CurrentRowBgColor != Color) {
      bgColor = CurrentRowBgColor;
    } else if (rx == Col && CurrentColBgColor != Color) {
      bgColor = CurrentColBgColor;
    } else if (rx > DataRight || ry > DataBottom ||
               rx > GetRowDataRight(ry)) {
      bgColor = DummyBgColor;
    } else if (AY % 2 == 0) {
      bgColor = EvenRowBgColor;
    } else {
      bgColor = Color;
    }
  }

  return TFormattedCell(str, fgColor, bgColor, alignment);
}
//---------------------------------------------------------------------------
TFormattedCell TMainGrid::GetCellToDraw(int RX, int RY)
{
  int ax = RXtoAX(RX);
  int ay = RYtoAY(RY);

  TCalculatedCell calculatedCell = GetCalculatedCell(ax, ay);
  if (ax == 0 && ay > 0 && RY <= DataBottom) {
    calculatedCell.text = ay;
  } else if (ay == 0 && ax > 0 && RX <= DataRight) {
    calculatedCell.text = ax;
  }

  return GetFormattedCell(calculatedCell, ax, ay);
}
//---------------------------------------------------------------------------
String TMainGrid::GetACells(int ACol, int ARow)
{
  String Str = GetCalculatedCell(ACol, ARow).text;
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
void TMainGrid::SetACells(int ACol, int ARow, String Val){
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
  SetCell(rx, ry, Val);
  UpdateDataRightBottom(rx, ry);
}
//---------------------------------------------------------------------------
void TMainGrid::SetCell(int X, int Y, String Value)
{
  UndoList->ChangeCell(RXtoAX(X), RYtoAY(Y), Cells[X][Y], Value,
                       RXtoAX(DataRight), RYtoAY(DataBottom));
  Cells[X][Y] = Value;
}
//---------------------------------------------------------------------------
void TMainGrid::SetModified(bool Value)
{
  bool Before = FModified;
  FModified = Value;
  ClearCalcCache();
  if(FModified != Before && OnChangeModified) OnChangeModified(this);
}
//---------------------------------------------------------------------------
void TMainGrid::Clear(int AColCount, int ARowCount, bool UpdateRightBottom)
{
  if(FileOpenThread){
    FileOpenThread->Terminate();
    FileOpenThread = nullptr;
  }
  Row = FixedRows;  Col = FixedCols;
  DefaultColWidth = 64;               //列の幅を64に戻す
  ColWidths[0] = 32;
  if(FixedCols >= AColCount){ AColCount = FixedCols + 1; }
  if(FixedRows >= ARowCount){ ARowCount = FixedRows + 1; }
  ChangeColCount(AColCount + 1);
  ChangeRowCount(ARowCount + 1);
  for (int i = DataTop; i <= RowCount; i++) {
    Rows[i]->Clear();
  }
  UndoList->Clear();
  Modified = false;
  TypeOption = fmMain->TypeList.DefItem();
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
  Invalidate();
}
//---------------------------------------------------------------------------
void TMainGrid::SetShowRowCounter(bool Value)
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
      InsertColumn(0);
      SetWidth(0);
    } else {
      if(Col <= FixedCols) Col = FixedCols + 1;
      if(ColCount <= FixedCols+1) FShowRowCounter = true;
      else  DeleteColumn(0);
    }
    Modified = M;
    if(FShowRowCounter || ShowColCounter) Cells[0][0] = "";
    Invalidate();
  }
}
//---------------------------------------------------------------------------
void TMainGrid::SetShowColCounter(bool Value)
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
    Invalidate();
  }
}
//---------------------------------------------------------------------------
int TMainGrid::TextWidth(TCanvas *cnvs, String str)
{
  if((NumberComma > 0) && IsNumber(str)){
    str = FormatNumComma(str, NumberComma);
  }
  TRect MaxRect(0, 0, INT_MAX, INT_MAX);
  TRect R = DrawTextRect(cnvs, MaxRect, str, false, true);

  return R.Right;
}

//---------------------------------------------------------------------------
void TMainGrid::SetWidth(int i)
{
  int WMax = 0;                     //その列で幅の最大値
  if(i==0 && ShowRowCounter){
    if(DataBottom >= 0){
      ColWidths[i] = max(MinColWidth,
        TextWidth(Canvas, (String) DataBottom) + (2 * LRMargin));
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
    WMax = max(WMax, TextWidth(Canvas, GetCellToDraw(i, j).text));
  }
  if(WMax > 0){ WMax += 2 * LRMargin; }
  if(WMax < MinColWidth){ //狭すぎないように
    WMax = MinColWidth;
  }else if( WMax >= ClientWidth - ColWidths[0]){ //広すぎないように
    WMax = ClientWidth - ColWidths[0] - 2 * GridLineWidth;
  }
  ColWidths[i] = WMax;
}
//---------------------------------------------------------------------------
void TMainGrid::SetWidth()             //列幅の最適化
{
  if (CompactColWidth) {
    ShowAllColumn();
    return;
  }

  for(int i=0; i < ColCount; i++) {
    SetWidth(i);
  }
}
//---------------------------------------------------------------------------
void TMainGrid::SetHeight(int j, bool useMaxRowHeightLines)
{
  if(j==0 && ShowColCounter){
    RowHeights[j] = DefaultRowHeight;
    return;
  }

  int maxRowHeight = ClientHeight;
  for (int y = 0; y < FixedRows; y++) {
    maxRowHeight -= (RowHeights[y] + 1);
  }
  if (useMaxRowHeightLines) {
    maxRowHeight = min(maxRowHeight,
        (Canvas->TextHeight(L"あ") * MaxRowHeightLines)
            + (CellLineMargin * (MaxRowHeightLines - 1))
            + (2 * TBMargin));
  }

  int HMax = DefaultRowHeight;
  int right = min(LeftCol + VisibleColCount, ColCount);
  for (int i = LeftCol; i <= right; i++) {
    TRect MaxRect(0, 0, ColWidths[i] - (2 * LRMargin), maxRowHeight);
    String str = GetCellToDraw(i, j).text;
    TRect R = DrawTextRect(Canvas, MaxRect, str, WordWrap, true);
    HMax = max(HMax, R.Bottom);
  }
  RowHeights[j] = HMax;
}
//---------------------------------------------------------------------------
void TMainGrid::SetHeight()
{
  for(int i=0; i < FixedRows; i++)
  {
    SetHeight(i, true);
  }

  int RC = min(TopRow + VisibleRowCount, RowCount);
  for(int i=TopRow; i < RC; i++)
  {
    SetHeight(i, true);
  }
}
//---------------------------------------------------------------------------
void TMainGrid::CompactWidth(int *Widths, int WindowSize, int Minimum)
{
  std::vector<int> sortedWidths = {0};
  int sum = 0;
  for (int i = DataLeft; i <= DataRight; i++) {
    sortedWidths.push_back(Widths[i]);
    sum += Widths[i];
  }
  if (sum <= WindowSize) {
    return;
  }

  std::sort(sortedWidths.begin(), sortedWidths.end());
  int i = sortedWidths.size();
  while (i > 1 && sum > WindowSize) {
    i--;
    sum -= (sortedWidths[i] - sortedWidths[i - 1]) * (sortedWidths.size() - i);
  }
  int bigColSize = max(Minimum,
      sortedWidths[i - 1] + (WindowSize - sum) / (sortedWidths.size() - i));
  for (int x = DataLeft; x <= DataRight; x++) {
    if (Widths[x] >= sortedWidths[i]) {
      Widths[x] = bigColSize;
    }
  }
}
//---------------------------------------------------------------------------
void TMainGrid::ShowAllColumn()
{
  int *widths = new int[ColCount];
  int windowSize =
      ClientWidth - 16 - (DataRight - DataLeft + 1) * GridLineWidth;

  if (ShowRowCounter) {
    widths[0] = max(MinColWidth,
        TextWidth(Canvas, GetCellToDraw(0, DataBottom).text) + (2 * LRMargin));
    windowSize -= widths[0];
  }

  int top;
  int bottom;
  if (CalcWidthForAllRow) {
    top = DataTop;
    bottom = DataBottom;
  } else {
    top = TopRow;
    bottom = min(TopRow + VisibleRowCount, RowCount);
  }

  for (int i = DataLeft; i <= DataRight; i++) {
    int maxWidth = 0;
    for (int j = top; j <= bottom; j++) {
      maxWidth = max(maxWidth, TextWidth(Canvas, GetCellToDraw(i, j).text));
    }
    widths[i] = max(MinColWidth, maxWidth + (2 * LRMargin));
  }
  for (int i = DataRight + 1; i < ColCount; i++) {
    widths[i] = MinColWidth;
  }

  CompactWidth(widths, windowSize, MinColWidth);
  for (int i = 0; i < ColCount; i++) {
    ColWidths[i] = widths[i];
  }
  delete[] widths;
}
//---------------------------------------------------------------------------
void TMainGrid::Cut()                  //右、下の余計な項目を削除
{
  if(Col > DataRight){ Col = DataRight; }
  if(Row > DataBottom){ Row = DataBottom; }
  ChangeColCount(DataRight + 2);
  ChangeRowCount(DataBottom + 2);
}
//---------------------------------------------------------------------------
void TMainGrid::UpdateDataRight()
{
  for (int x = ColCount - 1; x >= DataLeft; x--) {
    for (int y = RowCount - 1; y >= DataTop; y--) {
      if (HasData(x, y) ||
          (x < ColCount - 1 &&
              TypeOption->DummyEof && Objects[x + 1][y] == EOFMarker)) {
        FDataRight = x;
        return;
      }
    }
  }
  FDataRight = DataLeft;
}
//---------------------------------------------------------------------------
void TMainGrid::UpdateDataBottom()
{
  for (int y = RowCount - 1; y >= DataTop; y--) {
    for (int x = ColCount - 1; x >= DataLeft; x--) {
      if (HasData(x, y) ||
          (y < RowCount - 1 &&
              TypeOption->DummyEof && Objects[x][y + 1] == EOFMarker)) {
        FDataBottom = y;
        return;
      }
    }
  }
  FDataBottom = DataTop;
}
//---------------------------------------------------------------------------
void TMainGrid::UpdateDataRightBottom(int modx, int mody)
{
  int oldRight = DataRight;
  int oldBottom = DataBottom;

  if(modx <= 0 && mody <= 0){
    UpdateDataRight();
    UpdateDataBottom();
    int cc = FDataRight + 2;
    if(cc <= modx){ cc = modx + 1; }
    if(cc <= Col){ cc = Col + 1; }
    ChangeColCount(cc);
    int rc = FDataBottom+2;
    if(rc <= mody){ rc = mody + 1; }
    if(rc <= Row){ rc = Row + 1; }
    ChangeRowCount(rc);
  }else if(Cells[modx][mody] == ""){
    if(modx == oldRight){ UpdateDataRight(); }
    if(mody == oldBottom){ UpdateDataBottom(); }
  }else{
    if(modx > oldRight){
      UpdateDataRight();
      int cc = FDataRight + 2;
      if(cc <= modx){ cc = modx + 1; }
      if(cc <= Col){ cc = Col + 1; }
      ChangeColCount(cc);
    }
    if(mody > oldBottom){
      UpdateDataBottom();
      int rc = FDataBottom+2;
      if(rc <= mody){ rc = mody + 1; }
      if(rc <= Row){ rc = Row + 1; }
      ChangeRowCount(rc);
    }
  }

  if(FDataRight != oldRight || FDataBottom != oldBottom){
    if(TypeOption->DummyEof) {
      UpdateEOFMarker(oldRight, oldBottom);
    }
    Invalidate();
  }
}
//---------------------------------------------------------------------------
void TMainGrid::UpdateEOFMarker(int oldRight, int oldBottom)
{
  if(oldRight+1 < ColCount && oldBottom+1 < RowCount &&
     Objects[oldRight+1][oldBottom+1] == EOFMarker){
    if(Col == oldRight+1 && Row == oldBottom+1 && InplaceEditor){
      // Objects更新時に選択範囲が初期化されるのを復元
      int ss = InplaceEditor->SelStart;
      int sl = InplaceEditor->SelLength;
      Objects[oldRight+1][oldBottom+1] = nullptr;
      InplaceEditor->SelStart = ss;
      InplaceEditor->SelLength = sl;
    }else{
      Objects[oldRight+1][oldBottom+1] = nullptr;
    }
  }
  if(FDataRight+1 < ColCount && FDataBottom+1 < RowCount){
    Objects[FDataRight+1][FDataBottom+1] = EOFMarker;
  }
}
//---------------------------------------------------------------------------
void TMainGrid::SetDataRightBottom(int rx, int by, bool updateTableSize)
{
  int oldRight = DataRight;
  int oldBottom = DataBottom;

  FDataRight = max(rx, 1);
  FDataBottom = max(by, 1);
  if(updateTableSize){
    int cc = rx + 2;
    if(cc <= Col){ cc = Col + 1; }
    ChangeColCount(cc);
  }
  if(updateTableSize){
    int rc = by+2;
    if(rc <= Row){ rc = Row + 1; }
    ChangeRowCount(rc);
  }

  if (TypeOption->DummyEol) {
    for (int y = FDataBottom + 1; y <= oldBottom; y++) {
      for (int x = 0; x <= oldRight + 1; x++) {
        if (Objects[x][y] != nullptr) {
          Objects[x][y] = nullptr;
        }
      }
    }
  }

  if(TypeOption->DummyEof) {
    if(FDataRight != oldRight || FDataBottom != oldBottom
       || (FDataRight+1 < ColCount && FDataBottom+1 < RowCount
           && Objects[FDataRight+1][FDataBottom+1] != EOFMarker)){
      UpdateEOFMarker(oldRight, oldBottom);
      Invalidate();
    }
  }else if(FDataRight != oldRight || FDataBottom != oldBottom){
    Invalidate();
  }
}
//---------------------------------------------------------------------------
int TMainGrid::GetRowDataRight(int Row)
{
  if (!TypeOption->DummyEol) {
    return FDataRight;
  }
  for (int x = ColCount - 1; x >= 0; x--) {
    if (HasData(x, Row)) {
      return x;
    }
  }
  return 0;
}
//---------------------------------------------------------------------------
void TMainGrid::SetRowDataRight(int Row, int Right, bool ExpandOnly)
{
  if (!TypeOption->DummyEol) {
    return;
  }
  for (int x = ColCount - 1; x > Right; x--) {
    if (Cells[x][Row] != "") {
      return;
    }
    if (Objects[x][Row] == EOLMarker) {
      if (ExpandOnly) {
        return;
      }
      Objects[x][Row] = nullptr;
    }
  }
  if (Objects[Right][Row] != EOLMarker) {
    Objects[Right][Row] = EOLMarker;
    Invalidate();
  }
}
//---------------------------------------------------------------------------
void GetReturnCode(const DynamicArray<wchar_t> &charBuffer, bool useQuote,
                   TReturnCode *code, TReturnCode *inCellCode)
{
  int crlf[2] = {0, 0};
  int lf[2] = {0, 0};
  int cr[2] = {0, 0};
  int inCell = 0;
  int len = charBuffer.Length;
  for (int i = 0; i < len - 1; i++) {
    wchar_t d = charBuffer[i];
    if (d == L'\x0D') {
      if (charBuffer[i + 1] == L'\x0A') {
        crlf[inCell]++;
        i++;
      } else {
        cr[inCell]++;
      }
    } else if (d == L'\x0A') {
      lf[inCell]++;
    } else if (d == L'"') {
      inCell = 1 - inCell;
    }
  }
  if (len > 0) {
    // LastChar
    wchar_t d = charBuffer[len - 1];
    if (d == L'\x0D') {
      cr[inCell]++;
    } else if (d == L'\x0A') {
      lf[inCell]++;
    }
  }


  if (cr[0] > crlf[0] && cr[0] > lf[0]) {
    *code = CR;
  } else if (lf[0] > crlf[0]) {
    *code = LF;
  } else {
    *code = CRLF;
  }

  if (crlf[1] == 0 && lf[1] == 0 && cr[1] > 0) {
    *inCellCode = CR;
  } else if (crlf[1] == 0 && lf[1] > 0 && cr[1] == 0) {
    *inCellCode = LF;
  } else if (crlf[1] > 0 && lf[1] == 0 && cr[1] == 0) {
    *inCellCode = CRLF;
  } else {
    *inCellCode = *code;
  }
}
//---------------------------------------------------------------------------
void TMainGrid::SetDragAcceptFiles(bool Accept)
{
  ::DragAcceptFiles(Handle, Accept);
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::DropCsvFiles(TWMDropFiles inMsg)
{
  int count = DragQueryFile((HDROP)inMsg.Drop, 0xffffffff, nullptr, 255);
  String *fileNames = new String[count];
  TCHAR fileName[255];
  for (int i = 0; i < count; i++) {
    ::DragQueryFile((HDROP)inMsg.Drop, i, fileName, 255);
    fileNames[i] = fileName;
  }
  if (FOnDropFiles != nullptr) {
    FOnDropFiles(this, count, fileNames);
  }
  delete[] fileNames;
}
//---------------------------------------------------------------------------
static bool HasBom(DynamicArray<Byte> buf, TEncoding *encoding)
{
  switch (encoding->CodePage) {
    case CODE_PAGE_UTF8:
      return buf.Length > 3
             && buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf;
    case CODE_PAGE_UTF16LE:
      return buf.Length > 2 && buf[0] == 0xff && buf[1] == 0xfe;
    case CODE_PAGE_UTF16BE:
      return buf.Length > 2 && buf[0] == 0xfe && buf[1] == 0xff;
    default:
      return false;
  }
}
//---------------------------------------------------------------------------
void TMainGrid::LoadFromFile(String FileName, TEncoding *encoding,
    bool isDetectedEncoding, const TTypeOption *Format,
    TNotifyEvent OnTerminate)
{
  TFileStream *File = new TFileStream(FileName, fmOpenRead|fmShareDenyNone);
  int bufLength = min(File->Size, 1024);
  Clear();
  if (bufLength == 0) {
    delete File;
    Modified = false;
    OnTerminate(this);
    return;
  }
  DynamicArray<Byte> byteBuffer;
  byteBuffer.Length = bufLength;
  bufLength = File->Read(&(byteBuffer[0]), bufLength);
  byteBuffer.Length = bufLength;
  delete File;

  DynamicArray<wchar_t> charBuffer;
  charBuffer.Length = bufLength;
  TStreamReader *reader =
      new TStreamReader(FileName, encoding, /* DetectBOM= */ true,
                        /* BufferSize= */ 4096);
  try {
    Encoding = encoding;
    int readCount = reader->ReadBlock(charBuffer, 0, bufLength);
    charBuffer.Length = readCount;
    AddBom = HasBom(byteBuffer, encoding);
  } catch (...) {
    if (!isDetectedEncoding) {
      Application->MessageBox(
          L"指定された文字コードではファイルを開けません。",
          CASSAVA_TITLE, 0);
    }
    Encoding = TEncoding::Default;
    charBuffer = TEncoding::Default->GetChars(byteBuffer);
    AddBom = false;
  }
  delete reader;

  TypeOption = Format;
  Cursor = crAppStart;
  Hint = L"ファイルを読み込み中です。";
  ShowHint = true;

  GetReturnCode(charBuffer, TypeOption->UseQuote(), &ReturnCode,
                &InCellReturnCode);

  OnFileOpenThreadTerminate = OnTerminate;
  FileOpenThread = ThreadFileOpen(this, FileName, Encoding, isDetectedEncoding);
  FileOpenThread->OnTerminate = FileOpenThreadTerminate;
  FileOpenThread->FreeOnTerminate = true;
  FileOpenThread->Start();
  Modified = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::FileOpenThreadTerminate(System::TObject* Sender)
{
  FileOpenThread = nullptr;
  Cursor = crDefault;
  Hint = "";
  ShowHint = false;
  ParentShowHint = true;
  if(OnFileOpenThreadTerminate){
    OnFileOpenThreadTerminate(Sender);
    OnFileOpenThreadTerminate = nullptr;
  }
  ClearCalcCache();
  Invalidate();
}
//---------------------------------------------------------------------------
static String GetClipboardText()
{
  TClipboard *clip = new TClipboard;
  if (!clip->HasFormat(CF_TEXT)) {
    clip->Close();
    delete clip;
    return "";
  }
  String clipboardText;
  for (int i = 0;; i++) {
    try {
      clipboardText = clip->AsText;
      break;
    } catch (...) {
      if (i >= 10) {
        clip->Close();
        delete clip;
        throw;
      }
      Sleep(200);
    }
  }
  clip->Close();
  delete clip;
  return clipboardText;
}
//---------------------------------------------------------------------------
void TMainGrid::PasteCSV(TStrings *List, int Left, int Top, int Way,
                         int ClipCols, int ClipRows, const TTypeOption *Format)
{
  // Way
  //  0: 重なった部分のみ
  //  1: くり返し処理
  //  2: データサイズ上書き
  //  3: 右挿入
  //  4: 下挿入
  //  5: テキストとしてセル内に
  if (Way == 5) {
    ShowEditor();
    InplaceEditor->SelText = GetClipboardText();
    InplaceEditor->Text = TrimRight(InplaceEditor->Text);
    return;
  }

  int selBottom = Selection.Bottom;
  int selRight = Selection.Right;
  int iEnd = ClipRows;
  if (Way == 0) {
    int bottom = min(selBottom, Top + ClipRows - 1);
    if (DataBottom < bottom) {
      InsertRow(DataBottom + 1, bottom);
    }
    int right = min(selRight, Left + ClipCols - 1);
    if (DataRight < right) {
      InsertColumn(DataRight + 1, right);
    }
    iEnd = bottom - Top + 1;
  } else if (Way == 1) {
    if (DataBottom < selBottom) {
      InsertRow(DataBottom + 1, selBottom);
    }
    if (DataRight < selRight) {
      InsertColumn(DataRight + 1, selRight);
    }
    iEnd = selBottom - Top + 1;
  } else if (Way == 2) {
    if (DataBottom < Top + ClipRows - 1) {
      InsertRow(DataBottom + 1, Top + ClipRows - 1);
    }
    if (DataRight < Left + ClipCols - 1) {
      InsertColumn(DataRight + 1, Left + ClipCols - 1);
    }
  } else if (Way == 3) {
    if (DataBottom < Top + ClipRows - 1) {
      InsertRow(DataBottom + 1, ClipRows + Top - 1);
    }
    InsertCells_Right(Left, Left + ClipCols - 1, Top, Top + ClipRows - 1);
  } else if (Way == 4) {
    if (DataRight < Left + ClipCols - 1) {
      InsertColumn(DataRight + 1, Left + ClipCols - 1);
    }
    InsertCells_Down(Left, Left + ClipCols - 1, Top, Top + ClipRows - 1);
  }
  TStringList *OneRow = new TStringList;
  for (int i = 0; i < iEnd; i++) {
    int ii = (Way == 1 && ClipRows > 0) ? i % ClipRows : i;
    SetCsv(OneRow, List->Strings[ii], Format);
    int jEnd = OneRow->Count;
    if (Way == 0) {
      jEnd = min(selRight - Left + 1, jEnd);
    } else if (Way == 1) {
      jEnd = selRight - Left + 1;
    }
    for (int j = 0; j < jEnd; j++) {
      int jj = (Way == 1 && ClipCols > 0) ? j % ClipCols : j;
      if (jj < OneRow->Count) {
        SetCell(j + Left, i + Top, OneRow->Strings[jj]);
      }
    }
    SetRowDataRight(i + Top, jEnd + Left - 1, /* Expand= */ true);
  }
  delete OneRow;
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::SetCsv(TStringList *Dest, String Src, const TTypeOption *Format)
{
  int CellBegin = 1;
  bool Quoted = false;
  int Kugiri = 2; // 0:通常 1:弱区切り（" "） 2:強区切り（"," "\t"）
  Dest->Clear();
  for(int i=1; i<=Src.Length(); i++){
    if (Format->SepChars.Pos(Src[i]) > 0) {
      if(!Quoted){
	      if(Kugiri != 1) Dest->Add(Src.SubString(CellBegin,i-CellBegin));
	      Kugiri = 2;
	      CellBegin = i+1;
      }
    } else if (Format->WeakSepChars.Pos(Src[i]) > 0) {
      if(!Quoted){
      	if(Kugiri == 0){
      	  Dest->Add(Src.SubString(CellBegin,i-CellBegin));
      	  Kugiri = 1;
      	}
      	CellBegin = i+1;
      }
    } else if (Format->UseQuote() && Format->QuoteChars.Pos(Src[i]) > 0) {
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
}
//---------------------------------------------------------------------------
void TMainGrid::SaveToFile(String FileName, const TTypeOption *Format,
    bool SetModifiedFalse)
{
  TFileStream *fs = new TFileStream(FileName, fmCreate | fmShareDenyWrite);
  EncodedWriter *ew = new EncodedWriter(fs, Encoding, AddBom);

  WriteGrid(ew, Format);

  delete ew;
  delete fs;
  if(SetModifiedFalse){
    Modified = false;
  }
}
//---------------------------------------------------------------------------
void TMainGrid::WriteGrid(EncodedWriter *Writer, const TTypeOption *Format)
{
  if (Format == nullptr) { Format = TypeOption; }

  TStringList* Data = new TStringList;
  for (int i = DataTop; i <= DataBottom; i++) {
    Data->Assign(Rows[i]);

    if (Format->DummyEol || !Format->OmitComma) {
      int right = GetRowDataRight(i);
      for (int i = Data->Count - 1; i > right; i--) {
        Data->Delete(i);
      }
    } else {
      for (int i = Data->Count - 1; i > 0 && Data->Strings[i] == ""; i--) {
        Data->Delete(i);
      }
    }
    if (ShowRowCounter) {
      Data->Delete(0);   // カウンタセルの削除
    }
    Writer->Write(StringsToCSV(Data, Format) + ReturnCodeString(ReturnCode));
  }
  delete Data;
}
//---------------------------------------------------------------------------
String TMainGrid::StringsToCSV(TStrings* Data, const TTypeOption *Format)
{
  char Sep = Format->DefSepChar();
  String Text = "";
  String Delim = Format->SepChars + Format->WeakSepChars
      + Format->QuoteChars + L"\r\n";

  for (int i = 0; i < Data->Count; i++) {
    String Str = Data->Strings[i];

    if (Format->QuoteOption != soNone) {
      for (int j = Str.Length(); j > 0; j--) {
        if (Str[j] == L'\"') {
          Str.Insert(L"\"", j);
        } else if (Str[j] == L'\r' && InCellReturnCode == LF) {
          Str.Delete(j, 1);
        } else if (Str[j] == L'\n' && InCellReturnCode == CR) {
          Str.Delete(j, 1);
        }
      }
    }

    if (Format->QuoteOption == soNone ||
        (Format->QuoteOption == soString && IsNumber(Str))) {
      Text += Str;
    } else if (Format->QuoteOption == soNormal) {
      if (Str.LastDelimiter(Delim) > 0) {
        Text += (String) L"\"" + Str + L"\"";
      } else {
        Text += Str;
      }
    } else {
      Text += (String) L"\"" + Str + L"\"";
    }
    if (i < Data->Count - 1) Text += Sep;
  }
  return Text;
}
//---------------------------------------------------------------------------
void TMainGrid::QuotedDataToStrings(TStrings *Lines, String Text,
    const TTypeOption *Format)
{
  Lines->Text = Text;
  if(Format->QuoteOption==soNone){ return; }
  int i=0;
  while(i<Lines->Count){
    int qc = 0;
    String str = Lines->Strings[i];
    for(int j=1; j<=str.Length(); j++){
      if(str.IsDelimiter(Format->QuoteChars, j)){ qc++; }
    }
    if((qc % 2) && i+1 <Lines->Count){
      Lines->Strings[i] =
          str + ReturnCodeString(ReturnCode) + Lines->Strings[i+1];
      Lines->Delete(i+1);
    }else{
      i++;
    }
  }
}
//---------------------------------------------------------------------------
static void SetClipboard(String text) {
  TClipboard *clip = new TClipboard;
  for (int i = 0;; i++) {
    try {
      clip->AsText = text;
      break;
    } catch (...) {
      if (i >= 10) {
        clip->Close();
        delete clip;
        throw;
      }
      Sleep(200);
    }
  }
  clip->Close();
  delete clip;
}
//---------------------------------------------------------------------------
void TMainGrid::CopyToClipboard(const TTypeOption *Format, bool Cut)
{
  if (EditorMode) {
    SingleCellCopiedText = InplaceEditor->SelText;
    SetClipboard(SingleCellCopiedText);
    if (Cut) {
      InplaceEditor->SelText = "";
      Modified = true;
    }
    return;
  }
  SingleCellCopiedText = "";

  UndoList->Push();

  int SLeft = SelLeft;
  int STop = SelTop;
  int SRight = Selection.Right;
  int SBottom = Selection.Bottom;

  TStringList *Data = new TStringList;
  TStringList *OneLine = new TStringList;
  for (int i = STop; i <= SBottom; i++) {
    OneLine->Clear();
    for (int j = SLeft; j <= SRight; j++) {
      OneLine->Add(GetACells(RXtoAX(j), RYtoAY(i)));
      if (Cut) {
        SetCell(j, i, "");
      }
    }
    Data->Add(StringsToCSV(OneLine, Format != nullptr ? Format : TypeOption));
  }
  delete OneLine;
  String Txt = Data->Text;
  Txt.SetLength(Txt.Length()-2);
  SetClipboard(Txt);
  delete Data;

  String select = (String)"Select(" + RXtoAX(SLeft) + ", " + RYtoAY(STop) +
      ", " + RXtoAX(SRight) + ", " + RYtoAY(SBottom) + ");\n";
  if(Cut) {
    UndoList->PopWithRecordedMacro(select + "Cut();");
    Modified = true;
  } else {
    UndoList->PopWithRecordedMacro(select + "Copy();");
  }
}
//---------------------------------------------------------------------------
void TMainGrid::CutToClipboard(const TTypeOption *Format)
{
  CopyToClipboard(Format, true);
}
//---------------------------------------------------------------------------
void TMainGrid::PasteFromClipboard(int Way, const TTypeOption *Format)
{
  if (Format == nullptr) {
    Format = TypeOption;
  }

  String clipboardText = GetClipboardText();
  if (EditorMode
      && (InplaceEditor->SelStart > 0
          || InplaceEditor->SelLength < InplaceEditor->Text.Length()
          || clipboardText == SingleCellCopiedText)) {
    InplaceEditor->SelText = GetClipboardText();
    InplaceEditor->Text = TrimRight(InplaceEditor->Text);
    Modified = true;
    Invalidate();
    return;
  }

  int STop = SelTop;
  int SLeft = SelLeft;
  int SBottom = Selection.Bottom;
  int SRight = Selection.Right;
  int SelectRowCount = SBottom - STop + 1;
  int SelectColCount = SRight - SLeft + 1;

  TStringList *Data = new TStringList;
  QuotedDataToStrings(Data, clipboardText, Format);
  int ClipRowCount = Data->Count;
  int ClipColCount = 0;
  TStringList *ARow = new TStringList;
  for (int i = 0; i < ClipRowCount; i++) {
    SetCsv(ARow, Data->Strings[i], Format);
    ClipColCount = max(ARow->Count, ClipColCount);
  }
  delete ARow;

  if (Way < 0) {
    if (PasteOption >= 0) {
      Way = PasteOption;
    } else if (ClipRowCount == SelectRowCount &&
               ClipColCount == SelectColCount) {
      Way = EditorMode ? PASTE_OPTION_EDITOR : PASTE_OPTION_OVERWRITE;
    } else {
      TfmPasteDialog *PstDlg = new TfmPasteDialog(Application);
      PstDlg->Way->ItemIndex = DefWay;
      PstDlg->lbMessage->Caption =
          (String)L"選択サイズ： " + SelectColCount + L" × " + SelectRowCount +
          L"　クリップボードサイズ： " + ClipColCount + L" × " + ClipRowCount;
      if (PstDlg->ShowModal() != IDOK) {
        delete PstDlg;
        delete Data;
        return;
      }
      Way = DefWay = PstDlg->Way->ItemIndex;
      delete PstDlg;
    }
  }

  UndoList->Push();
  if (Way == PASTE_OPTION_INSERT_ROW) {
    InsertRow(STop, STop + ClipRowCount - 1);
    Way = PASTE_OPTION_OVERWRITE;
  } else if (Way == PASTE_OPTION_INSERT_COL) {
    InsertColumn(SLeft, SLeft + ClipColCount - 1);
    Way = PASTE_OPTION_OVERWRITE;
  }
  UndoList->Push();
  PasteCSV(Data, SLeft, STop, Way, ClipColCount, ClipRowCount, Format);
  UndoList->PopWithRecordedMacro((String)"Select(" + RXtoAX(SLeft) + ", " +
      RYtoAY(STop) + ", " + RXtoAX(SRight) + ", " + RYtoAY(SBottom) +
      ");\nPaste(" + Way + ");");
  UndoList->Pop();

  if (Way <= 1) {
    SetSelection(SLeft, SLeft + SelectColCount - 1,
                 STop, STop + SelectRowCount - 1);
  } else if (Way <= 4) {
    SetSelection(SLeft, SLeft + ClipColCount - 1,
                 STop, STop + ClipRowCount - 1);
  }

  delete Data;
  Modified = true;
  Invalidate();
}
//---------------------------------------------------------------------------
bool TMainGrid::IsNumber(String Str)
{
  try {
    // In Win32, ToDouble() fails if the number is too big.
    // Disallow too long string and E/e.
    if (Str == "" || Str.Length() > 100 ||
        Str.Pos("E") > 0 || Str.Pos("e") > 0) {
      return false;
    }
    Str.ToDouble();
    return true;
  } catch (...) {}

  return false;
}
//---------------------------------------------------------------------------
typedef struct { double Num; TStringList *Data; int Row; } DoubleData;
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
typedef struct { String Str; TStringList *Data; int Row; } StringData;
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
void TMainGrid::Sort(int SLeft, int STop, int SRight, int SBottom, int SCol,
  bool Shoujun, bool NumSort, bool IgnoreCase, bool IgnoreZenhan)
{
  UndoList->Push();
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

  for (int y = STop; y <= SBottom; y++){
    int index = Shoujun ? y - STop : SBottom - y;
    TStringList *list;
    if (index < NumList->Count) {
      DoubleData *dd = static_cast<DoubleData*>(NumList->Items[index]);
      list = dd->Data;
      delete dd;
    } else {
      StringData *sd =
          static_cast<StringData*>(StrList->Items[index - NumList->Count]);
      list = sd->Data;
      delete sd;
    }
    for (int x = SLeft; x <= SRight; x++){
      SetCell(x, y, list->Strings[x - SLeft]);
    }
    delete list;
  }

  delete StrList;
  delete NumList;
  UndoList->Pop((String)"Sort(" + RXtoAX(SLeft) + ", " + RYtoAY(STop) + ", "
      + RXtoAX(SRight) + ", " + RYtoAY(SBottom) + ", " + RXtoAX(SCol) + ", "
      + boolToStr(!Shoujun) + ", " + boolToStr(NumSort) + ", "
      + boolToStr(IgnoreCase) + ", " + boolToStr(IgnoreZenhan) + ");");
  Modified = true;
}
//---------------------------------------------------------------------------
double TMainGrid::GetSum(int l, int t, int r, int b, int *Count)
{
  double Sum = 0;
  int NumCount = 0;
  for (int i=l; i <= r; i++) {
    for (int j=t; j <= b; j++) {
      String Str = GetACells(i,j);
      if (IsNumber(Str)) {
        try {
          Sum += Str.ToDouble();
          NumCount++;
        } catch(...) {}
      }
    }
  }

  if (Count) {
    *Count = NumCount;
  }
  return Sum;
}
//---------------------------------------------------------------------------
double TMainGrid::GetAvr(int l, int t, int r, int b)
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
double TMainGrid::SelectSum(int *Count)
{
  double Sum = 0;
  int NumCount = 0;
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      String Str = GetACells(RXtoAX(i),RYtoAY(j));
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
void TMainGrid::CopySum()
{
  SetClipboard(SelectSum(nullptr));
}
//---------------------------------------------------------------------------
void TMainGrid::CopyAvr()
{
  int Count;
  double Sum = SelectSum(&Count);

  if (Count==0) {
    SetClipboard("0");
  } else {
    SetClipboard(Sum / Count);
  }
}
//---------------------------------------------------------------------------
wchar_t Hankaku2Zenkaku(wchar_t wc)
{
  if(wc == L' ') return L'　';
  else if(wc < L'｡' || wc > L'ﾟ') return wc;
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
  else if(wc == L'｡') return L'。';
  else if(wc == L'｢') return L'「';
  else if(wc == L'｣') return L'」';
  else if(wc == L'､') {
    return L'、';
  }else if(wc == L'･') return L'・';

  return wc;
}
//---------------------------------------------------------------------------
int Zenkaku2Hankaku(wchar_t wc, wchar_t *ans)
{
  if(wc == L'　'){ *ans = L' '; return 1; }
  else if(wc == L'ワ'){ *ans = L'ﾜ'; return 1; }
  else if(wc == L'ヲ'){ *ans = L'ｦ'; return 1; }
  else if(wc == L'ン'){ *ans = L'ﾝ'; return 1; }
  else if(wc == L'ヴ'){ *ans = L'ｳ'; *(ans+1) = L'ﾞ'; return 2; }
  else if(wc == L'゛'){ *ans = L'ﾞ'; return 1; }
  else if(wc == L'゜'){ *ans = L'ﾟ'; return 1; }
  else if(wc == L'ー'){ *ans = L'ｰ'; return 1; }
  else if(wc == L'。'){ *ans = L'｡'; return 1; }
  else if(wc == L'「'){ *ans = L'｢'; return 1; }
  else if(wc == L'」'){ *ans = L'｣'; return 1; }
  else if(wc == L'、'){ *ans = L'､'; return 1; }
  else if(wc == L'・'){ *ans = L'･'; return 1; }
  else if(wc < L'ァ' || wc > L'ロ'){ *ans = wc; return 1; }
  else if(wc == L'ッ'){ *ans = L'ｯ'; return 1; }
  else if(wc >= L'ァ' && wc <= L'オ'){
    int x = (wc - L'ァ');
    *ans = ((x % 2) ? (L'ｱ' + (x / 2)) : (L'ｧ' + (x / 2)));
    return 1;
  }else if(wc >= L'カ' && wc <= L'ヂ'){
    int x = (wc - L'カ');
    *ans = L'ｶ' + (x / 2);
    if(x % 2){
      *(ans+1) = L'ﾞ';
      return 2;
    }else{
      return 1;
    }
  }else if(wc >= L'ツ' && wc <= L'ド'){
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
  }else if(wc >= L'ハ' && wc <= L'ポ'){
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
void TMainGrid::TransChar(int Type)
{
  UndoList->Push();
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      SetCell(i, j, TransChar(Cells[i][j], Type));
    }
  }
  UndoList->Pop((String)"Select(" + RXtoAX(SelLeft) + ", " + RYtoAY(SelTop)
      + ", " + RXtoAX(Selection.Right) + ", " + RYtoAY(Selection.Bottom)
      + ");\nTransChar" + Type + "();");
  Modified = true;
}
//---------------------------------------------------------------------------
String TMainGrid::TransChar(String Str, int Type)
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
        if(*p == L'　')      *q = L' ';
        else if(*p == L'’') *q = L'\'';
        else if(*p == L'”') *q = L'\"';
        else if(*p == L'￥') *q = L'\\';
        else if(*p >= L'！' && *p <= L'〜') *q = *p - (L'Ａ' - L'A');
        else                 *q = *p;
      }else{
        if(*p == L' ')       *q = L'　';
        else if(*p == L'\'') *q = L'’';
        else if(*p == L'\"') *q = L'”';
        else if(*p == L'\\') *q = L'￥';
        else if(*p >= L'!' && *p <= L'~') *q = *p + (L'Ａ' - L'A');
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
void TMainGrid::TransKana(int Type)
{
  UndoList->Push();
  for(int i=SelLeft; i <= Selection.Right; i++){
    for(int j=SelTop; j <= Selection.Bottom; j++){
      SetCell(i, j, TransKana(Cells[i][j], Type));
    }
  }
  UndoList->Pop((String)"Select(" + RXtoAX(SelLeft) + ", " + RYtoAY(SelTop)
      + ", " + RXtoAX(Selection.Right) + ", " + RYtoAY(Selection.Bottom)
      + ");\nTransChar" + Type + "();");
  Modified = true;
}
//---------------------------------------------------------------------------
String TMainGrid::TransKana(String Str, int Type)
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
      if(*(p+1) == L'ﾞ' &&
          ((*p >= L'ｶ' && *p <= L'ﾄ') || (*p >= L'ﾊ' && *p <= L'ﾎ'))) {
        (*q)++;
        p++;
      }else if(*(p+1) == L'ﾟ' && *p >= L'ﾊ' && *p <= L'ﾎ') {
        (*q) += 2;
        p++;
      }else if(*(p+1) == L'ﾞ' && *p == L'ｳ') {
        (*q) = L'ヴ';
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
void TMainGrid::Sequence(bool Inc)
{
  int selLeft = Selection.Left;
  int selRight = Selection.Right;
  int selTop = Selection.Top;
  int selBottom = Selection.Bottom;

  UndoList->Push();
  if (selBottom > DataBottom) {
    UndoList->AddMacro(
        (String)"DeleteRow(" + (DataBottom + 1) + ", " + selBottom + ");", "");
    SetDataRightBottom(DataRight, selBottom, true);
    SetSelection(selLeft, selRight, selTop, selBottom);
  }

  for (int x = selLeft; x <= selRight; x++) {
    int IncType = 0;
    double Increment = 1.0;
    double TopValue = 0.0;
    String TopStr = Cells[x][selTop];

    if(Inc && TopStr != ""){
      char LastChar = *(TopStr.LastChar());
      if(TopStr.Length() >= 2 && TopStr[1] == '0'
         && TopStr[2] >= '0' && TopStr[2] <= '9'
         && LastChar>='0' && LastChar<='9'){
        IncType = 2;
      }else if(IsNumber(TopStr)){
        IncType = 1;
        TopValue = TopStr.ToDouble();
        String Snd = Cells[x][selTop + 1];
        if(IsNumber(Snd)) Increment = Snd.ToDouble() - TopValue;
      }else if(LastChar>='0' && LastChar<='9'){
        IncType = 2;
      }
    }
    for (int y = selTop + 1; y <= selBottom; y++) {
      String value;
      switch(IncType){
        case 0:
          value = TopStr;
          break;
        case 1:
          value = (String)(TopValue + Increment * (y - selTop));
          break;
        case 2:{
          int p = TopStr.Length();
          while(p>0 && TopStr[p]=='9'){ TopStr[p]='0'; p--; }
          if(p==0) TopStr = (String)"1" + TopStr;
          else if(TopStr[p]>='0' && TopStr[p]<'9') TopStr[p]++;
          else TopStr.Insert("1",p+1);
          value = TopStr;
        } break;
      }
      SetCell(x, y, value);
    }
  }

  UndoList->Pop((String)"Select(" + RXtoAX(selLeft) + ", " + RYtoAY(selTop)
      + ", " + RXtoAX(selRight) + ", " + RYtoAY(selBottom) + ");\nSequence"
      + (Inc ? "S" : "C") + "();");
  Modified = true;
}
//---------------------------------------------------------------------------
//  挿入
//
void TMainGrid::ChangeRowCount(int Count)
{
  if (Count > RowCount) {
    int r = RowCount;
    RowCount = Count;
    for (int i = r; i < Count; i++) {
      Rows[i]->Clear();
    }
  } else if (Count < RowCount) {
    RowCount = max(Count, 5);
    for (int i = max(Count, 0); i < RowCount; i++) {
      Rows[i]->Clear();
    }
  }
}
//---------------------------------------------------------------------------
void TMainGrid::ChangeColCount(int Count)
{
  if (Count > ColCount) {
    int c = ColCount;
    ColCount = Count;
    for (int i = c; i < Count; i++) {
      Cols[i]->Clear();
    }
  } else if (Count < ColCount) {
    ColCount = max(Count, 5);
    for (int i = max(Count, 0); i < ColCount; i++) {
      Cols[i]->Clear();
    }
  }
}
//---------------------------------------------------------------------------
void TMainGrid::InsertRow(int Top, int Bottom)
{
  if (Top == Bottom) {
    int t = RYtoAY(Top);
    UndoList->AddMacro((String)"DeleteRow(" + t + ");",
                       (String)"InsertRow(" + t + ");");
  } else {
    int t = RYtoAY(Top);
    int b = RYtoAY(Bottom);
    UndoList->AddMacro((String)"DeleteRow(" + t + ", " + b + ");",
                       (String)"InsertRow(" + t + ", " + b + ");");
  }
  UndoList->Lock();
  int addingRows = Bottom - Top + 1;
  FDataBottom = max(FDataBottom + addingRows, Bottom);
  int updatedRowCount = max(FDataBottom + 2, Row + addingRows+ 1);
  if (RowCount < updatedRowCount) {
    ChangeRowCount(updatedRowCount);
  }
  for(int y = Top; y <= Bottom; y++){
    TStringGrid::MoveRow(RowCount - 1, y);
  }
  UndoList->Unlock();
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::InsertColumn(int Left, int Right)
{
  if (Left == Right) {
    int l = RXtoAX(Left);
    UndoList->AddMacro((String)"DeleteCol(" + l + ");",
                       (String)"InsertCol(" + l + ");");
  } else {
    int l = RXtoAX(Left);
    int r = RXtoAX(Right);
    UndoList->AddMacro((String)"DeleteCol(" + l + ", " + r + ");",
                       (String)"InsertCol(" + l + ", " + r + ");");
  }
  UndoList->Lock();
  int addingCols = Right - Left + 1;
  FDataRight = max(FDataRight + addingCols, Right);
  int updatdColCount = max(FDataRight + 2, Col + addingCols + 1);
  if (ColCount < updatdColCount) {
    ChangeColCount(updatdColCount);
  }
  for(int x = Left; x <= Right; x++){
    TStringGrid::MoveColumn(ColCount - 1, x);
  }
  UndoList->Unlock();
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::DeleteRow(int Top, int Bottom)
{
  if (Top > DataBottom) {
    return;
  }
  if (Bottom > DataBottom) {
    Bottom = DataBottom;
  }
  if (Top == Bottom) {
    DeleteRow(Top);
    return;
  }
  if (RowCount <= FixedRows + 1) {
    return;
  }
  UndoList->Push();
  if (RangeSelect) {
    Col = FixedCols;
  }
  int delta = Bottom - Top + 1;
  if (Row >= Top && Row <= Bottom) {
    Row = Top;
  } else if (Row > Bottom) {
    Row = Row - delta;
  }
  DeleteCells_Up(0, ColCount - 1, Top, Bottom, true);
  SetDataRightBottom(DataRight, DataBottom - delta, true);
  UndoList->Pop(
      (String)"DeleteRow(" + RYtoAY(Top) + ", " + RYtoAY(Bottom) + ");");
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::DeleteColumn(int Left, int Right)
{
  if (Left > DataRight) {
    return;
  }
  if (Right > DataRight) {
    Right = DataRight;
  }
  if (Left == Right) {
    DeleteColumn(Left);
    return;
  }
  if (ColCount <= FixedCols + 1) {
    return;
  }
  UndoList->Push();
  if (RangeSelect) {
    Row = FixedRows;
  }
  int delta = Right - Left + 1;
  if (Col >= Left && Col <= Right) {
    Col = Left;
  } else if (Col > Right) {
    Col = Col - delta;
  }
  DeleteCells_Left(Left, Right, 0, RowCount - 1, true);
  SetDataRightBottom(DataRight - delta, DataBottom, true);
  UndoList->Pop(
      (String)"DeleteCol(" + RXtoAX(Left) + ", " + RXtoAX(Right) + ");");
  Modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::InsertRow(int Index)
{
  InsertRow(Index, Index);
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::InsertColumn(int Index)
{
  InsertColumn(Index, Index);
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::DeleteRow(int ARow)
{
  if (ARow > DataBottom || RowCount <= FixedRows + 1) {
    return;
  }
  UndoList->Push();
  if (RangeSelect) {
    Col = FixedCols;
  }
  if (Row == ARow) {
    if(ARow == RowCount - 1){
      Rows[RowCount++]->Clear();
    }
    Row = ARow + 1;
  }
  for (int x = DataLeft; x <= DataRight; x++) {
    UndoList->ChangeCell(RXtoAX(x), RYtoAY(ARow), Cells[x][ARow], "",
                         RXtoAX(DataRight), RYtoAY(DataBottom));
  }
  UndoList->AddMacro((String)"InsertRow(" + RYtoAY(ARow) + ");",
                     (String)"DeleteRow(" + RYtoAY(ARow) + ");");
  UndoList->Lock();
  TStringGrid::DeleteRow(ARow);
  SetDataRightBottom(DataRight, DataBottom - 1, true);
  UndoList->Unlock();
  UndoList->Pop((String)"DeleteRow(" + RYtoAY(ARow) + ");");
  Modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::DeleteColumn(int ACol)
{
  if (ACol > DataRight || ColCount <= FixedCols + 1) {
    return;
  }
  UndoList->Push();
  if (RangeSelect) {
    Row = FixedRows;
  }
  if (Col == ACol) {
    if(ACol == ColCount-1){
      Cols[ColCount++]->Clear();
    }
    Col = ACol + 1;
  }
  for (int y = DataTop; y <= DataBottom; y++) {
    UndoList->ChangeCell(RXtoAX(ACol), RYtoAY(y), Cells[ACol][y], "",
                         RXtoAX(DataRight), RYtoAY(DataBottom));
  }
  UndoList->AddMacro((String)"InsertCol(" + RXtoAX(ACol) + ");",
                     (String)"DeleteCol(" + RXtoAX(ACol) + ");");
  UndoList->Lock();
  TStringGrid::DeleteColumn(ACol);
  SetDataRightBottom(DataRight - 1, DataBottom, true);
  UndoList->Unlock();
  UndoList->Pop((String)"DeleteCol(" + RXtoAX(ACol) + ");");
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::InsertEnter()
{
  long X = SelLeft, Y = Selection.Top, L = DataLeft;
  int SelLen = Selection.Right - SelLeft;

  bool OneCell = EditorMode && InplaceEditor->SelStart > 0;
  if (X == 1 && !OneCell) {
    InsertRow(Y);
    return;
  }
  UndoList->Push();
  InsertRow(Y + 1);
  int rowRight = GetRowDataRight(Y);
  if (OneCell) {
    SelLen = InplaceEditor->SelLength;
    SetCell(L, Y + 1, InplaceEditor->Text.SubString(
        InplaceEditor->SelStart + 1, InplaceEditor->Text.Length()));
    SetCell(X, Y, InplaceEditor->Text.SubString(1, InplaceEditor->SelStart));
  } else {
    SetCell(L, Y + 1, Cells[X][Y]);
    SetCell(X, Y, "");
  }

  for (int i = 1; i + X < ColCount; i++) {
    SetCell(i + L, Y + 1, Cells[i + X][Y]);
    SetCell(i + X, Y, "");
  }
  if (TypeOption->DummyEol) {
    SetRowDataRight(Y, OneCell ? X : X - 1);
    SetRowDataRight(Y + 1, rowRight - X + 1);
  }
  if (OneCell) {
    Row = Y + 1;
    Col = FixedCols;
    if (SelLen >= 0 && Col == DataLeft) { InplaceEditor->SelLength = SelLen; }
  } else {
    SetSelection(L, L + SelLen, Y + 1, Y + 1);
  }
  if (OneCell || X > DataRight) {
    UndoList->Pop();
  } else {
    UndoList->Pop((String)"Select(" + RXtoAX(X) + ", " + RYtoAY(Y) + ", "
        + RXtoAX(X + SelLen) + ", " + RYtoAY(Y) + ");\nEnter();");
  }
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::InsertNewLine()
{
  if(EditorMode){
    InplaceEditor->SelText = "\r\n";

    TRect MaxRect(0, 0, ColWidths[Col] - 4, ClientHeight);
    TRect R = DrawTextRect(Canvas, MaxRect, InplaceEditor->Text, WordWrap, true);
    if(RowHeights[Row] < R.Bottom){
      RowHeights[Row] = R.Bottom;
    }
  }
}
//---------------------------------------------------------------------------
void TMainGrid::ConnectCell()
{
  UndoList->Push();
  int L = Selection.Left;
  int T = Selection.Top;
  int R = Selection.Right;
  int B = Selection.Bottom;
  if(RangeSelect){
    String str = "";
    for(int j = T; j <= B; j++){
      for(int i = L; i <= R; i++){
        str += Cells[i][j];
        SetCell(i, j, "");
      }
    }
    Cells[L][T] = str;
  }else{
    int C = Col;
    if(Row > DataBottom){ // ダミーセルは処理しない
      Row = DataBottom;
      Col = DataRight;
    }else if(C > FixedCols && C > GetRowDataRight(Row)){
      Col = max(GetRowDataRight(Row), FixedCols);
    }else if(C > FixedCols){ //カンマの削除
      String str = Cells[C - 1][Row] + Cells[C][Row];
      DeleteCells_Left(C, C, Row, Row, false);
      SetCell(C - 1, Row, str);
      Col--;
    }else if(Row > FixedRows && Row <= DataBottom){ //リターンの削除
      int UpColCount, ThisColCount;
      if (TypeOption->DummyEol) {
        UpColCount = GetRowDataRight(Row - 1) - DataLeft + 1;
        ThisColCount = GetRowDataRight(Row) - DataLeft + 1;
      } else {
        int x;
        for (x = ColCount - 1; x >= DataLeft && Cells[x][Row - 1] == ""; x--) {}
        UpColCount = x - DataLeft + 1;
        for (x = ColCount - 1; x >= DataLeft && Cells[x][Row] == ""; x--) {}
        ThisColCount = x - DataLeft + 1;
      }
      //列数が足りなければ増やす
      if (AXtoRX(UpColCount + ThisColCount) > DataRight) {
        InsertColumn(DataRight + 1, AXtoRX(UpColCount + ThisColCount));
      }
      for (int x = DataLeft; x <= AXtoRX(ThisColCount); x++) {
        SetCell(x + UpColCount, Row - 1, Cells[x][Row]);
      }
      SetRowDataRight(Row - 1, AXtoRX(UpColCount + ThisColCount));
      Row--;
      Col = (UpColCount >= FixedCols) ? UpColCount + 1 : FixedCols;
      DeleteRow(Row + 1);
    }
  }
  UndoList->Pop((String)"Select(" + RXtoAX(L) + ", " + RYtoAY(T) + ", "
      + RXtoAX(R) + ", " + RYtoAY(B) + ");\n" + "ConnectCell();");
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::InsertCell_Right()
{
  InsertCells_Right(SelLeft, Selection.Right, SelTop, Selection.Bottom);
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::InsertCell_Down()
{
  InsertCells_Down(SelLeft, Selection.Right, SelTop, Selection.Bottom);
  Modified = true;
}
//---------------------------------------------------------------------------
void TMainGrid::DeleteCell_Left()
{
  if(Col > DataRight || Row > DataBottom){
    return;
  }
  if(RangeSelect){
    DeleteCells_Left(SelLeft, Selection.Right, SelTop, Selection.Bottom);
  }else{
    int i;
    for (i = Col; i < ColCount; i++) {
      if (HasData(i, Row)) {
        break;
      }
    }

    if(i >= ColCount){
      if(Row < DataBottom){
        // 次の行から連結
        Row++; Col = FixedCols; ConnectCell();
      }else{
        // 列の縮小を試みる
        for (int j = DataTop; j < RowCount; j++) {
          if (HasData(FDataRight, j)) {
            return;
          }
        }
        DeleteColumn(FDataRight);
      }
    }else{
      // 現在の行内で左につめる
      DeleteCells_Left(Col,Col,Row,Row);
    }
  }
  Modified = true;
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void TMainGrid::DeleteCell_Up()
{
  DeleteCells_Up(SelLeft, Selection.Right, SelTop,Selection.Bottom);
  Modified = true;
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
void TMainGrid::InsertCells_Right(long Left, long Right, long Top, long Bottom)
{
  UndoList->Push();

  int colsToAdd = Right - Left + 1;
  for (int x = DataRight; colsToAdd > 0 && DataRight + colsToAdd > Right;
       x--, colsToAdd--) {
    bool dataExists = false;
    for (int y = Top; y <= Bottom; y++) {
      if (HasData(x, y)) {
        dataExists = true;
        break;
      }
    }
    if (dataExists) {
      break;
    }
  }
  if (Right > DataRight + colsToAdd) {
    colsToAdd = Right - DataRight;
  }

  if (colsToAdd > 0) {
    InsertColumn(DataRight + 1, DataRight + colsToAdd);
  }

  TStringList *Temp = new TStringList;
  int iEnd = min(Bottom, DataBottom);
  for (int i = Top; i <= iEnd; i++) {
    Temp->Assign(Rows[i]);
    for (int j = Left; j <= Right; j++) {
      Temp->Insert(Left, "");
      Temp->Delete(Temp->Count-1);
    }
    Rows[i]->Assign(Temp);
  }
  delete Temp;
  SetSelection(Left, Right, Top, Bottom);

  String select = (String)"Select(" + RXtoAX(Left) + ", " + RYtoAY(Top) + ", "
      + RXtoAX(Right) + ", " + RYtoAY(Bottom) + ");\n";
  UndoList->AddMacro(select + "DeleteCellLeft();",
                     select + "InsertCellRight();");
  UndoList->Pop();
}
//---------------------------------------------------------------------------
void TMainGrid::InsertCells_Down(long Left, long Right, long Top, long Bottom)
{
  UndoList->Push();

  int rowsToAdd = Bottom - Top + 1;
  for (int y = DataBottom; rowsToAdd > 0 && DataBottom + rowsToAdd > Bottom;
       y--, rowsToAdd--) {
    bool dataExists = false;
    for (int x = Left; x <= Right; x++) {
      if (HasData(x, y)) {
        dataExists = true;
        break;
      }
    }
    if (dataExists) {
      break;
    }
  }
  if (Bottom > DataBottom + rowsToAdd) {
    rowsToAdd = Bottom - DataBottom;
  }
  if (rowsToAdd > 0) {
    InsertRow(DataBottom + 1, DataBottom + rowsToAdd);
  }

  TStringList *Temp = new TStringList;
  int iEnd = min(Right, DataRight);
  for (int i = Left; i <= iEnd; i++) {
    Temp->Assign(Cols[i]);
    for(int j = Top; j <= Bottom; j++) {
      Temp->Insert(Top, "");
      Temp->Delete(Temp->Count-1);
    }
    Cols[i]->Assign(Temp);
  }
  delete Temp;
  SetSelection(Left, Right, Top, Bottom);

  String select = (String)"Select(" + RXtoAX(Left) + ", " + RYtoAY(Top) + ", "
      + RXtoAX(Right) + ", " + RYtoAY(Bottom) + ");\n";
  UndoList->AddMacro(select + "DeleteCellUp();", select + "InsertCellDown();");
  UndoList->Pop();
}
//---------------------------------------------------------------------------
void TMainGrid::DeleteCells_Left(long Left, long Right, long Top, long Bottom,
        bool UpdateWidth)
{
  UndoList->Push();
  TStringList *Temp = new TStringList;
  for (int i = Top; i <= min(Bottom, RowCount - 1); i++) {
    Temp->Assign(Rows[i]);
    for (int j = Left; j <= min(Right, Temp->Count - 1); j++) {
      Temp->Delete(Left);
      Temp->Add("");
      UndoList->ChangeCell(RXtoAX(j), RYtoAY(i), Cells[j][i], "",
                           RXtoAX(DataRight), RYtoAY(DataBottom));
    }
    Rows[i]->Assign(Temp);
  }
  delete Temp;
  if (UpdateWidth) {
    int delta = Right - Left + 1;
    for (int j = Left; j<ColCount - delta; j++) {
      ColWidths[j] = ColWidths[j + delta];
    }
  }
  String select = (String)"Select(" + RXtoAX(Left) + ", " + RYtoAY(Top) + ", "
      + RXtoAX(Right) + ", " + RYtoAY(Bottom) + ");\n";
  UndoList->AddMacro(select + "InsertCellRight();", "");
  UndoList->Pop(select + "DeleteCellLeft();");
}
//---------------------------------------------------------------------------
void TMainGrid::DeleteCells_Up(long Left, long Right, long Top, long Bottom,
        bool UpdateHeight)
{
  UndoList->Push();
  TStringList *Temp = new TStringList;
  for (int i = Left; i <= min(Right, ColCount - 1); i++) {
    Temp->Assign(Cols[i]);
    for (int j = Top; j <= min(Bottom, Temp->Count - 1) ; j++) {
      Temp->Delete(Top);
      Temp->Add("");
      UndoList->ChangeCell(RXtoAX(i), RYtoAY(j), Cells[i][j], "",
                           RXtoAX(DataRight), RYtoAY(DataBottom));
    }
    Cols[i]->Assign(Temp);
  }
  delete Temp;
  if (UpdateHeight) {
    int delta = Bottom - Top + 1;
    for (int j = Top; j < RowCount - delta; j++) {
      RowHeights[j] = RowHeights[j + delta];
    }
  }
  String select = (String)"Select(" + RXtoAX(Left) + ", " + RYtoAY(Top) + ", "
      + RXtoAX(Right) + ", " + RYtoAY(Bottom) + ");\n";
  UndoList->AddMacro(select + "InsertCellDown();", "");
  UndoList->Pop(select + "DeleteCellUp();");
}
//---------------------------------------------------------------------------
//  選択
//
void TMainGrid::SetSelection(long Left, long Right, long Top, long Bottom)
{
  if(Left   < FixedCols) Left   = FixedCols;
  if(Right  < FixedCols) Right  = FixedCols;
  if(Top    < FixedRows) Top    = FixedRows;
  if(Bottom < FixedRows) Bottom = FixedRows;

  if (Left >= ColCount || Right >= ColCount) {
    ChangeColCount(max(Left, Right) + 2);
  }
  if (Top >= RowCount || Bottom >= RowCount) {
    ChangeRowCount(max(Top, Bottom) + 2);
  }

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
    Options >> goEditing >> goAlwaysShowEditor;
    if (Left == Right && Top == Bottom) {
      Row = Top;
      Col = Left;
    }
    Selection = theRect;
  }
}
//---------------------------------------------------------------------------
//  セル移動時に全体再描画
//
bool __fastcall TMainGrid::SelectCell(int ACol, int ARow)
{
  if(CurrentRowBgColor != Color || CurrentColBgColor != Color){
    Invalidate();
  }
  return TStringGrid::SelectCell(ACol,ARow);
}
//---------------------------------------------------------------------------
//  ドラッグイベントの処理
//
void __fastcall TMainGrid::MouseDown(Controls::TMouseButton Button,
    Classes::TShiftState Shift, int X, int Y)
{
  MouseToCell(X, Y, MouseDownCol, MouseDownRow);

  if (!EditorMode && Col == MouseDownCol && Row == MouseDownRow &&
      !RangeSelect) {
    SameCellClicking = true;
  }
  EditorMode = false;
  Options >> goAlwaysShowEditor >> goEditing;
  if (DragBehavior == dbMove ||
      (DragBehavior == dbMoveIfSelected &&
       ((Selection.Left == MouseDownCol && Selection.Right == MouseDownCol &&
         Selection.Top <= FixedRows && Selection.Bottom >= FDataBottom) ||
        (Selection.Top == MouseDownRow && Selection.Bottom == MouseDownRow &&
         Selection.Left <= FixedCols && Selection.Right >= FDataRight)))) {
    Options << goRowMoving << goColMoving;
  } else {
    Options >> goRowMoving >> goColMoving;
  }
  Dragging = true;
  TStringGrid::MouseDown(Button, Shift, X, Y);

  if (MouseDownRow < FixedRows && MouseDownCol >= FixedCols) {
    int XLeft4, XRight4, YTemp;
    MouseToCell(X - 4, Y, XLeft4, YTemp);
    MouseToCell(X + 4, Y, XRight4, YTemp);
    ColToResize = XLeft4;
    OldWidthHeight = ColWidths[XLeft4];
    MouseDownColBorder = (XLeft4 != MouseDownCol || XRight4 != MouseDownCol);
    if (MouseDownColBorder) {
      return;
    }
  } else {
    MouseDownColBorder = false;
  }

  if (MouseDownCol < FixedCols && MouseDownRow >= FixedRows) {
    int YTop4, YBottom4, XTemp;
    MouseToCell(X, Y - 4, XTemp, YTop4);
    MouseToCell(X, Y + 4, XTemp, YBottom4);
    RowToResize = YTop4;
    OldWidthHeight = RowHeights[YTop4];
    MouseDownRowBorder = (YTop4 != MouseDownRow || YBottom4 != MouseDownRow);
    if (MouseDownRowBorder) {
      return;
    }
  }else{
    MouseDownRowBorder = false;
  }

  if (Button == mbRight &&
      (IsRowSelected(MouseDownRow) || IsColSelected(MouseDownCol))) {
    return;
  }
  if (MouseDownCol >= 0 && MouseDownRow >= 0) {
    if (MouseDownCol < FixedCols && MouseDownRow < FixedRows) {
      SelectAll();
    } else if (MouseDownCol < FixedCols) {
      if (Shift.Contains(ssShift)) {
        SelectRows(MouseDownRow, Row);
        MouseDownRow = Row;
      } else {
        SelectRow(MouseDownRow);
      }
    } else if (MouseDownRow < FixedRows) {
      if (Shift.Contains(ssShift)) {
        SelectCols(MouseDownCol, Col);
        MouseDownCol = Col;
      } else {
        SelectColumn(MouseDownCol);
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::MouseMove(Classes::TShiftState Shift, int X, int Y)
{
  int ACol, ARow;
  MouseToCell(X,Y,ACol,ARow);

  if (Dragging && !MouseDownColBorder && !MouseDownRowBorder &&
      ((MouseDownRow >= 0 && MouseDownRow < FixedRows &&
        !Options.Contains(goRowMoving)) ||
       (MouseDownCol >= 0 && MouseDownCol < FixedCols &&
        !Options.Contains(goColMoving)))) {
    if (MouseDownCol < FixedCols && MouseDownRow < FixedRows) {
      SelectAll();
    } else if (MouseDownCol < FixedCols) {
      if (ARow < 0) {
        ARow = TopRow + VisibleRowCount - 1;
      } else if (ARow < TopRow) {
        if (TopRow > FixedRows) {
          TopRow--;
        }
        ARow = TopRow;
      } else if (ARow >= TopRow + VisibleRowCount) {
        if (TopRow + VisibleRowCount < RowCount) {
          TopRow++;
        }
        ARow = TopRow + VisibleRowCount - 1;
      }
      SelectRows(ARow, MouseDownRow);
    } else if(MouseDownRow < FixedRows) {
      if (ACol < 0) {
        ACol = LeftCol + VisibleColCount - 1;
      } else if (ACol < LeftCol) {
        if (LeftCol > FixedCols) {
          LeftCol--;
        }
        ACol = LeftCol;
      } else if (ACol >= LeftCol + VisibleColCount) {
        if (LeftCol + VisibleColCount < ColCount) {
          LeftCol++;
        }
        ACol = LeftCol + VisibleColCount - 1;
      }
      SelectCols(ACol, MouseDownCol);
    }
  }

  if(ARow >= 0 && ACol >= 0) {
    TRect MaxRect(0, 0, ColWidths[ACol] - 4, ClientHeight);
    TRect R = DrawTextRect(Canvas, MaxRect, Cells[ACol][ARow], WordWrap, true);
    if(ShowToolTipForLongCell &&
       (R.Right > ColWidths[ACol] || R.Bottom > RowHeights[ARow])){
      String cell = Cells[ACol][ARow];
      String data = cell + "|[" + ACol + "," + ARow + "]=" + cell;
      if(Hint != data){
        Application->CancelHint();
      }
      Hint = data;
      ShowHint = true;
    }else if(FileOpenThread){
      Hint = L"ファイルを読み込み中です。";
      ShowHint = true;
    }else if(GetRunningMacroCount() > 0){
      Hint = L"マクロを実行中です。";
      ShowHint = true;
    }else{
      Application->CancelHint();
      Hint = "";
      ShowHint = false;
      ParentShowHint = true;
    }

    if(DblClickOpenURL == 2){
      if(ACol >= FixedCols && ARow >= FixedRows && isUrl(Cells[ACol][ARow])){
        Cursor = crHandPoint;
      }else if(FileOpenThread || GetRunningMacroCount() > 0){
        Cursor = crAppStart;
      }else{
        Cursor = crDefault;
      }
    }
  }

  TStringGrid::MouseMove(Shift,X,Y);
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::MouseUp(Controls::TMouseButton Button,
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
      Selection.Top == FixedRows && Selection.Bottom == FDataBottom){
      double scale = (double) ColWidths[ColToResize] / (double) OldWidthHeight;
      for(int i=Selection.Left; i<=Selection.Right; i++){
        if(i != ColToResize){
          int newWidth = ColWidths[i] * scale;
          if(newWidth >= ClientWidth - ColWidths[0]){ //広すぎないように
            newWidth = ClientWidth - ColWidths[0] - 2 * GridLineWidth;
          }
          if(newWidth < 16){ newWidth = 16; } //狭すぎないように
          ColWidths[i] = newWidth;
        }
      }
    }
  }else if(MouseDownRowBorder){
    if(Selection.Top <= RowToResize && Selection.Bottom >= RowToResize &&
       Selection.Left == FixedCols && Selection.Right == FDataRight){
      double scale = (double) RowHeights[RowToResize] / (double) OldWidthHeight;
      int maxRowHeight = ((ClientHeight - RowHeights[0]) / 4 - 1);
      for (int j = Selection.Top; j <= Selection.Bottom; j++) {
        if (j != RowToResize) {
          int newHeight = RowHeights[j] * scale;
          if (newHeight > maxRowHeight) {
            newHeight = maxRowHeight;
          }
          if (newHeight < 16){
            newHeight = 16;
          }
          RowHeights[j] = newHeight;
        }
      }
    }
  }

  UpdateDataRightBottom(0,0);

  if ((ShowColCounter && Options.Contains(goColMoving)) ||
      (ShowRowCounter && Options.Contains(goRowMoving))) {
    // 行・列を移動すると番号が狂う
    Invalidate();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::MoveRow(int FromIndex, int ToIndex)
{
  TStringGrid::MoveRow(FromIndex,ToIndex);
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::RowMoved(int FromIndex, int ToIndex)
{
  UndoList->AddMacro(
      (String)"MoveRow(" + RYtoAY(ToIndex) + ", " + RYtoAY(FromIndex) + ");",
      (String)"MoveRow(" + RYtoAY(FromIndex) + ", " + RYtoAY(ToIndex) + ");");
  TStringGrid::RowMoved(FromIndex,ToIndex);
  Modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::MoveColumn(int FromIndex, int ToIndex)
{
  TStringGrid::MoveColumn(FromIndex,ToIndex);
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::ColumnMoved(int FromIndex, int ToIndex)
{
  UndoList->AddMacro(
      (String)"MoveCol(" + RXtoAX(ToIndex) + ", " + RXtoAX(FromIndex) + ");",
      (String)"MoveCol(" + RXtoAX(FromIndex) + ", " + RXtoAX(ToIndex) + ");");
  TStringGrid::ColumnMoved(FromIndex,ToIndex);
  Modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::DblClick(void)
{
  if(MouseDownColBorder){
    if(Selection.Left <= ColToResize && Selection.Right >= ColToResize &&
       Selection.Top == FixedRows && Selection.Bottom == FDataBottom){
      for(int i=Selection.Left; i<=Selection.Right; i++){
        SetWidth(i);
      }
    }else{
      SetWidth(ColToResize);
    }
  }else if(MouseDownRowBorder){
    if(Selection.Top <= RowToResize && Selection.Bottom >= RowToResize &&
       Selection.Left == FixedCols && Selection.Right == FDataRight){
      for(int j=Selection.Top; j<=Selection.Bottom; j++){
        SetHeight(j, false);
      }
    }else{
      SetHeight(RowToResize, false);
    }
  }else if(Selection.Top == FixedRows &&
     Selection.Bottom == FDataBottom &&
     Selection.Left == Selection.Right) {
    if(ColWidths[Selection.Left] > 16) {
      ColWidths[Selection.Left] = 16;
    }else{
      SetWidth(Selection.Left);
    }
  }else if(Selection.Left == FixedCols &&
           Selection.Right == FDataRight &&
           Selection.Top == Selection.Bottom) {
    SetHeight(Selection.Top, false);
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
void TMainGrid::OpenURL(String FileName)
{
  if (BrowserFileName == "") {
    AutoOpen(FileName, "");
  } else {
    _wspawnl(P_NOWAITO, BrowserFileName.c_str(),
        ((String)"\"" + BrowserFileName + "\"").c_str(),
        ((String)"\"" + FileName + "\"").c_str(), nullptr);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::MouseWheelUp(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled)
{
  if (Shift.Contains(ssCtrl)) {
    Font->Size++;
    Canvas->Font->Size = Font->Size;
    UpdateDefaultRowHeight();
  } else if (Shift.Contains(ssShift)) {
    ScrollCols(-WheelScrollStep);
  } else {
    ScrollRows(-WheelScrollStep);
  }
  Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::MouseWheelDown(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled)
{
  if (Shift.Contains(ssCtrl)) {
    if (Font->Size > 1) {
      Font->Size--;
      Canvas->Font->Size = Font->Size;
      UpdateDefaultRowHeight();
    }
  } else if (Shift.Contains(ssShift)) {
    ScrollCols(WheelScrollStep);
  } else {
    ScrollRows(WheelScrollStep);
  }
  Handled = true;
}
//---------------------------------------------------------------------------
void TMainGrid::ScrollRows(int Delta)
{
  if (WheelMoveCursol == 1) {
    int r = Row + Delta;
    if (r < FixedRows) { r = FixedRows; }
    if (r > RowCount - 1) { r = RowCount - 1; }
    Row = r;
  } else if (WheelMoveCursol == 2) {
    int tr = TopRow + Delta;
    if (tr < FixedRows) { tr = FixedRows; }
    if (tr > RowCount - VisibleRowCount) { tr = RowCount - VisibleRowCount; }
    int r = Row + (tr - TopRow);
    TopRow = tr;
    Row = r;
  } else {
    int tr = TopRow + Delta;
    if (tr < FixedRows) { tr = FixedRows; }
    if (tr > RowCount - VisibleRowCount) { tr = RowCount - VisibleRowCount; }
    TopRow = tr;
  }
}
//---------------------------------------------------------------------------
void TMainGrid::ScrollCols(int Delta)
{
  if (WheelMoveCursol == 1) {
    int c = Col + Delta;
    if (c < FixedCols) { c = FixedCols; }
    if (c > ColCount - 1) { c = ColCount - 1; }
    Col = c;
  } else if (WheelMoveCursol == 2) {
    int lc = LeftCol + Delta;
    if (lc < FixedCols) { lc = FixedCols; }
    if (lc > ColCount - VisibleColCount) { lc = ColCount - VisibleColCount; }
    int c = Col + (lc - LeftCol);
    LeftCol = lc;
    Col = c;
  } else {
    int lc = LeftCol + Delta;
    if (lc < FixedCols) { lc = FixedCols; }
    if (lc > ColCount - VisibleColCount) { lc = ColCount - VisibleColCount; }
    LeftCol = lc;
  }
}
//---------------------------------------------------------------------------
//  検索
//
inline static String AddCr(String str) {
  return StringReplace(str, "\n", "\r\n", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------
inline static String StripCr(String str) {
  return StringReplace(str, "\r\n", "\n", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------
static void UpdateMatch(const boost::wcmatch& From, TStrings *To, bool AddsCr)
{
  To->Clear();
  for (size_t i = 0; i < From.size(); i++) {
    String str = From[i].str().c_str();
    To->Add(AddsCr ? AddCr(str) : str);
  }
}
//---------------------------------------------------------------------------
static int FindHit(String CellText, String FindText, bool Case, bool Regex,
    bool Word, bool Back, int *Length, TStrings *LastMatch)
{
  if (!Regex) {
    *Length = FindText.Length();
    if(!Case){
      CellText = CellText.LowerCase();
      FindText = FindText.LowerCase();
    }
    if (Word) {
      return (CellText == FindText ? 1 : 0);
    }
    if (Back) {
      for (int i = CellText.Length() - FindText.Length() + 1; i > 0; i--){
        if (CellText.SubString(i, FindText.Length()) == FindText) {
          return i;
        }
      }
      return 0;
    }
    return CellText.Pos(FindText);
  }
  bool hasCr = CellText.Pos("\r\n") > 0;
  String withoutCr = hasCr ? StripCr(CellText) : CellText;
  boost::wregex regex = Case ? boost::wregex(FindText.c_str())
      : boost::wregex(FindText.c_str(), boost::regex::icase);
  if (Word) {
    boost::wcmatch match;
    if (boost::regex_match(withoutCr.c_str(), match, regex)) {
      UpdateMatch(match, LastMatch, hasCr);
      *Length = CellText.Length();
      return 1;
    }
    return 0;
  }
  const wchar_t *next = withoutCr.c_str();
  if(Back) {
    while(true){
      boost::wcmatch match;
      if (boost::regex_search(next, match, regex, boost::match_not_null)) {
        UpdateMatch(match, LastMatch, hasCr);
        next = match[0].first + 1;
      } else {
        break;
      }
    }
  } else {
    boost::wcmatch match;
    if (boost::regex_search(next, match, regex, boost::match_not_null)) {
      UpdateMatch(match, LastMatch, hasCr);
      next = match[0].first + 1;
    }
  }
  if (next > withoutCr.c_str()) {
    *Length = LastMatch->Strings[0].Length();
    return AddCr(withoutCr.SubString(1, next - withoutCr.c_str() - 1)).Length()
        + 1;
  }
  return 0;
}
//---------------------------------------------------------------------------
static bool FindHit(String CellText, int x, int y)
{
  if (CellText == "") {
    return false;
  }
  TGridRect range = fmFind->GetRange();
  if (x < range.Left || x > range.Right || y < range.Top || y > range.Bottom) {
    return false;
  }
  String findText;
  if (fmFind->Visible) {
    if (fmFind->Visible && fmFind->PageControl->TabIndex == 0) {
      findText = fmFind->edFindText->Text;
    } else {
      return fmFind->HitNum(CellText);
    }
  } else if (fmMain->pnlSearch->Visible) {
    findText = fmMain->edFindText->Text;
  } else {
    return false;
  }
  if (findText == "") {
    return false;
  }
  bool found;
  int length;
  TStrings *lastMatch = new TStringList();
  try {
    found = FindHit(CellText, findText, fmFind->Case(), fmFind->Regex(),
        fmFind->Word(), false, &length, lastMatch) > 0;
  } catch (...) {
    found = false;
  }
  delete lastMatch;
  return found;
}
//---------------------------------------------------------------------------
void ShowRegexErrorMessage(const boost::regex_error& e) {
  Application->MessageBox(
     ((String)L"正規表現検索中にエラーが発生しました。\n" +
         L"正規表現検索オプションを解除するか検索文字列を修正してください。\n" +
         e.what()).c_str(),
     CASSAVA_TITLE, 0);
}
//---------------------------------------------------------------------------
bool TMainGrid::Find(String FindText, TGridRect Range, bool Case, bool Regex,
                     bool Word, bool Back)
{
  LastMatch->Clear();

  if (FindText == "") {
    return false;
  }

  bool isRangeSelected = RangeSelect;
  if (isRangeSelected) {
    TGridRect s = Selection;
    if (Back) {
      Col = s.Right;
      Row = s.Bottom;
      s.Left = s.Right;
      s.Top = s.Bottom;
    } else {
      Col = s.Left;
      Row = s.Top;
      s.Right = s.Left;
      s.Bottom = s.Top;
    }
    Selection = s;
  }

  Options << goEditing << goAlwaysShowEditor;
  ShowEditor();
  TInplaceEdit *ipEd = InplaceEditor;
  if (!ipEd) {
    return false;
  }

  if (isRangeSelected) {
    ipEd->SelLength = 0;
    if (Back) {
      ipEd->SelStart = ipEd->Text.Length();
    } else {
      ipEd->SelStart = 0;
    }
  }

  String InCellText = "";
  if (Word) {
    if ((!Back && ipEd->SelStart == 0 && ipEd->SelLength == 0) ||
        (Back && ipEd->SelStart == ipEd->Text.Length())) {
      InCellText = ipEd->Text;
    }
  } else {
    if (Back) {
      InCellText = ipEd->Text.SubString(1, ipEd->SelStart);
    } else {
      InCellText = ipEd->Text.SubString(ipEd->SelStart + ipEd->SelLength + 1,
                                        ipEd->Text.Length());
    }
    if (Regex && InCellText != ipEd->Text) {
      if ((!Back && FindText[1] == '^') ||
          (Back && *(FindText.LastChar()) == '$')) {
        InCellText = "";
      }
    }
  }

  int Hit, Len;
  if (InCellText != "") {
    try {
      Hit = FindHit(
          InCellText, FindText, Case, Regex, Word, Back, &Len, LastMatch);
    } catch (const boost::regex_error& e) {
      ShowRegexErrorMessage(e);
      return false;
    }
    if (Hit > 0) {
      if (Back) {
        ipEd->SelStart = Hit - 1;
      } else {
        ipEd->SelStart += ipEd->SelLength + Hit - 1;
      }
      ipEd->SelLength = Len;
      ipEd->SetFocus();
      return true;
    }
  }

  int step = (Back ? -1 : 1);
  int YStart = Row;
  int YEnd = (Back ? Range.Top : Range.Bottom);
  for (int y = YStart; y * step <= YEnd * step; y += step) {
    int XStart = (y == Row ? Col + step : (Back ? Range.Right : Range.Left));
    int XEnd = (Back ? Range.Left : Range.Right);
    for (int x = XStart; x * step <= XEnd * step; x += step) {
      try {
        Hit = FindHit(
            Cells[x][y], FindText, Case, Regex, Word, Back, &Len, LastMatch);
      } catch (const boost::regex_error& e) {
        ShowRegexErrorMessage(e);
        return false;
      }
      if (Hit > 0) {
        if (x >= FixedCols) {
          Row = y;
          Col = x;
          ipEd->SelStart = Hit - 1;
          ipEd->SelLength = Len;
        } else {
          Row = y;
          Col = FixedCols;
          EditorMode = true;
          ipEd->SelStart = 0;
          ipEd->SelLength = 0;
        }
        ipEd->SetFocus();
        return true;
      }
    }
  }
  return false;
}
//---------------------------------------------------------------------------
static String GetStringForReplace(
    String ReplaceText, bool Regex, TStrings *Match) {
  if (!Regex) {
    return ReplaceText;
  }
  for (int i = 1; i < ReplaceText.Length(); i++) {
    TCHAR nextChar = ReplaceText[i + 1];
    if (ReplaceText[i] == '$' && nextChar >= '0' && nextChar <= '9') {
      int group = nextChar - '0';
      if (Match->Count > group) {
        ReplaceText.Delete(i, 2);
        String str = Match->Strings[group];
        ReplaceText.Insert(str, i);
        i += str.Length() - 1;
      }
    }
    if (ReplaceText[i] == '\\') {
      if (nextChar == 'n') {
        ReplaceText[i] = '\r';
        ReplaceText[i+1] = '\n';
      } else if (nextChar == 't') {
        ReplaceText[i] = '\t';
        ReplaceText.Delete(i + 1, 1);
      } else if(nextChar == '\\' || nextChar == '$') {
        ReplaceText.Delete(i, 1);
      }
    }
  }
  return ReplaceText;
}
//---------------------------------------------------------------------------
bool TMainGrid::Replace(String FindText , String ReplaceText, TGridRect Range,
                        bool Case, bool Regex, bool Word, bool Back)
{
  TInplaceEdit *ipEd = InplaceEditor;
  if ((Regex || ReplaceText != FindText) && ipEd) {
    ipEd->SetFocus();
    if (Word) {
      ipEd->SelectAll();
    }
    if (Regex) {
      if (LastMatch->Count > 0 && ipEd->SelText == LastMatch->Strings[0]) {
        ipEd->SelText = GetStringForReplace(ReplaceText, Regex, LastMatch);
      }
    } else {
      if ((Case && ipEd->SelText == FindText) ||
          (!Case && ipEd->SelText.LowerCase() == FindText.LowerCase())) {
        ipEd->SelText = ReplaceText;
        Modified = true;
      }
    }
  }
  return Find(FindText, Range, Case, Regex, Word, Back);
}
//---------------------------------------------------------------------------
int TMainGrid::ReplaceAll(String FindText, String ReplaceText, int SLeft,
    int STop, int SRight, int SBottom, bool Case, bool Regex, bool Word)
{
  UndoList->Push();
  int count = 0;
  for (int y = STop; y <= SBottom; y++) {
    for (int x = SLeft; x <= SRight; x++) {
      String replacedText = AddCr(ReplaceAll(
          StripCr(Cells[x][y]), StripCr(FindText), StripCr(ReplaceText),
          Case, Regex, Word));
      if (replacedText != Cells[x][y]) {
        SetCell(x, y, replacedText);
        count++;
      }
    }
  }
  String range = "";
  if (SLeft != FixedCols || STop != FixedRows || SRight != DataRight ||
      SBottom != DataBottom) {
    range = (String)", " + RXtoAX(SLeft) + ", " + RYtoAY(STop) + ", " +
        ((SLeft == FixedCols && SRight == DataRight)
            ? (String)"Right" : (String)RXtoAX(SRight)) + ", " +
        ((STop == FixedRows && SBottom == DataBottom)
            ? (String)"Bottom" : (String)RYtoAY(SBottom));
  }
  UndoList->Pop((String)"ReplaceAll(\"" + EscapeMacroString(FindText) +
      "\", \"" + EscapeMacroString(ReplaceText) + "\", " + boolToStr(!Case) +
      ", " + boolToStr(Word) + ", " + boolToStr(Regex) + range + ");");
  if (count > 0) {
    Modified = true;
  }
  return count;
}
//---------------------------------------------------------------------------
String TMainGrid::ReplaceAll(String OriginalText, String FindText,
    String ReplaceText, bool Case, bool Regex, bool Word)
{
  if (Regex && !Word) {
    bool hasCr = OriginalText.Pos("\r\n") > 0;
    std::wstring withoutCr =
        (hasCr ? StripCr(OriginalText) : OriginalText).c_str();
    boost::wregex regex =
        Case ? boost::wregex(FindText.c_str())
             : boost::wregex(FindText.c_str(), boost::regex::icase);
    String result = boost::regex_replace(
        withoutCr, regex, (std::wstring) ReplaceText.c_str(),
        boost::format_perl | boost::match_single_line | boost::match_not_null)
        .c_str();
    return hasCr ? AddCr(result) : result;
  }

  String replacedText = "";
  while (true) {
    int len;
    int hit = FindHit(OriginalText, FindText, Case, Regex, Word, false, &len,
                      LastMatch);
    if (!hit) {
      return replacedText + OriginalText;
    }
    if (len == 0) {
      if (Word) {
        return GetStringForReplace(ReplaceText, Regex, LastMatch);
      }
      return replacedText + OriginalText;
    }
    replacedText += OriginalText.SubString(1, hit - 1)
        + GetStringForReplace(ReplaceText, Regex, LastMatch);
    OriginalText = OriginalText.SubString(hit + len, OriginalText.Length());
  }
}
//---------------------------------------------------------------------------
bool TMainGrid::NumFind(double *Min, double *Max, TGridRect Range, bool Back)
{
  SetFocus();
  int step = (Back ? -1 : 1);
  int YStart = Row;
  int YEnd = (Back ? Range.Top : Range.Bottom);

  for (int y = YStart; y * step <= YEnd * step; y += step) {
    int XStart = (Back ? Range.Right : Range.Left);
    int XEnd   = (Back ? Range.Left : Range.Right);
    if (y == Row) {
      TInplaceEdit *ipEd = InplaceEditor;
      if (ipEd && (ipEd->SelStart + ipEd->SelLength == 0)) {
        XStart = Col;
      } else {
        XStart = Col + step;
      }
    }

    for (int x = XStart; x * step <= XEnd * step; x += step) {
      try {
        double Val = Cells[x][y].ToDouble();
        if ((Min == nullptr || Val >= *Min) &&
            (Max == nullptr || Val <= *Max)) {
          if (x >= FixedCols) {
            Row = y; Col = x;
          } else {
            Row = y; Col = FixedCols;
          }
          TInplaceEdit *ipEd = InplaceEditor;
          if (ipEd) {
            ipEd->SelectAll();
          }
          return true;
        }
      } catch (...) {}
    }
  }
  return false;
}
//---------------------------------------------------------------------------
//  キー入力の監視
//
void __fastcall TMainGrid::KeyDown(Word &Key, Classes::TShiftState Shift)
{
  if(Key == VK_DELETE && EditorMode == false)
  {
    UndoList->Push();
    for (int i = SelTop; i <= Selection.Bottom; i++) {
      for (int j = SelLeft; j <= Selection.Right; j++) {
        SetCell(j, i, "");
      }
    }
    UndoList->Pop();
    Modified = true;
  }else if(Key == VK_DELETE){
    Modified = true;
  }else if(Key == VK_RIGHT || Key == VK_LEFT){
    if(Shift.Contains(ssShift)){
      EditorMode = false;
      Options >> goAlwaysShowEditor;
    }else if(AlwaysShowEditor){
      Options << goEditing << goAlwaysShowEditor;
      ShowEditor();
    }
  }else if(Key == VK_UP || Key == VK_DOWN){
    if(!Shift.Contains(ssShift) && AlwaysShowEditor){
      Options << goEditing << goAlwaysShowEditor;
      ShowEditor();
    }
  }else if(Key == VK_RETURN && EditorMode == false){
    Options << goEditing << goAlwaysShowEditor;
    ShowEditor();
    InplaceEditor->SelLength = 0;
    InplaceEditor->SelStart = InplaceEditor->Text.Length();
  }

  if ((Key == VK_RIGHT || Key == VK_LEFT || Key == VK_UP || Key == VK_DOWN)
      && Shift.Contains(ssCtrl)) {
    if (Shift.Contains(ssShift)) {
      EditorMode = false;
      Options >> goAlwaysShowEditor;
    }
    TGridRect s = Selection;
    if (Key == VK_LEFT || Key == VK_RIGHT) {
      int x = Col;
      int step = (Key == VK_LEFT ? -1 : 1);
      int max = (step < 0 ? -FixedCols : FDataRight);
      if (x * step < max) {
        x += step;
      }
      bool hasData = HasDataInRange(x, x, s.Top, s.Bottom);
      while (x * step < max &&
             HasDataInRange(x + step, x + step, s.Top, s.Bottom) == hasData) {
        x += step;
      }
      if (!hasData && x * step < max) {
        x += step;
      }
      FocusCell(x, Row, !Shift.Contains(ssShift));
    } else if (Key == VK_UP || Key == VK_DOWN) {
      int y = Row;
      int step = (Key == VK_UP ? -1 : 1);
      int max = (step < 0 ? -FixedRows : FDataBottom);
      if (y * step < max) {
        y += step;
      }
      bool hasData = HasDataInRange(s.Left, s.Right, y, y);
      while (y * step < max &&
             HasDataInRange(s.Left, s.Right, y + step, y + step) == hasData) {
        y += step;
      }
      if (!hasData && y * step < max) {
        y += step;
      }
      FocusCell(Col, y, !Shift.Contains(ssShift));
    }
    return;
  }
  TStringGrid::KeyDown(Key,Shift);
}
//---------------------------------------------------------------------------
bool TMainGrid::HasDataInRange(int Left, int Right, int Top, int Bottom)
{
  for (int y = Top; y <= Bottom; y++) {
    for (int x = Left; x <= Right; x++) {
      if (HasData(x, y)) {
        return true;
      }
    }
  }
  return false;
}
//---------------------------------------------------------------------------
bool TMainGrid::HasData(int X, int Y)
{
  return Cells[X][Y] != "" ||
      (TypeOption->DummyEol && Objects[X][Y] == EOLMarker);
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::KeyUp(Word &Key, Classes::TShiftState Shift)
{
  TStringGrid::KeyUp(Key,Shift);
  UpdateDataRightBottom(Col, Row);
}
//---------------------------------------------------------------------------
int calcMoveUp(String Text, int FromPos)
{
  int previousLineStart = -1;
  int currentLineStart = 0;
  for (int i = 1; i <= FromPos; i++) {
    if (Text[i] == '\n') {
      previousLineStart = currentLineStart;
      currentLineStart = i;
    }
  }
  if (previousLineStart < 0) {
    return -1;
  }
  int toPos = previousLineStart + (FromPos - currentLineStart);
  if (toPos >= currentLineStart) {
    toPos = currentLineStart - 1;
  }
  if (toPos < 0) {
    toPos = 0;
  } else if (toPos > 0 && Text[toPos] == '\r') {
    toPos--;
  }
  return toPos;
}
//---------------------------------------------------------------------------
int calcMoveDown(String Text, int FromPos)
{
  int currentLineStart = 0;
  for (int i = 1; i <= FromPos; i++) {
    if (Text[i] == '\n') {
      currentLineStart = i;
    }
  }
  int nextLineStart = -1;
  for (int i = FromPos + 1; i <= Text.Length(); i++) {
    if (Text[i] == '\n') {
      nextLineStart = i;
      break;
    }
  }
  if (nextLineStart < 0) {
    return -1;
  }
  int nextNextLineStart = Text.Length() + 1;
  for (int i = nextLineStart + 1; i <= Text.Length(); i++) {
    if (Text[i] == '\n') {
      nextNextLineStart = i;
      break;
    }
  }
  int toPos = nextLineStart + (FromPos - currentLineStart);
  if (toPos >= nextNextLineStart) {
    toPos = nextNextLineStart - 1;
  }
  if (toPos < Text.Length() && Text[toPos] == '\r') {
    toPos--;
  }
  return toPos;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::KeyDownSub(System::TObject* Sender,
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
  }
  else if(Key == VK_RETURN && EditorMode)
  {
    switch(EnterMove){
      case cssv_EnterDown:
        if(Row == RowCount-1){ ChangeRowCount(RowCount+1); }
        Row++;
        break;
      case cssv_EnterRight:
        if(Col == ColCount-1){ ChangeColCount(ColCount+1); }
        Col++;
        break;
      case cssv_EnterNextRow:
        if(Row == RowCount-1){ ChangeRowCount(RowCount+1); }
        Row++;
        Col = FixedCols;
        break;
      case cssv_EnterTabbedNextRow:
        if(Row == RowCount-1){ ChangeRowCount(RowCount+1); }
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
    if (!AlwaysShowEditor && EditorMode && InplaceEditor->SelStart == 0) {
      InplaceEditor->SelLength = 0;
      Key = '\0';
    } else if (InplaceEditor->SelLength == InplaceEditor->Text.Length() &&
        Col > FixedCols && !LeftArrowInCell){
      Col--; Key = '\0';
      TabStartCol = -1;
      TabStartRow = -1;
    }
  }else if(Key == VK_RIGHT && !Shift.Contains(ssShift)){
    if (!AlwaysShowEditor && EditorMode && InplaceEditor->SelStart +
        InplaceEditor->SelLength == InplaceEditor->Text.Length()) {
      InplaceEditor->SelLength = 0;
      InplaceEditor->SelStart = InplaceEditor->Text.Length();
      Key = '\0';
    } else if (InplaceEditor->SelLength == InplaceEditor->Text.Length() ||
        (InplaceEditor->SelStart == InplaceEditor->Text.Length() &&
         InplaceEditor->SelLength == 0)){
      if(Col == ColCount-1){ ChangeColCount(ColCount+1); }
      Col++; Key = '\0';
      TabStartCol = -1;
      TabStartRow = -1;
    }
  }else if(Key == VK_UP && !Shift.Contains(ssCtrl)){
    if (EditorMode) {
      int toPos = calcMoveUp(InplaceEditor->Text, InplaceEditor->SelStart);
      if (toPos < 0 && !AlwaysShowEditor) {
        toPos = 0;
      }
      if (toPos >= 0) {
        if (Shift.Contains(ssShift)) {
          InplaceEditor->SelStart =
              InplaceEditor->SelStart + InplaceEditor->SelLength;
          InplaceEditor->SelLength = toPos - InplaceEditor->SelStart;
        } else {
          InplaceEditor->SelLength = 0;
          InplaceEditor->SelStart = toPos;
        }
        Key = '\0';
      } else if (Shift.Contains(ssShift)) {
        EditorMode = false;
        Options >> goAlwaysShowEditor;
      }
    }
  }else if(Key == VK_DOWN && !Shift.Contains(ssCtrl)){
    if (EditorMode) {
      int toPos = calcMoveDown(InplaceEditor->Text,
          InplaceEditor->SelStart + InplaceEditor->SelLength);
      if (toPos < 0 && !AlwaysShowEditor) {
        toPos = InplaceEditor->Text.Length();
      }
      if (toPos >= 0) {
        if (Shift.Contains(ssShift)) {
          InplaceEditor->SelLength = toPos - InplaceEditor->SelStart;
        } else {
          InplaceEditor->SelLength = 0;
          InplaceEditor->SelStart = toPos;
        }
        Key = '\0';
        return;
      }
    }

    if (Shift.Contains(ssShift)) {
      EditorMode = false;
      Options >> goAlwaysShowEditor;
    } else {
      if(Row == RowCount-1){ ChangeRowCount(RowCount+1); }
      Row++; Key = '\0';
      TabStartCol = -1;
      TabStartRow = -1;
    }
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
  TCsvInplaceEdit(TMainGrid *G) : TInplaceEdit(G) {}

  void __fastcall BoundsChanged() {
    TInplaceEdit::BoundsChanged();
    TMainGrid *G = static_cast<TMainGrid*>(Owner);
    RECT R;
    R.left = G->LRMargin;
    R.top = G->TBMargin;
    R.right = Width - 2;
    R.bottom = Height;
    SendMessage(Handle, EM_SETRECT, 0, reinterpret_cast<LPARAM>(&R));
  }

  void __fastcall WmCopy(TWMCopy inMsg) {
    static_cast<TMainGrid*>(Owner)->CopyToClipboard();
  }

  void __fastcall WmCut(TWMCut inMsg) {
    static_cast<TMainGrid*>(Owner)->CutToClipboard();
  }

  void __fastcall WmPaste(TWMPaste inMsg) {
    static_cast<TMainGrid*>(Owner)->PasteFromClipboard(PASTE_OPTION_UNKNOWN);
  }

  BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_COPY, TWMCopy, WmCopy)
    MESSAGE_HANDLER(WM_CUT, TWMCut, WmCut)
    MESSAGE_HANDLER(WM_PASTE, TWMPaste, WmPaste)
  END_MESSAGE_MAP(TInplaceEdit)
};
//---------------------------------------------------------------------------
TInplaceEdit* __fastcall TMainGrid::CreateEditor()
{
  TInplaceEdit *editor = new TCsvInplaceEdit(this);
  editor->Parent = this;
  editor->Brush->Color = Color;
  return editor;
}
//---------------------------------------------------------------------------
void __fastcall TMainGrid::SetEditText(int ACol, int ARow, String Value)
{
  String old = Cells[ACol][ARow];
  int oldRowRight = GetRowDataRight(ARow);
  if(Value != old){
    Modified = true;
    UndoList->ChangeCell(RXtoAX(Col), RYtoAY(Row), old, Value,
                         RXtoAX(DataRight), RYtoAY(DataBottom));
    UpdateDataRightBottom(Col, Row);
  }
  TStringGrid::SetEditText(ACol, ARow, Value);
  if (Value == "" && old != "") {
    SetRowDataRight(ARow, ACol, /* ExpandOnly= */ true);
  }
  if (GetRowDataRight(ARow) != oldRowRight) {
    Invalidate();
  }
}
//---------------------------------------------------------------------------
//  Undo 処理
//
static void MacroScriptExec(String script)
{
  String cmsname = "$undo";
  TStringList *modules = new TStringList;
  TStringList *inPaths = new TStringList;
  bool ok = MacroCompile(&script, cmsname, inPaths, modules, true);
  if (ok) {
    ExecMacro(cmsname, 0, modules, -1, -1);
  }
  for (int i = modules->Count-1; i >= 0; i--) {
    if (modules->Objects[i]) { delete modules->Objects[i]; }
  }
  delete modules;
  delete inPaths;
}
//---------------------------------------------------------------------------
void TMainGrid::Undo()
{
  TUndoCommand* command = UndoList->Undo();
  if (command) {
    UndoList->Lock();
    if (command->type == UNDO_SET_CELL) {
      int x = AXtoRX(command->x);
      int y = AYtoRY(command->y);
      Cells[x][y] = command->undoData;
      SetSelection(x, x, y, y);
      SetDataRightBottom(AXtoRX(command->right), AYtoRY(command->bottom), true);
    } else if (command->type == UNDO_MACRO) {
      MacroScriptExec(command->undoData);
      if (EditorMode) {
        InplaceEditor->Text = Cells[Col][Row];
      }
    }
    Modified = true;
    UndoList->Unlock();
  }
}
//---------------------------------------------------------------------------
void TMainGrid::Redo()
{
  TUndoCommand* command = UndoList->Redo();
  if(command) {
    UndoList->Lock();
    if (command->type == UNDO_SET_CELL) {
      int x = AXtoRX(command->x);
      int y = AYtoRY(command->y);
      if (x >= ColCount) {
        ChangeColCount(x + 1);
      }
      if (y >= RowCount) {
        ChangeRowCount(y + 1);
      }
      Cells[x][y] = command->redoData;
      SetSelection(x, x, y, y);
    } else if (command->type == UNDO_MACRO) {
      MacroScriptExec(command->redoData);
      if (EditorMode) {
        InplaceEditor->Text = Cells[Col][Row];
      }
    }
    Modified = true;
    UndoList->Unlock();
  }
  UpdateDataRightBottom(0,0);
}
//---------------------------------------------------------------------------
