//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Find.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmFind *fmFind;
//---------------------------------------------------------------------------
__fastcall TfmFind::TfmFind(TComponent* Owner)
        : TForm(Owner)
{
  cbCase->Checked = fmMain->FindCase;
  cbWordSearch->Checked = fmMain->FindWordSearch;
  cbRegex->Checked = fmMain->FindRegex;
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::FormShow(TObject *Sender)
{
  int left = fmMain->Left;
  int top = fmMain->Top + fmMain->Height;
  if(top > Screen->Height - Height){
    top = Screen->Height - Height;
  }
  Left = left + 8;
  Top = top - 8;
  edFindText->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnSearchFromTopClick(TObject *Sender)
{
  fmMain->MainGrid->Options << goEditing << goAlwaysShowEditor;
  fmMain->MainGrid->ShowEditor();

  if(rgDirection->ItemIndex == 0){
    if(rgRange->ItemIndex != 0){
      fmMain->MainGrid->Row = fmMain->MainGrid->DataBottom + 1;
    }
    if(rgRange->ItemIndex != 1){
      fmMain->MainGrid->Col = fmMain->MainGrid->DataRight + 1;
    }
  }else{
    if(rgRange->ItemIndex != 0){
      fmMain->MainGrid->Row = fmMain->MainGrid->FixedRows;
    }
    if(rgRange->ItemIndex != 1){
      fmMain->MainGrid->Col = fmMain->MainGrid->FixedCols;
    }
    TInplaceEdit *ipEd = fmMain->MainGrid->InplaceEditor;
    if(ipEd){
      ipEd->SelLength = 0; ipEd->SelStart = 0;
      ipEd->SetFocus();
    }
  }
  btnNextClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnNextClick(TObject *Sender)
{
  if(PageControl->TabIndex == 0){
    fmMain->MainGrid->Find(edFindText->Text,rgRange->ItemIndex,
      cbCase->Checked, cbRegex->Checked, cbWordSearch->Checked,
      (rgDirection->ItemIndex == 0));
  }else{
    double Min,Max;
    double *pMin, *pMax;
    pMin = pMax = NULL;
    if(edMin->Text != ""){
      Min = (edMin->Text).ToDouble();
      pMin=&Min;
    }
    if(edMax->Text != ""){
      Max = (edMax->Text).ToDouble();
      pMax=&Max;
    }
    fmMain->MainGrid->NumFind(pMin,pMax,rgRange->ItemIndex,
      (rgDirection->ItemIndex == 0));
  }
  fmMain->MainGrid->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnReplaceClick(TObject *Sender)
{
  fmMain->MainGrid->Replace(edFindText->Text,edReplaceText->Text,
    rgRange->ItemIndex, cbCase->Checked, cbRegex->Checked,
    cbWordSearch->Checked, (rgDirection->ItemIndex == 0));
  fmMain->MainGrid->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnAllReplaceClick(TObject *Sender)
{
  if (edFindText->Text == edReplaceText->Text) { return; }
  int left = (rgRange->ItemIndex == 1 ? fmMain->MainGrid->Col
                                      : fmMain->MainGrid->FixedCols);
  int top = (rgRange->ItemIndex == 0 ? fmMain->MainGrid->Row
                                     : fmMain->MainGrid->FixedRows);
  int right = (rgRange->ItemIndex == 1 ? fmMain->MainGrid->Col
                                       : fmMain->MainGrid->DataRight);
  int bottom = (rgRange->ItemIndex == 0 ? fmMain->MainGrid->Row
                                        : fmMain->MainGrid->DataBottom);
  int count = fmMain->MainGrid->ReplaceAll(edFindText->Text,
      edReplaceText->Text, left, top, right, bottom, cbCase->Checked,
      cbRegex->Checked, cbWordSearch->Checked);
  Application->MessageBox(((String)count + " 個のセルを置換しました。").c_str(),
                          TEXT("すべて置換"), 0);
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnCancelClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::PageControlChange(TObject *Sender)
{
  if(PageControl->TabIndex == 0){
    btnReplace->Enabled = true;
    btnAllReplace->Enabled = true;
    cbWordSearch->Enabled = true;
  }else{
    btnReplace->Enabled = false;
    btnAllReplace->Enabled = false;
    cbWordSearch->Enabled = false;
  }
}
//---------------------------------------------------------------------------

