//---------------------------------------------------------------------------
#ifndef OptionH
#define OptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include "CSPIN.h"
//---------------------------------------------------------------------------
class TfmOption : public TForm
{
__published:	// IDE 管理のコンポーネント
        TPageControl *pcOption;
        TBitBtn *btnOK;
        TBitBtn *btnCancel;
        TColorDialog *dlgColor;
        TFontDialog *dlgFont;
        TTabSheet *tsLaunch;
        TTabSheet *tsOther;
        TButton *btnFont;
        TButton *btnFgColor;
        TButton *btnBgColor;
        TStaticText *stFont;
        TLabel *lbFixColor;
        TOpenDialog *dlgOpen;
        TTabSheet *tsBackUp;
        TCheckBox *cbBackupOnSave;
        TLabel *Label1;
        TEdit *edBuFileNameS;
        TCheckBox *cbBackupOnTime;
        TLabel *Label2;
        TLabel *Label3;
        TCSpinEdit *seBuInterval;
        TEdit *edBuFileNameT;
        TLabel *Label4;
        TCheckBox *cbDelBuS;
        TCheckBox *cbBackupOnOpen;
        TCheckBox *cbDelBuT;
        TLabel *Label5;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label6;
        TTabSheet *tsCsv;
        TGroupBox *gbKugiri;
        TLabel *Label10;
  TEdit *edDefSepChar;
        TLabel *Label11;
        TEdit *edSepChars;
        TLabel *Label12;
        TEdit *edWeakSepChars;
        TComboBox *cbType;
        TButton *btnNewType;
        TButton *btnDeleteType;
        TRadioGroup *rgSaveQuote;
        TCheckBox *cbCommaRect;
  TGroupBox *gbExt;
        TEdit *edExts;
        TEdit *edDefExt;
        TLabel *Label9;
        TLabel *Label13;
        TCheckBox *cbForceExt;
  TButton *btnRename;
  TButton *btnFixedColor;
        TCheckBox *cbDummyEOF;
        TGroupBox *gbAppli;
        TEdit *edLaunchName1;
        TEdit *edLaunchName0;
        TEdit *edLaunch0;
        TSpeedButton *sbRefer0;
        TSpeedButton *sbRefer1;
        TEdit *edLaunch1;
        TEdit *edLaunch2;
        TSpeedButton *sbRefer2;
        TEdit *edLaunchName2;
        TBitBtn *btnKanrenR;
        TGroupBox *gbBrowser;
        TSpeedButton *sbRefer3;
        TLabel *Label16;
        TEdit *edBrowser;
        TRadioGroup *rgUseURL;
        TTabSheet *tsAction;
        TLabel *Label14;
        TComboBox *cbDragMove;
        TComboBox *cbEnterMove;
        TLabel *Label15;
        TCheckBox *cbLeftArrowInCell;
        TCheckBox *cbWheelMoveCursol;
        TCheckBox *cbAlwaysShowEditor;
        TGroupBox *GroupBox1;
        TCheckBox *cbNewWindow;
        TCheckBox *cbTitleFullPath;
        TCheckBox *cbCheckTimeStamp;
        TCheckBox *cbJis2000;
        TGroupBox *GroupBox2;
        TCheckBox *cbStopMacro;
        TCSpinEdit *seStopMacroCount;
        TLabel *Label17;
        TGroupBox *gbKanren;
        TCheckBox *cbKanren;
        TCheckBox *cbKanrenNew;
        void __fastcall btnFgColorClick(TObject *Sender);
        void __fastcall btnFontClick(TObject *Sender);
        void __fastcall btnBgColorClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall btnOKClick(TObject *Sender);
        void __fastcall btnFixColorClick(TObject *Sender);
        void __fastcall edLaunchChange(TObject *Sender);
        void __fastcall sbReferClick(TObject *Sender);
        void __fastcall btnKanrenRClick(TObject *Sender);
        void __fastcall cbBackupOnSaveClick(TObject *Sender);
        void __fastcall cbBackupOnTimeClick(TObject *Sender);
  void __fastcall cbTypeChange(TObject *Sender);
  void __fastcall btnRenameClick(TObject *Sender);
  void __fastcall btnNewTypeClick(TObject *Sender);
  void __fastcall btnDeleteTypeClick(TObject *Sender);
  void __fastcall edDefSepCharChange(TObject *Sender);
        void __fastcall cbCommaRectClick(TObject *Sender);
        void __fastcall cbCommaRectKeyUpDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall cbCommaRectKeyPress(TObject *Sender, char &Key);
        void __fastcall cbStopMacroClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
        __fastcall TfmOption(TComponent* Owner);
        void SetKanren();
        bool Kanren;
        bool KanrenNew;
        AnsiString FileType;

        void StoreDataPage();
        void RestoreDataPage(int id);
        int TypeIndex;

        bool Renaming;
};
//---------------------------------------------------------------------------
extern PACKAGE TfmOption *fmOption;
//---------------------------------------------------------------------------
#endif
