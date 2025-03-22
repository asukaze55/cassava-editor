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
  rgRange->ItemIndex = fmMain->FindRange;
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
  if (PageControl->TabIndex == 0) {
    edFindText->SetFocus();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnSearchFromTopClick(TObject *Sender)
{
  TGridRect searchRange = GetRange();

  fmMain->MainGrid->Options << goEditing << goAlwaysShowEditor;
  fmMain->MainGrid->ShowEditor();

  if (rgDirection->ItemIndex == 0) {
    fmMain->MainGrid->Row = searchRange.Bottom;
    fmMain->MainGrid->Col = searchRange.Right;
    TInplaceEdit *ipEd = fmMain->MainGrid->InplaceEditor;
    if (ipEd) {
      ipEd->SelLength = 0;
      ipEd->SelStart = ipEd->Text.Length();
      ipEd->SetFocus();
    }
  } else {
    fmMain->MainGrid->Row = searchRange.Top;
    fmMain->MainGrid->Col = searchRange.Left;
    TInplaceEdit *ipEd = fmMain->MainGrid->InplaceEditor;
    if (ipEd) {
      ipEd->SelLength = 0;
      ipEd->SelStart = 0;
      ipEd->SetFocus();
    }
  }
  btnNextClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnNextClick(TObject *Sender)
{
  if(PageControl->TabIndex == 0){
    fmMain->MainGrid->Find(edFindText->Text, GetRange(), cbCase->Checked,
        cbRegex->Checked, cbWordSearch->Checked, (rgDirection->ItemIndex == 0));
  }else{
    double Min,Max;
    double *pMin = nullptr;
    double *pMax = nullptr;
    if(edMin->Text != ""){
      Min = (edMin->Text).ToDouble();
      pMin=&Min;
    }
    if(edMax->Text != ""){
      Max = (edMax->Text).ToDouble();
      pMax=&Max;
    }
    fmMain->MainGrid->NumFind(pMin, pMax, GetRange(),
                              (rgDirection->ItemIndex == 0));
  }
  fmMain->MainGrid->Invalidate();
  fmMain->MainGrid->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnReplaceClick(TObject *Sender)
{
  fmMain->MainGrid->Replace(edFindText->Text,edReplaceText->Text, GetRange(),
      cbCase->Checked, cbRegex->Checked, cbWordSearch->Checked,
      (rgDirection->ItemIndex == 0));
  fmMain->MainGrid->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmFind::btnAllReplaceClick(TObject *Sender)
{
  if (edFindText->Text == edReplaceText->Text) { return; }
  TGridRect range = GetRange();
  int count = fmMain->MainGrid->ReplaceAll(edFindText->Text,
      edReplaceText->Text, range.Left, range.Top, range.Right, range.Bottom,
      cbCase->Checked, cbRegex->Checked, cbWordSearch->Checked);
  Application->MessageBox(
      ((String)count + L" 個のセルを置換しました。").c_str(), L"すべて置換", 0);
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
void __fastcall TfmFind::edFindTextKeyDown(TObject *Sender, WORD &Key,
    TShiftState Shift)
{
  fmMain->MainGrid->Invalidate();
}
//---------------------------------------------------------------------------
TGridRect TfmFind::GetRange()
{
  TMainGrid *grid = fmMain->MainGrid;
  switch (rgRange->ItemIndex) {
    case 0:
      range.Left = grid->FixedCols;
      range.Top = grid->Row;
      range.Right = grid->DataRight;
      range.Bottom = grid->Row;
      break;
    case 1:
      range.Left = grid->Col;
      range.Top = grid->FixedRows;
      range.Right = grid->Col;
      range.Bottom = grid->DataBottom;
      break;
    case 2:
      if (grid->RangeSelect) {
        range = grid->Selection;
        break;
      } else if (range.Left != range.Right || range.Top != range.Bottom) {
        // Keep previous search range.
        break;
      }
      // Fall through.
    default:
      range.Left = grid->FixedCols;
      range.Top = grid->FixedRows;
      range.Right = grid->DataRight;
      range.Bottom = grid->DataBottom;
  }
  return range;
}
//---------------------------------------------------------------------------
bool TfmFind::Case()
{
  return cbCase->Checked;
}
//---------------------------------------------------------------------------
bool TfmFind::Regex()
{
  return cbRegex->Checked;
}
//---------------------------------------------------------------------------
bool TfmFind::Word()
{
  return cbWordSearch->Checked;
}
//---------------------------------------------------------------------------
bool TfmFind::HitNum(String Value)
{
  try {
    if (PageControl->TabIndex == 0) {
      return false;
    }
    double doubleValue = Value.ToDouble();
    if (edMin->Text != "" && doubleValue < (edMin->Text).ToDouble()) {
      return false;
    }
    if (edMax->Text != "" && doubleValue > (edMax->Text).ToDouble()) {
      return false;
    }
  } catch (...) {
    return false;
  }
  return true;
}
//---------------------------------------------------------------------------

