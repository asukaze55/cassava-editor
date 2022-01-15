//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MultiLineInputBox.h"
#include "MainForm.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TfmMultiLineInputBox *fmMultiLineInputBox;
//--------------------------------------------------------------------- 
__fastcall TfmMultiLineInputBox::TfmMultiLineInputBox(TComponent* AOwner)
  : TForm(AOwner)
{
}
//---------------------------------------------------------------------
bool InputBoxMultiLine(
    const String ACaption, const String APrompt, String& AValue)
{
  TfmMultiLineInputBox *dialog = new TfmMultiLineInputBox(fmMain);
  dialog->Caption = ACaption;
  dialog->Label1->Caption = APrompt;
  dialog->Memo1->Lines->Text = AValue;
  int result = dialog->ShowModal();
  bool isOk;
  if (result == mrOk) {
    AValue = dialog->Memo1->Lines->Text;
    isOk = true;
  } else {
    isOk = false;
  }
  delete dialog;
  return isOk;
}
//---------------------------------------------------------------------


