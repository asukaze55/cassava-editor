//---------------------------------------------------------------------------

#ifndef SortH
#define SortH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfmSort : public TForm
{
__published:	// IDE 管理のコンポーネント
    TEdit *seSortLeft;
    TUpDown *udSortLeft;
    TEdit *seSortTop;
    TUpDown *udSortTop;
    TLabel *Label4;
    TEdit *seSortRight;
    TUpDown *udSortRight;
    TEdit *seSortBottom;
    TUpDown *udSortBottom;
    TEdit *seSortCol;
    TUpDown *udSortCol;
    TLabel *Label5;
    TButton *btnSort;
    TButton *btnSortCancel;
    TRadioGroup *rgSortDirection;
    TCheckBox *cbNumSort;
    TLabel *Label1;
    TLabel *Label2;
    TCheckBox *cbCaseSensitive;
    TCheckBox *cbZenhanSensitive;
    void __fastcall btnSortClick(TObject *Sender);
    void __fastcall btnSortCancelClick(TObject *Sender);
    void __fastcall seSortChange(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// ユーザー宣言
public:		// ユーザー宣言
    __fastcall TfmSort(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSort *fmSort;
//---------------------------------------------------------------------------
#endif
