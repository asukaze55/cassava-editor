//---------------------------------------------------------------------------
#include <vcl.h>
#include <registry.hpp>
#pragma hdrstop

#include "OptionFile.h"
#include "MainForm.h"
#include "OptionLaunch.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrOptionFile *frOptionFile;
//---------------------------------------------------------------------------
__fastcall TfrOptionFile::TfrOptionFile(TComponent* Owner)
    : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionFile::RestoreFromMainForm()
{
  cbCheckKanji->Checked = fmMain->MainGrid->CheckKanji;
  cbUseUtf8AsDefault->Checked =
      (fmMain->MainGrid->DefaultCharCode == CHARCODE_UTF8);
  cbNewWindow->Checked = fmMain->MakeNewWindow;
  cbTitleFullPath->Checked = fmMain->TitleFullPath;
  cbLockFile->ItemIndex = fmMain->LockFile;
  cbCheckTimeStamp->Checked = fmMain->CheckTimeStamp;
}
//---------------------------------------------------------------------------
void TfrOptionFile::StoreToMainForm()
{
  fmMain->MainGrid->CheckKanji = cbCheckKanji->Checked;
  fmMain->MainGrid->DefaultCharCode =
      cbUseUtf8AsDefault->Checked ? CHARCODE_UTF8 : CHARCODE_SJIS;
  fmMain->MakeNewWindow = cbNewWindow->Checked;
  fmMain->TitleFullPath = cbTitleFullPath->Checked;
  fmMain->LockFile = cbLockFile->ItemIndex;
  fmMain->CheckTimeStamp = cbCheckTimeStamp->Checked;
}
//---------------------------------------------------------------------------
