//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "OptionView.h"
#include "MainForm.h"
#include "OptionColor.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrOptionView *frOptionView;
//---------------------------------------------------------------------------
__fastcall TfrOptionView::TfrOptionView(TComponent* Owner)
    : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionView::RestoreFromMainForm()
{
  stFont->Font = fmMain->MainGrid->Font;
  stFont->Color = fmMain->MainGrid->Color;
  stFont->Caption = stFont->Font->Name + " (" + stFont->Font->Size + ")";
  udTBMargin->Position = fmMain->MainGrid->TBMargin;
  udLRMargin->Position = fmMain->MainGrid->LRMargin;
  udCellLineMargin->Position = fmMain->MainGrid->CellLineMargin;

  cbWordWrap->Checked = fmMain->MainGrid->WordWrap;
  cbNumAlignRight->Checked =
      (fmMain->MainGrid->TextAlignment == cssv_taNumRight);
  cbNum3->Checked = (fmMain->MainGrid->NumberComma != 0);
  int decimalDigits = fmMain->MainGrid->DecimalDigits;
  cbOmitDecimal->Checked = (decimalDigits >= 0);
  udOmitDigits->Position = (decimalDigits >= 0 ? decimalDigits : 2);

  cbCompactColWidth->Checked = fmMain->MainGrid->CompactColWidth;
  cbCalcWidthForAllRow->Checked = fmMain->MainGrid->CalcWidthForAllRow;
  edMaxRowHeightLines->Text = fmMain->MainGrid->MaxRowHeightLines;

  udToolBarSize->Position = fmMain->ToolBarSize;

  cbShowToolTipForLongCell->Checked = fmMain->MainGrid->ShowToolTipForLongCell;
  udHintPause->Position = Application->HintPause;
  udHintHidePause->Position = Application->HintHidePause;
}
//---------------------------------------------------------------------------
void TfrOptionView::StoreToMainForm()
{
  fmMain->SetGridFont(stFont->Font);

  fmMain->MainGrid->TBMargin = udTBMargin->Position;
  fmMain->MainGrid->LRMargin = udLRMargin->Position;
  fmMain->MainGrid->CellLineMargin = udCellLineMargin->Position;

  fmMain->MainGrid->WordWrap = cbWordWrap->Checked;
  fmMain->MainGrid->TextAlignment = (cbNumAlignRight->Checked ?
    cssv_taNumRight : cssv_taLeft);
  fmMain->MainGrid->NumberComma = (cbNum3->Checked ? 3 : 0);
  fmMain->MainGrid->DecimalDigits = (cbOmitDecimal->Checked ?
    udOmitDigits->Position : -1);

  fmMain->MainGrid->CompactColWidth = cbCompactColWidth->Checked;
  fmMain->MainGrid->CalcWidthForAllRow = cbCalcWidthForAllRow->Checked;
  fmMain->MainGrid->MaxRowHeightLines = edMaxRowHeightLines->Text.ToDouble();

  fmMain->ToolBarSize = udToolBarSize->Position;

  fmMain->MainGrid->ShowToolTipForLongCell = cbShowToolTipForLongCell->Checked;
  Application->HintPause = udHintPause->Position;
  Application->HintHidePause = udHintHidePause->Position;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionView::btnFontClick(TObject *Sender)
{
  TFontDialog *dlgFont = new TFontDialog(this);
  dlgFont->Font = stFont->Font;
  if(dlgFont->Execute()){
    stFont->Font = dlgFont->Font;
    stFont->Caption = stFont->Font->Name + " (" + stFont->Font->Size + ")";
    frOptionColor->cbFgColor->Selected = stFont->Font->Color;
  }
  delete dlgFont;
}
//---------------------------------------------------------------------------

