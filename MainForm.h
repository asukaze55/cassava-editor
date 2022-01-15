//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Grids.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Dialogs.hpp>

#include "MainGrid.h"
#include "Preference.h"
#include "CSPIN.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <ActnList.hpp>
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <System.ImageList.hpp>
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
  TMenuItem *mnCharCode;
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
  TMenuItem *mnRedo;
  TMenuItem *mnpSort;
  TTimer *tmAutoSaver;
  TMenuItem *mnMacro;
  TMenuItem *mnN15;
  TMenuItem *mnExport;
  TMenuItem *mnPasteFormat;
  TMenuItem *mnPasteFormatDefault;
  TMenuItem *mnpPasteFormat;
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
  TMenuItem *mnPasteOption5;
  TMenuItem *mnNewLine;
  TMenuItem *mnReloadCodeUTF16BE;
  TImageList *imlMenu;
  TPopupMenu *PopMenuOpen;
  TMenuItem *mnFindBack;
  TMenuItem *mnCopyFormat;
  TMenuItem *mnCopyFormatDefault;
  TMenuItem *mnpCopyFormat;
  TMenuItem *mnCalcExpression;
  TMenuItem *mnN16;
  TMenuItem *mnMacroOpenUserFolder;
  TMenuItem *mnQuickFind;
  TStatusBar *StatusBar;
  TPanel *MainPanel;
  TPanel *pnlSearch;
  TSpeedButton *btnNext;
  TSpeedButton *btnPrevious;
  TLabel *Label1;
  TSpeedButton *btnSearchOption;
  TSpeedButton *btnSearchCancel;
  TEdit *edFindText;
  TMenuItem *mnShowStatusbar;
  TImageList *imlNormal;
    TControlBar *CoolBar;
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
    TToolButton *tsbFont;
    TToolButton *sbCalcExpression;
    TToolButton *ToolButton23;
    TToolButton *tsbFixUpLeft;
    TToolButton *tsbFixFirstRow;
    TToolButton *tsbFixFirstCol;
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
    TMenuItem *mnCheckUpdate;
    TMenuItem *mnMacroTerminate;
    TActionList *ActionList;
    TAction *acMacroTerminate;
    TAction *acSave;
    TAction *acUndo;
    TAction *acRedo;
    TAction *acCut;
    TAction *acCopy;
    TAction *acPaste;
  TMenuItem *mnpKugiri2;
  TMenuItem *mnCutFormat;
  TMenuItem *mnCutFormatDefault;
  TMenuItem *mnpCutFormat;
  TMenuItem *mnN10;
  void __fastcall mnNewClick(TObject *Sender);
  void __fastcall mnOpenClick(TObject *Sender);
  void __fastcall mnEndClick(TObject *Sender);
  void __fastcall mnSaveAsClick(TObject *Sender);
  void __fastcall mnInsRowClick(TObject *Sender);
  void __fastcall mnCutRowClick(TObject *Sender);
  void __fastcall mnInsColClick(TObject *Sender);
  void __fastcall mnCutColClick(TObject *Sender);
  void __fastcall mnSelectAllClick(TObject *Sender);
  void __fastcall mnRefreshClick(TObject *Sender);
  void __fastcall mnFontClick(TObject *Sender);
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
  void __fastcall mnSortClick(TObject *Sender);
  void __fastcall sbSortClick(TObject *Sender);
  void __fastcall mnKCodeClick(TObject *Sender);
  void __fastcall mnOpenHistorysClick(TObject *Sender);
  void __fastcall mnOpenHistoryClearClick(TObject *Sender);
  void __fastcall mnOptionDlgClick(TObject *Sender);
  void __fastcall mnPrintClick(TObject *Sender);
  void __fastcall mnTransCharClick(TObject *Sender);
  void __fastcall mnShowToolbarClick(TObject *Sender);
  void __fastcall mnAppliClick(TObject *Sender);
  void __fastcall mnSequenceClick(TObject *Sender);
  void __fastcall mnSequenceCClick(TObject *Sender);
  void __fastcall mnSequenceSClick(TObject *Sender);
  void __fastcall mnpSortClick(TObject *Sender);
  void __fastcall tmAutoSaverTimer(TObject *Sender);
  void __fastcall mnMacroExecClick(TObject *Sender);
  void __fastcall mnMacroUserExecClick(TObject *Sender);
  void __fastcall mnPasteFormatDefaultClick(TObject *Sender);
  void __fastcall mnMacroOpenFolderClick(TObject *Sender);
  void __fastcall mnMacroExecuteClick(TObject *Sender);
  void __fastcall mnStayOnTopClick(TObject *Sender);
  void __fastcall mnExportClick(TObject *Sender);
  void __fastcall mnTransKanaClick(TObject *Sender);
  void __fastcall mnReloadCodeClick(TObject *Sender);
  void __fastcall mnNewLineClick(TObject *Sender);
  void __fastcall PopMenuOpenPopup(TObject *Sender);
  void __fastcall mnFindBackClick(TObject *Sender);
  void __fastcall mnCopyFormatDefaultClick(TObject *Sender);
  void __fastcall mnCalcExpressionClick(TObject *Sender);
  bool __fastcall FormHelp(WORD Command, int Data, bool &CallHelp);
  void __fastcall mnMacroOpenUserFolderClick(TObject *Sender);
  void __fastcall mnQuickFindClick(TObject *Sender);
  void __fastcall btnSearchCancelClick(TObject *Sender);
  void __fastcall edFindTextKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall btnNextClick(TObject *Sender);
  void __fastcall btnPreviousClick(TObject *Sender);
  void __fastcall btnSearchOptionClick(TObject *Sender);
  void __fastcall mnShowStatusbarClick(TObject *Sender);
    void __fastcall CoolBarResize(TObject *Sender);
    void __fastcall mnCheckUpdateClick(TObject *Sender);
    void __fastcall acMacroTerminateExecute(TObject *Sender);
    void __fastcall acMacroTerminateUpdate(TObject *Sender);
    void __fastcall acSaveExecute(TObject *Sender);
    void __fastcall acUndoExecute(TObject *Sender);
    void __fastcall acRedoExecute(TObject *Sender);
    void __fastcall acUndoUpdate(TObject *Sender);
    void __fastcall acRedoUpdate(TObject *Sender);
    void __fastcall acCutExecute(TObject *Sender);
    void __fastcall acCopyExecute(TObject *Sender);
    void __fastcall acPasteExecute(TObject *Sender);
    void __fastcall acCutUpdate(TObject *Sender);
    void __fastcall acCopyUpdate(TObject *Sender);
    void __fastcall acPasteUpdate(TObject *Sender);
  void __fastcall mnCutFormatDefaultClick(TObject *Sender);
  void __fastcall mnMacroClick(TObject *Sender);
  void __fastcall mnCharCodeClick(TObject *Sender);
  void __fastcall mnReturnCodeClick(TObject *Sender);

private:    // ���[�U�[�錾
  void OpenFile(String OpenFileName, int KCode=CHARCODE_AUTO);
  bool StartupMacroDone;
  void ExecStartupMacro();
  void ExecOpenMacro(System::TObject* Sender);
  void SaveFile(TTypeOption *Format);
  void GetCheckedMenus(TStringList *list);
  void AddCheckedMenus(TStringList *list, TMenuItem* item);
  void RestoreCheckedMenus(TStringList *list);
  void RestoreCheckedMenus(TStringList *list, TMenuItem* item);
  void SearchMacro();


public:     // ���[�U�[�錾
  TMainGrid *MainGrid;
  __fastcall TfmMain(TComponent* Owner);
  __fastcall virtual ~TfmMain();
  bool IfModifiedThenSave();
  void ReadIni();
  void WriteIni(bool PosSlide = false);
  void ReadToolBar();
  void __fastcall UserToolBarAction(TObject *Sender);
  void __fastcall MainGridChangeModified(TObject *Sender);
  void __fastcall MainGridDropFiles(TObject *Sender, int iFiles,
									String *DropFileNames);
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

  String FileName;
  String FullPath;
  Preference *Pref;
  TDateTime TimeStamp;
  TStringList *History;
  void SetHistory(String S);
  void SetFilter();
  String MakeId(String prefix, String caption, int i);
  void SetCutMenu(TMenuItem *Item);
  void SetCopyMenu(TMenuItem *Item);
  void SetPasteMenu(TMenuItem *Item);
  void SetGridFont(TFont *AFont);

  void MacroExec(String CmsFile, EncodedWriter *io);
  void MacroScriptExec(String cmsname, String script);
  String GetCalculatedCell(String Str, int ACol, int ARow);
  String GetFormattedCell(int ACol, int ARow);
  void Export(String filename, String type);

  bool MakeNewWindow;
  bool TitleFullPath;

  bool BackupOnSave;
  bool BackupOnTime;
  bool BackupOnOpen;
  String BuFileNameS;
  String BuFileNameT;
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
  bool SortByNumber;
  bool SortIgnoreCase;
  bool SortIgnoreZenhan;

  // After fmFind loaded, Use fmFind->cbCase->Checked.
  bool FindCase;
  bool FindWordSearch;
  bool FindRegex;

  String PrintFontName;
  int PrintFontSize;
  int PrintMargin[4];

  TStringList *SystemMacroCache;
  String FormatCmsFile;
  String StatusbarCmsFile;
  void UpdateStatusbar();
};
//---------------------------------------------------------------------------
extern TfmMain *fmMain;
//---------------------------------------------------------------------------
#endif
