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
constexpr wchar_t LightModeName[] = L"Windows";
constexpr wchar_t DarkModeName[] = L"Windows10 Dark";
constexpr int LightModeIndex = 0;
constexpr int DarkModeIndex = 1;
//---------------------------------------------------------------------------
__fastcall TfrOptionColor::TfrOptionColor(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionColor::RestoreFromMainForm()
{
  cbStyle->ItemIndex =
      fmMain->Style == DarkModeName ? DarkModeIndex : LightModeIndex;

  TMainGrid *grid = fmMain->MainGrid;
  cbBgColor->Selected = grid->Color;
  cbFgColor->Selected = grid->Font->Color;
  cbCalcBgColor->Selected = grid->CalcBgColor;
  cbCalcErrorFgColor->Selected = grid->CalcErrorFgColor;
  cbCalcErrorBgColor->Selected = grid->CalcErrorBgColor;
  cbCalcFgColor->Selected = grid->CalcFgColor;
  cbCurrentColBgColor->Selected = grid->CurrentColBgColor;
  cbCurrentRowBgColor->Selected = grid->CurrentRowBgColor;
  cbDummyBgColor->Selected = grid->DummyBgColor;
  cbEvenRowBgColor->Selected = grid->EvenRowBgColor;
  cbFixFgColor->Selected = grid->FixFgColor;
  cbFixBgColor->Selected = grid->FixedColor;
  cbFoundBgColor->Selected = grid->FoundBgColor;
  cbUrlColor->Selected = grid->UrlColor;
}
//---------------------------------------------------------------------------
void TfrOptionColor::StoreToMainForm()
{
  fmMain->Style =
      cbStyle->ItemIndex == DarkModeIndex ? DarkModeName : LightModeName;

  TMainGrid *grid = fmMain->MainGrid;
  grid->Color = cbBgColor->Selected;
  grid->InplaceEditor->Brush->Color = cbBgColor->Selected;
  grid->Font->Color = cbFgColor->Selected;
  grid->CalcBgColor = cbCalcBgColor->Selected;
  grid->CalcErrorBgColor = cbCalcErrorBgColor->Selected;
  grid->CalcErrorFgColor = cbCalcErrorFgColor->Selected;
  grid->CalcFgColor = cbCalcFgColor->Selected;
  grid->CurrentColBgColor = cbCurrentColBgColor->Selected;
  grid->CurrentRowBgColor = cbCurrentRowBgColor->Selected;
  grid->DummyBgColor = cbDummyBgColor->Selected;
  grid->EvenRowBgColor = cbEvenRowBgColor->Selected;
  grid->FixFgColor = cbFixFgColor->Selected;
  grid->FixedColor = cbFixBgColor->Selected;
  grid->FoundBgColor = cbFoundBgColor->Selected;
  grid->UrlColor = cbUrlColor->Selected;
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
static void MaybeUpdateSelected(TColorBox *ColorBox, TColor Light, TColor Dark,
    bool DarkMode)
{
  if (ColorBox->Selected == (DarkMode ? Light : Dark)) {
    ColorBox->Selected = (DarkMode ? Dark : Light);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionColor::cbStyleChange(TObject *Sender)
{
  bool isDarkMode = (cbStyle->ItemIndex == DarkModeIndex);
  MaybeUpdateSelected(cbBgColor, clWindow, clBlack, isDarkMode);
  MaybeUpdateSelected(cbFgColor, clWindowText, clWhite, isDarkMode);
  MaybeUpdateSelected(cbCalcBgColor, clAqua, clNavy, isDarkMode);
  MaybeUpdateSelected(cbCalcFgColor, clWindowText, clWhite, isDarkMode);
  MaybeUpdateSelected(cbCurrentColBgColor, clWindow, clBlack, isDarkMode);
  MaybeUpdateSelected(cbCurrentRowBgColor, clWindow, clBlack, isDarkMode);
  MaybeUpdateSelected(cbDummyBgColor, clCream, (TColor) 0x202020, isDarkMode);
  MaybeUpdateSelected(cbEvenRowBgColor, clWindow, clBlack, isDarkMode);
  MaybeUpdateSelected(cbFixBgColor, clBtnFace, (TColor) 0x404040, isDarkMode);
  MaybeUpdateSelected(cbFixFgColor, clWindowText, clWhite, isDarkMode);
  MaybeUpdateSelected(cbFoundBgColor, clYellow, clOlive, isDarkMode);
  MaybeUpdateSelected(cbUrlColor, clBlue, clAqua, isDarkMode);
}
//---------------------------------------------------------------------------
