//---------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include "MultiLineInputBox.h"
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
  std::unique_ptr<TfmMultiLineInputBox> dialog =
      std::make_unique<TfmMultiLineInputBox>(nullptr);
  dialog->Caption = ACaption;
  dialog->Label1->Caption = APrompt;
  dialog->Memo1->Lines->Text = AValue;
  if (dialog->ShowModal() != mrOk) {
    return false;
  }
  AValue = dialog->Memo1->Lines->Text;
  return true;
}
//---------------------------------------------------------------------


