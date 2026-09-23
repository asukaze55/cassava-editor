//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include "KeyCustomize.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfmKey::TfmKey(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TfmKey::AddMenu(TTreeNode* Node, TMenuItem* MenuItem)
{
  if (MenuItem->Caption == "-") {
    return;
  }

  if (MenuItem->Count > 0 && MenuItem->Parent == fmMain->mnMacro) {
    // Do not add directories under the macro menu.
    // These will not be loaded in the start up time.
    return;
  }

  TTreeNode* New = tvMenu->Items->AddChild(Node, MenuItem->Caption);
  Changes.emplace(New, MenuItem);

  if (MenuItem->Count > 0) {
    for (int i = 0; i < MenuItem->Count; i++) {
      AddMenu(New, MenuItem->Items[i]);
    }
  }
}
//---------------------------------------------------------------------------
void TfmKey::MakeTree()
{
  TTreeNode *Root = tvMenu->Items->Item[0];
  TMenuItem *Menu = fmMain->MainMenu->Items;
  for(int i=0; i<Menu->Count; i++)
    AddMenu(Root, Menu->Items[i]);
  Root->Expanded = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::FormShow(TObject *Sender)
{
  MakeTree();
  Font = fmMain->Font;
}
//---------------------------------------------------------------------------
TMenuShortCutChange* TfmKey::UpdateChange()
{
  auto it = Changes.find(tvMenu->Selected);
  if (it == Changes.end()) {
    return nullptr;
  }
  TMenuShortCutChange& change = it->second;

  TShiftState shift;
  String shiftString = "";
  if (cbShift->Checked) {
    shift << ssShift;
    shiftString += "Shift+";
  }
  if (cbCtrl->Checked) {
    shift << ssCtrl;
    shiftString += "Ctrl+";
  }
  if (cbAlt->Checked) {
    shift << ssAlt;
    shiftString += "Alt+";
  }
  change.Shift = shift;

  switch (rgSCKey->ItemIndex) {
    case 1:
      if (edSCKey->Text.Length() >= 1) {
        change.ShortCut = TextToShortCut(shiftString + edSCKey->Text);
      } else {
        change.Key = '\0';
      }
      break;
    case 2:
      change.Key = static_cast<Word>(VK_F1 + udFNumber->Position - 1);
      break;
    case 3: change.Key = VK_RETURN; break;
    case 4: change.Key = VK_SPACE; break;
    case 5: change.Key = VK_INSERT; break;
    case 6: change.Key = VK_DELETE; break;
    case 7: change.Key = VK_BACK; break;
    default: change.Key = '\0'; break;
  }
  return &change;
}
//---------------------------------------------------------------------------
int TfmKey::KeyToIndex(Word Key)
{
  if(Key == '\0') return 0;
  else if(Key >= VK_F1 && Key <= VK_F24) return 2;
  else if(Key == VK_RETURN) return 3;
  else if(Key == VK_SPACE)  return 4;
  else if(Key == VK_INSERT) return 5;
  else if(Key == VK_DELETE) return 6;
  else if(Key == VK_BACK)   return 7;

  return 1;
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::tvMenuChange(TObject *Sender, TTreeNode *Node)
{
  const TMenuShortCutChange* change = nullptr;

  Word SCKey;
  TShiftState SCShift;
  stUseSC->Visible = false;
  edSelected->Text = Node->Text;

  const auto& it = Changes.find(Node);
  if (it == Changes.end() || Node->Count > 0) {
    SCKey = '\0';
    rgSCKey->Enabled = false;
  } else {
    change = &it->second;
    SCKey = change->Key;
    SCShift = change->Shift;
    rgSCKey->Enabled = true;
    if (change->Modified) {
      stUseSC->Caption = L"★";
      stUseSC->Visible = true;
    } else {
      stUseSC->Caption = L"☆";
      stUseSC->Visible = (SCKey != '\0');
    }
  }

  edSCKey->Enabled = false;
  csFNumber->Enabled = false;
  udFNumber->Enabled = false;

  if(SCKey == '\0'){
    rgSCKey->ItemIndex = 0;
    cbCtrl->Checked = true;
    cbShift->Checked = false;
    cbAlt->Checked = false;
  } else {
    cbCtrl->Checked = SCShift.Contains(ssCtrl);
    cbShift->Checked = SCShift.Contains(ssShift);
    cbAlt->Checked = SCShift.Contains(ssAlt);

    int ii = KeyToIndex(SCKey);
    rgSCKey->ItemIndex = ii;
    if(ii == 1) {
      edSCKey->Enabled = true;
      String text = ShortCutToText(change->ShortCut);
      int index = text.LastDelimiter("+");
      edSCKey->Text = text.SubString(index + 1, text.Length() - index);
    } else if(ii == 2){
      csFNumber->Enabled = true;
      udFNumber->Enabled = true;
      csFNumber->Text = String(static_cast<long>(SCKey - VK_F1) + 1);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::tvMenuChanging(TObject *Sender, TTreeNode *Node,
      bool &AllowChange)
{
  TMenuShortCutChange* updatedChange = UpdateChange();
  if (updatedChange == nullptr || updatedChange->Key == '\0') {
    return;
  }

  for (const auto& [node, change] : Changes) {
    if (change.MenuItem != updatedChange->MenuItem
        && updatedChange->ShortCut == change.ShortCut) {
      String message = ShortCutToText(updatedChange->ShortCut) + L" は、「"
          + change.MenuItem->Caption + L"」ですでに設定されています。";
      if (Application->MessageBox(message.c_str(), CASSAVA_TITLE, MB_OKCANCEL)
          == IDCANCEL){
        AllowChange = false;
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::rgSCKeyClick(TObject *Sender)
{
  int ii = rgSCKey->ItemIndex;
  edSCKey->Enabled = (ii == 1);
  csFNumber->Enabled = (ii == 2);
  udFNumber->Enabled = (ii == 2);
  if((ii == 1) && rgSCKey->Focused()) edSCKey->SetFocus();
  else if((ii == 2) && rgSCKey->Focused()) csFNumber->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::btnOKClick(TObject *Sender)
{
  UpdateChange();
  SaveKey(fmMain->Pref->Path + "AutoKey.csv");
  UpdateMenu();
}
//---------------------------------------------------------------------------
void TfmKey::UpdateMenu()
{
  for (const auto& [node, change] : Changes) {
    if (change.Modified) {
      change.MenuItem->ShortCut = change.ShortCut;
    }
    if (change.Caption != ""){
      change.MenuItem->Caption = change.Caption;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::edSCKeyKeyPress(TObject *Sender, wchar_t &Key)
{
  edSCKey->Text = "";
  if (Key >= L'a' && Key <= L'z') {
    Key -= 32;  // toupper
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::edSelectedChange(TObject *Sender)
{
  auto it = Changes.find(tvMenu->Selected);
  if (it != Changes.end()) {
    it->second.Caption = edSelected->Text;
  }
  tvMenu->Selected->Text = edSelected->Text;
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::btnSaveClick(TObject *Sender)
{
  UpdateChange();

  if (dlgSave->Execute()) {
    String KeyFileName = dlgSave->FileName;
    if (ExtractFileExt(KeyFileName) == "") {
      KeyFileName += ".csv";
    }

    if (SaveKey(KeyFileName)) {
      Application->MessageBox(L"保存しました", CASSAVA_TITLE, 0);
    } else {
      Application->MessageBox(L"保存に失敗しました", CASSAVA_TITLE, 0);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::btnOpenClick(TObject *Sender)
{
  if (dlgOpen->Execute()) {
    String KeyFileName = dlgOpen->FileName;
    if (ExtractFileExt(KeyFileName) == "") {
      KeyFileName += ".csv";
    }
    if (LoadKey(KeyFileName)) {
      Application->MessageBox(
          L"ファイルを読み込みました", CASSAVA_TITLE, 0);
    } else {
      Application->MessageBox(L"読み込みに失敗しました", CASSAVA_TITLE, 0);
    }
  }
}
//---------------------------------------------------------------------------
bool TfmKey::SaveKey(String FileName)
{
  std::unique_ptr<TStringList> File = std::make_unique<TStringList>();
  std::unique_ptr<TStringList> OneRow = std::make_unique<TStringList>();
  File->Add("(Cassava-KeySetting)");

  for (int i = 0; i < tvMenu->Items->Count; i++) {
    const auto& it = Changes.find(tvMenu->Items->Item[i]);
    if (it != Changes.end()){
      const TMenuShortCutChange& change = it->second;
      OneRow->Clear();
      OneRow->Add(ShortCutToText(change.ShortCut));
      OneRow->Add(change.MenuItem->Name);
      OneRow->Add(change.Caption);
      File->Add(OneRow->CommaText);
    }
  }
  try {
    File->SaveToFile(FileName);
  } catch(...) {
    return false;
  }

  return true;
}
//---------------------------------------------------------------------------
bool TfmKey::LoadKey(String FileName)
{
  std::unique_ptr<TStringList> File = std::make_unique<TStringList>();
  std::unique_ptr<TStringList> OneRow = std::make_unique<TStringList>();
  try {
    File->LoadFromFile(FileName);
  } catch(...) {
    return false;
  }

  OneRow->CommaText = File->Strings[0];
  if(OneRow->Strings[0] != "(Cassava-KeySetting)"){
    return false;
  }

  for(int i=1; i<File->Count; i++){
    OneRow->CommaText = File->Strings[i];
    if(OneRow->Count < 2 || OneRow->Strings[1] == "") continue;
    for (auto& [node, change] : Changes) {
      if (change.MenuItem->Name == OneRow->Strings[1]
          || change.MenuItem->Caption == OneRow->Strings[1]) {
        if (OneRow->Strings[0] != "-") {
          change.ShortCut = TextToShortCut(OneRow->Strings[0]);
        }
        if (OneRow->Count > 2 && OneRow->Strings[2] != "") {
          change.Caption = OneRow->Strings[2];
        }
        break;
      }
    }
  }

  return true;
}
//---------------------------------------------------------------------------

