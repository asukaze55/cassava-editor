//---------------------------------------------------------------------------
#ifndef CsvGridH
#define CsvGridH
//---------------------------------------------------------------------------
#include <vcl\SysUtils.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Grids.hpp>
#include "TypeOption.h"
#include "Nkf.h"
//---------------------------------------------------------------------------
typedef struct { double Num; void *Data; int Row; } DoubleData;

int __fastcall CompareDoubleData(void *a, void *b) {
  DoubleData *dda = static_cast<DoubleData*>(a);
  DoubleData *ddb = static_cast<DoubleData*>(b);
  if(dda->Num == ddb->Num){
    if(dda->Row == ddb->Row) return 0;
    return ((dda->Row < ddb->Row) ? -1 : 1);
  }else return ((dda->Num < ddb->Num) ? -1 : 1);
}
//---------------------------------------------------------------------------
typedef struct { AnsiString Str; void *Data; int Row; } StringData;

int __fastcall CompareOrderedString(void *a, void *b) {
  StringData *osa = static_cast<StringData*>(a);
  StringData *osb = static_cast<StringData*>(b);
  if(osa->Str == osb->Str){
    if(osa->Row == osb->Row) return 0;
    return ((osa->Row < osb->Row) ? -1 : 1);
  }else return ((osa->Str < osb->Str) ? -1 : 1);
}
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TDropCsvFiles)
    (System::TObject *Sender, int iFiles, System::AnsiString *DropFileNames);
//---------------------------------------------------------------------------
class TCsvGrid : public TStringGrid
{
private:
    int drgRow , drgCol;
    bool Dragging;
    int ColToResize;
    bool MouseDownCellBorder;
    bool GetRangeSelect() {
      return(Selection.Top  != Selection.Bottom
	  || Selection.Left != Selection.Right);
    };
    TNotifyEvent FOnChangeModified;
    bool FModified;
    void SetModified(bool Value) {
      bool Before = FModified;
      FModified = Value;
      if(FModified != Before && OnChangeModified) OnChangeModified(this);
    };
    bool GetSelected(int ACol,int ARow) {
      return (ARow>=Selection.Top  && ARow<=Selection.Bottom &&
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
    int GetDataRight();
    int GetDataBottom();

    AnsiString GetACells(int ACol, int ARow);
    void SetACells(int ACol, int ARow, AnsiString Val){
      Cells[ACol+DataLeft-1][ARow+DataTop-1] = Val;
    }

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

    TStrings *UndoMacro;
    AnsiString *UndoCsv;
    int UndoCsvWidth, UndoCsvHeight;
    TStrings *RedoMacro;
    AnsiString *RedoCsv;             
    int RedoCsvWidth, RedoCsvHeight;
    void SetUndoMacro(bool inRedo=false);
    void SetRedoMacro();
    void ClearUndo();

    void SaveToNotSjisFile(AnsiString FileName, char *sjis, int length);
    TDropCsvFiles FOnDropFiles;
    bool FDragDropAccept;
    void __fastcall SetDragDropAccept(bool Accept);

    //int CommaCount, TabCount;

    int TextWidth(TCanvas *cnvs, AnsiString str);

protected:
    void PasteCSV(TStrings *List , int Left=1 , int Top=1 , int Way=2 ,
		  int ClipCols=0 , int ClipRows=0);
    AnsiString StringsToCSV(TStrings* Data, TTypeOption *Format);
    void GridToCSV(TStrings *Lines, TTypeOption *Format);
    void GridToHtml(TStrings *Lines);
    DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button,
    	Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button,
	    Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall DblClick(void);
    DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);
    DYNAMIC void __fastcall KeyPress(char &Key);
    DYNAMIC void __fastcall RowMoved(int FromIndex, int ToIndex);
    DYNAMIC void __fastcall ColumnMoved(int FromIndex, int ToIndex);
    void __fastcall DrawCell(int ACol, int ARow,
      const TRect &ARect, TGridDrawState AState);
    void InsertCells_Right(long Left, long Right, long Top, long Bottom);
    void InsertCells_Down(long Left, long Right, long Top, long Bottom);
    void DeleteCells_Left(long Left, long Right, long Top, long Bottom);
    void DeleteCells_Up(long Left, long Right, long Top, long Bottom);

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
    __property bool RangeSelect = {read=GetRangeSelect};
    __property bool Modified = {read=FModified, write=SetModified};
    void __fastcall ShowEditor(){
      TStringGrid::ShowEditor();
      InplaceEditor->Brush->Color = Color;
    }
    __property bool Selected[int ACol][int ARow] = {read=GetSelected};
    void SetSelection(long Left, long Right, long Top, long Bottom);
    __property int SelLeft = {read=GetSelLeft};
    __property int SelTop  = {read=GetSelTop};

    __property int DataLeft   = {read=GetDataLeft};
    __property int DataTop    = {read=GetDataTop};
    __property int DataRight  = {read=GetDataRight};
    __property int DataBottom = {read=GetDataBottom};
    __property bool DragDrogAccept = {read=FDragDropAccept,
                                      write=SetDragDropAccept};

    __property AnsiString ACells[int ACol][int ARow]
                            = {read=GetACells, write=SetACells};
    __fastcall TCsvGrid(TComponent* Owner);
    __fastcall ~TCsvGrid(){ ClearUndo(); };

    void Clear(int AColCount=4, int ARowCount=4);
    void ReNum();
    void SetWidth(int i);
    void SetWidth();
    void SetHeight(int i);
    void SetHeight();
    void CompactWidth(int *Widths, int WindowSize, int Minimum,
                      TCanvas *Cnvs = NULL);
    void ShowAllColumn();
    void Cut();
    void SpreadRightBottom();
    bool LoadFromFile(AnsiString FileName, int KanjiCode=CHARCODE_AUTO);
    void SaveToFile(AnsiString FileName, TTypeOption *Format, bool html=false);
    void CopyToClipboard(bool Cut = false);
    void CutToClipboard();
    void PasteFromClipboard();
    bool SetCsv(TStringList *Dest, AnsiString Src);
    void QuotedDataToStrings(TStrings *Lines, AnsiString Text, TTypeOption *Format);
    TTypeList TypeList;
    int TypeIndex;
    TTypeOption *TypeOption;
//    AnsiString SepChars;
//    AnsiString WeakSepChars;
//    AnsiString QuoteChars;
    int PasteOption;
    int DefWay;
//    int SaveOption;
//    bool OmitComma;
    bool CheckKanji;
    int KanjiCode;
    #define LFCR '\0'
    char ReturnCode;
//    #define sfCSV 1
//    #define sfHTML 2
//    #define sfTSV 3
//    int SaveFormat;
//    bool CommaSeparated, TabSeparated;
    bool LeftArrowInCell;
    #define cssv_taLeft 0
    #define cssv_taNumRight 1
    int TextAlignment;
    bool AlwaysShowEditor;
    bool WheelMoveCursol;
    int EnterMove;
    #define cssv_EnterNone 0
    #define cssv_EnterDown 1
    #define cssv_EnterRight 2

    int DblClickOpenURL;
    bool ShowURLBlue;
    AnsiString BrowserFileName;
    void OpenURL(AnsiString FileName);

    int  DefaultViewMode;
    #define cssv_viewallcolumn 0
    #define cssv_viewalltext   1
    int NumberComma;

    bool IsNumber(AnsiString Str);
    bool IsNumberAt(int X, int Y){ return IsNumber(ACells[X][Y]); };
    void Sort(int SLeft, int STop, int SRight, int SBottom,
              int SCol, bool Shoujun, bool NumSort);
    void CopySum();
    void CopyAvr();

    void ChangeRowCount(int Count);
    void ChangeColCount(int Count);
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
    void TransKana(int Type);
    void Sequence(bool Inc);

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

    bool Find(AnsiString FindText, int Range, bool Asterisk, bool Word, bool Back);
    bool Replace(AnsiString FindText , AnsiString ReplaceText,
                 int Range, bool Asterisk, bool Word, bool Back);
    void AllReplace(AnsiString FindText , AnsiString ReplaceText,
                 int Range, bool Asterisk, bool Word, bool Back);
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

};
//---------------------------------------------------------------------------
#endif

