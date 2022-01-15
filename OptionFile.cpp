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

  OSVERSIONINFO osVer;
  osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  ::GetVersionEx( &osVer );

  if(osVer.dwMajorVersion < 6){
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
  }else{
    // Vista�͖��Ή�
    CSVFileType = "";
    KanrenCSV = false;
    KanrenCSVNew = false;
    TSVFileType = "";
    KanrenTSV = false;
    KanrenTSVNew = false;
    cbKanrenCSV->Checked = KanrenCSV;
    cbKanrenCSV->Enabled = false;
    cbKanrenCSVNew->Checked = KanrenCSVNew;
    cbKanrenCSVNew->Enabled = false;
    cbKanrenTSV->Checked = KanrenTSV;
    cbKanrenTSV->Enabled = false;
    cbKanrenTSVNew->Checked = KanrenTSVNew;
    cbKanrenTSVNew->Enabled = false;
    gbKanren->Caption = "�֘A�t�� (Windows Vista ���Ή�)";
    frOptionLaunch->btnKanrenR->Enabled = false;
  }
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

  SetKanren("\\Cassava.CSV", "CSV �t�@�C��", "\\.csv",
            CSVFileType, KanrenCSV, KanrenCSVNew,
            cbKanrenCSV->Checked, cbKanrenCSVNew->Checked);
  SetKanren("\\Cassava.TSV", "TSV �t�@�C��", "\\.tsv",
            TSVFileType, KanrenTSV, KanrenTSVNew,
            cbKanrenTSV->Checked, cbKanrenTSVNew->Checked);
}
//---------------------------------------------------------------------------
void TfrOptionFile::SetKanren(char *CassavaTypePath, char *CassavaTypeName,
        char *ExtPath, AnsiString OldFileType, bool OldKanren,
        bool OldKanrenNew, bool NewKanren, bool NewKanrenNew)
{
  if(NewKanren != OldKanren){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CLASSES_ROOT;
    if(NewKanren){
      if(Application->MessageBox("�֘A�t����ݒ肵�܂�", CassavaTypePath + 1,
                                 MB_OKCANCEL)==IDOK){
        Reg->OpenKey(CassavaTypePath, true);
        Reg->WriteString("", CassavaTypeName);
        Reg->WriteString("OldType", OldFileType);
        Reg->OpenKey("shell",true);
        Reg->OpenKey("open",true);
        Reg->WriteString("", "Cassava");
        Reg->OpenKey("command",true);
        Reg->WriteString("", (AnsiString)'\"'+ParamStr(0)+"\" \"%1\"");
        Reg->OpenKey((AnsiString)CassavaTypePath + "\\DefaultIcon",true);
        Reg->WriteString("", ParamStr(0)+",0");
        Reg->CloseKey();
        Reg->OpenKey(ExtPath, true);
        Reg->WriteString("", CassavaTypePath + 1);
        Reg->CloseKey();
      }
    }else{
      if(Application->MessageBox("�֘A�t�����������܂�", CassavaTypePath + 1,
                                 MB_OKCANCEL)==IDOK){
        AnsiString FileType = "";
        if(Reg->OpenKey(CassavaTypePath, false)){
          FileType = Reg->ReadString("OldType");
          Reg->CloseKey();
          Reg->DeleteKey(CassavaTypePath);
        }
        if(FileType==""){
          Reg->DeleteKey(ExtPath);
        }else{
          Reg->OpenKey(ExtPath, true);
          Reg->WriteString("", FileType);
          Reg->CloseKey();
        }
      }
    }
    delete Reg;
  }

  if(NewKanrenNew != OldKanrenNew){
    TRegistry *Reg = new TRegistry;
    Reg->RootKey = HKEY_CLASSES_ROOT;
    if(NewKanrenNew){
      if(Application->MessageBox("�V�K�쐬�ɒǉ����܂�", CassavaTypePath + 1,
                                 MB_OKCANCEL)==IDOK){
        Reg->OpenKey((AnsiString)ExtPath + "\\ShellNew",true);
        Reg->WriteString("NullFile", "");
        Reg->CloseKey();
      }
    }else if(Reg->KeyExists((AnsiString)ExtPath + "\\ShellNew")){
      if(Application->MessageBox("�V�K�쐬���������܂�", CassavaTypePath + 1,
                                 MB_OKCANCEL)==IDOK){
          Reg->DeleteKey((AnsiString)ExtPath + "\\ShellNew");
      }
    }
    delete Reg;
  }
}
//---------------------------------------------------------------------------
