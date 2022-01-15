//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "OptionColor.h"
#include "MainForm.h"
#include "OptionView.h"
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
void __fastcall TfrOptionColor::cbFgColorChange(TObject *Sender)
{
  frOptionView->stFont->Font->Color = cbFgColor->Selected;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionColor::cbBgColorChange(TObject *Sender)
{
  frOptionView->stFont->Color = cbBgColor->Selected;
}
//---------------------------------------------------------------------------
