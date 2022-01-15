//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <vcl\clipbrd.hpp>
#include <vcl\inifiles.hpp>
#include <process.h>
#include <stdio.h>
#pragma hdrstop

#include "MainForm.h"
#include "Letter.h"
#include "AutoOpen.h"
#include "KeyCustomize.h"
#include "Option.h"
#include "Print.h"
#include "Size.h"
#include "FormattedFileName.h"
#include "Macro.h"
#include "Compiler.h"
#include "Find.h"
#include "EncodedStream.h"
//---------------------------------------------------------------------------
#pragma link "CSPIN"
#pragma resource "*.dfm"
TfmMain *fmMain;
//---------------------------------------------------------------------------
#define min(X,Y) ((X)<(Y) ? (X) : (Y))
#define max(X,Y) ((X)>(Y) ? (X) : (Y))
// #define CssvMacro
//---------------------------------------------------------------------------
__fastcall TfmMain::TfmMain(TComponent* Owner)
	: TForm(Owner)
{
  FullPath = ExtractFilePath(ParamStr(0));
  if(*(FullPath.AnsiLastChar()) != '\\') FullPath += "\\";

  MainGrid = new TCsvGrid(this);
  MainGrid->Parent = this;
  MainGrid->Align = alClient;
  MainGrid->PopupMenu = PopMenu;
  MainGrid->OnChangeModified = MainGridChangeModified;
  MainGrid->OnDropFiles = MainGridDropFiles;
  MainGrid->OnKeyDown = MainGridKeyDown;
  MainGrid->OnMouseWheelUp = MainGrid->MouseWheelUp;
  MainGrid->OnMouseWheelDown = MainGrid->MouseWheelDown;
  MainGrid->OnGetCalculatedCell = GetCalculatedCell;
  MainGrid->DragDrogAccept = true;
  MainGrid->TabStop = false;
  Application->OnActivate = ApplicationActivate;
  History = new TStringList;
 
  TIniFile *Ini = new TIniFile(ExtractFilePath(ParamStr(0))+"Cassava.ini");
    Left = Ini->ReadInteger("Position","Left",
      GetSystemMetrics(SM_CXSCREEN)/2 - Width/2);
    Top = Ini->ReadInteger("Position","Top",
      GetSystemMetrics(SM_CYSCREEN)/2 - Height/2);
    Width = Ini->ReadInteger("Position","Width",Width);

  Show();
#ifdef CssvMacro
  mnMacroSearchClick(this);
#endif
    TTBAddBitmap AddBitmap;
    AddBitmap.hInst= HINST_COMMCTRL;
    AddBitmap.nID  = IDB_STD_SMALL_COLOR;
    tbarNormal->Perform(TB_ADDBITMAP, 0, (long)&AddBitmap);
    tbarNormal->Refresh();
 
    Height = Ini->ReadInteger("Position","Height",Height);
    WindowState = (TWindowState)Ini->ReadInteger("Position","Mode",wsNormal);
    Font->Name = Ini->ReadString("Font","Dialog", Font->Name);
    Font->Size = Ini->ReadInteger("Font","Dialog", Font->Size);
    MainGrid->Font->Name = Ini->ReadString("Font","Name",MainGrid->Font->Name);
    MainGrid->Font->Size = Ini->ReadInteger("Font","Size",MainGrid->Font->Size);
    MainGrid->LineMargin = Ini->ReadInteger("Font","LineMargin", 4);
    MainGrid->CellLineMargin = Ini->ReadInteger("Font","CellLineMargin", MainGrid->LineMargin);
    MainGrid->Font->Color = (TColor)Ini->ReadInteger("Font","FgColor",MainGrid->Font->Color);
    MainGrid->Canvas->Font = MainGrid->Font;
    MainGrid->Color       = (TColor)Ini->ReadInteger("Font","BgColor",MainGrid->Color);
    MainGrid->FixFgColor  = (TColor)Ini->ReadInteger("Font","FixFgColor",MainGrid->Font->Color);
    MainGrid->FixedColor  = (TColor)Ini->ReadInteger("Font","FixedColor",MainGrid->FixedColor);
    MainGrid->DummyBgColor  = (TColor)Ini->ReadInteger("Font","DummyBgColor",clCream);
    MainGrid->CalcFgColor  = (TColor)Ini->ReadInteger("Font","CalcFgColor",MainGrid->Font->Color);
    MainGrid->CalcBgColor  = (TColor)Ini->ReadInteger("Font","CalcBgColor",clAqua);
    MainGrid->CalcErrorFgColor  = (TColor)Ini->ReadInteger("Font","CalcErrorFgColor",MainGrid->Font->Color);
    MainGrid->CalcErrorBgColor  = (TColor)Ini->ReadInteger("Font","CalcErrorBgColor",clRed);
    MainGrid->WordWrap = Ini->ReadBool("Font","WordWrap", false);
    mnWordWrap->Checked = MainGrid->WordWrap;
    MainGrid->TextAlignment = Ini->ReadInteger("Font","TextAlign", MainGrid->TextAlignment);
    mnNumAlignRight->Checked = (MainGrid->TextAlignment == cssv_taNumRight);
    MainGrid->NumberComma = Ini->ReadInteger("Font","NumberComma", 0);
    mnNum3->Checked = (MainGrid->NumberComma > 0);

    PrintFontName = Ini->ReadString("Print","FontName",MainGrid->Font->Name);
    PrintFontSize = Ini->ReadInteger("Print","FontSize",MainGrid->Font->Size);
    PrintMargin[0] = Ini->ReadInteger("Print","MarginLeft",15);
    PrintMargin[1] = Ini->ReadInteger("Print","MarginRight",15);
    PrintMargin[2] = Ini->ReadInteger("Print","MarginTop",15);
    PrintMargin[3] = Ini->ReadInteger("Print","MarginBottom",15);

    int TypeCount = Ini->ReadInteger("DataType","Count", 0);
    if(TypeCount > 0){
      MainGrid->TypeList.Clear();
      for(int i=0; i<TypeCount; i++){
        AnsiString Section = (AnsiString)"DataType:" + (AnsiString)i;
        TTypeOption *TO = new TTypeOption;
        TO->Name = Ini->ReadString(Section, "Name", "[新規]");
        AnsiString exts = Ini->ReadString(Section, "Exts", "csv");
        TO->SetExts(exts);
        TO->ForceExt = Ini->ReadBool(Section, "ForceExt", false);
        TO->SepChars = Ascii2Ctrl(Ini->ReadString(Section, "SepChars", ",\\t"));
        TO->WeakSepChars = Ascii2Ctrl(Ini->ReadString(Section, "WeakSepChars", "\\_"));
        TO->QuoteChars = Ascii2Ctrl(Ini->ReadString(Section, "QuoteChars", "\""));
        TO->QuoteOption = Ini->ReadInteger(Section, "Quote", 1);
        TO->OmitComma = Ini->ReadBool(Section, "OmitComma", true);
        TO->DummyEof = Ini->ReadBool(Section, "DummyEof", false);
        MainGrid->TypeList.Add(TO);
      }
    }
    SetFilter();
    SetCopyMenu(mnCopyFormat);
    SetCopyMenu(mnpCopyFormat);
    SetPasteMenu(mnPasteFormat);
    SetPasteMenu(mnpPasteFormat);

    BackupOnSave = Ini->ReadBool("Backup","OnSave",true);
    BackupOnTime = Ini->ReadBool("Backup","OnTime",false);
    BackupOnOpen = Ini->ReadBool("Backup","OnOpen",true);
    BuFileNameS  = Ini->ReadString("Backup","FileNameS","%f.%x~");
    BuFileNameT  = Ini->ReadString("Backup","FileNameT","#%f.%x#");
    DelBuSSaved  = Ini->ReadBool("Backup","DeleteSSaved",
      !(Ini->ValueExists("Backup","OnSave")));
    DelBuSExit   = Ini->ReadBool("Backup","DeleteS",false);
    DelBuT       = Ini->ReadBool("Backup","DeleteT",true);
    BuInterval   = Ini->ReadInteger("Backup","Interval",5);

    int X = Ini->ReadInteger("Mode","Paste",-1);
      MainGrid->PasteOption = X;
      if(X >= 0) mnPasteNormal->Checked = false;
      if     (X == 0) mnPasteOption0->Checked = true;
      else if(X == 1) mnPasteOption1->Checked = true;
      else if(X == 2) mnPasteOption2->Checked = true;
      else if(X == 3) mnPasteOption3->Checked = true;
      else if(X == 4) mnPasteOption4->Checked = true;
      else if(X == 5) mnPasteOption5->Checked = true;
    MainGrid->DragMove = Ini->ReadBool("Mode","DragCounter",true);
    MainGrid->EnterMove = Ini->ReadInteger("Mode","EnterMove",0);
    int FixedRows = Ini->ReadInteger("Mode","FixTopRow",0);
    int FixedCols = Ini->ReadInteger("Mode","FixLeftCol",0);
    bool STB = Ini->ReadBool("Mode","ShowToolbar",true);
      if(STB != mnShowToolbar->Checked) mnShowToolbarClick(this);
    Application->HintHidePause
      = Ini->ReadInteger("Mode","HintHidePause",Application->HintHidePause);

    MainGrid->LeftArrowInCell = Ini->ReadBool("Mode","LeftArrowInCell",false);
    MainGrid->WheelMoveCursol = Ini->ReadBool("Mode","WheelMoveCursol",false);
    MainGrid->AlwaysShowEditor = Ini->ReadBool("Mode","AlwaysShowEditor",true);
    SortAll = Ini->ReadBool("Mode","SortAll",false);
    int UseURL = Ini->ReadInteger("Mode","UseURL",1);
      MainGrid->ShowURLBlue     = UseURL;
      MainGrid->DblClickOpenURL = UseURL;
    MakeNewWindow = Ini->ReadBool("Mode","NewWindow",false);
    TitleFullPath = Ini->ReadBool("Mode","TitleFullPath",false);
    LockFile = Ini->ReadInteger("Mode","LockFile",0);
    LockingFile = NULL;
    CheckTimeStamp = Ini->ReadBool("Mode","CheckTimeStamp", true);
    cbNumSort->Checked = Ini->ReadBool("Mode","SortByNumber",true);
    MainGrid->CheckKanji = Ini->ReadBool("Mode","CheckKanji",false);
    MainGrid->UnicodeWindowsMapping = Ini->ReadBool("Mode","UnicodeWindowsMapping",false);
    MainGrid->DefaultViewMode = Ini->ReadBool("Mode","DefaultViewMode", 0);
    StopMacroCount = Ini->ReadInteger("Mode","StopMacro", 100000);

    AnsiString LaunchName[3];
    mnAppli0->Hint = Ini->ReadString("Application", "E0", "");
    mnAppli0->Enabled = (mnAppli0->Hint != "");
    LaunchName[0]  = Ini->ReadString("Application", "N0", "未設定");
    mnAppli1->Hint = Ini->ReadString("Application", "E1", "");
    mnAppli1->Enabled = (mnAppli1->Hint != "");
    LaunchName[1]  = Ini->ReadString("Application", "N1", "未設定");
    mnAppli2->Hint = Ini->ReadString("Application", "E2", "");
    mnAppli2->Enabled = (mnAppli2->Hint != "");
    LaunchName[2]  = Ini->ReadString("Application", "N2", "未設定");
    MainGrid->BrowserFileName = Ini->ReadString("Application", "Browser", "");

    History->Clear();
      for(int i=0; i<10; i++){
        AnsiString S = Ini->ReadString("History", (AnsiString)i, "");
        if(S != "") History->Add(S);
        else break;
      }

  delete Ini;

  if(FileExists(FullPath + "AutoKey.csv") ||
     FileExists(FullPath + "AutoKey.dat")   )
  {
    fmKey = new TfmKey(Application);
      fmKey->MakeTree();
      if(fmKey->LoadKey(FullPath + "AutoKey.csv") ||
         fmKey->LoadKey(FullPath + "AutoKey.dat")   )
        fmKey->MenuUpDate();
    delete fmKey;
  }

  mnAppli0->Caption = (AnsiString)"&0: " + LaunchName[0];
  mnAppli1->Caption = (AnsiString)"&1: " + LaunchName[1];
  mnAppli2->Caption = (AnsiString)"&2: " + LaunchName[2];
  SetHistory("");

  bool FileOpened = false;
  TimeStamp = 0;
  int wd=1, ht=1;
  for(int i=1; i<=ParamCount(); i++){
    if(ParamStr(i)[1]=='-'){
      if(ParamStr(i)[2]=='w'){
        wd = ParamStr(++i).ToIntDef(wd);
      }else if(ParamStr(i)[2]=='h'){
        ht = ParamStr(++i).ToIntDef(ht);
      }
    }else{
      FileName = ParamStr(i);
      OpenFile(FileName);
      FileOpened = true;
      break;
    }
  }
  if(!FileOpened){
    FileName = "";
    Caption = "Cassava";
    Application->Title = Caption;
    MainGrid->Clear(wd+1, ht+1, true); // ダミーセル分広げる
  }

  if(FileOpened && FixedRows > 0 && MainGrid->RowCount-1 > FixedRows){
    MainGrid->ShowColCounter = false;
    MainGrid->FixedRows = FixedRows;
  } else {
    MainGrid->ShowColCounter = true;
  }
  mnFixFirstRow->Checked = !(MainGrid->ShowColCounter);
  tsbFixFirstRow->Down   = !(MainGrid->ShowColCounter);

  if(FileOpened && FixedCols > 0 && MainGrid->ColCount-1 > FixedCols){
    MainGrid->ShowRowCounter = false;
    MainGrid->FixedCols = FixedCols;
  } else {
    MainGrid->ShowRowCounter = true;
  }
  mnFixFirstCol->Checked = !(MainGrid->ShowRowCounter);
  tsbFixFirstCol->Down   = !(MainGrid->ShowRowCounter);

  if(MainGrid->AlwaysShowEditor){
    MainGrid->SetFocus();
    MainGrid->ShowEditor();
  }else{
    MainGrid->Options >> goAlwaysShowEditor;
    MainGrid->SetFocus();
    MainGrid->EditorMode = false;
  }

#ifdef CssvMacro
  AnsiString CmsFile = FullPath + "Macro\\!startup.cms";
  if(FileExists(CmsFile)){
    MacroExec(CmsFile, NULL);
  }
#endif

}
//---------------------------------------------------------------------------
__fastcall TfmMain::~TfmMain()
{
  delete MainGrid;
  delete History;
  if(LockingFile){ delete LockingFile; }
}
//---------------------------------------------------------------------------
void TfmMain::WriteIni(bool PosSlide)
{
  try {
    TIniFile *Ini = new TIniFile(ExtractFilePath(ParamStr(0))+"Cassava.ini");
    Ini->WriteInteger("Position","Mode",WindowState);
    if(WindowState == wsNormal){
      int Slide = PosSlide ? 32 : 0;
      Ini->WriteInteger("Position","Left",Left+Slide);
      Ini->WriteInteger("Position","Top",Top+Slide);
      Ini->WriteInteger("Position","Width",Width);
      Ini->WriteInteger("Position","Height",Height);
    }
    Ini->WriteString("Font","Dialog", Font->Name);
    Ini->WriteInteger("Font","Dialog", Font->Size);
    Ini->WriteString("Font","Name",MainGrid->Font->Name);
    Ini->WriteInteger("Font","Size",MainGrid->Font->Size);
    Ini->WriteInteger("Font","LineMargin",MainGrid->LineMargin);
    Ini->WriteInteger("Font","CellLineMargin",MainGrid->CellLineMargin);
    Ini->WriteInteger("Font","FgColor",MainGrid->Font->Color);
    Ini->WriteInteger("Font","BgColor",MainGrid->Color);
    Ini->WriteInteger("Font","FixFgColor",MainGrid->FixFgColor);
    Ini->WriteInteger("Font","FixedColor",MainGrid->FixedColor);
    Ini->WriteInteger("Font","DummyBgColor",MainGrid->DummyBgColor);
    Ini->WriteInteger("Font","CalcFgColor",MainGrid->CalcFgColor);
    Ini->WriteInteger("Font","CalcBgColor",MainGrid->CalcBgColor);
    Ini->WriteInteger("Font","CalcErrorFgColor",MainGrid->CalcErrorFgColor);
    Ini->WriteInteger("Font","CalcErrorBgColor",MainGrid->CalcErrorBgColor);
    Ini->WriteBool("Font","WordWrap",MainGrid->WordWrap);
    Ini->WriteInteger("Font","TextAlign", MainGrid->TextAlignment);
    Ini->WriteInteger("Font","NumberComma", MainGrid->NumberComma);

    Ini->WriteString("Print","FontName",PrintFontName);
    Ini->WriteInteger("Print","FontSize",PrintFontSize);
    Ini->WriteInteger("Print","MarginLeft",PrintMargin[0]);
    Ini->WriteInteger("Print","MarginRight",PrintMargin[1]);
    Ini->WriteInteger("Print","MarginTop",PrintMargin[2]);
    Ini->WriteInteger("Print","MarginBottom",PrintMargin[3]);

    int DataCount = MainGrid->TypeList.Count;
    Ini->WriteInteger("DataType", "Count", DataCount);
    for(int i=0; i<DataCount; i++){
      AnsiString Section = (AnsiString)"DataType:" + (AnsiString)i;
      TTypeOption *TO = MainGrid->TypeList.Items(i);
      Ini->WriteString(Section, "Name", TO->Name);
      Ini->WriteString(Section, "Exts", TO->GetExtsStr(0));
      Ini->WriteBool(Section, "ForceExt", TO->ForceExt);
      Ini->WriteString(Section, "SepChars", Ctrl2Ascii(TO->SepChars));
      Ini->WriteString(Section, "WeakSepChars", Ctrl2Ascii(TO->WeakSepChars));
      Ini->WriteString(Section, "QuoteChars", Ctrl2Ascii(TO->QuoteChars));
      Ini->WriteInteger(Section, "Quote", TO->QuoteOption);
      Ini->WriteBool(Section, "OmitComma", TO->OmitComma);
      Ini->WriteBool(Section, "DummyEof", TO->DummyEof);
    }

    Ini->WriteBool("Backup","OnSave",BackupOnSave);
    Ini->WriteBool("Backup","OnTime",BackupOnTime);
    Ini->WriteBool("Backup","OnOpen",BackupOnOpen);
    Ini->WriteString("Backup","FileNameS",BuFileNameS);
    Ini->WriteString("Backup","FileNameT",BuFileNameT);
    Ini->WriteBool("Backup","DeleteSSaved",DelBuSSaved);
    Ini->WriteBool("Backup","DeleteS",DelBuSExit);
    Ini->WriteBool("Backup","DeleteT",DelBuT);
    Ini->WriteInteger("Backup","Interval",BuInterval);

    Ini->WriteInteger("Mode","Paste",MainGrid->PasteOption);
    Ini->WriteBool("Mode","DragCounter",MainGrid->DragMove);
    Ini->WriteInteger("Mode","EnterMove",MainGrid->EnterMove);
    Ini->WriteInteger("Mode","FixTopRow",
      MainGrid->ShowColCounter ? 0 : MainGrid->FixedRows);
    Ini->WriteInteger("Mode","FixLeftCol",
      MainGrid->ShowRowCounter ? 0 : MainGrid->FixedCols);
    Ini->WriteBool("Mode","ShowToolbar",mnShowToolbar->Checked);
    Ini->WriteInteger("Mode","HintHidePause",Application->HintHidePause);
    Ini->WriteBool("Mode","LeftArrowInCell",MainGrid->LeftArrowInCell);
    Ini->WriteBool("Mode","WheelMoveCursol",MainGrid->WheelMoveCursol);
    Ini->WriteBool("Mode","AlwaysShowEditor", MainGrid->AlwaysShowEditor);
    Ini->WriteBool("Mode","SortAll", SortAll);
    Ini->WriteInteger("Mode","UseURL", MainGrid->DblClickOpenURL);

    Ini->WriteBool("Mode","NewWindow",MakeNewWindow);
    Ini->WriteBool("Mode","TitleFullPath",TitleFullPath);
    Ini->WriteInteger("Mode","LockFile",LockFile);
    Ini->WriteBool("Mode","CheckTimeStamp",CheckTimeStamp);
    Ini->WriteBool("Mode","SortByNumber",cbNumSort->Checked);
    Ini->WriteBool("Mode","CheckKanji",MainGrid->CheckKanji);
    Ini->WriteBool("Mode","UnicodeWindowsMapping",MainGrid->UnicodeWindowsMapping);
    Ini->WriteBool("Mode","DefaultViewMode", MainGrid->DefaultViewMode);
    Ini->WriteInteger("Mode","StopMacro", StopMacroCount);

    Ini->WriteString("Application", "E0", mnAppli0->Hint);
    Ini->WriteString("Application", "N0", mnAppli0->Caption.c_str() + 4);
    Ini->WriteString("Application", "E1", mnAppli1->Hint);
    Ini->WriteString("Application", "N1", mnAppli1->Caption.c_str() + 4);
    Ini->WriteString("Application", "E2", mnAppli2->Hint);
    Ini->WriteString("Application", "N2", mnAppli2->Caption.c_str() + 4);
    Ini->WriteString("Application", "Browser", MainGrid->BrowserFileName);

    for(int i=0; i<History->Count; i++)
      Ini->WriteString("History", (AnsiString)i,History->Strings[i]);
    for(int i=History->Count; i<10; i++)
      Ini->DeleteKey("History", (AnsiString)i);

    delete Ini;
  }catch(...){}
}
//---------------------------------------------------------------------------
void TfmMain::SetFilter()
{
  AnsiString OFilter = "";
  AnsiString SFilter = "";
  AnsiString FilterExt;
  TStringList *AllExts = new TStringList;
  for(int i=0; i<MainGrid->TypeList.Count; i++){
    TTypeOption *p = MainGrid->TypeList.Items(i);
    FilterExt = "";
    for(int j=0; j<p->Exts->Count; j++){
      if(j > 0) FilterExt += ";";
      AnsiString str = p->Exts->Strings[j];
      FilterExt += "*." + str;
      if(AllExts->IndexOf(str) < 0){ AllExts->Add(str); }
    }
    if(i > 0){
      OFilter += p->Name + " (" + FilterExt + ")|" + FilterExt + "|";
    }
    SFilter += p->Name + " (*." + p->DefExt() + ")|*." + p->DefExt() + "|";
  }
  FilterExt = "";
  for(int i=0; i<AllExts->Count; i++){
    if(i > 0) FilterExt += ";";
    FilterExt += "*." + AllExts->Strings[i];
  }
  AnsiString CFilter = (AnsiString)"Cassava (" + FilterExt + ")|" + FilterExt + "|";
  dlgOpen->Filter = CFilter + OFilter + "すべてのファイル (*.*)|*.*";
  dlgSave->Filter = SFilter;
  delete AllExts;
}
//---------------------------------------------------------------------------
AnsiString TfmMain::MakeId(AnsiString prefix, AnsiString caption, int i)
{
  AnsiString id = prefix + "_";
  for(int i=1; i<=caption.Length(); i++){
    if(caption[i] >= 'A' && caption[i] <= 'Z' ||
       caption[i] >= 'a' && caption[i] <= 'z' ||
       caption[i] >= '0' && caption[i] <= '9'){
      id += caption[i];
    }
  }
  return id + "_" + i;
}
//---------------------------------------------------------------------------
void TfmMain::SetCopyMenu(TMenuItem *Item)
{
  Item->Clear();
  for(int i=0; i<MainGrid->TypeList.Count; i++){
    TMenuItem *MI = new TMenuItem(Item->Owner);
    MI->Caption = MainGrid->TypeList.Items(i)->Name;
    if(Item == mnCopyFormat){
      MI->Name = MakeId("copyformat", MI->Caption, i);
    }
    MI->Tag = i;
    MI->OnClick = mnCopyFormatDefaultClick;
    Item->Add(MI);
  }
}
//---------------------------------------------------------------------------
void TfmMain::SetPasteMenu(TMenuItem *Item)
{
  Item->Clear();
  for(int i=0; i<MainGrid->TypeList.Count; i++){
    TMenuItem *MI = new TMenuItem(Item->Owner);
    MI->Caption = MainGrid->TypeList.Items(i)->Name;
    if(Item == mnPasteFormat){
      MI->Name = MakeId("pasteformat", MI->Caption, i);
    }
    MI->Tag = i;
    MI->OnClick = mnPasteFormatDefaultClick;
    Item->Add(MI);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridChangeModified(TObject *Sender)
{
  bool Enab = MainGrid->Modified;
  mnSave->Enabled = Enab;
  tsbSave->Enabled = Enab;
  if(Enab && LockFile == cssv_lfEdit && LockingFile == NULL && FileName != ""){
    LockingFile = new TFileStream(FileName, fmOpenWrite|fmShareDenyWrite);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridKeyDown(System::TObject* Sender,
                               Word &Key, Classes::TShiftState Shift)
{
  if(Key == VK_ESCAPE){
    if(!MainGrid->AlwaysShowEditor){
      MainGrid->Options >> goAlwaysShowEditor;
      MainGrid->EditorMode = false;
    }
    if(gbxSort->Visible){
      tsbSort->Down = false;
      gbxSort->Visible = false;
    }
  }else{
    MainGrid->KeyDownSub(Sender, Key, Shift);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::ApplicationActivate(System::TObject* Sender)
{
  if(!CheckTimeStamp || FileName == "" || !FileExists(FileName)){ return; }
  int age = FileAge(FileName);
  if(age > TimeStamp && TimeStamp > 0){
    if(Application->MessageBox(
      "他のアプリケーションによってファイルが更新されました。\n再読み込みしますか？",
      FileName.c_str(), MB_YESNO + MB_ICONQUESTION) == IDYES){
      mnReloadClick(Sender);
    }
  }
  TimeStamp = age;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnNewClick(TObject *Sender)
{
  if(MakeNewWindow){
    WriteIni(true);
    spawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(), NULL);
  }else{
    if(mnFixFirstCol->Checked) mnFixFirstColClick(this);
    if(mnFixFirstRow->Checked) mnFixFirstRowClick(this);

    if(IfModifiedThenSave())
    {
      FileName = "";
      Caption = "Cassava";
      Application->Title = Caption;
      dlgSave->FilterIndex = 0;
      MainGrid->Clear();
      mnReloadCode->Enabled = false;

      if(LockingFile){
        delete LockingFile;
        LockingFile = NULL;
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnNewSizeClick(TObject *Sender)
{
  int wd, ht;

  if(!MakeNewWindow && !IfModifiedThenSave()) return;

  fmSize = new TfmSize(Application);
    if(fmSize->ShowModal() == mrOk){
      wd = fmSize->udWidth->Position;
      ht = fmSize->udHeight->Position;
    }else{
      return;
    }
  delete fmSize;

  if(MakeNewWindow){
    AnsiString W = wd;
    AnsiString H = ht;
    WriteIni(true);
    spawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
      "-w", W.c_str(), "-h", H.c_str(), NULL);
  }else{
    if(mnFixFirstCol->Checked) mnFixFirstColClick(this);
    if(mnFixFirstRow->Checked) mnFixFirstRowClick(this);

    FileName = "";
    Caption = "Cassava";
    Application->Title = Caption;
    MainGrid->Clear(wd+1, ht+1, true); // ダミーセル分広げる
    mnReloadCode->Enabled = false;

    if(LockingFile){
      delete LockingFile;
      LockingFile = NULL;
    }
  }
}
//---------------------------------------------------------------------------
void TfmMain::OpenFile (AnsiString OpenFileName, int KCode)
{
  if(!FileExists(OpenFileName)){
    Application->MessageBox(
      ("ファイル " + OpenFileName + " は存在しません").c_str(),
      ExtractFileName(OpenFileName).c_str(), MB_ICONERROR);
    return;
  }
  if(LockingFile){
    delete LockingFile;
    LockingFile = NULL;
  }
  if(!MainGrid->LoadFromFile(OpenFileName, KCode)){
    return;
  }
  FileName = OpenFileName;
  Caption = TitleFullPath ? FileName : ExtractFileName(FileName);
  Application->Title = Caption;
  SetHistory(FileName);
  TimeStamp = FileAge(FileName);
  switch(MainGrid->KanjiCode){
    case CHARCODE_JIS:     mnJis->Checked=true;     break;
    case CHARCODE_EUC:     mnEuc->Checked=true;     break;
    case CHARCODE_UTF8:    mnUtf8->Checked=true;    break;
    case CHARCODE_UTF16BE: mnUtf16be->Checked=true; break;
    case CHARCODE_UTF16LE: mnUnicode->Checked=true; break;
    default:               mnSjis->Checked=true;    break;
  }
  switch(MainGrid->ReturnCode){
    case '\x0A': mnLf->Checked=true;   break;
    case '\x0D': mnCr->Checked=true;   break;
    default:     mnLfcr->Checked=true; break;
  }
  dlgSave->FilterIndex = MainGrid->TypeIndex + 1;
  tmAutoSaver->Enabled = false;
  mnReloadCode->Enabled = true;

  try{
    if(LockFile == cssv_lfOpen){
      LockingFile = new TFileStream(FileName, fmOpenWrite|fmShareDenyWrite);
    }
  }catch(...){}
  try{
    if(BackupOnTime){
      if(BackupOnOpen){
        AnsiString BuFN = FormattedFileName(BuFileNameT, FileName);
        CopyFile(FileName.c_str(), BuFN.c_str(), false);
      }
      tmAutoSaver->Interval = BuInterval * 60000;
      tmAutoSaver->Enabled = true;
    }
  }catch(...){}
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOpenClick(TObject *Sender)
{
  if(MakeNewWindow){
    dlgOpen->Options << ofAllowMultiSelect;
    if(dlgOpen->Execute()){
      WriteIni(true);
      TStrings *files = dlgOpen->Files;
      for(int i=0; i<files->Count; i++){
        spawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
          ((AnsiString)("\"") + files->Strings[i] + "\"").c_str(), NULL);
      }
    }
    dlgOpen->Options >> ofAllowMultiSelect;
  }else{
    if(IfModifiedThenSave()) {
      if(dlgOpen->Execute()) {
        OpenFile(dlgOpen->FileName);
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridDropFiles(TObject *Sender, int iFiles,
                                      AnsiString *DropFileNames){
  if(MakeNewWindow){
    WriteIni(true);
    for(int i=0; i<iFiles; i++){
      spawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
        ((AnsiString)("\"") + DropFileNames[i] + "\"").c_str(), NULL);
    }
  }else{
    if(IfModifiedThenSave()){
      OpenFile(DropFileNames[0]);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnReloadClick(TObject *Sender)
{
  if(FileName == "") return;

  if(MainGrid->Modified)
  {
    int A =
    Application->MessageBox(
      (Caption + "への変更を破棄しますか？").c_str(),
      "Cassava",MB_YESNO + MB_ICONQUESTION);
    if(A == IDNO) return;
  }

  OpenFile(FileName);
}
//---------------------------------------------------------------------------
bool TfmMain::IfModifiedThenSave()
{
  if(MainGrid->Modified)
  {
    AnsiString FN;
    if(FileName != "") FN = Caption; else FN="無題";
    int A =
    Application->MessageBox(
      (FN + " への変更を保存しますか？").c_str(),
      "Cassava",MB_YESNOCANCEL + MB_ICONQUESTION);
    if(A == IDYES) mnSaveClick(this);
    else if(A == IDCANCEL) return(false);
  }

  try {
    if(FileName != ""){
      if(BackupOnSave && DelBuSExit){
        AnsiString BuFN = FormattedFileName(BuFileNameS, FileName);
        if(FileExists(BuFN)) DeleteFile(BuFN);
      }
      if(BackupOnTime && DelBuT){
        AnsiString BuFN = FormattedFileName(BuFileNameT, FileName);
        if(FileExists(BuFN)) DeleteFile(BuFN);
      }
    }
  }catch(...){}

  return(true);
}
//---------------------------------------------------------------------------
void TfmMain::UpdateKCode()
{
  if(mnJis->Checked)         { MainGrid->KanjiCode = CHARCODE_JIS;     }
  else if(mnEuc->Checked)    { MainGrid->KanjiCode = CHARCODE_EUC;     }
  else if(mnUtf8->Checked)   { MainGrid->KanjiCode = CHARCODE_UTF8;    }
  else if(mnUtf16be->Checked){ MainGrid->KanjiCode = CHARCODE_UTF16BE; }
  else if(mnUnicode->Checked){ MainGrid->KanjiCode = CHARCODE_UTF16LE; }
  else                       { MainGrid->KanjiCode = CHARCODE_SJIS;    }
  if(mnLf->Checked)      { MainGrid->ReturnCode = '\x0A'; }
  else if(mnCr->Checked) { MainGrid->ReturnCode = '\x0D'; }
  else                   { MainGrid->ReturnCode = '\0';   }
}
//---------------------------------------------------------------------------
void TfmMain::SaveFile(TTypeOption *Format)
{
  UpdateKCode();

  if(FileName == "")
    mnSaveAsClick(this);
  else {
    // ロックを解除
    if(LockingFile){
      delete LockingFile;
      LockingFile = NULL;
    }

    // バックアップ前に、アクセス権限を確認
    if(FileExists(FileName)){
      TFileStream *AccessTest = NULL;
      try {
        AccessTest = new TFileStream(FileName, fmOpenReadWrite|fmShareDenyWrite);
        delete AccessTest;
        AccessTest = NULL;
      }catch(Exception &ex){
        if(AccessTest){
          delete AccessTest;
        }
        Application->ShowException(&ex);
        return;
      }
    }

    // 必要ならばバックアップ
    AnsiString NewFile = "";
    try {
      if(BackupOnSave && FileExists(FileName)){
        NewFile = FormattedFileName(BuFileNameS, FileName);
        if(FileExists(NewFile)) DeleteFile(NewFile);
        RenameFile(FileName, NewFile);
      }
    }catch(...){}

    // 保存
    MainGrid->SaveToFile(FileName, Format);
    SetHistory(FileName);
    TimeStamp = FileAge(FileName);

    // 必要ならばバックアップを削除
    try {
      if(NewFile != ""){
        if(BackupOnSave && DelBuSSaved){
          if(FileExists(NewFile)) DeleteFile(NewFile);
        }
      }
    }catch(...){}

    // 必要ならば再度ロック
    if(LockFile == cssv_lfOpen){
      LockingFile = new TFileStream(FileName, fmOpenWrite|fmShareDenyWrite);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSaveClick(TObject *Sender)
{
  SaveFile(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSaveAsClick(TObject *Sender)
{
  if(FileName == ""){
    TTypeOption *Format = MainGrid->TypeList.Items(MainGrid->TypeIndex);
    dlgSave->FileName = "無題." + Format->DefExt();
  }else{
    dlgSave->FileName = FileName;
  }

  dlgSave->FilterIndex = MainGrid->TypeIndex + 1;
  if(dlgSave->Execute())
  {
    FileName = dlgSave->FileName;
    MainGrid->TypeIndex = dlgSave->FilterIndex - 1;
    TTypeOption *Format = MainGrid->TypeList.Items(MainGrid->TypeIndex);
    MainGrid->TypeOption = Format;

    AnsiString Ext = ExtractFileExt(FileName);
    AnsiString DefExt = Format->DefExt();
    if(Ext == "" || (Format->ForceExt && Ext != DefExt)){
      FileName += "." + DefExt;
    }
    Caption = TitleFullPath ? FileName : ExtractFileName(FileName);
    Application->Title = Caption;
    SaveFile(Format);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::tmAutoSaverTimer(TObject *Sender)
{
  try{
    if(BackupOnTime && FileName != ""){
      AnsiString BuFN = FormattedFileName(BuFileNameT, FileName);
      MainGrid->SaveToFile(BuFN, NULL, false);
    }else{
      tmAutoSaver->Enabled = false;
    }
  }catch(...){}
}
//---------------------------------------------------------------------------
void TfmMain::GetCheckedMenus(TStringList *list)
{
  list->Clear();
  TMenuItem *items = Menu->Items;
  for(int i=0; i<items->Count; i++){
    AddCheckedMenus(list, items->Items[i]);
  }
}
//---------------------------------------------------------------------------
void TfmMain::AddCheckedMenus(TStringList *list, TMenuItem* item)
{
   if(item->Caption == "-"){ return; }

   if(item->Checked){
     list->Add(item->Name);
   }

   if(item->Count > 0){
     for(int i=0; i<item->Count; i++){
       AddCheckedMenus(list, item->Items[i]);
     }
   }
}
//---------------------------------------------------------------------------
void TfmMain::RestoreCheckedMenus(TStringList *list)
{
  TMenuItem *items = Menu->Items;
  for(int i=0; i<items->Count; i++){
    RestoreCheckedMenus(list, items->Items[i]);
  }
}
//---------------------------------------------------------------------------
void TfmMain::RestoreCheckedMenus(TStringList *list, TMenuItem* item)
{
  if(item->Caption == "-"){ return; }

  bool isChecked = item->Checked;
  bool toChecked = (list->IndexOf(item->Name) >= 0);
  bool toChange;
  if(item->GroupIndex > 0){
    toChange = (!isChecked && toChecked);
  }else{
    toChange = (isChecked != toChecked);
  }
  if(toChange){
    if(item->OnClick){
      item->OnClick(item);
    }
    item->Checked = toChecked;
  }

  if(item->Count > 0){
    for(int i=0; i<item->Count; i++){
      RestoreCheckedMenus(list, item->Items[i]);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnExportClick(TObject *Sender)
{
#ifdef CssvMacro
  UpdateKCode();
  AnsiString strFilter = "";
  AnsiString strOrgFilter = dlgSave->Filter;
  TStringList *Exts = new TStringList;
  TSearchRec sr;
  if(FindFirst(FullPath+"Export\\*.cms", faAnyFile, sr) == 0){
    do{
      AnsiString Ext = ChangeFileExt(sr.Name,"");
      strFilter += Ext.UpperCase() + " 形式 (*." + Ext + ")|*." + Ext + "|";
      Exts->Add(Ext);
    }while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if(strFilter == ""){
    Application->MessageBox("エクスポート可能な形式はありません。",
                            "Cassava Export", 0);
    delete Exts;
    return;
  }
  dlgSave->Filter = strFilter;
  if(dlgSave->Execute()){
    dlgSave->Filter = strOrgFilter;
    AnsiString Ext = Exts->Strings[dlgSave->FilterIndex - 1];
    if(ExtractFileExt(dlgSave->FileName) == ""){
      dlgSave->FileName = dlgSave->FileName + "." + Ext;
    }
    AnsiString CmsFile = FullPath + "Export\\" + Ext + ".cms";
    AnsiString CmbFile = FullPath + "Macro\\Cmb\\" + Ext + ".cmb";

    TStream *out = NULL;
    EncodedStream *es = NULL;
    TStringList *checkedMenus = NULL;
    try{
      out = new TFileStream(dlgSave->FileName, fmCreate | fmShareDenyWrite);
      es = new EncodedStream(out, MainGrid->KanjiCode,
                             MainGrid->UnicodeWindowsMapping);
      checkedMenus = new TStringList();
      GetCheckedMenus(checkedMenus);
      MacroExec(CmsFile, es);
      RestoreCheckedMenus(checkedMenus);
    }catch(Exception *e){
      Application->MessageBox(e->Message.c_str(), "Cassava Macro Interpreter", 0);
    }
    if(checkedMenus) { delete checkedMenus; }
    if(es) { delete es; }
    if(out) { delete out; }
  }
  dlgSave->Filter = strOrgFilter;
  delete Exts;
#endif
}
//---------------------------------------------------------------------------
void TfmMain::SetHistory(AnsiString S)
{
  if(S != ""){
    for(int i=History->Count-1; i>=0; i--){
      if(History->Strings[i] == S) History->Delete(i);
    }
    History->Insert(0,S);
  }
  for(int i=History->Count-1; i>=10; i--){
    History->Delete(i);
  }

  TMenuItem *MnHist[10] = {
    mnOpenHistory0, mnOpenHistory1, mnOpenHistory2, mnOpenHistory3,
    mnOpenHistory4, mnOpenHistory5, mnOpenHistory6, mnOpenHistory7,
    mnOpenHistory8, mnOpenHistory9 };

  for(int i=0; i<10; i++){
    if(i < History->Count){
      MnHist[i]->Caption = "&" +(AnsiString)i + ": " + History->Strings[i];
      MnHist[i]->Enabled = true;
      MnHist[i]->Visible = true;
    } else {
      MnHist[i]->Visible = false;
      MnHist[i]->Caption = "&" +(AnsiString)i + ": (なし)";
    }
  }
  if(History->Count == 0){
    mnOpenHistory0->Enabled = false;
    mnOpenHistory0->Visible = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOpenHistorysClick(TObject *Sender)
{
  int Num = static_cast<TMenuItem *>(Sender)->Tag;
  AnsiString FN = History->Strings[Num];

  if(!FileExists(FN)){
    History->Delete(Num);
    SetHistory("");
    Application->MessageBox(
      ("ファイル " + FN + " は存在しません").c_str(),
      ExtractFileName(FN).c_str(), MB_ICONERROR);
    return;
  }

  if(MakeNewWindow){
    WriteIni(true);
    spawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
          ((AnsiString)("\"") + FN + "\"").c_str(), NULL);
  }else{
    if(IfModifiedThenSave()) {
      OpenFile(FN);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::PopMenuOpenPopup(TObject *Sender)
{
  TMenuItem *mi = PopMenuOpen->Items;
  for(int i=mi->Count - 1; i>=0; i--){
    TMenuItem *OldItem = mi->Items[i];
    mi->Delete(i);
    delete OldItem;
  }

  for(int i=0; i<History->Count; i++){
    TMenuItem *NewItem = new TMenuItem(mi->Owner);
    NewItem->Caption = "&" +(AnsiString)i + ": " + History->Strings[i];
    NewItem->Tag = i;
    NewItem->OnClick = mnOpenHistorysClick;
    mi->Add(NewItem);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnReloadCodeClick(TObject *Sender)
{
  int code = static_cast<TMenuItem *>(Sender)->Tag;
  if(MainGrid->Modified)
  {
    int A =
    Application->MessageBox(
      (Caption + "への変更を破棄しますか？").c_str(),
      "Cassava",MB_YESNO + MB_ICONQUESTION);
    if(A == IDNO) return;
  }
  OpenFile(FileName, code);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOpenHistoryClearClick(TObject *Sender)
{
  History->Clear();
  SetHistory("");
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOpenCellFileClick(TObject *Sender)
{
  AnsiString FileName = MainGrid->Cells[MainGrid->Col][MainGrid->Row];
  if(FileName.SubString(1,7) == "http://"){
    MainGrid->OpenURL(FileName);
  }else{
    AutoOpen(FileName);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnPrintClick(TObject *Sender)
{
  fmPrint = new TfmPrint(Application);
    fmPrint->ShowModal();
  delete fmPrint;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnLetterPrintClick(TObject *Sender)
{
  fmLetter = new TfmLetter(Application);
    fmLetter->ShowModal();
  delete fmLetter;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnEndClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  try
  {
    CanClose = IfModifiedThenSave();
    if(CanClose){
      WriteIni();
    }
  }catch(...){}
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnEditClick(TObject *Sender)
{
  mnUndo->Enabled = MainGrid->CanUndo();
  mnRedo->Enabled = MainGrid->CanRedo();
  bool Clipped = Clipboard()->HasFormat(CF_TEXT);

  if(! MainGrid->EditorMode)
  {
    mnPaste->Enabled = Clipped;
    mnpPaste->Enabled = Clipped;
    mnCut->Enabled = true;
    mnpCut->Enabled = true;
    mnCopy->Enabled = true;
    mnpCopy->Enabled = true;
  }
  else if(MainGrid->InplaceEditor)
  {
    mnPaste->Enabled = Clipped;
    mnpPaste->Enabled = Clipped;
    bool Sel = (MainGrid->InplaceEditor->SelLength > 0);
    mnCut->Enabled = Sel;
    mnpCut->Enabled = Sel;
    mnCopy->Enabled = Sel;
    mnpCopy->Enabled = Sel;
  }
  else
  {
    mnPaste->Enabled = false;
    mnpPaste->Enabled = false;
    mnCut->Enabled = false;
    mnpCut->Enabled = false;
    mnCopy->Enabled = false;
    mnpCopy->Enabled = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::PopMenuPopup(TObject *Sender)
{
  mnEditClick(this);
  mnpKugiri->Visible = false;
  mnpInsRow->Visible = false;
  mnpCutRow->Visible = false;
  mnpInsCol->Visible = false;
  mnpCutCol->Visible = false;
  mnpSort->Visible = false;
  mnpDefWidth->Visible = false;
  mnpDefWidth->Default = false;
  mnpNarrow->Visible = false;
  mnpNarrow->Default = false;
  mnpPaste->Caption = "貼り付け(&P)";
  TGridRect *Sel = &(MainGrid->Selection);
  if(Sel->Left == MainGrid->FixedCols &&
     Sel->Right == MainGrid->ColCount-1)
  {
    mnpKugiri->Visible = true;
    mnpInsRow->Visible = true;
    mnpCutRow->Visible = true;

    if(Sel->Top == Sel->Bottom){
      mnpDefWidth->Visible = true;
      if(MainGrid->PasteOption < 0) mnpPaste->Caption = "挿入貼り付け(&P)";
      if(MainGrid->RowHeights[Sel->Top] > 8) {
        mnpNarrow->Visible = true;
      }
    }
  }
  if(Sel->Top == MainGrid->FixedRows &&
     Sel->Bottom == MainGrid->RowCount-1)
  {
    mnpKugiri->Visible = true;
    mnpInsCol->Visible = true;
    mnpCutCol->Visible = true;

    if(Sel->Left == Sel->Right){
      mnpSort->Visible = true;
      mnpDefWidth->Visible = true;
      if(MainGrid->PasteOption < 0) mnpPaste->Caption = "挿入貼り付け(&P)";
      if(MainGrid->ColWidths[Sel->Left] > 16) {
        mnpNarrow->Visible = true;
        mnpNarrow->Default = true;
      }else{
        mnpDefWidth->Default = true;
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnUndoClick(TObject *Sender)
{
  MainGrid->Undo();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnRedoClick(TObject *Sender)
{
  MainGrid->Redo();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCopyClick(TObject *Sender)
{
  if(fmFind->edFindText->Focused()){
    fmFind->edFindText->CopyToClipboard();
  }else if(fmFind->edReplaceText->Focused()){
    fmFind->edReplaceText->CopyToClipboard();
  }else if(fmFind->edMin->Focused()){
    fmFind->edMin->CopyToClipboard();
  }else if(fmFind->edMax->Focused()){
    fmFind->edMax->CopyToClipboard();
  }else{
    MainGrid->CopyToClipboard();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCutClick(TObject *Sender)
{
  if(fmFind->edFindText->Focused()){
    fmFind->edFindText->CutToClipboard();
  }else if(fmFind->edReplaceText->Focused()){
    fmFind->edReplaceText->CutToClipboard();
  }else if(fmFind->edMin->Focused()){
    fmFind->edMin->CutToClipboard();
  }else if(fmFind->edMax->Focused()){
    fmFind->edMax->CutToClipboard();
  }else{
    MainGrid->CutToClipboard();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnPasteClick(TObject *Sender)
{
  if(fmFind->edFindText->Focused()){
    fmFind->edFindText->PasteFromClipboard();
  }else if(fmFind->edReplaceText->Focused()){
    fmFind->edReplaceText->PasteFromClipboard();
  }else if(fmFind->edMin->Focused()){
    fmFind->edMin->PasteFromClipboard();
  }else if(fmFind->edMax->Focused()){
    fmFind->edMax->PasteFromClipboard();
  }else{
    MainGrid->PasteFromClipboard();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCopyFormatDefaultClick(TObject *Sender)
{
  int XTypeIndex = MainGrid->TypeIndex;
  MainGrid->TypeIndex = static_cast<TMenuItem *>(Sender)->Tag;
  MainGrid->TypeOption = MainGrid->TypeList.Items(MainGrid->TypeIndex);

  MainGrid->CopyToClipboard();

  MainGrid->TypeIndex = XTypeIndex;
  MainGrid->TypeOption = MainGrid->TypeList.Items(MainGrid->TypeIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnPasteFormatDefaultClick(TObject *Sender)
{
  int XTypeIndex = MainGrid->TypeIndex;
  MainGrid->TypeIndex = static_cast<TMenuItem *>(Sender)->Tag;
  MainGrid->TypeOption = MainGrid->TypeList.Items(MainGrid->TypeIndex);

  MainGrid->PasteFromClipboard();

  MainGrid->TypeIndex = XTypeIndex;
  MainGrid->TypeOption = MainGrid->TypeList.Items(MainGrid->TypeIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSelectAllClick(TObject *Sender)
{
  MainGrid->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSelectRowClick(TObject *Sender)
{
  MainGrid->SelectRows(MainGrid->Selection.Top, MainGrid->Selection.Bottom);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSelectColClick(TObject *Sender)
{
  MainGrid->SelectCols(MainGrid->Selection.Left, MainGrid->Selection.Right);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnPasteOptionsClick(TObject *Sender)
{
  TMenuItem *Sdr = static_cast<TMenuItem*>(Sender);
  MainGrid->PasteOption = Sdr->Tag;
  Sdr->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCopySumClick(TObject *Sender)
{
  MainGrid->CopySum();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCopyAvrClick(TObject *Sender)
{
  MainGrid->CopyAvr();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnTransCharClick(TObject *Sender)
{
  MainGrid->SetUndoCsv();
  MainGrid->UndoSetLock++;
  MainGrid->Invalidate();
  MainGrid->TransChar(static_cast<TMenuItem*>(Sender)->Tag);
  MainGrid->Repaint();
  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnTransKanaClick(TObject *Sender)
{
  MainGrid->SetUndoCsv();
  MainGrid->UndoSetLock++;
  MainGrid->Invalidate();
  MainGrid->TransKana(static_cast<TMenuItem*>(Sender)->Tag);
  MainGrid->Repaint();
  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSequenceClick(TObject *Sender)
{
  bool ea = ((MainGrid->Selection.Bottom - MainGrid->Selection.Top) > 0);
  mnSequenceC->Enabled = ea;
  mnSequenceS->Enabled = ea;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSequenceCClick(TObject *Sender)
{
  MainGrid->SetUndoCsv();
  MainGrid->UndoSetLock++;
  MainGrid->Invalidate();
  MainGrid->Sequence(false);
  MainGrid->Repaint();
  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSequenceSClick(TObject *Sender)
{
  MainGrid->SetUndoCsv();
  MainGrid->UndoSetLock++;
  MainGrid->Invalidate();
  MainGrid->Sequence(true);
  MainGrid->Repaint();
  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnInsRowClick(TObject *Sender)
{
  int T = MainGrid->Selection.Top;
  MainGrid->InsertRow(T, MainGrid->Selection.Bottom);

  if(mnFixFirstCol->Checked){ mnFixFirstColClick(this); }
  MainGrid->Row = T;
  MainGrid->Col = MainGrid->FixedCols;
  MainGrid->Options << goEditing << goAlwaysShowEditor;
  MainGrid->EditorMode = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnInsColClick(TObject *Sender)
{
  MainGrid->InsertColumn(MainGrid->Selection.Left, MainGrid->Selection.Right);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCutRowClick(TObject *Sender)
{
  MainGrid->DeleteRow(MainGrid->Selection.Top, MainGrid->Selection.Bottom);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCutColClick(TObject *Sender)
{
  MainGrid->DeleteColumn(MainGrid->Selection.Left, MainGrid->Selection.Right);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnEnterClick(TObject *Sender)
{
  MainGrid->InsertEnter();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnNewLineClick(TObject *Sender)
{
  MainGrid->InsertNewLine();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnConnectCellClick(TObject *Sender)
{
  MainGrid->ConnectCell();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnInsertCellRightClick(TObject *Sender)
{
  MainGrid->InsertCell_Right();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnInsertCellDownClick(TObject *Sender)
{
  MainGrid->InsertCell_Down();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnDeleteCellLeftClick(TObject *Sender)
{
  MainGrid->DeleteCell_Left();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnDeleteCellUpClick(TObject *Sender)
{
  MainGrid->DeleteCell_Up();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnpDefWidthClick(TObject *Sender)
{
  TGridRect *Sel = &(MainGrid->Selection);
  if(Sel->Top == Sel->Bottom)
    MainGrid->RowHeights[Sel->Top] = MainGrid->DefaultRowHeight;
  else if(Sel->Left == Sel->Right)
    MainGrid->SetWidth(Sel->Left);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnpNarrowClick(TObject *Sender)
{
  TGridRect *Sel = &(MainGrid->Selection);
  if(Sel->Top == Sel->Bottom)
    MainGrid->RowHeights[Sel->Top] = 8;
  else if(Sel->Left == Sel->Right)
    MainGrid->ColWidths[Sel->Left] = 16;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnRefreshClick(TObject *Sender)
{
  MainGrid->Cut();
  MainGrid->ReNum();
  MainGrid->SetWidth();
  MainGrid->SetHeight();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnShowAllColumnClick(TObject *Sender)
{
  MainGrid->Cut();
  MainGrid->ReNum();
  MainGrid->ShowAllColumn();
  MainGrid->SetHeight();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFontClick(TObject *Sender)
{
  dlgFont->Font = MainGrid->Font;
  if(dlgFont->Execute())
  {
    MainGrid->Font = dlgFont->Font;
    MainGrid->DefaultRowHeight = MainGrid->Font->Size * Screen->PixelsPerInch / 48;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnStayOnTopClick(TObject *Sender)
{
  bool EM = MainGrid->EditorMode;
  MainGrid->EditorMode = false;
  mnStayOnTop->Checked = !mnStayOnTop->Checked;
  FormStyle = ((mnStayOnTop->Checked) ? fsStayOnTop : fsNormal);
  MainGrid->EditorMode = EM;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnWordWrapClick(TObject *Sender)
{
  MainGrid->WordWrap = ! MainGrid->WordWrap;
  mnWordWrap->Checked = MainGrid->WordWrap;
  MainGrid->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnNumAlignRightClick(TObject *Sender)
{
  if(MainGrid->TextAlignment == cssv_taLeft){
    MainGrid->TextAlignment = cssv_taNumRight;
  }else if(MainGrid->TextAlignment == cssv_taNumRight){
    MainGrid->TextAlignment = cssv_taLeft;
  }

  mnNumAlignRight->Checked = (MainGrid->TextAlignment == cssv_taNumRight);
  MainGrid->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnNum3Click(TObject *Sender)
{
  if(MainGrid->NumberComma == 0){
    MainGrid->NumberComma = 3;
  }else{
    MainGrid->NumberComma = 0;
  }

  mnNum3->Checked = (MainGrid->NumberComma == 3);
  MainGrid->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCalcExpressionClick(TObject *Sender)
{
  MainGrid->ExecCellMacro = ! MainGrid->ExecCellMacro;
  mnCalcExpression->Checked = MainGrid->ExecCellMacro;
  sbCalcExpression->Down = MainGrid->ExecCellMacro;
  MainGrid->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnShowToolbarClick(TObject *Sender)
{
  bool Visible = !(mnShowToolbar->Checked);
  mnShowToolbar->Checked = Visible;
  CoolBar->Visible = Visible;
  bvlToolBar->Visible = Visible;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFixFirstRowClick(TObject *Sender)
{
  MainGrid->UndoSetLock++;

  if(mnFixFirstRow->Checked){
    MainGrid->ShowColCounter = true;
  }else{
    MainGrid->ShowColCounter = false;
  }
  mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
  tsbFixFirstRow->Down   = !MainGrid->ShowColCounter;

  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFixFirstColClick(TObject *Sender)
{
  MainGrid->UndoSetLock++;

  if(mnFixFirstCol->Checked){
    MainGrid->ShowRowCounter = true;
  }else{
    MainGrid->ShowRowCounter = false;
  }
  mnFixFirstCol->Checked = !MainGrid->ShowRowCounter;
  tsbFixFirstCol->Down   = !MainGrid->ShowRowCounter;

  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFixUpLeftClick(TObject *Sender)
{
  MainGrid->UndoSetLock++;

  int X,Y;
  if(MainGrid->RangeSelect){
    X=MainGrid->Selection.Left;
    Y=MainGrid->Selection.Top;
  }else{
    X=MainGrid->Col;
    Y=MainGrid->Row;
  }
  if(MainGrid->ShowRowCounter) X--;
  if(MainGrid->ShowColCounter) Y--;

  if(X>0){
    MainGrid->ShowRowCounter = false;
    if(MainGrid->ColCount <= X){
      MainGrid->ChangeColCount(X + 1);
    }
    MainGrid->Col = X;
    MainGrid->FixedCols = X;
  }
  if(Y>0){
    MainGrid->ShowColCounter = false;
    if(MainGrid->RowCount <= Y){
      MainGrid->ChangeRowCount(Y + 1);
    }
    MainGrid->Row = Y;
    MainGrid->FixedRows = Y;
  }

  mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
  tsbFixFirstRow->Down   = !MainGrid->ShowColCounter;
  mnFixFirstCol->Checked = !MainGrid->ShowRowCounter;
  tsbFixFirstCol->Down   = !MainGrid->ShowRowCounter;

  MainGrid->Col = MainGrid->FixedCols;
  MainGrid->Row = MainGrid->FixedRows;

  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnUnFixClick(TObject *Sender)
{
  MainGrid->UndoSetLock++;

  if(mnFixFirstRow->Checked){
    MainGrid->ShowColCounter = true;
  }
  if(mnFixFirstCol->Checked){
    MainGrid->ShowRowCounter = true;
  }

  mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
  tsbFixFirstRow->Down   = !MainGrid->ShowColCounter;
  mnFixFirstCol->Checked = !MainGrid->ShowRowCounter;
  tsbFixFirstCol->Down   = !MainGrid->ShowRowCounter;

  MainGrid->UndoSetLock--;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOptionDlgClick(TObject *Sender)
{
  fmOption->Font->Name = Font->Name;
  fmOption->ShowModal();

  if(FileName != ""){
    Caption = TitleFullPath ? FileName : ExtractFileName(FileName);
    Application->Title = Caption;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnAppliClick(TObject *Sender)
{
  if(IfModifiedThenSave()){
    if(MainGrid->Modified) return;
    AnsiString Exe = static_cast<TMenuItem*>(Sender)->Hint;
    AnsiString Arg0 = (AnsiString)("\"") + Exe + "\"";
    if(FileName == ""){
      spawnl(P_NOWAITO, Exe.c_str(), Arg0.c_str(), NULL);
    }else{
      AnsiString Arg1 = (AnsiString)("\"") + FileName + "\"";
      spawnl(P_NOWAITO, Exe.c_str(), Arg0.c_str(), Arg1.c_str(), NULL);
    }
    Close();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroSearchClick(TObject *Sender)
{
#ifdef CssvMacro
  for(int i=mnMacro->Count - 1; i>=4; i--){
    TMenuItem *OldItem = mnMacro->Items[i];
    mnMacro->Delete(i);
    delete OldItem;
  }

  TSearchRec sr;
  if(FindFirst(FullPath+"Macro\\*.cms", faAnyFile, sr) == 0){
    int i = 0;
    do{
      TMenuItem *NewItem = new TMenuItem(mnMacro->Owner);
      AnsiString macroname = ChangeFileExt(sr.Name,"");
      NewItem->Hint = macroname;
      NewItem->Caption = macroname;
      NewItem->Name = MakeId("macro", macroname, i);
      NewItem->OnClick = mnMacroExecClick;
      mnMacro->Add(NewItem);
      i++;
    } while (FindNext(sr) == 0);
    FindClose(sr);
  }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroOpenFolderClick(TObject *Sender)
{
#ifdef CssvMacro
  spawnlp(P_NOWAITO, "Explorer.exe", "/idlist",
         (FullPath + "Macro").c_str(), NULL);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroExecuteClick(TObject *Sender)
{
#ifdef CssvMacro
  dlgOpenMacro->InitialDir = FullPath + "Macro";
  if(dlgOpenMacro->Execute()){
    AnsiString CmsFile = dlgOpenMacro->FileName;
    MacroExec(CmsFile, NULL);
  }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroExecClick(TObject *Sender)
{
#ifdef CssvMacro
  TMenuItem *Menu = static_cast<TMenuItem *>(Sender);
  AnsiString CmsFile = FullPath + "Macro\\" + Menu->Hint + ".cms";
  MacroExec(CmsFile, NULL);
#endif
}
//---------------------------------------------------------------------------
void TfmMain::MacroExec(AnsiString CmsFile, TStream *io)
{
#ifdef CssvMacro
  AnsiString InName = ChangeFileExt(ExtractFileName(CmsFile),"");
  TStringList *Modules = new TStringList;
  bool C = MacroCompile(CmsFile.c_str(), Modules, true);
  if(!C){
    C = (Application->MessageBox(
    "マクロコンパイル中にエラーが発生しました。\n実行を続けますか？",
    "Cassava Macro", MB_YESNO) == ID_YES);
  }
  if(C){
    MainGrid->SetUndoCsv();
    MainGrid->UndoSetLock++;
    MainGrid->Invalidate();
    ExecMacro(InName, StopMacroCount, Modules, -1, -1, io);
    MainGrid->Invalidate();
    MainGrid->UndoSetLock--;
  }
  for(int i=Modules->Count-1; i>=0; i--){
    if(Modules->Objects[i]) delete Modules->Objects[i];
  }
  delete Modules;
#endif
}
//---------------------------------------------------------------------------
AnsiString TfmMain::GetCalculatedCell(AnsiString Str, int ACol, int ARow)
{
  AnsiString ResultCell = (AnsiString)CALC_NG + Str;
#ifdef CssvMacro
  AnsiString CmsName = "";

  if(Str.Length() == 0 || Str[1] != '='){ return (AnsiString)CALC_NOTEXPR + Str; }
  Str.Delete(1,1);
  TStream *fs = NULL;
  TStringList *Modules = NULL;
  try{
    CmsName = (AnsiString)"$@cell_" + ACol + "_" + ARow;
    TStream *fs = new TMemoryStream();
    fs->Write("return ", 7);
    fs->Write(Str.c_str(), Str.Length());
    fs->Write(";", 1);
    fs->Position = 0;
	TStringList *Modules = new TStringList;
	bool C = MacroCompile(fs, FullPath + "Macro\\", CmsName, ".cms", Modules, false);
    if(C){
      AnsiString Result = ExecMacro(CmsName, StopMacroCount, Modules, ACol, ARow, NULL, true);
	  ResultCell = (AnsiString)CALC_OK + Result;
    }
  }catch(...){
    // エラー用のResultCell文字列は設定済み
  }
  if(fs){ delete fs; }
  if(Modules){
	for(int i=Modules->Count-1; i>=0; i--){
	  if(Modules->Objects[i]) delete Modules->Objects[i];
	}
	delete Modules;
  }
#endif
  return ResultCell;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFindClick(TObject *Sender)
{
  fmFind->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFindNextClick(TObject *Sender)
{
  fmFind->rgDirection->ItemIndex = 1;
  fmFind->btnNext->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFindBackClick(TObject *Sender)
{
  fmFind->rgDirection->ItemIndex = 0;
  fmFind->btnNext->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::sbFindClick(TObject *Sender)
{
  mnFindClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSortClick(TObject *Sender)
{
  TGridRect R;
  int sortcol;
  if(MainGrid->RangeSelect){
    if(SortAll && MainGrid->Selection.Left == MainGrid->Selection.Right){
      R.Left   = 1;
      R.Top    = MainGrid->SelTop;
      R.Right  = MainGrid->DataRight;
      R.Bottom = MainGrid->DataBottom;
      sortcol = MainGrid->SelLeft;
    }else{
      R = MainGrid->Selection;
      sortcol = MainGrid->SelLeft;
    }
  }else{
    R.Left   = 1;
    R.Top    = MainGrid->Row;
    R.Right  = MainGrid->DataRight;
    R.Bottom = MainGrid->DataBottom;
    sortcol = MainGrid->Col;
  }

  gbxSort->Visible = true;
  seSortLeft->Show();
  seSortTop->Show();
  seSortRight->Show();
  seSortBottom->Show();
  seSortCol->Show();

  seSortLeft->Text   = R.Left;
  seSortTop->Text    = R.Top;
  seSortRight->Text  = R.Right;
  seSortBottom->Text = R.Bottom;
  seSortCol->Text    = sortcol;

  if(R.Left < MainGrid->FixedCols) R.Left = MainGrid->FixedCols;
  // MainGrid->Selection = R;

  tsbSort->Down = true;
  seSortCol->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnpSortClick(TObject *Sender)
{
  int sortcol = MainGrid->Selection.Left;

  gbxSort->Visible = true;
  seSortLeft->Show();
  seSortTop->Show();
  seSortRight->Show();
  seSortBottom->Show();
  seSortCol->Show();

  seSortLeft->Text   = MainGrid->DataLeft;
  seSortTop->Text    = MainGrid->DataTop;
  seSortRight->Text  = MainGrid->DataRight;
  seSortBottom->Text = MainGrid->DataBottom;
  seSortCol->Text = sortcol;
  MainGrid->SelectAll();

  tsbSort->Down = true;
  gbxSort->Visible = true;
  seSortCol->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::sbSortClick(TObject *Sender)
{
  if(tsbSort->Down){
    mnSortClick(this);
  }else{
    btnSortCancelClick(this);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnSortClick(TObject *Sender)
{
  int l = seSortLeft->Text.ToInt();
  int t = seSortTop->Text.ToInt();
  int r = seSortRight->Text.ToInt();
  int b = seSortBottom->Text.ToInt();
  int c = seSortCol->Text.ToInt();
  bool d = (rgSortDirection->ItemIndex == 0);
  bool n = cbNumSort->Checked;
  if(t > b || l > r){
    if(t > b){ seSortTop->Text  = b; seSortBottom->Text = t; }
    if(l > r){ seSortLeft->Text = r; seSortRight->Text  = l; }
    return;
  }
  int DatBtm = MainGrid->DataBottom;
  if(b > DatBtm)               b = DatBtm;
  if(r >= MainGrid->ColCount)  r = MainGrid->ColCount - 1;

  MainGrid->SetUndoCsv();
  MainGrid->UndoSetLock++;
  MainGrid->Invalidate();
  MainGrid->Sort(l,t,r,b,c,d,n);
  MainGrid->Repaint();
  MainGrid->UndoSetLock--;

  tsbSort->Down = false;
  MainGrid->SetFocus();
  MainGrid->ReNum();
  gbxSort->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnSortCancelClick(TObject *Sender)
{
  tsbSort->Down = false;
  MainGrid->SetFocus();
  gbxSort->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::seSortKeyPress(TObject *Sender, char &Key)
{ if(Key==VK_ESCAPE) btnSortCancelClick(this); }
//---------------------------------------------------------------------------
void __fastcall TfmMain::seSortChange(TObject *Sender)
{
  if(gbxSort->Visible == false){ return; }
  TGridRect R;
  R.Left = seSortLeft->Text.ToInt();
  if(R.Left < MainGrid->FixedCols) R.Left = MainGrid->FixedCols;
  R.Top = seSortTop->Text.ToInt();
  if(R.Top < MainGrid->FixedRows) R.Top = MainGrid->FixedRows;
  R.Right = seSortRight->Text.ToInt();
  if(R.Right >= MainGrid->ColCount) R.Right = MainGrid->ColCount-1;
  R.Bottom = seSortBottom->Text.ToInt();
  if(R.Bottom >= MainGrid->RowCount) R.Bottom = MainGrid->RowCount-1;
  MainGrid->Selection = R;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::udSortClick(TObject *Sender, TUDBtnType Button)
{
  TUpDown *ud = static_cast<TUpDown*>(Sender);
  TEdit *edit;
  switch(ud->Tag){
    case 0: edit = seSortLeft; break;
    case 1: edit = seSortTop; break;
    case 2: edit = seSortRight; break;
    case 3: edit = seSortBottom; break;
    case 4: edit = seSortCol; break;
    default: return;
  }
  try{
    int value = edit->Text.ToInt();
    if(Button == btNext){
      value++;
    }else if(Button == btPrev){
      if(value > 1){
        value--;
      }
    }
    edit->Text = (AnsiString)value;
  }catch(...){
    return;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnKeyClick(TObject *Sender)
{
  fmKey = new TfmKey(Application);
    fmKey->ShowModal();
  delete fmKey;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnKCodeClick(TObject *Sender)
{
  TMenuItem *Menu = static_cast<TMenuItem *>(Sender);
  Menu->Checked = true;
}
//---------------------------------------------------------------------------
bool __fastcall TfmMain::FormHelp(WORD Command, int Data, bool &CallHelp)
{
  CallHelp = false;
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnHelpContentsClick(TObject *Sender)
{
  AutoOpen(FullPath + "Help\\index.html");
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnAboutClick(TObject *Sender)
{
  Application->MessageBox
    ("Cassava Editor\n   Ver.1.5.12\n   by あすかぜ\n\t2009/03/20",
     "バージョン情報",0);
}
//---------------------------------------------------------------------------

