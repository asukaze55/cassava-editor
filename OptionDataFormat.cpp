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
  Font = fmMain->Font;

  Renaming = true;
  TypeList = fmMain->MainGrid->TypeList;
  cbType->Items->Clear();
  for(int i=0; i<TypeList.Count; i++){
    cbType->Items->Add(TypeList.Items(i)->Name);
  }
  ActiveTypeIndex = fmMain->MainGrid->TypeIndex;
  TypeIndex = ActiveTypeIndex;
  cbType->ItemIndex = TypeIndex;
  Renaming = false;
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
  TTypeOption *p = TypeList.Items(id);

  edDefExt->Text = p->DefExt();
  edExts->Text = p->GetExtsStr(1);
  cbForceExt->Checked = p->ForceExt;

  edDefSepChar->Text   = Ctrl2Ascii((AnsiString)(p->DefSepChar()));
  edSepChars->Text     = Ctrl2Ascii((AnsiString)(p->SepChars.c_str() + 1));
  edWeakSepChars->Text = Ctrl2Ascii(p->WeakSepChars);
  rgSaveQuote->ItemIndex = p->QuoteOption;
  cbCommaRect->Checked = !(p->OmitComma);
  cbDummyEOF->Checked = !(p->OmitComma) && p->DummyEof;
  cbDummyEOF->Enabled = !(p->OmitComma);

  bool isUserType = (id > 0);
  btnRename->Enabled = isUserType;
  btnDeleteType->Enabled = isUserType;
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::StoreDataPage()
{
  TTypeOption *p = TypeList.Items(TypeIndex);

  p->Name = cbType->Items->Strings[TypeIndex];
  p->SetExts(edDefExt->Text + ";" + edExts->Text);

  p->ForceExt = cbForceExt->Checked;

  p->SepChars = Ascii2Ctrl(edDefSepChar->Text) + Ascii2Ctrl(edSepChars->Text);
  p->WeakSepChars = Ascii2Ctrl(edWeakSepChars->Text);

  p->QuoteOption = rgSaveQuote->ItemIndex;
  p->OmitComma = !(cbCommaRect->Checked);
  p->DummyEof = cbCommaRect->Checked && cbDummyEOF->Checked;
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::UpdateOptionTree()
{
  fmOption->tnDataFormat->DeleteChildren();
  for(int i=0; i<TypeList.Count; i++){
    AnsiString name = TypeList.Items(i)->Name;
    if (i == ActiveTypeIndex){ name += " (アクティブ)"; }
    fmOption->tvCategory->Items->AddChild(fmOption->tnDataFormat, name);
  }
  fmOption->tnDataFormat->Expand(false);
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::UpdateActiveOptionTree()
{
  for(int i=0; i<TypeList.Count; i++){
    AnsiString name = TypeList.Items(i)->Name;
    if (i == ActiveTypeIndex){ name += " (アクティブ)"; }
    fmOption->tnDataFormat->Item[i]->Text = name;
  }
}
//---------------------------------------------------------------------------
void TfrOptionDataFormat::Select(int index)
{
  if(Renaming){
    btnNewTypeClick(this);
  }
  cbType->ItemIndex = index;
  StoreDataPage();
  RestoreDataPage(cbType->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::cbTypeChange(TObject *Sender)
{
  if(!Renaming){
    StoreDataPage();
    RestoreDataPage(cbType->ItemIndex);
    ActiveTypeIndex = TypeIndex;
    UpdateActiveOptionTree();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::btnRenameClick(TObject *Sender)
{
  if(Renaming){
    AnsiString name = cbType->Text;
    cbType->Items->Strings[TypeIndex] = name;
    cbType->Style = Stdctrls::csDropDownList;
    cbType->ItemIndex = TypeIndex;
    TypeList.Items(TypeIndex)->Name = name;
    btnRename->Caption = "名前変更";
    btnNewType->Caption = "新規作成";
    btnDeleteType->Enabled = true;
    UpdateOptionTree();
    Renaming = false;
  }else{
    Renaming = true;
    cbType->Style = Stdctrls::csDropDown;
    cbType->SetFocus();
    btnRename->Caption = "名前確定";
    btnNewType->Caption = "キャンセル";
    btnDeleteType->Enabled = false;
    UpdateOptionTree();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::btnNewTypeClick(TObject *Sender)
{
  if(Renaming){
    cbType->Style = Stdctrls::csDropDownList;
    cbType->ItemIndex = TypeIndex;
    btnRename->Caption = "名前変更";
    btnNewType->Caption = "新規作成";
    btnDeleteType->Enabled = true;
    UpdateOptionTree();
    Renaming = false;
  }else{
    StoreDataPage();
    Renaming = true;
    TTypeOption *to = new TTypeOption();
    to->Name = "[新規]";
    TypeList.Add(to);
    TypeIndex = cbType->Items->Add(to->Name);
    ActiveTypeIndex = TypeIndex;
    cbType->ItemIndex = TypeIndex;
    cbType->Style = Stdctrls::csDropDown;
    cbType->SetFocus();
    btnRename->Caption = "名前確定";
    btnRename->Enabled = true;
    btnNewType->Caption = "キャンセル";
    btnDeleteType->Enabled = false;
    UpdateOptionTree();
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::btnDeleteTypeClick(TObject *Sender)
{
  if(!Renaming){
    int dt = TypeIndex;
    if(dt == 0) return;
    cbType->ItemIndex = dt - 1;
    RestoreDataPage(dt - 1);
    if(ActiveTypeIndex >= dt){ ActiveTypeIndex--; };
    cbType->Items->Delete(dt);
    TypeList.Delete(dt);
    UpdateOptionTree();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::edDefSepCharChange(TObject *Sender)
{
  AnsiString Sep = edDefSepChar->Text;
  if(Sep.Length() > 0 && Sep[1] == '&'){ Sep = (AnsiString)"&" + Sep; }
  cbCommaRect->Caption = "セーブ時に各行の「" + Sep + "」の数を統一する";
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::cbCommaRectClick(TObject *Sender)
{
  cbDummyEOF->Enabled = cbCommaRect->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrOptionDataFormat::cbCommaRectKeyUpDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
  cbDummyEOF->Enabled = cbCommaRect->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionDataFormat::cbCommaRectKeyPress(TObject *Sender,
	  char &Key)
{
  cbDummyEOF->Enabled = cbCommaRect->Checked;
}
//---------------------------------------------------------------------------

