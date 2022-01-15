//---------------------------------------------------------------------------

#ifndef OptionBehaviorH
#define OptionBehaviorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrOptionBehavior : public TFrame
{
__published:	// IDE 管理のコンポーネント
	TLabel *Label14;
	TComboBox *cbDragMove;
	TComboBox *cbEnterMove;
	TLabel *Label15;
	TCheckBox *cbLeftArrowInCell;
	TCheckBox *cbAlwaysShowEditor;
    TGroupBox *gbMacro;
  TLabel *Label17;
  TCheckBox *cbStopMacro;
  TEdit *seStopMacroCount;
  TCheckBox *cbSortAll;
  TCheckBox *cbCalcWidthForAllRow;
    TGroupBox *gbWheel;
    TLabel *Label1;
    TEdit *edWheelScrollStep;
    TUpDown *udWheelScrollStep;
  TComboBox *cbWheelMoveCursol;
  TLabel *Label2;
  void __fastcall cbStopMacroClick(TObject *Sender);
  void __fastcall cbStopMacroKeyUpDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall cbStopMacroKeyPress(TObject *Sender, char &Key);
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TfrOptionBehavior(TComponent* Owner);
	void RestoreFromMainForm();
	void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionBehavior *frOptionBehavior;
//---------------------------------------------------------------------------
#endif
