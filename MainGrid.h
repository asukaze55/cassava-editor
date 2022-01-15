//---------------------------------------------------------------------------
#ifndef MainGridH
#define MainGridH
//---------------------------------------------------------------------------
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Grids.hpp>
#include "EncodedWriter.h"
#include "TypeList.h"
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TDropCsvFiles)
    (System::TObject *Sender, int iFiles, System::String *DropFileNames);
//---------------------------------------------------------------------------
class TMainGrid : public TStringGrid
{
private:
    int drgRow , drgCol;
    int ColToResize;
    int RowToResize;
    int OldWidthHeight;
    bool MouseDownColBorder;
    bool MouseDownRowBorder;
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
    bool IsRowSelected(int ARow) {
      return (ARow>=Selection.Top && ARow<=Selection.Bottom &&
          FixedCols>=Selection.Left && ColCount-1<=Selection.Right);
    }
    bool IsColSelected(int ACol) {
      return (FixedRows>=Selection.Top && RowCount-1<=Selection.Bottom &&
          ACol>=Selection.Left && ACol<=Selection.Right);
    }
    int GetSelLeft() {
      if(Selection.Right-Selection.Left+1 == ColCount-FixedCols)
      { return (ShowRowCounter ? 1 : 0); }
      return (Selection.Left);
    }
    int GetSelTop() {
      if(Selection.Bottom-Selection.Top+1 == RowCount-FixedRows)
      { return (ShowColCounter ? 1 : 0); }
      return (Selection.Top);
    }

    int GetDataLeft(){ return (ShowRowCounter ? 1 : 0); }
    int GetDataTop() { return (ShowColCounter ? 1 : 0); }
    int FDataRight, FDataBottom;
    TObject *EOFMarker;

    int FTBMargin;
    void SetTBMargin(int Value);

	String GetACells(int ACol, int ARow);
    void SetACells(int ACol, int ARow, String Val);

    double SelectSum(int *Count);
    bool FDragMove;
    void SetDragMove(bool Value) {
      FDragMove = Value;
      if(Value) Options << goRowMoving << goColMoving;
      else      Options >> goRowMoving >> goColMoving;
    }
    bool FShowRowCounter, FShowColCounter;
    void SetShowRowCounter(bool Value);
    void SetShowColCounter(bool Value);

    bool FExecCellMacro;
    TStringList *CalculatedCellCache;
    TStringList *UsingCellMacro;
    void SetExecCellMacro(bool Value);
    void ClearCalcCache();
    void ErrorCalcLoop();

    TStrings *UndoMacro;
	String *UndoCsv;
	int UndoCsvWidth, UndoCsvHeight;
	TStrings *RedoMacro;
    String *RedoCsv;
    int RedoCsvWidth, RedoCsvHeight;
    void SetUndoMacro(bool inRedo=false);
    void SetRedoMacro();
    void ClearUndo();

    TDropCsvFiles FOnDropFiles;
    bool FDragDropAccept;
    void __fastcall SetDragDropAccept(bool Accept);

	int TextWidth(TCanvas *cnvs, String str);
    TRect DrawTextRect(TCanvas *Canvas, TRect Rect, String Str,
                       bool Wrap, bool MeasureOnly=false);

    void (__closure *OnFileOpenThreadTerminate)(System::TObject* Sender);

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
    DYNAMIC void __fastcall RowMoved(int FromIndex, int ToIndex);
    DYNAMIC void __fastcall ColumnMoved(int FromIndex, int ToIndex);
    void __fastcall DrawCell(int ACol, int ARow,
        const TRect &ARect, TGridDrawState AState);
    void InsertCells_Right(long Left, long Right, long Top, long Bottom);
    void InsertCells_Down(long Left, long Right, long Top, long Bottom);
    void DeleteCells_Left(long Left, long Right, long Top, long Bottom,
        bool UpdateWidth=false);
    void DeleteCells_Up(long Left, long Right, long Top, long Bottom,
        bool UpdateHeight=false);

    void __fastcall DropCsvFiles(TWMDropFiles inMsg);
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, DropCsvFiles)
    END_MESSAGE_MAP(TStringGrid)

__published:
    __property InplaceEditor;
    __property DefaultColWidth = {default=64};
    __property DefaultRowHeight = {default=24};
    __property TNotifyEvent OnChangeModified
      = {read=FOnChangeModified, write=FOnChangeModified};
    __property TDropCsvFiles OnDropFiles = {read=FOnDropFiles,write=FOnDropFiles};

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
    __property bool DragDrogAccept = {read=FDragDropAccept,
                                      write=SetDragDropAccept};

    __property int TBMargin = {read=FTBMargin, write=SetTBMargin};
    int LRMargin;
    int CellLineMargin;
    void UpdateDefaultRowHeight();

    __property String ACells[int ACol][int ARow]
      = {read=GetACells, write=SetACells};

    __fastcall TMainGrid(TComponent* Owner);
    __fastcall ~TMainGrid();

    void Clear(int AColCount=4, int ARowCount=4, bool UpdateRightBottom=false);
    void SetWidth(int i);
    void SetWidth();
    void SetHeight(int i);
    void SetHeight();
    void CompactWidth(int *Widths, int WindowSize, int Minimum,
                      TCanvas *Cnvs = NULL);
    void ShowAllColumn();
    void Cut();
    void UpdateDataRight();
    void UpdateDataBottom();
    void UpdateDataRightBottom(int modx, int mody, bool updateTableSize=true);
    void UpdateEOFMarker(int oldRight, int oldBottom);
    void SetDataRightBottom(int rx, int by, bool updateTableSize=true);
    void SaveToFile(String FileName, TTypeOption *Format, bool SetModifiedFalse=true);
    void CopyToClipboard(bool Cut = false);
    void CutToClipboard();
    void PasteFromClipboard();
    bool SetCsv(TStringList *Dest, String Src);
    void QuotedDataToStrings(TStrings *Lines, String Text, TTypeOption *Format);
    TTypeList TypeList;
    int TypeIndex;
    TTypeOption *TypeOption;
    int PasteOption;
    int DefWay;
    bool CheckKanji;
    int KanjiCode;
#define LFCR TEXT('\0')
    wchar_t ReturnCode;
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

    int DefaultViewMode;
#define cssv_viewallcolumn 0
#define cssv_viewalltext   1
    int NumberComma;
    int DecimalDigits;
    int MinColWidth;
    bool CalcWidthForAllRow;
    bool ShowToolTipForLongCell;

    __property bool ExecCellMacro
        = {read=FExecCellMacro, write=SetExecCellMacro};

    bool IsNumber(String Str);
    bool IsNumberAtACell(int X, int Y){ return IsNumber(GetACells(X,Y)); };
    void Sort(int SLeft, int STop, int SRight, int SBottom, int SCol,
              bool Shoujun, bool NumSort, bool IgnoreCase, bool IgnoreZenhan);
    double GetSum(int l, int t, int r, int b, int *Count=NULL);
    double GetAvr(int l, int t, int r, int b);
    void CopySum();
    void CopyAvr();

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

#define CALC_NOTEXPR '\x01'
#define CALC_OK '\x02'
#define CALC_NG '\x04'
#define CALC_LOOP '\x0c'
	String GetCalculatedCell(int ACol, int ARow);
    String (__closure *OnGetCalculatedCell)(String Str, int ACol, int ARow);

    void SelectRow(long Index){
      SetSelection(ColCount-1, FixedCols, Index, Index);
    };
    void SelectColumn(long Index){
      SetSelection(Index, Index, RowCount-1, FixedRows);
    };
    void SelectRows(long Top, long Bottom){
      SetSelection(ColCount-1, FixedCols, Top, Bottom);
    };
    void SelectCols(long Left, long Right){
      SetSelection(Left, Right, RowCount-1, FixedRows);
    };
    void SelectAll(){
      SetSelection(ColCount-1, FixedCols, RowCount-1, FixedRows);
    };

	bool Find(String FindText,
			int Range, bool Case, bool Regex, bool Word, bool Back);
	bool Replace(String FindText , String ReplaceText,
			int Range, bool Case, bool Regex, bool Word, bool Back);
    void AllReplace(String FindText , String ReplaceText,
            int Range, bool Case, bool Regex, bool Word, bool Back);
    bool NumFind(double *Min, double *Max, int Range, bool Back);

    __property bool DragMove = {read=FDragMove, write=SetDragMove};
    __property bool ShowRowCounter
                 = {read=FShowRowCounter, write=SetShowRowCounter};
    __property bool ShowColCounter
                 = {read=FShowColCounter, write=SetShowColCounter};

    void SetUndoCsv(bool inRedo=false);
    bool CanUndo(){return(UndoMacro || UndoCsv);};
    int UndoSetLock;
    void Undo();

    void SetRedoCsv();
    bool CanRedo(){return(RedoMacro || RedoCsv);};
    void Redo();

    void __fastcall KeyDownSub(System::TObject* Sender,
                          Word &Key, Classes::TShiftState Shift);
    void __fastcall MouseWheelUp(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled);
    void __fastcall MouseWheelDown(System::TObject* Sender,
      Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled);

    bool LoadFromFile(String FileName, int KCode=CHARCODE_AUTO,
      void (__closure *OnTerminate)(System::TObject* Sender)=NULL);
    TThread *FileOpenThread;
    void __fastcall FileOpenThreadTerminate(System::TObject* Sender);

    TColor UrlColor;
    TColor FixFgColor;
    TColor CurrentRowBgColor;
    TColor CurrentColBgColor;
    TColor DummyBgColor;
    TColor CalcFgColor;
    TColor CalcBgColor;
    TColor CalcErrorFgColor;
    TColor CalcErrorBgColor;
};
//---------------------------------------------------------------------------
#endif

