//---------------------------------------------------------------------------
#ifndef OptionViewH
#define OptionViewH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrOptionView : public TFrame
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
    TGroupBox *gbView;
    TCheckBox *cbWordWrap;
    TCheckBox *cbNumAlignRight;
    TCheckBox *cbNum3;
    TCheckBox *cbOmitDecimal;
    TLabel *Label1;
    TUpDown *udOmitDigits;
    TEdit *edOmitDigits;
    TGroupBox *gbFont;
  TLabel *lblTBMargin;
  TLabel *lblTBMarginPt;
    TButton *btnFont;
    TStaticText *stFont;
  TEdit *edTBMargin;
  TUpDown *udTBMargin;
  TLabel *lblLRMargin;
  TEdit *edLRMargin;
  TUpDown *udLRMargin;
  TLabel *lblLRMarginPt;
  TLabel *lblCellLineMargin;
  TEdit *edCellLineMargin;
  TUpDown *udCellLineMargin;
  TLabel *lblCellLineMarginPt;
    void __fastcall btnFontClick(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
    __fastcall TfrOptionView(TComponent* Owner);
    void RestoreFromMainForm();
    void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionView *frOptionView;
//---------------------------------------------------------------------------
#endif
