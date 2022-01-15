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
__published:	// IDE �Ǘ��̃R���|�[�l���g
  TGroupBox *gbColor;
  TLabel *lblFgColor;
  TLabel *lblBgColor;
  TLabel *lblFixBgColor;
  TLabel *lblCurrentRowBgColor;
  TLabel *lblCurrentColBgColor;
  TLabel *lblCalcFgColor;
  TLabel *lblCalcBgColor;
  TLabel *lblCalcErrorFgColor;
  TLabel *lblCalcErrorBgColor;
  TLabel *lblFixFgColor;
  TLabel *lblDummyBgColor;
  TLabel *lblUrlColor;
  TColorBox *cbFgColor;
  TColorBox *cbBgColor;
  TColorBox *cbFixFgColor;
  TColorBox *cbFixBgColor;
  TColorBox *cbCurrentColBgColor;
  TColorBox *cbCurrentRowBgColor;
  TColorBox *cbCalcFgColor;
  TColorBox *cbCalcBgColor;
  TColorBox *cbCalcErrorFgColor;
  TColorBox *cbCalcErrorBgColor;
  TColorBox *cbDummyBgColor;
  TColorBox *cbUrlColor;
	void __fastcall cbFgColorChange(TObject *Sender);
	void __fastcall cbBgColorChange(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
	__fastcall TfrOptionColor(TComponent* Owner);
	void RestoreFromMainForm();
	void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionColor *frOptionColor;
//---------------------------------------------------------------------------
#endif
