//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "OptionLaunch.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrOptionLaunch *frOptionLaunch;
//---------------------------------------------------------------------------
__fastcall TfrOptionLaunch::TfrOptionLaunch(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionLaunch::RestoreFromMainForm()
{
  edLaunch0->Text     = fmMain->mnAppli0->Hint;
  edLaunchName0->Text = fmMain->mnAppli0->Caption.c_str() + 4;
  cbClose0->Checked   = fmMain->mnAppli0->Tag;
  edLaunch1->Text     = fmMain->mnAppli1->Hint;
  edLaunchName1->Text = fmMain->mnAppli1->Caption.c_str() + 4;
  cbClose1->Checked   = fmMain->mnAppli1->Tag;
  edLaunch2->Text     = fmMain->mnAppli2->Hint;
  edLaunchName2->Text = fmMain->mnAppli2->Caption.c_str() + 4;
  cbClose2->Checked   = fmMain->mnAppli2->Tag;

  edBrowser->Text     = fmMain->MainGrid->BrowserFileName;
  rgUseURL->ItemIndex = fmMain->MainGrid->DblClickOpenURL;
}
//---------------------------------------------------------------------------
void TfrOptionLaunch::StoreToMainForm()
{
  fmMain->mnAppli0->Caption = (String)"&0: " + edLaunchName0->Text;
  fmMain->mnAppli0->Hint    = edLaunch0->Text;
  fmMain->mnAppli0->Tag     = cbClose0->Checked;
  fmMain->mnAppli0->Enabled = (edLaunch0->Text != "");
  fmMain->mnAppli1->Caption = (String)"&1: " + edLaunchName1->Text;
  fmMain->mnAppli1->Hint    = edLaunch1->Text;
  fmMain->mnAppli1->Tag     = cbClose1->Checked;
  fmMain->mnAppli1->Enabled = (edLaunch1->Text != "");
  fmMain->mnAppli2->Caption = (String)"&2: " + edLaunchName2->Text;
  fmMain->mnAppli2->Hint    = edLaunch2->Text;
  fmMain->mnAppli2->Tag     = cbClose2->Checked;
  fmMain->mnAppli2->Enabled = (edLaunch2->Text != "");

  fmMain->MainGrid->BrowserFileName = edBrowser->Text;
  fmMain->MainGrid->DblClickOpenURL = rgUseURL->ItemIndex;
  fmMain->MainGrid->ShowURLBlue = rgUseURL->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionLaunch::edLaunchChange(TObject *Sender)
{
  TEdit *Ed = static_cast<TEdit*>(Sender);
  int Tag = Ed->Tag;
  TEdit *LName;
  switch(Tag){
    case 0: LName = edLaunchName0; break;
    case 1: LName = edLaunchName1; break;
    case 2: LName = edLaunchName2; break;
  }

  if(Ed->Text==""){
    LName->Enabled = false;
    LName->Text = L"���ݒ�";
  }else{
    LName->Text = ChangeFileExt(ExtractFileName(Ed->Text), "");
    LName->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionLaunch::sbReferClick(TObject *Sender)
{
  TSpeedButton *SB = static_cast<TSpeedButton*>(Sender);
  int Tag = SB->Tag;
  TEdit *Launch;
  switch(Tag){
    case 0: Launch = edLaunch0; break;
    case 1: Launch = edLaunch1; break;
    case 2: Launch = edLaunch2; break;
    case 3: Launch = edBrowser; break;
  }

  dlgOpen->FileName = Launch->Text;
  if(dlgOpen->Execute()){
    Launch->Text = dlgOpen->FileName;
  }
}
//---------------------------------------------------------------------------

