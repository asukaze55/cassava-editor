//---------------------------------------------------------------------------
#ifndef MainGridH
#define MainGridH
//---------------------------------------------------------------------------
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <map>
#include "EncodedWriter.h"
#include "TypeList.h"
#include "Undo.h"
//---------------------------------------------------------------------------
#define CASSAVA_TITLE L"Cassava Editor"
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TDropCsvFiles)
    (System::TObject *Sender, int iFiles, System::String *DropFileNames);
//---------------------------------------------------------------------------
enum TReturnCode {
  CRLF, LF, CR
};
inline String ReturnCodeString(TReturnCode AReturnCode) {
  switch (AReturnCode) {
    case CRLF: return L"\r\n";
    case LF: return L"\n";
    case CR: return L"\r";
  }
}
//---------------------------------------------------------------------------
enum TDragBehavior {
  dbMove = 0,
  dbSelect = 1,
  dbMoveIfSelected = 2
};
//---------------------------------------------------------------------------
enum TCalcType {
  ctNotExpr, ctOk, ctError
};
//---------------------------------------------------------------------------
struct TCalculatedCell {
  String text;
  TCalcType calcType;

  TCalculatedCell() : text(""), calcType(ctNotExpr) {}
  TCalculatedCell(String t, TCalcType c) : text(t), calcType(c) {}
};
//---------------------------------------------------------------------------
struct TFormattedCell {
  String text;
  TColor fgColor;
  TColor bgColor;
  TAlignment alignment;

  TFormattedCell() {}
  TFormattedCell(String t, TColor f, TColor b, TAlignment a)
      : text(t), fgColor(f), bgColor(b), alignment(a) {}
};
//---------------------------------------------------------------------------
class TMainGrid : public TStringGrid
{
private:
    int MouseDownRow;
    int MouseDownCol;
    int ColToResize;
    int RowToResize;
    int OldWidthHeight;
    bool MouseDownColBorder;
    bool MouseDownRowBorder;
    bool SameCellClicking;
    bool GetRangeSelect() {
      return(Selection.Top  != Selection.Bottom
      || Selection.Left != Selection.Right);
    };
    TNotifyEvent FOnChangeModified;
    bool FModified;
    void SetModified(bool Value);
    bool GetSelected(int ACol,int ARow) {
      return (ARow>=Selection.Top  && ARow<=Selection.Bottom &&
          ACol>=Selection.Left && ACol<=Selection.Right);
    }
    int GetSelLeft() { return Selection.Left; }
    int GetSelTop() { return Selection.Top; }

    int GetDataLeft(){ return (ShowRowCounter ? 1 : 0); }
    int GetDataTop() { return (ShowColCounter ? 1 : 0); }
    int FDataRight, FDataBottom;
    TObject *EOFMarker;
    TObject *EOLMarker;
    void UpdateDataRight();
    void UpdateDataBottom();
    void UpdateDataRightBottom(int modx, int mody);
    void UpdateEOFMarker(int oldRight, int oldBottom);
    bool HasDataInRange(int Left, int Right, int Top, int Bottom);
    bool HasData(int X, int Y);

    int FTBMargin;
    void SetTBMargin(int Value);

    String GetACells(int ACol, int ARow);
    void SetACells(int ACol, int ARow, String Val);

    double SelectSum(int *Count);
    bool FShowRowCounter, FShowColCounter;
    void SetShowRowCounter(bool Value);
    void SetShowColCounter(bool Value);

    bool FExecCellMacro;
    std::map<String, TCalculatedCell> CalculatedCellCache;
    std::map<String, TFormattedCell> FormattedCellCache;
    std::map<String, String> UsingCellMacro;
    void SetExecCellMacro(bool Value);
    void ClearCalcCache();
    void ErrorCalcLoop();

    TUndoList *FUndoList;

    TDropCsvFiles FOnDropFiles;

    int TextWidth(TCanvas *cnvs, String str);
    void ShowAllColumn();

    void (__closure *OnFileOpenThreadTerminate)(System::TObject* Sender);

    TStrings *LastMatch;

protected:
    void PasteCSV(TStrings *List , int Left=1 , int Top=1 , int Way=2 ,
        int ClipCols=0 , int ClipRows=0);
    String StringsToCSV(TStrings* Data, TTypeOption *Format);
    void WriteGrid(EncodedWriter *Writer, TTypeOption *Format);

    virtual bool __fastcall SelectCell(int ACol, int ARow);
    DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button,
        Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button,
        Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall DblClick(void);
    DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);
    DYNAMIC void __fastcall KeyUp(Word &Key, Classes::TShiftState Shift);
    TInplaceEdit* __fastcall CreateEditor();
    DYNAMIC void __fastcall SetEditText(int ACol, int ARow, String Value);
    void __fastcall DrawCell(int ACol, int ARow,
        const TRect &ARect, TGridDrawState AState);
    void InsertCells_Right(long Left, long Right, long Top, long Bottom);
    void InsertCells_Down(long Left, long Right, long Top, long Bottom);
    void DeleteCells_Left(long Left, long Right, long Top, long Bottom,
        bool UpdateWidth=false);
    void DeleteCells_Up(long Left, long Right, long Top, long Bottom,
        bool UpdateHeight=false);
    DYNAMIC void __fastcall RowMoved(int FromIndex, int ToIndex);
    DYNAMIC void __fastcall ColumnMoved(int FromIndex, int ToIndex);

    void __fastcall DropCsvFiles(TWMDropFiles inMsg);
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, DropCsvFiles)
    END_MESSAGE_MAP(TStringGrid)

__published:
    __property InplaceEditor;
    __property DefaultColWidth = {default=64};
    __property DefaultRowHeight = {default=24};
    __property TNotifyEvent OnChangeModified =
        {read=FOnChangeModified, write=FOnChangeModified};
    __property TDropCsvFiles OnDropFiles =
        {read=FOnDropFiles, write=FOnDropFiles};

public:
    bool Dragging;

    __property bool RangeSelect = {read=GetRangeSelect};
    __property bool Modified = {read=FModified, write=SetModified};
    void __fastcall ShowEditor();
    __property bool Selected[int ACol][int ARow] = {read=GetSelected};
    void SetSelection(long Left, long Right, long Top, long Bottom);
    __property int SelLeft = {read=GetSelLeft};
    __property int SelTop  = {read=GetSelTop};

    __property int DataLeft   = {read=GetDataLeft};
    __property int DataTop    = {read=GetDataTop};
    __property int DataRight  = {read=FDataRight};
    __property int DataBottom = {read=FDataBottom};

    __property int TBMargin = {read=FTBMargin, write=SetTBMargin};
    int LRMargin;
    int CellLineMargin;
    void UpdateDefaultRowHeight();

    __property String ACells[int ACol][int ARow]
      = {read=GetACells, write=SetACells};

    __fastcall TMainGrid(TComponent* Owner);
    __fastcall ~TMainGrid();

    void SetDragAcceptFiles(bool Accept);
    void Clear(int AColCount=4, int ARowCount=4, bool UpdateRightBottom=false);
    void SetWidth(int i);
    void SetWidth();
    void SetHeight(int j, bool useMaxRowHeightLines);
    void SetHeight();
    void CompactWidth(int *Widths, int WindowSize, int Minimum,
                      TCanvas *Cnvs = nullptr);
    void Cut();
    void SetDataRightBottom(int rx, int by, bool updateTableSize=true);
    int GetRowDataRight(int Row);
    void SetRowDataRight(int Row, int Right, bool ExpandOnly = false);
    void SaveToFile(String FileName, TTypeOption *Format,
                    bool SetModifiedFals = true);
    void CopyToClipboard(bool Cut = false);
    void CutToClipboard();
#define PASTE_OPTION_UNKNOWN -1
#define PASTE_OPTION_OVERWRITE 2
#define PASTE_OPTION_EDITOR 5
#define PASTE_OPTION_INSERT_ROW 11
#define PASTE_OPTION_INSERT_COL 12
    void PasteFromClipboard(int Way);
    bool SetCsv(TStringList *Dest, String Src);
    void QuotedDataToStrings(TStrings *Lines, String Text, TTypeOption *Format);
    TTypeList TypeList;
    int TypeIndex;
    TTypeOption *TypeOption;
    int PasteOption;
    int DefWay;
    bool CheckKanji;
    int DefaultCharCode;
    int KanjiCode;
    bool AddBom;
    TReturnCode ReturnCode;
    TReturnCode InCellReturnCode;
    bool LeftArrowInCell;
#define cssv_taLeft 0
#define cssv_taNumRight 1
    int TextAlignment;
    bool AlwaysShowEditor;
    int WheelMoveCursol;
    int WheelScrollStep;
    int EnterMove;
#define cssv_EnterNone 0
#define cssv_EnterDown 1
#define cssv_EnterRight 2
#define cssv_EnterNextRow 3
#define cssv_EnterTabbedNextRow 4
    int TabStartCol;
    int TabStartRow;

    int DblClickOpenURL;
    bool ShowURLBlue;
    bool WordWrap;
	String BrowserFileName;
    void OpenURL(String FileName);

    int NumberComma;
    int DecimalDigits;
    int MinColWidth;
    bool CompactColWidth;
    bool CalcWidthForAllRow;
    double MaxRowHeightLines;
    bool ShowToolTipForLongCell;

    __property bool ExecCellMacro
        = {read=FExecCellMacro, write=SetExecCellMacro};

    bool IsNumber(String Str);
    bool IsNumberAtACell(int X, int Y){ return IsNumber(GetACells(X,Y)); };
    void Sort(int SLeft, int STop, int SRight, int SBottom, int SCol,
              bool Shoujun, bool NumSort, bool IgnoreCase, bool IgnoreZenhan);
    double GetSum(int l, int t, int r, int b, int *Count = nullptr);
    double GetAvr(int l, int t, int r, int b);
    void CopySum();
    void CopyAvr();

    void SetCell(int X, int Y, String Value);
    void ChangeRowCount(int Count);
    void ChangeColCount(int Count);
    void InsertRow(int Top, int Bottom);
    void InsertColumn(int Left, int Right);
    void DeleteRow(int Top, int Bottom);
    void DeleteColumn(int Left, int Right);
    void __fastcall InsertRow(int Index);
    void __fastcall InsertColumn(int Index);
    void __fastcall DeleteRow(int ARow);
    void __fastcall DeleteColumn(int ACol);
    void __fastcall MoveRow(int FromIndex, int ToIndex);
    void __fastcall MoveColumn(int FromIndex, int ToIndex);
    void InsertEnter();
    void InsertNewLine();
    void ConnectCell();
    void InsertCell_Right();
    void InsertCell_Down();
    void DeleteCell_Left();
    void DeleteCell_Up();
    void TransChar(int Type);
    String TransChar(String Str, int Type);
    void TransKana(int Type);
    String TransKana(String Str, int Type);
    void Sequence(bool Inc);

    TCalculatedCell GetCalculatedCell(int AX, int AY);
    TCalculatedCell (__closure *OnGetCalculatedCell)(
        String Str, int AX, int AY);

    TFormattedCell GetFormattedCell(TCalculatedCell Cell, int AX, int AY);
    TFormattedCell (__closure *OnGetFormattedCell)(
        TCalculatedCell Cell, int AX, int AY);

    TFormattedCell GetStyledCell(TCalculatedCell Cell, int AX, int AY);
    TFormattedCell GetCellToDraw(int RX, int RY);

    TRect DrawTextRect(TCanvas *Canvas, TRect Rect, String Str,
                       bool Wrap, bool MeasureOnly=false);

    void SelectRow(long Index){
      SetSelection(FixedCols, FDataRight, Index, Index);
    }
    void SelectColumn(long Index){
      SetSelection(Index, Index, FixedRows, FDataBottom);
    }
    void SelectRows(long Top, long Bottom){
      SetSelection(FixedCols, FDataRight, Top, Bottom);
    }
    void SelectCols(long Left, long Right){
      SetSelection(Left, Right, FixedRows, FDataBottom);
    }
    void SelectAll(){
      SetSelection(FixedCols, FDataRight, FixedRows, FDataBottom);
    }
    bool IsRowSelected() {
      return MouseDownCol < FixedCols;
    }
    bool IsRowSelected(int ARow) {
      return FixedCols >= Selection.Left && FDataRight <= Selection.Right &&
          ARow >= Selection.Top && ARow <= Selection.Bottom;
    }
    bool IsColSelected() {
      return MouseDownRow < FixedRows;
    }
    bool IsColSelected(int ACol) {
      return FixedRows >= Selection.Top && FDataBottom <= Selection.Bottom &&
          ACol >= Selection.Left && ACol <= Selection.Right;
    }

    bool Find(String FindText, TGridRect Range, bool Case, bool Regex,
              bool Word, bool Back);
    bool Replace(String FindText , String ReplaceText, TGridRect Range,
                 bool Case, bool Regex, bool Word, bool Back);
    int ReplaceAll(String FindText, String ReplaceText, int SLeft, int STop,
                   int SRight, int SBottom, bool Case, bool Regex, bool Word);
    String ReplaceAll(String OriginalText, String FindText, String ReplaceText,
                      bool Case, bool Regex, bool Word);
    bool NumFind(double *Min, double *Max, TGridRect Range, bool Back);

    TDragBehavior DragBehavior;
    __property bool ShowRowCounter
                 = {read=FShowRowCounter, write=SetShowRowCounter};
    __property bool ShowColCounter
                 = {read=FShowColCounter, write=SetShowColCounter};

    __property TUndoList *UndoList = {read=FUndoList};
    void SetUndoCsv(bool inRedo=false);
    bool CanUndo() {
      return UndoList->CanUndo();
    }
    void Undo();

    void SetRedoCsv();
    bool CanRedo(){
      return UndoList->CanRedo();
    }
    void Redo();

    void __fastcall KeyDownSub(System::TObject* Sender,
                          Word &Key, Classes::TShiftState Shift);
    void __fastcall MouseWheelUp(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled);
    void __fastcall MouseWheelDown(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled);

    bool LoadFromFile(String FileName, int KCode=CHARCODE_AUTO,
        void (__closure *OnTerminate)(System::TObject* Sender) = nullptr);
    TThread *FileOpenThread;
    void __fastcall FileOpenThreadTerminate(System::TObject* Sender);

    TColor CalcFgColor;
    TColor CalcBgColor;
    TColor CalcErrorFgColor;
    TColor CalcErrorBgColor;
    TColor CurrentRowBgColor;
    TColor CurrentColBgColor;
    TColor DummyBgColor;
    TColor EvenRowBgColor;
    TColor FixFgColor;
    TColor FoundBgColor;
    TColor UrlColor;
};
//---------------------------------------------------------------------------
#endif

