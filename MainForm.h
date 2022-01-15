//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Grids.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Dialogs.hpp>

#include "CsvGrid.h"
#include "Preference.h"
#include "CSPIN.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TfmMain : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
  TMainMenu *Menu;
  TMenuItem *mnFile;
  TMenuItem *mnOpen;
  TMenuItem *mnSave;
  TMenuItem *mnN2;
  TMenuItem *mnEnd;
  TMenuItem *mnNew;
  TBevel *bvlToolBar;
  TMenuItem *mnSaveAs;
  TOpenDialog *dlgOpen;
  TSaveDialog *dlgSave;
  TMenuItem *mnTable;
  TMenuItem *mnInsCol;
  TMenuItem *mnCutCol;
  TMenuItem *mnInsRow;
  TMenuItem *mnCutRow;
  TMenuItem *mnOption;
  TPopupMenu *PopMenu;
  TMenuItem *mnpCut;
  TMenuItem *mnpCopy;
  TMenuItem *mnpPaste;
  TMenuItem *mnpSelectAll;
  TFontDialog *dlgFont;
  TMenuItem *mnEdit;
  TMenuItem *mnCut;
  TMenuItem *mnCopy;
  TMenuItem *mnPaste;
  TMenuItem *mnSelectAll;
  TMenuItem *mnpKugiri;
  TMenuItem *mnEnter;
  TMenuItem *mnConnectCell;
  TMenuItem *mnInsertCell;
  TMenuItem *mnDeleteCell;
  TMenuItem *mnDeleteCellLeft;
  TMenuItem *mnDeleteCellUp;
  TMenuItem *mnN4;
  TMenuItem *mnInsertCellRight;
  TMenuItem *mnInsertCellDown;
  TMenuItem *mnN1;
  TMenuItem *mnLetterPrint;
  TMenuItem *mnSearch;
  TMenuItem *mnFind;
  TMenuItem *mnFindNext;
  TMenuItem *mnUndo;
  TMenuItem *mnN3;
  TMenuItem *mnHelp;
  TMenuItem *mnContents;
  TMenuItem *mnAbout;
  TMenuItem *mnpInsRow;
  TMenuItem *mnpInsCol;
  TMenuItem *mnpCutRow;
  TMenuItem *mnpCutCol;
  TMenuItem *mnOpenCellFile;
  TMenuItem *mnN6;
  TMenuItem *mnPasteOption;
  TMenuItem *mnPasteNormal;
  TMenuItem *mnN7;
  TMenuItem *mnPasteOption0;
  TMenuItem *mnPasteOption1;
  TMenuItem *mnPasteOption2;
  TMenuItem *mnPasteOption3;
  TMenuItem *mnPasteOption4;
  TMenuItem *mnN8;
  TMenuItem *mnCopySum;
  TMenuItem *mnCopyAvr;
  TMenuItem *mnN9;
  TMenuItem *mnSelectRow;
  TMenuItem *mnSelectCol;
  TMenuItem *mnpNarrow;
  TMenuItem *mnpDefWidth;
  TMenuItem *mnReload;
  TMenuItem *mnKey;
  TMenuItem *mnView;
  TMenuItem *mnRefresh;
  TMenuItem *mnFont;
  TMenuItem *mnN5;
  TMenuItem *mnFixFirstRow;
  TMenuItem *mnFixFirstCol;
  TMenuItem *mnFixUpLeft;
  TMenuItem *mnUnFix;
  TMenuItem *mnN12;
  TMenuItem *mnSort;
  TMenuItem *mnKanjiCode;
  TMenuItem *mnSjis;
  TMenuItem *mnEuc;
  TMenuItem *mnJis;
  TMenuItem *mnUnicode;
  TMenuItem *mnUtf8;
  TMenuItem *mnN13;
  TMenuItem *mnLfcr;
  TMenuItem *mnLf;
  TMenuItem *mnCr;
  TMenuItem *mnOpenHistory;
  TMenuItem *mnOpenHistory1;
  TMenuItem *mnOpenHistory2;
  TMenuItem *mnOpenHistory3;
  TMenuItem *mnOpenHistory4;
  TMenuItem *mnOpenHistory5;
  TMenuItem *N8;
  TMenuItem *mnOpenHistoryClear;
  TMenuItem *mnOpenHistory6;
  TMenuItem *mnOpenHistory7;
  TMenuItem *mnOpenHistory8;
  TMenuItem *mnOpenHistory9;
  TMenuItem *mnOpenHistory0;
  TMenuItem *mnUtf16be;
  TMenuItem *mnOptionDlg;
  TMenuItem *mnShowAllColumn;
  TMenuItem *mnPrint;
  TMenuItem *mnTransChar;
  TMenuItem *mnTransChar0;
  TMenuItem *mnTransChar1;
  TMenuItem *mnTransChar2;
  TMenuItem *mnTransChar3;
  TMenuItem *mnN14;
  TMenuItem *mnShowToolbar;
  TMenuItem *mnAppli;
  TMenuItem *mnAppli0;
  TMenuItem *mnAppli1;
  TMenuItem *mnAppli2;
  TMenuItem *mnSequence;
  TMenuItem *mnSequenceC;
  TMenuItem *mnSequenceS;
  TMenuItem *mnNewSize;
  TMenuItem *mnRedo;
  TMenuItem *mnpSort;
  TTimer *tmAutoSaver;
  TMenuItem *mnMacro;
  TMenuItem *mnN15;
  TMenuItem *mnNumAlignRight;
  TMenuItem *mnExport;
  TMenuItem *mnPasteFormat;
  TMenuItem *mnPasteFormatDefault;
  TMenuItem *mnpPasteFormat;
  TMenuItem *mnMacroSearch;
  TMenuItem *mnMacroOpenFolder;
  TMenuItem *mnMacroExecute;
  TMenuItem *N2;
  TOpenDialog *dlgOpenMacro;
  TMenuItem *mnStayOnTop;
  TMenuItem *mnTransChar4;
  TMenuItem *mnTransChar5;
  TMenuItem *N1;
  TMenuItem *N3;
  TMenuItem *mnReloadCode;
  TMenuItem *mnReloadCodeShiftJIS;
  TMenuItem *mnReloadCodeEUC;
  TMenuItem *mnReloadCodeJIS;
  TMenuItem *mnReloadCodeUnicode;
  TMenuItem *mnReloadCodeUTF8;
  TMenuItem *mnNum3;
  TMenuItem *mnPasteOption5;
  TMenuItem *mnNewLine;
  TMenuItem *mnReloadCodeUTF16BE;
  TCoolBar *CoolBar;
  TToolBar *tbarNormal;
  TToolButton *tsbNew;
  TToolButton *tsbOpen;
  TToolButton *tsbSave;
  TToolButton *ToolButton4;
  TToolButton *tsbCut;
  TToolButton *tsbCopy;
  TToolButton *tsbPaste;
  TToolButton *ToolButton8;
  TToolButton *tsbUndo;
  TToolButton *tsbRedo;
  TToolBar *tbarAdditional;
  TToolButton *tsbSort;
  TToolButton *ToolButton12;
  TToolButton *tsbInsRow;
  TToolButton *tsbInsCol;
  TToolButton *tsbCutRow;
  TToolButton *tsbCutCol;
  TToolButton *ToolButton17;
  TToolButton *tsbFind;
  TToolButton *ToolButton19;
  TToolButton *tsbRefresh;
  TToolButton *tsbShowAllColumn;
  TToolButton *tsbFont;
  TToolButton *ToolButton23;
  TToolButton *tsbFixUpLeft;
  TToolButton *tsbFixFirstRow;
  TToolButton *tsbFixFirstCol;
  TImageList *imlMenu;
  TPopupMenu *PopMenuOpen;
  TMenuItem *mnFindBack;
  TMenuItem *mnCopyFormat;
  TMenuItem *mnCopyFormatDefault;
  TMenuItem *mnpCopyFormat;
  TToolButton *sbCalcExpression;
  TMenuItem *mnWordWrap;
  TMenuItem *mnCalcExpression;
  TMenuItem *mnN16;
  TMenuItem *mnMacroOpenUserFolder;
  TMenuItem *mnQuickFind;
  TStatusBar *StatusBar;
  TPanel *MainPanel;
  TGroupBox *gbxSort;
  TLabel *Label4;
  TLabel *Label5;
  TButton *btnSort;
  TButton *btnSortCancel;
  TRadioGroup *rgSortDirection;
  TCheckBox *cbNumSort;
  TEdit *seSortLeft;
  TUpDown *udSortLeft;
  TUpDown *udSortTop;
  TEdit *seSortTop;
  TUpDown *udSortRight;
  TEdit *seSortRight;
  TUpDown *udSortBottom;
  TEdit *seSortBottom;
  TUpDown *udSortCol;
  TEdit *seSortCol;
  TPanel *pnlSearch;
  TSpeedButton *btnNext;
  TSpeedButton *btnPrevious;
  TLabel *Label1;
  TSpeedButton *btnSearchOption;
  TSpeedButton *btnSearchCancel;
  TEdit *edFindText;
  TMenuItem *mnShowStatusbar;
  TImageList *imlNormal;
  void __fastcall mnNewClick(TObject *Sender);
  void __fastcall mnOpenClick(TObject *Sender);
  void __fastcall mnEndClick(TObject *Sender);
  void __fastcall mnSaveClick(TObject *Sender);
  void __fastcall mnSaveAsClick(TObject *Sender);
  void __fastcall mnInsRowClick(TObject *Sender);
  void __fastcall mnCutRowClick(TObject *Sender);
  void __fastcall mnInsColClick(TObject *Sender);
  void __fastcall mnCutColClick(TObject *Sender);
  void __fastcall mnSelectAllClick(TObject *Sender);
  void __fastcall mnCopyClick(TObject *Sender);
  void __fastcall mnPasteClick(TObject *Sender);
  void __fastcall mnRefreshClick(TObject *Sender);
  void __fastcall mnFontClick(TObject *Sender);
  void __fastcall mnEditClick(TObject *Sender);
  void __fastcall mnCutClick(TObject *Sender);
  void __fastcall mnAboutClick(TObject *Sender);
  void __fastcall mnEnterClick(TObject *Sender);
  void __fastcall mnConnectCellClick(TObject *Sender);
  void __fastcall mnInsertCellRightClick(TObject *Sender);
  void __fastcall mnInsertCellDownClick(TObject *Sender);
  void __fastcall mnDeleteCellLeftClick(TObject *Sender);
  void __fastcall mnDeleteCellUpClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall mnLetterPrintClick(TObject *Sender);
  void __fastcall mnHelpContentsClick(TObject *Sender);
  void __fastcall mnFindClick(TObject *Sender);
  void __fastcall mnFindNextClick(TObject *Sender);
  void __fastcall sbFindClick(TObject *Sender);
  void __fastcall mnUndoClick(TObject *Sender);
  void __fastcall PopMenuPopup(TObject *Sender);
  void __fastcall mnOpenCellFileClick(TObject *Sender);
  void __fastcall mnPasteOptionsClick(TObject *Sender);
  void __fastcall mnCopySumClick(TObject *Sender);
  void __fastcall mnCopyAvrClick(TObject *Sender);
  void __fastcall mnSelectRowClick(TObject *Sender);
  void __fastcall mnSelectColClick(TObject *Sender);
  void __fastcall mnFixFirstRowClick(TObject *Sender);
  void __fastcall mnFixFirstColClick(TObject *Sender);
  void __fastcall mnpNarrowClick(TObject *Sender);
  void __fastcall mnpDefWidthClick(TObject *Sender);
  void __fastcall mnReloadClick(TObject *Sender);
  void __fastcall mnKeyClick(TObject *Sender);
  void __fastcall mnFixUpLeftClick(TObject *Sender);
  void __fastcall mnUnFixClick(TObject *Sender);
  void __fastcall btnSortCancelClick(TObject *Sender);
  void __fastcall mnSortClick(TObject *Sender);
  void __fastcall seSortKeyPress(TObject *Sender, char &Key);
  void __fastcall btnSortClick(TObject *Sender);
  void __fastcall seSortChange(TObject *Sender);
  void __fastcall sbSortClick(TObject *Sender);
  void __fastcall mnKCodeClick(TObject *Sender);
  void __fastcall mnOpenHistorysClick(TObject *Sender);
  void __fastcall mnOpenHistoryClearClick(TObject *Sender);
  void __fastcall mnOptionDlgClick(TObject *Sender);
  void __fastcall mnShowAllColumnClick(TObject *Sender);
  void __fastcall mnPrintClick(TObject *Sender);
  void __fastcall mnTransCharClick(TObject *Sender);
  void __fastcall mnShowToolbarClick(TObject *Sender);
  void __fastcall mnAppliClick(TObject *Sender);
  void __fastcall mnSequenceClick(TObject *Sender);
  void __fastcall mnSequenceCClick(TObject *Sender);
  void __fastcall mnSequenceSClick(TObject *Sender);
  void __fastcall mnNewSizeClick(TObject *Sender);
  void __fastcall mnRedoClick(TObject *Sender);
  void __fastcall mnpSortClick(TObject *Sender);
  void __fastcall tmAutoSaverTimer(TObject *Sender);
  void __fastcall mnMacroExecClick(TObject *Sender);
  void __fastcall mnMacroUserExecClick(TObject *Sender);
  void __fastcall mnNumAlignRightClick(TObject *Sender);
  void __fastcall mnPasteFormatDefaultClick(TObject *Sender);
  void __fastcall mnMacroSearchClick(TObject *Sender);
  void __fastcall mnMacroOpenFolderClick(TObject *Sender);
  void __fastcall mnMacroExecuteClick(TObject *Sender);
  void __fastcall mnStayOnTopClick(TObject *Sender);
  void __fastcall mnExportClick(TObject *Sender);
  void __fastcall mnTransKanaClick(TObject *Sender);
  void __fastcall mnReloadCodeClick(TObject *Sender);
  void __fastcall mnNum3Click(TObject *Sender);
  void __fastcall mnNewLineClick(TObject *Sender);
  void __fastcall PopMenuOpenPopup(TObject *Sender);
  void __fastcall mnFindBackClick(TObject *Sender);
  void __fastcall mnCopyFormatDefaultClick(TObject *Sender);
  void __fastcall mnWordWrapClick(TObject *Sender);
  void __fastcall mnCalcExpressionClick(TObject *Sender);
  bool __fastcall FormHelp(WORD Command, int Data, bool &CallHelp);
  void __fastcall udSortClick(TObject *Sender, TUDBtnType Button);
  void __fastcall mnMacroOpenUserFolderClick(TObject *Sender);
  void __fastcall mnQuickFindClick(TObject *Sender);
  void __fastcall btnSearchCancelClick(TObject *Sender);
  void __fastcall edFindTextKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall btnNextClick(TObject *Sender);
  void __fastcall btnPreviousClick(TObject *Sender);
  void __fastcall btnSearchOptionClick(TObject *Sender);
  void __fastcall mnShowStatusbarClick(TObject *Sender);

private:    // ���[�U�[�錾
  void UpdateKCode();
  void OpenFile(AnsiString OpenFileName, int KCode=CHARCODE_AUTO,
    void (__closure *OnTerminate)(System::TObject* Sender)=NULL);
  void ExecStartupMacro(System::TObject* Sender);
  void SaveFile(TTypeOption *Format);
  void GetCheckedMenus(TStringList *list);
  void AddCheckedMenus(TStringList *list, TMenuItem* item);
  void RestoreCheckedMenus(TStringList *list);
  void RestoreCheckedMenus(TStringList *list, TMenuItem* item);


public:     // ���[�U�[�錾
  TCsvGrid *MainGrid;
  __fastcall TfmMain(TComponent* Owner);
  __fastcall virtual ~TfmMain();
  bool IfModifiedThenSave();
  void ReadIni();
  void WriteIni(bool PosSlide = false);
  void __fastcall MainGridChangeModified(TObject *Sender);
  void __fastcall MainGridDropFiles(TObject *Sender, int iFiles,
                                    AnsiString *DropFileNames);
  void __fastcall MainGridKeyDown(System::TObject* Sender,
                        Word &Key, Classes::TShiftState Shift);
  void __fastcall MainGridKeyUp(System::TObject* Sender,
                        Word &Key, Classes::TShiftState Shift);
  void __fastcall MainGridMouseMove(System::TObject* Sender,
                        Classes::TShiftState Shift, int X, int Y);
  void __fastcall MainGridMouseUp(System::TObject* Sender,
                        Controls::TMouseButton Button,
                        Classes::TShiftState Shift, int X, int Y);
  void __fastcall ApplicationActivate(System::TObject* Sender);
  void __fastcall ApplicationHint(TObject *Sender);

  AnsiString FileName;
  AnsiString FullPath;
  Preference *Pref;
  int TimeStamp;
  TStringList *History;
  void SetHistory(AnsiString S);
  void SetFilter();
  AnsiString MakeId(AnsiString prefix, AnsiString caption, int i);
  void SetCopyMenu(TMenuItem *Item);
  void SetPasteMenu(TMenuItem *Item);
  void SetGridFont(TFont *AFont);

  void MacroExec(AnsiString CmsFile, TStream *io);
  AnsiString GetCalculatedCell(AnsiString Str, int ACol, int ARow);

  bool MakeNewWindow;
  bool TitleFullPath;

  bool BackupOnSave;
  bool BackupOnTime;
  bool BackupOnOpen;
  AnsiString BuFileNameS;
  AnsiString BuFileNameT;
  bool DelBuSSaved;
  bool DelBuSExit;
  bool DelBuT;
  int BuInterval;
  int LockFile;
#define cssv_lfNone 0
#define cssv_lfEdit 1
#define cssv_lfOpen 2
  TFileStream *LockingFile;
  bool CheckTimeStamp;
  int StopMacroCount;
  bool SortAll;

  AnsiString PrintFontName;
  int PrintFontSize;
  int PrintMargin[4];

  AnsiString StatusbarCmsFile;
  TStringList *StatusbarMacroCache;
  void UpdateStatusbar();
};
//---------------------------------------------------------------------------
extern TfmMain *fmMain;
//---------------------------------------------------------------------------
#endif
