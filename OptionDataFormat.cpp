//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "OptionDataFormat.h"
#include "MainForm.h"
#include "Option.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrOptionDataFormat *frOptionDataFormat;
//---------------------------------------------------------------------------
__fastcall TfrOptionDataFormat::TfrOptionDataFormat(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::RestoreFromMainForm()
{
  TypeList = fmMain->MainGrid->TypeList;
  ActiveTypeIndex = fmMain->MainGrid->TypeIndex;
  TypeIndex = ActiveTypeIndex;
  edName->Text = TypeList.Items(TypeIndex)->Name;
  RestoreDataPage(TypeIndex);
  UpdateOptionTree();
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::StoreToMainForm()
{
  StoreDataPage();
  fmMain->MainGrid->TypeList = TypeList;
  fmMain->MainGrid->TypeIndex = ActiveTypeIndex;
  fmMain->MainGrid->TypeOption = fmMain->MainGrid->TypeList.Items(ActiveTypeIndex);
  fmMain->SetFilter();
  fmMain->SetPasteMenu(fmMain->mnPasteFormat);
  fmMain->SetPasteMenu(fmMain->mnpPasteFormat);
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::RestoreDataPage(int id)
{
  TypeIndex = id;
  if (id >= TypeList.Count) {
    edName->Text = "";
    edDefExt->Text = "";
    edExts->Text = "";
    cbForceExt->Checked = false;

    edDefSepChar->Text = "";
    edSepChars->Text = "";
    edWeakSepChars->Text = "";
    rgSaveQuote->ItemIndex = 1;
    cbCommaRect->Checked = true;
    cbDummyEOF->Checked = true;

    edName->Enabled = true;
    btnDeleteType->Enabled = false;
    return;
  }

  TTypeOption *p = TypeList.Items(id);

  edName->Text = p->Name;
  edDefExt->Text = p->DefExt();
  edExts->Text = p->GetExtsStr(1);
  cbForceExt->Checked = p->ForceExt;

  String sepChars = p->SepChars;
  edDefSepChar->Text   = Ctrl2Ascii(sepChars.SubString(1, 1));
  edSepChars->Text     = Ctrl2Ascii(sepChars.SubString(2, sepChars.Length()));
  edWeakSepChars->Text = Ctrl2Ascii(p->WeakSepChars);
  rgSaveQuote->ItemIndex = p->QuoteOption;
  cbCommaRect->Checked = !(p->OmitComma);
  cbDummyEOF->Checked = p->DummyEof || p->DummyEol;

  bool isUserType = (id > 0);
  edName->Enabled = isUserType;
  btnDeleteType->Enabled = isUserType;
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::StoreDataPage()
{
  if (TypeIndex >= TypeList.Count) { return; }
  TTypeOption *p = TypeList.Items(TypeIndex);

  p->SetExts(edDefExt->Text + ";" + edExts->Text);
  p->ForceExt = cbForceExt->Checked;

  p->SepChars = Ascii2Ctrl(edDefSepChar->Text) + Ascii2Ctrl(edSepChars->Text);
  p->WeakSepChars = Ascii2Ctrl(edWeakSepChars->Text);
  p->QuoteOption = rgSaveQuote->ItemIndex;
  p->OmitComma = !(cbCommaRect->Checked);
  p->DummyEof = cbCommaRect->Checked && cbDummyEOF->Checked;
  p->DummyEol = !cbCommaRect->Checked && cbDummyEOF->Checked;

  if (edName->Text != p->Name) {
    p->Name = edName->Text;
    UpdateOptionTree();
  }
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::UpdateOptionTree()
{
  if (fmOption->tnDataFormat->Count > TypeList.Count + 1) {
    fmOption->tnDataFormat->DeleteChildren();
  }
  for (int i = 0; i < TypeList.Count + 1; i++){
    String name =
        (i < TypeList.Count ? TypeList.Items(i)->Name : (String)"(新規作成)");
    if (i == ActiveTypeIndex) { name += " (アクティブ)"; }

    if (fmOption->tnDataFormat->Count <= i) {
      fmOption->tvCategory->Items->AddChild(fmOption->tnDataFormat, name);
    } else {
      fmOption->tnDataFormat->Item[i]->Text = name;
    }
  }
  fmOption->tnDataFormat->Expand(false);
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::Select(int index)
{
  StoreDataPage();
  RestoreDataPage(index);
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::ApplyToCurrentFile(int index)
{
  ActiveTypeIndex = index;
  UpdateOptionTree();
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::Delete(int index)
{
  if (index <= 0 || index >= TypeList.Count) { return; }
  TypeList.Delete(index);
  if (TypeIndex == index) {
    RestoreDataPage(TypeList.Count);
  } else if (TypeIndex > index) {
    TypeIndex--;
  }
  if (ActiveTypeIndex == index) {
    ActiveTypeIndex = 0;
  } else if (ActiveTypeIndex > index) {
    ActiveTypeIndex--;
  }
  UpdateOptionTree();
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::edNameChange(TObject *Sender)
{
  if (TypeIndex == TypeList.Count) {
    if (edName->Text == "") { return; }
    TypeList.Add(TTypeOption());
    btnDeleteType->Enabled = true;
  }
  TTypeOption *p = TypeList.Items(TypeIndex);
  if (edName->Text != p->Name) {
    p->Name = edName->Text;
    UpdateOptionTree();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::btnDeleteTypeClick(TObject *Sender)
{
  Delete(TypeIndex);
}
//---------------------------------------------------------------------------

