//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Option.h"
#include "OptionDataFormat.h"
#include "OptionFile.h"
#include "OptionBackUp.h"
#include "OptionLaunch.h"
#include "OptionBehavior.h"
#include "OptionColor.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmOption *fmOption;
//---------------------------------------------------------------------------
__fastcall TfmOption::TfmOption(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::FormShow(TObject *Sender)
{
  TTreeNodes* items = tvCategory->Items;
  if(items->Count == 0){
    items->Clear();
    frOptionDataFormat->Visible = false;
    frOptionDataFormat->Parent = pnlOption;
    frOptionFile->Visible = false;
    frOptionFile->Parent = pnlOption;
    frOptionBackUp->Visible = false;
    frOptionBackUp->Parent = pnlOption;
    frOptionLaunch->Visible = false;
    frOptionLaunch->Parent = pnlOption;
    frOptionBehavior->Visible = false;
    frOptionBehavior->Parent = pnlOption;
    frOptionColor->Visible = false;
    frOptionColor->Parent = pnlOption;
    tnDataFormat = items->Add(NULL, "データ形式");
    tnFile = items->Add(NULL, "ファイル");
    tnBehavior = items->Add(NULL, "動作");
    tnBackUp = items->Add(NULL, "バックアップ");
    tnLaunch = items->Add(NULL, "外部アプリ連携");
    tnColor = items->Add(NULL, "フォント・色");
  }

  Font = fmMain->Font;
  lblHeader->Font->Size = Font->Size;
  frOptionDataFormat->RestoreFromMainForm();
  frOptionBackUp->RestoreFromMainForm();
  frOptionLaunch->RestoreFromMainForm();
  frOptionBehavior->RestoreFromMainForm();
  frOptionColor->RestoreFromMainForm();
  frOptionFile->RestoreFromMainForm();

  int sizeDiff;
  sizeDiff = frOptionDataFormat->Width - pnlOption->Width;
  if(sizeDiff > 0){
    Width += sizeDiff;
  }
  sizeDiff = frOptionDataFormat->Height - pnlOption->Height;
  if(sizeDiff > 0){
    Height += sizeDiff;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::tvCategoryChange(TObject *Sender, TTreeNode *Node)
{
  TFrame *Show = NULL;
  if(Node->Parent == tnDataFormat){
    frOptionDataFormat->Select(Node->Index);
    Node = tnDataFormat;
  }

  if(Node == tnDataFormat){
    Show = frOptionDataFormat;
  }else if(Node == tnBackUp){
    Show = frOptionBackUp;
  }else if(Node == tnLaunch){
    Show = frOptionLaunch;
  }else if(Node == tnBehavior){
    Show = frOptionBehavior;
  }else if(Node == tnColor){
    Show = frOptionColor;
  }else if(Node == tnFile){
    Show = frOptionFile;
  }
  frOptionDataFormat->Visible = (Show == frOptionDataFormat);
  frOptionBackUp->Visible = (Show == frOptionBackUp);
  frOptionLaunch->Visible = (Show == frOptionLaunch);
  frOptionBehavior->Visible = (Show == frOptionBehavior);
  frOptionColor->Visible = (Show == frOptionColor);
  frOptionFile->Visible = (Show == frOptionFile);

  lblHeader->Caption = (AnsiString)" " + Node->Text;
  lblHeader->Color = clBlack;
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnOKClick(TObject *Sender)
{
  if(frOptionDataFormat->Renaming){
    ModalResult = mrNone;
    frOptionDataFormat->btnRenameClick(this);
    return;
  }else if(Ascii2Ctrl(frOptionDataFormat->edDefSepChar->Text).Length() != 1){
    Application->MessageBox(
      "標準区切り文字は１文字にしてください。",
      "Cassava Option", MB_ICONWARNING);
    ModalResult = mrNone;
    return;
  }

  frOptionDataFormat->StoreToMainForm();
  frOptionBackUp->StoreToMainForm();
  frOptionLaunch->StoreToMainForm();
  frOptionBehavior->StoreToMainForm();
  frOptionColor->StoreToMainForm();
  frOptionFile->StoreToMainForm();
}
//---------------------------------------------------------------------------

