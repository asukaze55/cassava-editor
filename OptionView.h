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
__published:	// IDE 管理のコンポーネント
  TGroupBox *gbView;
  TCheckBox *cbWordWrap;
  TCheckBox *cbNumAlignRight;
  TCheckBox *cbNum3;
  TCheckBox *cbOmitDecimal;
  TLabel *lblOmitDigits;
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
  TGroupBox *gbTooltip;
  TCheckBox *cbShowToolTipForLongCell;
  TLabel *lblHintPause;
  TLabel *lblHintHidePause;
  TEdit *edHintPause;
  TUpDown *udHintPause;
  TEdit *edHintHidePause;
  TUpDown *udHintHidePause;
  TGroupBox *gbRefresh;
  TLabel *lblMaxRowHeightLines;
  TLabel *lblMaxRowHeightLinesUnit;
  TCheckBox *cbCompactColWidth;
  TEdit *edMaxRowHeightLines;
  TCheckBox *cbCalcWidthForAllRow;
  TGroupBox *gbToolBar;
  TLabel *lblToolBarSize;
  TEdit *edToolBarSize;
  TUpDown *udToolBarSize;
  TLabel *lblToolBarSizePx;
  void __fastcall btnFontClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
  __fastcall TfrOptionView(TComponent* Owner);
  void RestoreFromMainForm();
  void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionView *frOptionView;
//---------------------------------------------------------------------------
#endif
