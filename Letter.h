//---------------------------------------------------------------------------
#ifndef LetterH
#define LetterH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Dialogs.hpp>
#include "CSPIN.h"
//---------------------------------------------------------------------------
class TfmLetter : public TForm
{
__published:	// IDE 管理のコンポーネント
        TImage *imPreview;
    TGroupBox *bxFrom;
    TLabel *Label1;
    TEdit *edMyNumber;
    TLabel *Label2;
    TEdit *edMyAddress1;
    TEdit *edMyAddress2;
    TLabel *Label3;
    TEdit *edMyName;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TGroupBox *bxTo;
    TLabel *Label8;
    TLabel *Label9;
    TLabel *Label10;
    TLabel *Label11;
    TCheckBox *cbUseNote;
    TEdit *edUseNote;
    TComboBox *cbxHowUseNote;
    TLabel *Label12;
    TLabel *Label13;
    TButton *btnPrint;
    TButton *btnRead;
    TButton *btnSave;
    TButton *btnClose;
    TComboBox *cbxFont;
    TLabel *lbFont;
    TOpenDialog *dlgOpen;
    TSaveDialog *dlgSave;
    TLabel *Label15;
    TLabel *Label16;
    TEdit *edPrefix;
    TCheckBox *cbMyDataInCsv;
    TLabel *Label17;
    TCSpinEdit *edVert;
    TCSpinEdit *edHorz;
    TCSpinEdit *edTop;
    TCSpinEdit *edBottom;
    TCSpinEdit *edMyDataInCsv;
    TCSpinEdit *edToNumber;
    TCSpinEdit *edToAddress1;
    TCSpinEdit *edToAddress2;
    TCSpinEdit *edToName;
    TCSpinEdit *edToNote;
        TButton *btnPrinter;
        TPrinterSetupDialog *dlgPrinter;
        TCheckBox *cbNenga;
        TCheckBox *cbHorzNum;
        TLabel *Label14;
        TCSpinEdit *seHorzNumMax;
        TCSpinEdit *edToName2;
        TEdit *edMyName2;
        TButton *btnHelp;
  TCSpinEdit *seCopies;
  TLabel *Label18;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall imPreviewClick(TObject *Sender);
    void __fastcall btnPrintClick(TObject *Sender);
    void __fastcall cbxFontChange(TObject *Sender);
    void __fastcall btnSaveClick(TObject *Sender);
    void __fastcall btnReadClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall btnPrinterClick(TObject *Sender);
        void __fastcall edTopChange(TObject *Sender);
        void __fastcall btnHelpClick(TObject *Sender);
private:	// ユーザー宣言
    int NumberTop[2];
    int NumberLeft[2][7];
    int NumberSize[2];
    int AddressTop[2];
    int AddressBottom[2];
    int AddressMiddle[2][2];
    int AddressSize[2];
    int NameTop[2];
    int NameBottom[2];
    int NameMiddle[2];
    int NameSize[2];
    void DataSetDefault();

public:		// ユーザー宣言
    __fastcall TfmLetter(TComponent* Owner);
    int TateBytes(AnsiString Str);
    int Name2Indent(AnsiString Name1);
    bool Print(TCanvas *Canvas, int Index);
    void PrintOut();
    void PreView();
    void DataSave(AnsiString FileName);
    void DataRead(AnsiString FileName);
    AnsiString Tate(AnsiString Source);
};
//---------------------------------------------------------------------------
extern TfmLetter *fmLetter;
//---------------------------------------------------------------------------
#endif

