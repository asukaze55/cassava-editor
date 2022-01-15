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
static int activeNodeIndex = 0;
//---------------------------------------------------------------------------
__fastcall TfmOption::TfmOption(TComponent* Owner)
	: TForm(Owner)
{
  frOptionDataFormat = new TfrOptionDataFormat(Owner);
  frOptionFile = new TfrOptionFile(Owner);
  frOptionBackUp = new TfrOptionBackUp(Owner);
  frOptionLaunch = new TfrOptionLaunch(Owner);
  frOptionBehavior = new TfrOptionBehavior(Owner);
  frOptionView = new TfrOptionView(Owner);
  frOptionColor = new TfrOptionColor(Owner);
}
//---------------------------------------------------------------------------
TfmOption::~TfmOption()
{
  delete frOptionDataFormat;
  delete frOptionFile;
  delete frOptionBackUp;
  delete frOptionLaunch;
  delete frOptionBehavior;
  delete frOptionView;
  delete frOptionColor;
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
    tnDataFormat = items->Add(NULL, L"�f�[�^�`��");
    tnFile = items->Add(NULL, L"�t�@�C��");
    tnBehavior = items->Add(NULL, L"����");
    tnBackUp = items->Add(NULL, L"�o�b�N�A�b�v");
    tnLaunch = items->Add(NULL, L"�O���A�v���A�g");
    tnView = items->Add(NULL, L"�\��");
    tnColor = items->Add(NULL, L"�F");
  }

  frOptionDataFormat->RestoreFromMainForm();
  frOptionBackUp->RestoreFromMainForm();
  frOptionLaunch->RestoreFromMainForm();
  frOptionBehavior->RestoreFromMainForm();
  frOptionView->RestoreFromMainForm();
  frOptionColor->RestoreFromMainForm();
  frOptionFile->RestoreFromMainForm();

  tvCategory->Selected = tvCategory->Items->Item[activeNodeIndex];
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::tvCategoryChange(TObject *Sender, TTreeNode *Node)
{
  activeNodeIndex = Node->AbsoluteIndex;

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

  lblHeader->Caption = Node->Text;
  lblHeader->Color = clBlack;
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::tvCategoryMouseUp(
    TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button != mbRight) { return; }
  TTreeNode *selected = tvCategory->Selected;
  if (selected->Parent != tnDataFormat /* Not a data format */
      || selected->getNextSibling() == NULL /* "�V�K�쐬" */) {
    return;
  }
  mnDelete->Enabled = selected->getPrevSibling() != NULL; // Not "Default"
  popupMenu->PopupComponent = tvCategory;
  popupMenu->Tag = selected->Index;
  TPoint point = fmOption->ClientToScreen(
      TPoint(X + tvCategory->Left, Y + tvCategory->Top));
  popupMenu->Popup(point.x, point.y);
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::mnApplyToCurrentFileClick(TObject *Sender)
{
  frOptionDataFormat->ApplyToCurrentFile(popupMenu->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::mnDeleteClick(TObject *Sender)
{
  frOptionDataFormat->Delete(popupMenu->Tag);
}
//---------------------------------------------------------------------------
void __fastcall TfmOption::btnOKClick(TObject *Sender)
{
  frOptionDataFormat->StoreDataPage();
  for (int i = 0; i < frOptionDataFormat->TypeList.Count; i++) {
    TTypeOption *p = frOptionDataFormat->TypeList.Items(i);
    if (p->Name == "") {
      Application->MessageBox(
          L"�f�[�^�`�����̐ݒ肳��Ă��Ȃ��f�[�^�`��������܂��B",
          L"Cassava Option", MB_ICONWARNING);
      ModalResult = mrNone;
      return;
    }
    if (p->Exts.size() == 0) {
      Application->MessageBox(
          (p->Name + " �ɕW���g���q���ݒ肳��Ă��܂���B").c_str(),
          L"Cassava Option", MB_ICONWARNING);
      ModalResult = mrNone;
      return;
    }
    if (p->SepChars.Length() == 0) {
      Application->MessageBox(
          (p->Name + " �ɕW����؂蕶�����ݒ肳��Ă��܂���B").c_str(),
          L"Cassava Option", MB_ICONWARNING);
      ModalResult = mrNone;
      return;
    }
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

