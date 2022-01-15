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
    tnDataFormat = items->Add(NULL, "データ形式");
    tnFile = items->Add(NULL, "ファイル");
    tnBehavior = items->Add(NULL, "動作");
    tnBackUp = items->Add(NULL, "バックアップ");
    tnLaunch = items->Add(NULL, "外部アプリ連携");
    tnView = items->Add(NULL, "表示");
    tnColor = items->Add(NULL, "色");
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
      || selected->getNextSibling() == NULL /* "新規作成" */) {
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
          TEXT("データ形式名の設定されていないデータ形式があります。"),
          TEXT("Cassava Option"), MB_ICONWARNING);
      ModalResult = mrNone;
      return;
    }
    if (p->Exts->Count == 0) {
      Application->MessageBox(
          (p->Name + " に標準拡張子が設定されていません。").c_str(),
          TEXT("Cassava Option"), MB_ICONWARNING);
      ModalResult = mrNone;
      return;
    }
    if (p->SepChars.Length() == 0) {
      Application->MessageBox(
          (p->Name + " に標準区切り文字が設定されていません。").c_str(),
          TEXT("Cassava Option"), MB_ICONWARNING);
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

