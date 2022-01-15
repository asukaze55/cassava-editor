//---------------------------------------------------------------------------

#ifndef OptionColorH
#define OptionColorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrOptionColor : public TFrame
{
__published:	// IDE 管理のコンポーネント
	TButton *btnFont;
	TStaticText *stFont;
	TLabel *lblFgColor;
	TLabel *lblBgColor;
	TLabel *lblFixBgColor;
	TLabel *lblCalcFgColor;
	TLabel *lblCalcBgColor;
	TLabel *lblCalcErrorFgColor;
	TLabel *lblCalcErrorBgColor;
	TLabel *lblFixFgColor;
	TColorBox *cbFgColor;
	TColorBox *cbBgColor;
	TColorBox *cbFixFgColor;
	TColorBox *cbFixBgColor;
	TColorBox *cbCalcFgColor;
	TColorBox *cbCalcBgColor;
	TColorBox *cbCalcErrorFgColor;
	TColorBox *cbCalcErrorBgColor;
  TLabel *lblDummyBgColor;
  TColorBox *cbDummyBgColor;
  TLabel *lblLineMargin;
  TEdit *edLineMargin;
  TUpDown *udLineMargin;
  TLabel *lblLineMarginPt;
  TCheckBox *cbNoMarginInCellLines;
  TLabel *lblUrlColor;
  TColorBox *cbUrlColor;
	void __fastcall btnFontClick(TObject *Sender);
	void __fastcall cbFgColorChange(TObject *Sender);
	void __fastcall cbBgColorChange(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TfrOptionColor(TComponent* Owner);
	void RestoreFromMainForm();
	void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionColor *frOptionColor;
//---------------------------------------------------------------------------
#endif
