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
  TLabel *lblStyle;
  TLabel *lblBgColor;
  TLabel *lblCurrentRowBgColor;
  TLabel *lblCurrentColBgColor;
  TLabel *lblCalcFgColor;
  TLabel *lblCalcBgColor;
  TLabel *lblCalcErrorFgColor;
  TLabel *lblCalcErrorBgColor;
  TLabel *lblDummyBgColor;
  TLabel *lblEvenRowBgColor;
  TLabel *lblFgColor;
  TLabel *lblFixBgColor;
  TLabel *lblFixFgColor;
  TLabel *lblFoundBgColor;
  TLabel *lblUrlColor;
  TComboBox *cbStyle;
  TColorBox *cbBgColor;
  TColorBox *cbCurrentColBgColor;
  TColorBox *cbCurrentRowBgColor;
  TColorBox *cbCalcFgColor;
  TColorBox *cbCalcBgColor;
  TColorBox *cbCalcErrorFgColor;
  TColorBox *cbCalcErrorBgColor;
  TColorBox *cbDummyBgColor;
  TColorBox *cbEvenRowBgColor;
  TColorBox *cbFgColor;
  TColorBox *cbFixBgColor;
  TColorBox *cbFixFgColor;
  TColorBox *cbFoundBgColor;
  TColorBox *cbUrlColor;
  void __fastcall cbFgColorChange(TObject *Sender);
  void __fastcall cbBgColorChange(TObject *Sender);
  void __fastcall cbStyleChange(TObject *Sender);
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
