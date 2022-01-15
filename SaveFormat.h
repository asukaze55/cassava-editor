//---------------------------------------------------------------------------
#ifndef SaveFormatH
#define SaveFormatH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfmSaveFormat : public TForm
{
__published:	// IDE 管理のコンポーネント
        TPageControl *pcSaveFormat;
        TTabSheet *TabSheet1;
        TTabSheet *HTML;
        TLabel *Label1;
        TEdit *edRowSep;
        TLabel *Label2;
        TEdit *Edit1;
        TEdit *Edit2;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *Edit3;
        TEdit *Edit4;
        TEdit *Edit5;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TEdit *Edit6;
        TLabel *Label8;
        TEdit *Edit7;
private:	// ユーザー宣言
public:		// ユーザー宣言
        __fastcall TfmSaveFormat(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSaveFormat *fmSaveFormat;
//---------------------------------------------------------------------------
#endif
