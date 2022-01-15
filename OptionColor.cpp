//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "OptionColor.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrOptionColor *frOptionColor;
//---------------------------------------------------------------------------
__fastcall TfrOptionColor::TfrOptionColor(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionColor::RestoreFromMainForm()
{
  Font = fmMain->Font;

  stFont->Font = fmMain->MainGrid->Font;
  stFont->Color = fmMain->MainGrid->Color;
  stFont->Caption = stFont->Font->Name + " (" + stFont->Font->Size + ")";
  udLineMargin->Position = fmMain->MainGrid->LineMargin;
  cbNoMarginInCellLines->Checked = (fmMain->MainGrid->CellLineMargin == 0);

  cbFgColor->Selected = fmMain->MainGrid->Font->Color;
  cbUrlColor->Selected = fmMain->MainGrid->UrlColor;
  cbBgColor->Selected = fmMain->MainGrid->Color;
  cbFixFgColor->Selected = fmMain->MainGrid->FixFgColor;
  cbFixBgColor->Selected = fmMain->MainGrid->FixedColor;
  cbDummyBgColor->Selected = fmMain->MainGrid->DummyBgColor;
  cbCalcFgColor->Selected = fmMain->MainGrid->CalcFgColor;
  cbCalcBgColor->Selected = fmMain->MainGrid->CalcBgColor;
  cbCalcErrorFgColor->Selected = fmMain->MainGrid->CalcErrorFgColor;
  cbCalcErrorBgColor->Selected = fmMain->MainGrid->CalcErrorBgColor;
}
//---------------------------------------------------------------------------
void TfrOptionColor::StoreToMainForm()
{
  fmMain->SetGridFont(stFont->Font);

  fmMain->MainGrid->LineMargin = udLineMargin->Position;
  fmMain->MainGrid->CellLineMargin = (cbNoMarginInCellLines->Checked ? 0 :
    udLineMargin->Position);

  fmMain->MainGrid->Font->Color = cbFgColor->Selected;
  fmMain->MainGrid->Color = cbBgColor->Selected;
  fmMain->MainGrid->InplaceEditor->Brush->Color = cbBgColor->Selected;
  fmMain->MainGrid->UrlColor = cbUrlColor->Selected;
  fmMain->MainGrid->FixFgColor = cbFixFgColor->Selected;
  fmMain->MainGrid->FixedColor = cbFixBgColor->Selected;
  fmMain->MainGrid->DummyBgColor = cbDummyBgColor->Selected;
  fmMain->MainGrid->CalcFgColor = cbCalcFgColor->Selected;
  fmMain->MainGrid->CalcBgColor = cbCalcBgColor->Selected;
  fmMain->MainGrid->CalcErrorFgColor = cbCalcErrorFgColor->Selected;
  fmMain->MainGrid->CalcErrorBgColor = cbCalcErrorBgColor->Selected;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionColor::btnFontClick(TObject *Sender)
{
  TFontDialog *dlgFont = new TFontDialog(this);
  dlgFont->Font = stFont->Font;
  if(dlgFont->Execute()){
    stFont->Font = dlgFont->Font;
    stFont->Caption = stFont->Font->Name + " (" + stFont->Font->Size + ")";
    cbFgColor->Selected = stFont->Font->Color;
  }
  delete dlgFont;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionColor::cbFgColorChange(TObject *Sender)
{
  stFont->Font->Color = cbFgColor->Selected;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionColor::cbBgColorChange(TObject *Sender)
{
  stFont->Color = cbBgColor->Selected;
}
//---------------------------------------------------------------------------
