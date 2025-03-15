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
  cbCheckKanji->Checked = fmMain->EncodingDetector.Enabled;
  cbUseUtf8AsDefault->Checked =
      (fmMain->EncodingDetector.DefaultEncoding->CodePage == CODE_PAGE_UTF8);
  cbNewWindow->Checked = fmMain->MakeNewWindow;
  cbTitleFullPath->Checked = fmMain->TitleFullPath;
  cbLockFile->ItemIndex = fmMain->LockFile;
  cbCheckTimeStamp->Checked = fmMain->CheckTimeStamp;
}
//---------------------------------------------------------------------------
void TfrOptionFile::StoreToMainForm()
{
  fmMain->EncodingDetector.Enabled = cbCheckKanji->Checked;
  fmMain->EncodingDetector.DefaultEncoding = cbUseUtf8AsDefault->Checked
      ? TEncoding::UTF8 : TEncoding::GetEncoding(CODE_PAGE_SJIS);
  fmMain->MakeNewWindow = cbNewWindow->Checked;
  fmMain->TitleFullPath = cbTitleFullPath->Checked;
  fmMain->LockFile = cbLockFile->ItemIndex;
  fmMain->CheckTimeStamp = cbCheckTimeStamp->Checked;
}
//---------------------------------------------------------------------------
