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
AnsiString InputBoxMultiLine(const AnsiString ACaption,
  const AnsiString APrompt, const AnsiString ACancel, const AnsiString ADefault)
{
  TfmMultiLineInputBox *dialog = new TfmMultiLineInputBox(fmMain);
  dialog->Caption = ACaption;
  dialog->Label1->Caption = APrompt;
  dialog->Memo1->Lines->Text = ADefault;
  int result = dialog->ShowModal();
  AnsiString value;
  if(result == mrOk){
    value = dialog->Memo1->Lines->Text;
  }else{
    value = ACancel;
  }
  delete dialog;
  return value;
}
//---------------------------------------------------------------------


