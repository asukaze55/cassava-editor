//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Sort.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmSort *fmSort;
//---------------------------------------------------------------------------
__fastcall TfmSort::TfmSort(TComponent* Owner)
    : TForm(Owner)
{
  cbNumSort->Checked = fmMain->SortByNumber;
  cbCaseSensitive->Checked = !(fmMain->SortIgnoreCase);
  cbZenhanSensitive->Checked = !(fmMain->SortIgnoreZenhan);
}
//---------------------------------------------------------------------------
void __fastcall TfmSort::FormClose(TObject *Sender, TCloseAction &Action)
{
  fmMain->SortByNumber = cbNumSort->Checked;
  fmMain->SortIgnoreCase = !(cbCaseSensitive->Checked);
  fmMain->SortIgnoreZenhan = !(cbZenhanSensitive->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TfmSort::btnSortClick(TObject *Sender)
{
  int l = udSortLeft->Position;
  int t = udSortTop->Position;
  int r = udSortRight->Position;
  int b = udSortBottom->Position;
  int c = udSortCol->Position;
  bool d = (rgSortDirection->ItemIndex == 0);
  if(t > b || l > r){
    if(t > b){ seSortTop->Text  = b; seSortBottom->Text = t; }
    if(l > r){ seSortLeft->Text = r; seSortRight->Text  = l; }
    return;
  }
  if(b > fmMain->MainGrid->DataBottom) {
    b = fmMain->MainGrid->DataBottom;
  }
  if(r >= fmMain->MainGrid->ColCount) {
    r = fmMain->MainGrid->ColCount - 1;
  }
  fmMain->MainGrid->SetUndoCsv();
  fmMain->MainGrid->UndoSetLock++;
  fmMain->MainGrid->Invalidate();
  fmMain->MainGrid->Sort(l,t,r,b,c,d, cbNumSort->Checked,
    !(cbCaseSensitive->Checked), !(cbZenhanSensitive->Checked));
  fmMain->MainGrid->Repaint();
  fmMain->MainGrid->UndoSetLock--;

  fmMain->MainGrid->SetFocus();
  fmMain->MainGrid->ReNum();

  Close();
}
//---------------------------------------------------------------------------
void __fastcall TfmSort::btnSortCancelClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TfmSort::seSortChange(TObject *Sender)
{
  TGridRect R;
  R.Left = seSortLeft->Text.ToInt();
  if(R.Left < fmMain->MainGrid->FixedCols) {
    R.Left = fmMain->MainGrid->FixedCols;
  }
  R.Top = seSortTop->Text.ToInt();
  if(R.Top < fmMain->MainGrid->FixedRows) {
    R.Top = fmMain->MainGrid->FixedRows;
  }
  R.Right = seSortRight->Text.ToInt();
  if(R.Right >= fmMain->MainGrid->ColCount) {
    R.Right = fmMain->MainGrid->ColCount-1;
  }
  R.Bottom = seSortBottom->Text.ToInt();
  if(R.Bottom >= fmMain->MainGrid->RowCount) {
    R.Bottom = fmMain->MainGrid->RowCount-1;
  }
  fmMain->MainGrid->Selection = R;
}
//---------------------------------------------------------------------------

