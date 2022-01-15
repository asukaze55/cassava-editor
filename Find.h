//---------------------------------------------------------------------------
#ifndef FindH
#define FindH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <Vcl.Grids.hpp>
//---------------------------------------------------------------------------
class TfmFind : public TForm
{
__published:	// IDE 管理のコンポーネント
        TButton *btnSearchFromTop;
        TButton *btnNext;
        TButton *btnReplace;
        TButton *btnAllReplace;
        TButton *btnCancel;
        TPageControl *PageControl;
        TTabSheet *tsTextSearch;
        TTabSheet *tsNumSearch;
        TLabel *Label1;
        TEdit *edFindText;
        TLabel *Label2;
        TCheckBox *cbWordSearch;
        TEdit *edMin;
        TLabel *Label3;
        TEdit *edMax;
        TLabel *Label4;
        TRadioGroup *rgRange;
        TRadioGroup *rgDirection;
        TEdit *edReplaceText;
        TCheckBox *cbRegex;
        TCheckBox *cbCase;
        void __fastcall btnSearchFromTopClick(TObject *Sender);
        void __fastcall btnNextClick(TObject *Sender);
        void __fastcall btnReplaceClick(TObject *Sender);
        void __fastcall btnAllReplaceClick(TObject *Sender);
        void __fastcall btnCancelClick(TObject *Sender);
        void __fastcall PageControlChange(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall edFindTextKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private:	// ユーザー宣言
        TGridRect range;
public:		// ユーザー宣言
        __fastcall TfmFind(TComponent* Owner);
        TGridRect GetRange();
        bool Case();
        bool Regex();
        bool Word();
        bool HitNum(String Value);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmFind *fmFind;
//---------------------------------------------------------------------------
#endif
