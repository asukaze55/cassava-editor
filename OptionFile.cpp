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

  SetKanren(TEXT("Cassava.CSV"), TEXT("CSV ファイル"), TEXT(".csv"),
			CSVFileType, KanrenCSV, KanrenCSVNew,
			cbKanrenCSV->Checked, cbKanrenCSVNew->Checked);
  SetKanren(TEXT("Cassava.TSV"), TEXT("TSV ファイル"), TEXT(".tsv"),
            TSVFileType, KanrenTSV, KanrenTSVNew,
            cbKanrenTSV->Checked, cbKanrenTSVNew->Checked);
}
//---------------------------------------------------------------------------
void TfrOptionFile::SetKanren(TCHAR *CassavaType, TCHAR *CassavaTypeName,
		TCHAR *Ext, String OldFileType, bool OldKanren,
		bool OldKanrenNew, bool NewKanren, bool NewKanrenNew)
{
  if(NewKanren != OldKanren){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CURRENT_USER;
    if(NewKanren){
      if(Application->MessageBox(TEXT("関連付けを設定します"), CassavaType,
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
        Reg->WriteString("", (String)'\"'+ParamStr(0)+"\" \"%1\"");
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
      if(Application->MessageBox(TEXT("関連付けを解除します"), CassavaType,
                                 MB_OKCANCEL)==IDOK){
        String FileType = "";
        Reg->OpenKey("\\Software", false);
        Reg->OpenKey("Classes", false);
        if(Reg->OpenKey(CassavaType, false)){
          FileType = Reg->ReadString("OldType");
          Reg->CloseKey();
          Reg->DeleteKey((String)"\\Software\\Classes\\" + CassavaType);
        }
        if(FileType==""){
          Reg->DeleteKey((String)"\\Software\\Classes\\" + Ext);
        }else{
          Reg->OpenKey((String)"\\Software\\Classes\\" + Ext, true);
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
      if(Application->MessageBox(TEXT("新規作成に追加します"), CassavaType,
                                 MB_OKCANCEL)==IDOK){
        Reg->OpenKey((String)"\\Software\\Classes\\" + Ext + "\\ShellNew",
                     true);
        Reg->WriteString("NullFile", "");
        Reg->CloseKey();
      }
    }else if(Reg->KeyExists((String)"\\Software\\Classes\\" + Ext
             + "\\ShellNew")){
      if(Application->MessageBox(TEXT("新規作成を解除します"), CassavaType,
                                 MB_OKCANCEL)==IDOK){
          Reg->DeleteKey((String)"\\Software\\Classes\\" + Ext
                         + "\\ShellNew");
      }
    }
    delete Reg;
  }

  PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment");
}
//---------------------------------------------------------------------------
