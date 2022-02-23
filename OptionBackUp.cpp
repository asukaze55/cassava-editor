//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OptionBackUp.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrOptionBackUp *frOptionBackUp;
//---------------------------------------------------------------------------
__fastcall TfrOptionBackUp::TfrOptionBackUp(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TfrOptionBackUp::RestoreFromMainForm()
{
  cbBackupOnSave->Checked = fmMain->BackupOnSave;
  cbBackupOnTime->Checked = fmMain->BackupOnTime;
  cbBackupOnOpen->Checked = fmMain->BackupOnOpen;
  edBuFileNameS->Text     = fmMain->BuFileNameS;
  edBuFileNameT->Text     = fmMain->BuFileNameT;
  cbDelBuSSaved->Checked  = fmMain->DelBuSSaved;
  cbDelBuSExit->Checked   = fmMain->DelBuSExit;
  cbDelBuT->Checked       = fmMain->DelBuT;
  udBuInterval->Position  = fmMain->BuInterval;

  cbBackupOnSaveClick(nullptr);
  cbBackupOnTimeClick(nullptr);
}
//---------------------------------------------------------------------------
void TfrOptionBackUp::StoreToMainForm()
{
  fmMain->BackupOnSave = cbBackupOnSave->Checked;
  fmMain->BackupOnTime = cbBackupOnTime->Checked;
  fmMain->BackupOnOpen = cbBackupOnOpen->Checked;
  fmMain->BuFileNameS  = edBuFileNameS->Text;
  fmMain->BuFileNameT  = edBuFileNameT->Text;
  fmMain->DelBuSSaved  = cbDelBuSSaved->Checked;
  fmMain->DelBuSExit   = cbDelBuSExit->Checked;
  fmMain->DelBuT       = cbDelBuT->Checked;
  fmMain->BuInterval   = udBuInterval->Position;

  fmMain->tmAutoSaver->Enabled = false;
  if(cbBackupOnTime->Checked){
    fmMain->tmAutoSaver->Interval = fmMain->BuInterval * 60000;
    fmMain->tmAutoSaver->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionBackUp::cbBackupOnSaveClick(TObject *Sender)
{
  bool Chckd = cbBackupOnSave->Checked;
  edBuFileNameS->Enabled = Chckd;
  cbDelBuSSaved->Enabled = Chckd;
  cbDelBuSExit->Enabled = Chckd;
}
//---------------------------------------------------------------------------
void __fastcall TfrOptionBackUp::cbBackupOnTimeClick(TObject *Sender)
{
  bool Chckd = cbBackupOnTime->Checked;
  seBuInterval->Enabled = Chckd;
  udBuInterval->Refresh();
  edBuFileNameT->Enabled = Chckd;
  cbBackupOnOpen->Enabled = Chckd;
  cbDelBuT->Enabled = Chckd;
}
//---------------------------------------------------------------------------

