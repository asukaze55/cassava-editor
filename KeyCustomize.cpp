//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "KeyCustomize.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TfmKey *fmKey;
//---------------------------------------------------------------------------
__fastcall TfmKey::TfmKey(TComponent* Owner)
        : TForm(Owner)
{
  NowMSC = NULL;
}
//---------------------------------------------------------------------------
void TfmKey::AddMenu(TTreeNode* Node, TMenuItem* MenuItem)
{
   if(MenuItem->Caption == "-") return;

   TTreeNode* New =
     tvMenu->Items->AddChild(Node, MenuItem->Caption);
   New->Data = new TMenuShortCut(MenuItem);

   if(MenuItem->Count > 0){
     for(int i=0; i<MenuItem->Count; i++)
       AddMenu(New, MenuItem->Items[i]);
   }
}
//---------------------------------------------------------------------------
void TfmKey::MakeTree()
{
  TTreeNode *Root = tvMenu->Items->Item[0];
  TMenuItem *Menu = fmMain->Menu->Items;
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
void TfmKey::SetMSC(TMenuShortCut *MSC)
{
  if(MSC){
    TShiftState ss;
    String ssString = "";
    if (cbShift->Checked) {
      ss << ssShift;
      ssString += "Shift+";
    }
    if (cbCtrl->Checked) {
      ss << ssCtrl;
      ssString += "Ctrl+";
    }
    if (cbAlt->Checked) {
      ss << ssAlt;
      ssString += "Alt+";
    }
    MSC->Shift = ss;

    switch(rgSCKey->ItemIndex){
      case 1:
        if(edSCKey->Text.Length() >= 1){
          MSC->MShortCut = TextToShortCut(ssString + edSCKey->Text);
        }
        else MSC->Key = '\0';
        break;
      case 2:
        MSC->Key = static_cast<Word>(VK_F1 + udFNumber->Position - 1);
        break;
      case 3: MSC->Key = VK_RETURN; break;
      case 4: MSC->Key = VK_SPACE; break;
      case 5: MSC->Key = VK_INSERT; break;
      case 6: MSC->Key = VK_DELETE; break;
      case 7: MSC->Key = VK_BACK; break;
      default: MSC->Key = '\0'; break;
    }
  }

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
  NowMSC = NULL;

  Word SCKey;
  TShiftState SCShift;
  stUseSC->Visible = false;
  edSelected->Text = Node->Text;

  if(!Node->Data || Node->Count>0){
    SCKey = '\0';
    rgSCKey->Enabled = false;
  } else {
    NowMSC = static_cast<TMenuShortCut*>(Node->Data);
    SCKey = NowMSC->Key;
    SCShift = NowMSC->Shift;
    rgSCKey->Enabled = true;
    if(NowMSC->Modified){
      stUseSC->Caption = "★";
      stUseSC->Visible = true;
    } else {
      stUseSC->Caption = "☆";
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
      String text = ShortCutToText(NowMSC->MShortCut);
      int index = text.LastDelimiter("+");
      edSCKey->Text = text.SubString(index + 1, text.Length() - index);
    } else if(ii == 2){
      csFNumber->Enabled = true;
      udFNumber->Enabled = true;
      csFNumber->Text = AnsiString(static_cast<long>(SCKey - VK_F1) + 1);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::tvMenuChanging(TObject *Sender, TTreeNode *Node,
      bool &AllowChange)
{
  if(!NowMSC) return;
  SetMSC(NowMSC);
  if(NowMSC->Key == '\0') return;

  for(int i=0; i<tvMenu->Items->Count; i++){
    if(tvMenu->Items->Item[i]->Data){
      TMenuShortCut* MSC
        = static_cast<TMenuShortCut*>(tvMenu->Items->Item[i]->Data);
      if(MSC != NowMSC && NowMSC->MShortCut == MSC->MShortCut){
        if(Application->MessageBox(
             (ShortCutToText(NowMSC->MShortCut) + " は、「" +
             MSC->MenuItem->Caption +
             "」ですでに設定されています。").c_str(),
             TEXT("Cassava"),
             MB_OKCANCEL) == IDCANCEL){
          AllowChange = false;
        }
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
  if(NowMSC) SetMSC(NowMSC);
  SaveKey(fmMain->Pref->Path + "AutoKey.csv");
  MenuUpDate();
}
//---------------------------------------------------------------------------
void TfmKey::MenuUpDate()
{
  for(int i=0; i<tvMenu->Items->Count; i++){
    if(tvMenu->Items->Item[i]->Data){
      TMenuShortCut *MSC
        = static_cast<TMenuShortCut*>(tvMenu->Items->Item[i]->Data);
      if(MSC->Modified){
        MSC->MenuItem->ShortCut = MSC->MShortCut;
      }
      if(MSC->Caption != ""){
        MSC->MenuItem->Caption = MSC->Caption;
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::edSCKeyKeyPress(TObject *Sender, char &Key)
{
  edSCKey->Text = "";
  if(Key >= 'a' && Key <= 'z')
    Key -= (char)32u; // toupper
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::tvMenuEdited(TObject *Sender, TTreeNode *Node,
      AnsiString &S)
{
  static_cast<TMenuShortCut*>(Node->Data)->Caption = S;
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::edSelectedChange(TObject *Sender)
{
  if(NowMSC){
    NowMSC->Caption = edSelected->Text;
  }else if(tvMenu->Selected->Data){
    static_cast<TMenuShortCut*>(tvMenu->Selected->Data)->Caption
      = edSelected->Text;
  }
  tvMenu->Selected->Text = edSelected->Text;
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::btnSaveClick(TObject *Sender)
{
  if(NowMSC) SetMSC(NowMSC);

  if(dlgSave->Execute())
  {
    AnsiString KeyFileName = dlgSave->FileName;
    if(ExtractFileExt(KeyFileName) == "") KeyFileName += ".csv";
    if(SaveKey(KeyFileName))
	  Application->MessageBox(TEXT("保存しました"), TEXT("Cassava"),0);
	else
      Application->MessageBox(TEXT("保存に失敗しました"), TEXT("Cassava"),0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmKey::btnOpenClick(TObject *Sender)
{
  if(dlgOpen->Execute())
  {
    AnsiString KeyFileName = dlgOpen->FileName;
    if(ExtractFileExt(KeyFileName) == "") KeyFileName += ".csv";
    if(LoadKey(KeyFileName))
	  Application->MessageBox(TEXT("ファイルを読み込みました"), TEXT("Cassava"),0);
	else
	  Application->MessageBox(TEXT("読み込みに失敗しました"), TEXT("Cassava"),0);
  }
}
//---------------------------------------------------------------------------
bool TfmKey::SaveKey(AnsiString FileName)
{
  TStringList *File = new TStringList;
  TStringList *OneRow = new TStringList;
  File->Add("(Cassava-KeySetting)");

  for(int i=0; i<tvMenu->Items->Count; i++){
    if(tvMenu->Items->Item[i]->Data){
      TMenuShortCut *MSC
        = static_cast<TMenuShortCut*>(tvMenu->Items->Item[i]->Data);
      OneRow->Clear();
      OneRow->Add(ShortCutToText(MSC->MShortCut));
      if(MSC->MenuItem->Name != ""){
        OneRow->Add(MSC->MenuItem->Name);
      }else{
        AnsiString str = MSC->Caption;
        int L = str.Length();
        if(L > 4 && str[L-3] == '(' && str[L-2] == '&' && str[L] == ')'){
          str.SetLength(L-4);
        }
      }
      OneRow->Add(MSC->Caption);
      File->Add(OneRow->CommaText);
    }
  }
  try{
    File->SaveToFile(FileName);
  }catch(...){ delete File; delete OneRow; return false; }

  delete File;
  delete OneRow;
  return true;
}
//---------------------------------------------------------------------------
bool TfmKey::LoadKey(AnsiString FileName)
{
  TStringList *File = new TStringList;
  TStringList *OneRow = new TStringList;
  try{
    File->LoadFromFile(FileName);
  }catch(...){ delete File; delete OneRow; return false; }

  OneRow->CommaText = File->Strings[0];
  if(OneRow->Strings[0] != "(Cassava-KeySetting)"){
    delete File; delete OneRow; return false;
  }

  for(int i=1; i<File->Count; i++){
    OneRow->CommaText = File->Strings[i];
    if(OneRow->Count < 2 || OneRow->Strings[1] == "") continue;
    TMenuShortCut *MSC;
    for(int p=1; p<tvMenu->Items->Count; p++){
      MSC = static_cast<TMenuShortCut*>(tvMenu->Items->Item[p]->Data);
      if(MSC->MenuItem->Name == OneRow->Strings[1] ||
         MSC->MenuItem->Caption == OneRow->Strings[1]){
        if(OneRow->Strings[0] != "-")
          MSC->MShortCut = TextToShortCut(OneRow->Strings[0]);
        if(OneRow->Count > 2 && OneRow->Strings[2] != "")
          MSC->Caption = OneRow->Strings[2];
        break;
      }
    }
  }

  delete File;
  delete OneRow;
  return true;
}
//---------------------------------------------------------------------------

