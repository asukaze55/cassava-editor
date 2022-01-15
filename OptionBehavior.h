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
__published:	// IDE �Ǘ��̃R���|�[�l���g
    TLabel *lblDragMove;
	TComboBox *cbDragMove;
	TComboBox *cbEnterMove;
    TLabel *lblEnterMove;
	TCheckBox *cbLeftArrowInCell;
	TCheckBox *cbAlwaysShowEditor;
    TGroupBox *gbMacro;
    TLabel *lblStopMacroCount;
  TCheckBox *cbStopMacro;
  TEdit *seStopMacroCount;
  TCheckBox *cbSortAll;
    TGroupBox *gbWheel;
    TLabel *lblWheelScrollStep;
    TEdit *edWheelScrollStep;
    TUpDown *udWheelScrollStep;
  TComboBox *cbWheelMoveCursol;
    TLabel *lblWheelMoveCursol;
  TGroupBox *gbUndo;
  TLabel *lblUndoCount;
  TEdit *edUndoCount;
  TUpDown *udUndoCount;
  void __fastcall cbStopMacroClick(TObject *Sender);
  void __fastcall cbStopMacroKeyUpDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall cbStopMacroKeyPress(TObject *Sender, char &Key);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
	__fastcall TfrOptionBehavior(TComponent* Owner);
	void RestoreFromMainForm();
	void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionBehavior *frOptionBehavior;
//---------------------------------------------------------------------------
#endif
