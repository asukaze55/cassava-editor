//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OptionBehavior.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrOptionBehavior *frOptionBehavior;
//---------------------------------------------------------------------------
__fastcall TfrOptionBehavior::TfrOptionBehavior(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionBehavior::RestoreFromMainForm()
{
  Font = fmMain->Font;

  cbDragMove->ItemIndex = (fmMain->MainGrid->DragMove==true) ? 0 : 1;
  cbEnterMove->ItemIndex = fmMain->MainGrid->EnterMove;
  cbLeftArrowInCell->Checked = fmMain->MainGrid->LeftArrowInCell;
  cbWheelMoveCursol->Checked = fmMain->MainGrid->WheelMoveCursol;
  cbAlwaysShowEditor->Checked = fmMain->MainGrid->AlwaysShowEditor;
  cbSortAll->Checked = fmMain->SortAll;
  cbCalcWidthForAllRow->Checked = fmMain->MainGrid->CalcWidthForAllRow;

  cbStopMacro->Checked = (fmMain->StopMacroCount > 0);
  seStopMacroCount->Text = AnsiString(cbStopMacro->Checked ? fmMain->StopMacroCount : 100000);
  seStopMacroCount->Enabled = cbStopMacro->Checked;
}
//---------------------------------------------------------------------------
void TfrOptionBehavior::StoreToMainForm()
{
  fmMain->MainGrid->DragMove = (cbDragMove->ItemIndex == 0);
  fmMain->MainGrid->EnterMove = cbEnterMove->ItemIndex;
  fmMain->MainGrid->LeftArrowInCell = cbLeftArrowInCell->Checked;
  fmMain->MainGrid->WheelMoveCursol = cbWheelMoveCursol->Checked;
  fmMain->MainGrid->AlwaysShowEditor = cbAlwaysShowEditor->Checked;
  fmMain->SortAll = cbSortAll->Checked;
  fmMain->MainGrid->CalcWidthForAllRow = cbCalcWidthForAllRow->Checked;

  if(cbStopMacro->Checked){
    fmMain->StopMacroCount = seStopMacroCount->Text.ToIntDef(0);
  }else{
    fmMain->StopMacroCount = 0;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionBehavior::cbStopMacroClick(TObject *Sender)
{
  seStopMacroCount->Enabled = cbStopMacro->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrOptionBehavior::cbStopMacroKeyUpDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
  seStopMacroCount->Enabled = cbStopMacro->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrOptionBehavior::cbStopMacroKeyPress(TObject *Sender,
      char &Key)
{
  seStopMacroCount->Enabled = cbStopMacro->Checked;
}
//---------------------------------------------------------------------------

