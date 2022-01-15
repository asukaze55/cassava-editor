//---------------------------------------------------------------------------
#include <vcl.h>
#include <registry.hpp>
#pragma hdrstop

#include "Option.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TfmOption *fmOption;
TTypeList TypeList;
//---------------------------------------------------------------------------
__fastcall TfmOption::TfmOption(TComponent* Owner)
        : TForm(Owner)
{
  Renaming = false;
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::FormShow(TObject *Sender)
{
  Renaming = true;
  TypeList = fmMain->MainGrid->TypeList;
  cbType->Items->Clear();
  for(int i=0; i<TypeList.Count; i++){
    cbType->Items->Add(TypeList.Items(i)->Name);
  }
  TypeIndex = fmMain->MainGrid->TypeIndex;
  cbType->ItemIndex = TypeIndex;
  Renaming = false;
  RestoreDataPage(TypeIndex);

//  edSepChars->Text = fmMain->SepCharsAscii;
//  cbSepUseTab->Checked = fmMain->SepUseTab;
//  cbSepUseSpace->Checked = fmMain->SepUseSpace;
//  cbUseQuote->Checked = (fmMain->MainGrid->QuoteChars == "\"");
//  rgSaveQuote->ItemIndex = fmMain->MainGrid->SaveOption;
//  cbCommaRect->Checked = !(fmMain->MainGrid->OmitComma);
//  cbForceExt->Checked = fmMain->ForceExt;
  cbJis2000->Checked = fmMain->MainGrid->CheckKanji;
  cbDragMove->ItemIndex = (fmMain->MainGrid->DragMove==true) ? 0 : 1;
  cbEnterMove->ItemIndex = fmMain->MainGrid->EnterMove;
  cbLeftArrowInCell->Checked = fmMain->MainGrid->LeftArrowInCell;
  cbWheelMoveCursol->Checked = fmMain->MainGrid->WheelMoveCursol;
  cbAlwaysShowEditor->Checked = fmMain->MainGrid->AlwaysShowEditor;
  rgUseURL->ItemIndex = fmMain->MainGrid->DblClickOpenURL;
  cbNewWindow->Checked = fmMain->MakeNewWindow;
  cbTitleFullPath->Checked = fmMain->TitleFullPath;
  cbCheckTimeStamp->Checked = fmMain->CheckTimeStamp;
  cbStopMacro->Checked = (fmMain->StopMacroCount > 0);
  seStopMacroCount->Value = (cbStopMacro->Checked ? fmMain->StopMacroCount : 100000);
  seStopMacroCount->Enabled = cbStopMacro->Checked;

  cbBackupOnSave->Checked = fmMain->BackupOnSave;
  cbBackupOnTime->Checked = fmMain->BackupOnTime;
  cbBackupOnOpen->Checked = fmMain->BackupOnOpen;
  edBuFileNameS->Text     = fmMain->BuFileNameS;
  edBuFileNameT->Text     = fmMain->BuFileNameT;
  cbDelBuS->Checked       = fmMain->DelBuS;
  cbDelBuT->Checked       = fmMain->DelBuT;
  seBuInterval->Value     = fmMain->BuInterval;

  Font = fmMain->Font;
  stFont->Font = fmMain->MainGrid->Font;
  stFont->Color = fmMain->MainGrid->Color;
  lbFixColor->Color = fmMain->MainGrid->FixedColor;

  edLaunch0->Text     = fmMain->mnAppli0->Hint;
  edLaunchName0->Text = fmMain->mnAppli0->Caption.c_str() + 4;
  edLaunch1->Text     = fmMain->mnAppli1->Hint;
  edLaunchName1->Text = fmMain->mnAppli1->Caption.c_str() + 4;
  edLaunch2->Text     = fmMain->mnAppli2->Hint;
  edLaunchName2->Text = fmMain->mnAppli2->Caption.c_str() + 4;
  edBrowser->Text     = fmMain->MainGrid->BrowserFileName;

  OSVERSIONINFO osVer; 
  osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
  ::GetVersionEx( &osVer ); 
 
  if(osVer.dwMajorVersion < 6){ 
    TRegistry *Reg = new TRegistry; 
    Reg->RootKey = HKEY_CLASSES_ROOT; 
    if(Reg->OpenKeyReadOnly("\\.csv")){ 
      FileType = Reg->ReadString(""); 
      KanrenNew = Reg->KeyExists("\\.csv\\ShellNew"); 
      Reg->CloseKey(); 
    }else{ FileType = ""; } 
    Kanren = (FileType == "Cassava.CSV"); 
    cbKanren->Checked = Kanren; 
    cbKanrenNew->Checked = KanrenNew; 
    btnKanrenR->Enabled = Kanren;
    delete Reg; 
  }else{ 
    // Vistaは未対応 
    FileType = ""; 
    Kanren = false; 
    KanrenNew = false; 
    cbKanren->Checked = Kanren; 
    cbKanren->Enabled = false; 
    cbKanrenNew->Checked = KanrenNew; 
    cbKanrenNew->Enabled = false; 
    gbKanren->Caption = "関連付け (Windows Vista 未対応)"; 
    btnKanrenR->Enabled = Kanren; 
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnOKClick(TObject *Sender)
{
  if(Renaming){
    ModalResult = mrNone;
    btnRenameClick(this);
    return;
  }else if(Ascii2Ctrl(edDefSepChar->Text).Length() != 1){
    Application->MessageBox(
      "標準区切り文字は１文字にしてください。",
      "Cassava Option", MB_ICONWARNING);
    ModalResult = mrNone;
    return;
  }
  StoreDataPage();
  fmMain->MainGrid->TypeList = TypeList;
  fmMain->MainGrid->TypeIndex = TypeIndex;
  fmMain->MainGrid->TypeOption = fmMain->MainGrid->TypeList.Items(TypeIndex);
  fmMain->SetFilter();
  fmMain->SetPasteMenu(fmMain->mnPasteFormat);
  fmMain->SetPasteMenu(fmMain->mnpPasteFormat);

//  fmMain->SepCharsAscii = edSepChars->Text;
//  fmMain->SepUseTab = cbSepUseTab->Checked;
//  fmMain->SepUseSpace = cbSepUseSpace->Checked;
//  fmMain->MainGrid->SepChars = edDefSepChar->Text + edSepChars->Text;
//  fmMain->MainGrid->WeakSepChars = edWeakSepChars->Text;
//  fmMain->MainGrid->QuoteChars = (rgSaveQuote->ItemIndex > 0) ? "\"" : "";
//  fmMain->MainGrid->SaveOption = rgSaveQuote->ItemIndex;
//  fmMain->MainGrid->OmitComma = !(cbCommaRect->Checked);
//  fmMain->ForceExt = cbForceExt->Checked;
  fmMain->MainGrid->CheckKanji = cbJis2000->Checked;
  fmMain->MainGrid->DragMove = (cbDragMove->ItemIndex == 0);
  fmMain->MainGrid->EnterMove = cbEnterMove->ItemIndex;
  fmMain->MainGrid->LeftArrowInCell = cbLeftArrowInCell->Checked;
  fmMain->MainGrid->WheelMoveCursol = cbWheelMoveCursol->Checked;
  fmMain->MainGrid->AlwaysShowEditor = cbAlwaysShowEditor->Checked;
  fmMain->MainGrid->ShowURLBlue = rgUseURL->ItemIndex;
  fmMain->MainGrid->DblClickOpenURL = rgUseURL->ItemIndex;

  fmMain->MakeNewWindow = cbNewWindow->Checked;
  fmMain->TitleFullPath = cbTitleFullPath->Checked;
  fmMain->CheckTimeStamp = cbCheckTimeStamp->Checked;
  if(cbStopMacro->Checked){
    fmMain->StopMacroCount = seStopMacroCount->Value;
  }else{
    fmMain->StopMacroCount = 0;
  }

  fmMain->BackupOnSave = cbBackupOnSave->Checked;
  fmMain->BackupOnTime = cbBackupOnTime->Checked;
  fmMain->BackupOnOpen = cbBackupOnOpen->Checked;
  fmMain->BuFileNameS  = edBuFileNameS->Text;
  fmMain->BuFileNameT  = edBuFileNameT->Text;
  fmMain->DelBuS       = cbDelBuS->Checked;
  fmMain->DelBuT       = cbDelBuT->Checked;
  fmMain->BuInterval   = seBuInterval->Value;

  fmMain->tmAutoSaver->Enabled = false;
  if(cbBackupOnTime->Checked){
    fmMain->tmAutoSaver->Interval = seBuInterval->Value * 60000;
    fmMain->tmAutoSaver->Enabled = true;
  }

  if(stFont->Font->Size <= 12){
    fmMain->Font->Size = stFont->Font->Size;
  }
  fmMain->MainGrid->Font = stFont->Font;
  fmMain->MainGrid->DefaultRowHeight = fmMain->MainGrid->Font->Size * Screen->PixelsPerInch / 48;
  fmMain->MainGrid->Color = stFont->Color;
  fmMain->MainGrid->InplaceEditor->Brush->Color = stFont->Color;
  fmMain->MainGrid->FixedColor = lbFixColor->Color;

  fmMain->mnAppli0->Caption = (AnsiString)"&0: " + edLaunchName0->Text;
  fmMain->mnAppli0->Hint    = edLaunch0->Text;
  fmMain->mnAppli0->Enabled = (edLaunch0->Text != "");
  fmMain->mnAppli1->Caption = (AnsiString)"&1: " + edLaunchName1->Text;
  fmMain->mnAppli1->Hint    = edLaunch1->Text;
  fmMain->mnAppli1->Enabled = (edLaunch1->Text != "");
  fmMain->mnAppli2->Caption = (AnsiString)"&2: " + edLaunchName2->Text;
  fmMain->mnAppli2->Hint    = edLaunch2->Text;
  fmMain->mnAppli2->Enabled = (edLaunch2->Text != "");
  fmMain->MainGrid->BrowserFileName = edBrowser->Text;

  SetKanren();
}
//---------------------------------------------------------------------------
void TfmOption::StoreDataPage()
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
void TfmOption::RestoreDataPage(int id)
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
void TfmOption::SetKanren()
{
  if(cbKanren->Checked != Kanren){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CLASSES_ROOT;
    if(cbKanren->Checked){
      if(Application->MessageBox("関連付けを設定します","Cassava.CSV",
                                 MB_OKCANCEL)==IDOK){
        Reg->OpenKey("\\Cassava.CSV",true);
        Reg->WriteString("", "CSV ファイル");
        Reg->WriteString("OldType", FileType);
        Reg->OpenKey("shell",true);
        Reg->OpenKey("open",true);
        Reg->WriteString("", "Cassava");
        Reg->OpenKey("command",true);
        Reg->WriteString("", (AnsiString)'\"'+ParamStr(0)+"\" \"%1\"");
        Reg->OpenKey("\\Cassava.CSV\\DefaultIcon",true);
        Reg->WriteString("", ParamStr(0)+",0");
        Reg->CloseKey();
        Reg->OpenKey("\\.csv",true);
        Reg->WriteString("", "Cassava.CSV");
        Reg->CloseKey();
      }
    }else{
      if(Application->MessageBox("関連付けを解除します","Cassava.CSV",
                                 MB_OKCANCEL)==IDOK){
        FileType = "";
        if(Reg->OpenKeyReadOnly("\\Cassava.CSV")){
          FileType = Reg->ReadString("OldType");
          Reg->CloseKey();
          Reg->DeleteKey("\\Cassava.CSV");
        }
        if(FileType==""){
          Reg->DeleteKey("\\.csv");
        }else{
          Reg->OpenKey("\\.csv",true);
          Reg->WriteString("", FileType);
          Reg->CloseKey();
        }
      }
    }
    delete Reg;
  }

  if(cbKanrenNew->Checked != KanrenNew){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CLASSES_ROOT;
    if(cbKanrenNew->Checked){
      if(Application->MessageBox("新規作成に追加します","Cassava.CSV",
                                 MB_OKCANCEL)==IDOK){
        Reg->OpenKey("\\.csv\\ShellNew",true);
        Reg->WriteString("NullFile", "");
        Reg->CloseKey();
      }
    }else if(Reg->KeyExists("\\.csv\\ShellNew")){
      if(Application->MessageBox("新規作成を解除します","Cassava.CSV",
                                 MB_OKCANCEL)==IDOK){
          Reg->DeleteKey("\\.csv\\ShellNew");
      }
    }
    delete Reg;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnFontClick(TObject *Sender)
{
  dlgFont->Font = stFont->Font;
  if(dlgFont->Execute()){
    stFont->Font = dlgFont->Font;
    lbFixColor->Font->Color = stFont->Font->Color;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnFgColorClick(TObject *Sender)
{
  dlgColor->Color = stFont->Font->Color;
  if(dlgColor->Execute()){
    stFont->Font->Color = dlgColor->Color;
    lbFixColor->Font->Color = dlgColor->Color;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnBgColorClick(TObject *Sender)
{
  dlgColor->Color = stFont->Color;
  if(dlgColor->Execute()){
    stFont->Color = dlgColor->Color;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnFixColorClick(TObject *Sender)
{
  dlgColor->Color = lbFixColor->Color;
  if(dlgColor->Execute()){
    lbFixColor->Color = dlgColor->Color;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::edLaunchChange(TObject *Sender)
{
  TEdit *Ed = static_cast<TEdit*>(Sender);
  int Tag = Ed->Tag;
  TEdit *LName;
  switch(Tag){
    case 0: LName = edLaunchName0; break;
    case 1: LName = edLaunchName1; break;
    case 2: LName = edLaunchName2; break;
  }

  if(Ed->Text==""){
    LName->Enabled = false;
    LName->Text = "未設定";
  }else{
    LName->Text = ChangeFileExt(ExtractFileName(Ed->Text), "");
    LName->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::sbReferClick(TObject *Sender)
{
  TSpeedButton *SB = static_cast<TSpeedButton*>(Sender);
  int Tag = SB->Tag;
  TEdit *Launch;
  switch(Tag){
    case 0: Launch = edLaunch0; break;
    case 1: Launch = edLaunch1; break;
    case 2: Launch = edLaunch2; break;
    case 3: Launch = edBrowser; break;
  }

  dlgOpen->FileName = Launch->Text;
  if(dlgOpen->Execute()){
    Launch->Text = dlgOpen->FileName;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnKanrenRClick(TObject *Sender)
{
  if(Application->MessageBox("関連付けを設定します","Cassava.CSV",
                             MB_OKCANCEL)==IDOK){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CLASSES_ROOT;
    Reg->DeleteKey("\\Cassava.CSV\\shell");
    Reg->OpenKey("\\Cassava.CSV\\shell",true);
      Reg->OpenKey("open",true);
      Reg->WriteString("", "Cassava");
      Reg->OpenKey("command",true);
      Reg->WriteString("", (AnsiString)'\"'+ParamStr(0)+"\" \"%1\"");
    for(int i=0; i<3; i++){
      AnsiString Nm, Lc;
      switch(i){
        case 0: Nm = edLaunchName0->Text; Lc = edLaunch0->Text; break;
        case 1: Nm = edLaunchName1->Text; Lc = edLaunch1->Text; break;
        case 2: Nm = edLaunchName2->Text; Lc = edLaunch2->Text; break;
      }
      if(Lc != ""){
        Reg->OpenKey("\\Cassava.CSV\\shell",true);
        Reg->OpenKey(Nm,true);
        Reg->OpenKey("command",true);
        Reg->WriteString("", (AnsiString)'\"'+Lc+"\" \"%1\"");
      }
    }
    Reg->CloseKey();

    delete Reg;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::cbBackupOnSaveClick(TObject *Sender)
{
  bool Chckd = cbBackupOnSave->Checked;
  edBuFileNameS->Enabled = Chckd;
  cbDelBuS->Enabled = Chckd;
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::cbBackupOnTimeClick(TObject *Sender)
{
  bool Chckd = cbBackupOnTime->Checked;
  seBuInterval->Enabled = Chckd;
  edBuFileNameT->Enabled = Chckd;
  cbBackupOnOpen->Enabled = Chckd;
  cbDelBuT->Enabled = Chckd;
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::cbTypeChange(TObject *Sender)
{
  if(!Renaming){
    StoreDataPage();
    RestoreDataPage(cbType->ItemIndex);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnRenameClick(TObject *Sender)
{
  if(Renaming){
    cbType->Items->Strings[TypeIndex] = cbType->Text;
    cbType->Style = csDropDownList;
    cbType->ItemIndex = TypeIndex;
    btnRename->Caption = "名前変更";
    btnNewType->Caption = "新規作成";
    btnDeleteType->Enabled = true;
    Renaming = false;
  }else{
    Renaming = true;
    cbType->Style = csDropDown;
    cbType->SetFocus();
    btnRename->Caption = "名前確定";
    btnNewType->Caption = "キャンセル";
    btnDeleteType->Enabled = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnNewTypeClick(TObject *Sender)
{
  if(Renaming){
    cbType->Style = csDropDownList;
    cbType->ItemIndex = TypeIndex;
    btnRename->Caption = "名前変更";
    btnNewType->Caption = "新規作成";
    btnDeleteType->Enabled = true;
    Renaming = false;
  }else{
    StoreDataPage();
    Renaming = true;
    TypeList.Add(new TTypeOption());
    TypeIndex = cbType->Items->Add("[新規]");
    cbType->ItemIndex = TypeIndex;
    cbType->Style = csDropDown;
    cbType->SetFocus();
    btnRename->Caption = "名前確定";
    btnRename->Enabled = true;
    btnNewType->Caption = "キャンセル";
    btnDeleteType->Enabled = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnDeleteTypeClick(TObject *Sender)
{
  if(!Renaming){
    int dt = TypeIndex;
    if(dt == 0) return;
    cbType->ItemIndex = dt - 1;
    RestoreDataPage(dt - 1);
    cbType->Items->Delete(dt);
    TypeList.Delete(dt);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::edDefSepCharChange(TObject *Sender)
{
  AnsiString Sep = edDefSepChar->Text;
  if(Sep.Length() > 0 && Sep[1] == '&'){ Sep = (AnsiString)"&" + Sep; }
  cbCommaRect->Caption = "セーブ時に各行の「" + Sep + "」の数を統一する";
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::cbCommaRectClick(TObject *Sender)
{
  cbDummyEOF->Enabled = cbCommaRect->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::cbCommaRectKeyUpDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  cbDummyEOF->Enabled = cbCommaRect->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfmOption::cbCommaRectKeyPress(TObject *Sender, char &Key)
{
  cbDummyEOF->Enabled = cbCommaRect->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::cbStopMacroClick(TObject *Sender)
{
  seStopMacroCount->Enabled = cbStopMacro->Checked;
}
//---------------------------------------------------------------------------

