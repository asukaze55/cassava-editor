//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Option.h"
#include "OptionDataFormat.h"
#include "OptionFile.h"
#include "OptionBackUp.h"
#include "OptionLaunch.h"
#include "OptionBehavior.h"
#include "OptionView.h"
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
    frOptionView->Visible = false;
    frOptionView->Parent = pnlOption;
    frOptionColor->Visible = false;
    frOptionColor->Parent = pnlOption;
    tnDataFormat = items->Add(NULL, "�f�[�^�`��");
    tnFile = items->Add(NULL, "�t�@�C��");
    tnBehavior = items->Add(NULL, "����");
    tnBackUp = items->Add(NULL, "�o�b�N�A�b�v");
    tnLaunch = items->Add(NULL, "�O���A�v���A�g");
    tnView = items->Add(NULL, "�\��");
    tnColor = items->Add(NULL, "�F");
  }

  frOptionDataFormat->RestoreFromMainForm();
  frOptionBackUp->RestoreFromMainForm();
  frOptionLaunch->RestoreFromMainForm();
  frOptionBehavior->RestoreFromMainForm();
  frOptionView->RestoreFromMainForm();
  frOptionColor->RestoreFromMainForm();
  frOptionFile->RestoreFromMainForm();
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
  }else if(Node == tnView){
    Show = frOptionView;
  }else if(Node == tnColor){
    Show = frOptionColor;
  }else if(Node == tnFile){
    Show = frOptionFile;
  }
  frOptionDataFormat->Visible = (Show == frOptionDataFormat);
  frOptionBackUp->Visible = (Show == frOptionBackUp);
  frOptionLaunch->Visible = (Show == frOptionLaunch);
  frOptionBehavior->Visible = (Show == frOptionBehavior);
  frOptionView->Visible = (Show == frOptionView);
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
	  TEXT("�W����؂蕶���͂P�����ɂ��Ă��������B"),
      TEXT("Cassava Option"), MB_ICONWARNING);
    ModalResult = mrNone;
    return;
  }

  frOptionDataFormat->StoreToMainForm();
  frOptionBackUp->StoreToMainForm();
  frOptionLaunch->StoreToMainForm();
  frOptionBehavior->StoreToMainForm();
  frOptionView->StoreToMainForm();
  frOptionColor->StoreToMainForm();
  frOptionFile->StoreToMainForm();
}
//---------------------------------------------------------------------------

