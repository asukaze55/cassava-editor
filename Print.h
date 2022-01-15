//---------------------------------------------------------------------------
#ifndef PrintH
#define PrintH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "CSPIN.h"
//---------------------------------------------------------------------------
class TfmPrint : public TForm
{
__published:	// IDE 管理のコンポーネント
        TButton *btnPrint;
        TButton *btnPrinter;
        TButton *btnClose;
        TPrinterSetupDialog *dlgPrinter;
        TGroupBox *gbYohaku;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TCSpinEdit *csYohaku0;
        TCSpinEdit *csYohaku2;
        TCSpinEdit *csYohaku1;
        TCSpinEdit *csYohaku3;
        TButton *btnFont;
        TFontDialog *dlgFont;
        TLabel *lblFont;
        void __fastcall btnPrinterClick(TObject *Sender);
        void __fastcall btnPrintClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall btnFontClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
        __fastcall TfmPrint(TComponent* Owner);
        void PrintOut();
        void Print(TCanvas *Canvas, int Width, int Height, int *Top);
        int DataWidth;
        int DataHeight;
        int *Widths;
        int Yohaku[4];
};
//---------------------------------------------------------------------------
extern PACKAGE TfmPrint *fmPrint;
//---------------------------------------------------------------------------
#endif
