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
  Font = fmMain->Font;

  cbCheckKanji->Checked = fmMain->MainGrid->CheckKanji;
  cbUnicodeWindowsMapping->Checked = fmMain->MainGrid->UnicodeWindowsMapping;
  cbNewWindow->Checked = fmMain->MakeNewWindow;
  cbTitleFullPath->Checked = fmMain->TitleFullPath;
  cbLockFile->ItemIndex = fmMain->LockFile;
  cbCheckTimeStamp->Checked = fmMain->CheckTimeStamp;

  TRegistry *Reg = new TRegistry;
  Reg->RootKey = HKEY_CLASSES_ROOT;
  if(Reg->OpenKeyReadOnly("\\.csv")){
    CSVFileType = Reg->ReadString("");
    KanrenCSVNew = Reg->KeyExists("\\.csv\\ShellNew");
    Reg->CloseKey();
  }else{ CSVFileType = ""; }
  if(Reg->OpenKeyReadOnly("\\.tsv")){
    TSVFileType = Reg->ReadString("");
    KanrenTSVNew = Reg->KeyExists("\\.tsv\\ShellNew");
    Reg->CloseKey();
  }else{ TSVFileType = ""; }
  KanrenCSV = (CSVFileType == "Cassava.CSV");
  KanrenTSV = (TSVFileType == "Cassava.TSV");
  cbKanrenCSV->Checked = KanrenCSV;
  cbKanrenCSVNew->Checked = KanrenCSVNew;
  cbKanrenTSV->Checked = KanrenTSV;
  cbKanrenTSVNew->Checked = KanrenTSVNew;
  frOptionLaunch->btnKanrenR->Enabled = (KanrenCSV || KanrenTSV);
  delete Reg;
}
//---------------------------------------------------------------------------
void TfrOptionFile::StoreToMainForm()
{
  fmMain->MainGrid->CheckKanji = cbCheckKanji->Checked;
  fmMain->MainGrid->UnicodeWindowsMapping = cbUnicodeWindowsMapping->Checked;
  fmMain->MakeNewWindow = cbNewWindow->Checked;
  fmMain->TitleFullPath = cbTitleFullPath->Checked;
  fmMain->LockFile = cbLockFile->ItemIndex;
  fmMain->CheckTimeStamp = cbCheckTimeStamp->Checked;

  SetKanren("Cassava.CSV", "CSV ファイル", ".csv",
            CSVFileType, KanrenCSV, KanrenCSVNew,
            cbKanrenCSV->Checked, cbKanrenCSVNew->Checked);
  SetKanren("Cassava.TSV", "TSV ファイル", ".tsv",
            TSVFileType, KanrenTSV, KanrenTSVNew,
            cbKanrenTSV->Checked, cbKanrenTSVNew->Checked);
}
//---------------------------------------------------------------------------
void TfrOptionFile::SetKanren(char *CassavaType, char *CassavaTypeName,
        char *Ext, AnsiString OldFileType, bool OldKanren,
        bool OldKanrenNew, bool NewKanren, bool NewKanrenNew)
{
  if(NewKanren != OldKanren){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CURRENT_USER;
    if(NewKanren){
      if(Application->MessageBox("関連付けを設定します", CassavaType,
                                 MB_OKCANCEL)==IDOK){
        Reg->OpenKey("\\Software", true);
        Reg->OpenKey("Classes", true);
        Reg->OpenKey(CassavaType, true);
        Reg->WriteString("", CassavaTypeName);
        Reg->WriteString("OldType", OldFileType);
        Reg->OpenKey("shell",true);
        Reg->OpenKey("open",true);
        Reg->WriteString("", "Cassava Editor で開く");
        Reg->OpenKey("command",true);
        Reg->WriteString("", (AnsiString)'\"'+ParamStr(0)+"\" \"%1\"");
        Reg->CloseKey();

        Reg->OpenKey("\\Software", true);
        Reg->OpenKey("Classes", true);
        Reg->OpenKey(CassavaType, true);
        Reg->OpenKey("DefaultIcon",true);
        Reg->WriteString("", ParamStr(0)+",0");
        Reg->CloseKey();

        Reg->OpenKey("\\Software", true);
        Reg->OpenKey("Classes", true);
        Reg->OpenKey(Ext, true);
        Reg->WriteString("", CassavaType);
        Reg->CloseKey();
      }
    }else{
      if(Application->MessageBox("関連付けを解除します", CassavaType,
                                 MB_OKCANCEL)==IDOK){
        AnsiString FileType = "";
        Reg->OpenKey("\\Software", false);
        Reg->OpenKey("Classes", false);
        if(Reg->OpenKey(CassavaType, false)){
          FileType = Reg->ReadString("OldType");
          Reg->CloseKey();
          Reg->DeleteKey((AnsiString)"\\Software\\Classes\\" + CassavaType);
        }
        if(FileType==""){
          Reg->DeleteKey((AnsiString)"\\Software\\Classes\\" + Ext);
        }else{
          Reg->OpenKey((AnsiString)"\\Software\\Classes\\" + Ext, true);
          Reg->WriteString("", FileType);
          Reg->CloseKey();
        }
      }
    }
    delete Reg;
  }

  if(NewKanrenNew != OldKanrenNew){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CURRENT_USER;
    if(NewKanrenNew){
      if(Application->MessageBox("新規作成に追加します", CassavaType,
                                 MB_OKCANCEL)==IDOK){
        Reg->OpenKey((AnsiString)"\\Software\\Classes\\" + Ext + "\\ShellNew",
                     true);
        Reg->WriteString("NullFile", "");
        Reg->CloseKey();
      }
    }else if(Reg->KeyExists((AnsiString)"\\Software\\Classes\\" + Ext
             + "\\ShellNew")){
      if(Application->MessageBox("新規作成を解除します", CassavaType,
                                 MB_OKCANCEL)==IDOK){
          Reg->DeleteKey((AnsiString)"\\Software\\Classes\\" + Ext
                         + "\\ShellNew");
      }
    }
    delete Reg;
  }

  PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment");
}
//---------------------------------------------------------------------------
