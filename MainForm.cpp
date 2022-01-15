//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <vcl\clipbrd.hpp>
#include <vcl\inifiles.hpp>
#include <process.h>
#include <stdio.h>
#include <map>
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
#include "EncodedWriter.h"
#include "Version.h"
#include "Sort.h"
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
  if(*(FullPath.LastChar()) != '\\') FullPath += "\\";
  Pref = new Preference(FullPath);

  MainGrid = new TCsvGrid(this);
  MainGrid->Parent = MainPanel;
  MainGrid->Align = alClient;
  MainGrid->PopupMenu = PopMenu;
  MainGrid->OnChangeModified = MainGridChangeModified;
  MainGrid->OnDropFiles = MainGridDropFiles;
  MainGrid->OnKeyDown = MainGridKeyDown;
  MainGrid->OnKeyUp = MainGridKeyUp;
  MainGrid->OnMouseMove = MainGridMouseMove;
  MainGrid->OnMouseUp = MainGridMouseUp;
  MainGrid->OnMouseWheelUp = MainGrid->MouseWheelUp;
  MainGrid->OnMouseWheelDown = MainGrid->MouseWheelDown;
  MainGrid->OnGetCalculatedCell = GetCalculatedCell;
  MainGrid->DragDrogAccept = true;
  MainGrid->TabStop = false;
  Application->OnActivate = ApplicationActivate;
  Application->OnHint = ApplicationHint;

  History = new TStringList;

  ReadIni();
  ReadToolBar();

  bool FileOpening = false;
  StartupMacroDone = false;
  TimeStamp = 0;
  int wd=MainGrid->FixedCols;
  int ht=MainGrid->FixedRows;
  for(int i=1; i<=ParamCount(); i++){
    if(ParamStr(i)[1]=='-'){
      if(ParamStr(i)[2]=='w'){
        wd = ParamStr(++i).ToIntDef(wd);
      }else if(ParamStr(i)[2]=='h'){
        ht = ParamStr(++i).ToIntDef(ht);
      }
    }else{
      FileName = ParamStr(i);
      OpenFile(FileName, CHARCODE_AUTO);
      FileOpening = true;
      break;
    }
  }
  if(!FileOpening){
    FileName = "";
    Caption = "Cassava";
    Application->Title = Caption;
    MainGrid->Clear(wd+1, ht+1, true); // ダミーセル分広げる
  }

  if(MainGrid->AlwaysShowEditor){
    MainGrid->SetFocus();
    MainGrid->ShowEditor();
  }else{
    MainGrid->Options >> goAlwaysShowEditor;
    MainGrid->SetFocus();
    MainGrid->EditorMode = false;
  }

#ifdef CssvMacro
  if(!FileOpening){
    StartupMacroDone = true;
    ExecStartupMacro();
  }
  StatusbarMacroCache = new TStringList;
  AnsiString CmsFile = Pref->UserPath + "Macro\\!statusbar.cms";
  if(!FileExists(CmsFile)){
    CmsFile = Pref->SharedPath + "Macro\\!statusbar.cms";
  }
  if(FileExists(CmsFile)){
    TStringList *InPaths = new TStringList;
    InPaths->Add(Pref->UserPath + "Macro\\");
    InPaths->Add(Pref->SharedPath + "Macro\\");
    MacroCompile(CmsFile, InPaths, StatusbarMacroCache, false);
    delete InPaths;
    StatusbarCmsFile = ChangeFileExt(ExtractFileName(CmsFile),"");
    if(StatusbarMacroCache->IndexOf("!statusbar$init$0") >= 0){
      try{
        ExecMacro("!statusbar$init$0", StopMacroCount, StatusbarMacroCache,
                  -1, -1, NULL, true);
      }catch(...){}
    }
    UpdateStatusbar();
  }
#endif
}
//---------------------------------------------------------------------------
void TfmMain::ExecStartupMacro()
{
  AnsiString CmsFile;
  CmsFile = Pref->SharedPath + "Macro\\!startup.cms";
  if(FileExists(CmsFile)){
    MacroExec(CmsFile, NULL);
  }
  CmsFile = Pref->UserPath + "Macro\\!startup.cms";
  if(FileExists(CmsFile)){
    MacroExec(CmsFile, NULL);
  }
}
//---------------------------------------------------------------------------
void TfmMain::ExecOpenMacro(System::TObject* Sender)
{
  if(!StartupMacroDone){
    StartupMacroDone = true;
    ExecStartupMacro();
  }

  AnsiString CmsFile;
  CmsFile = Pref->SharedPath + "Macro\\!open.cms";
  if(FileExists(CmsFile)){
    MacroExec(CmsFile, NULL);
  }
  CmsFile = Pref->UserPath + "Macro\\!open.cms";
  if(FileExists(CmsFile)){
    MacroExec(CmsFile, NULL);
  }
}
//---------------------------------------------------------------------------
__fastcall TfmMain::~TfmMain()
{
  delete MainGrid;
  delete History;
  delete Pref;
  if(LockingFile){ delete LockingFile; }
  if(StatusbarMacroCache){ delete StatusbarMacroCache; }
}
//---------------------------------------------------------------------------
void TfmMain::ReadIni()
{
  History->Clear();

  TIniFile *Ini = Pref->GetInifile();
    Left = Ini->ReadInteger("Position","Left",
      GetSystemMetrics(SM_CXSCREEN)/2 - Width/2);
    Top = Ini->ReadInteger("Position","Top",
      GetSystemMetrics(SM_CYSCREEN)/2 - Height/2);
    Width = Ini->ReadInteger("Position","Width",Width);

    Show();
#ifdef CssvMacro
    mnMacroSearchClick(this);
#endif

    Height = Ini->ReadInteger("Position","Height",Height);
    WindowState = (TWindowState)Ini->ReadInteger("Position","Mode",wsNormal);
    MainGrid->Font->Name = Ini->ReadString("Font","Name", "ＭＳ Ｐゴシック");
    MainGrid->Font->Size = Ini->ReadInteger("Font","Size", 12);
    MainGrid->TBMargin = Ini->ReadInteger("Font","TBMargin", 2);
    MainGrid->LRMargin = Ini->ReadInteger("Font","LRMargin", 2);
    MainGrid->CellLineMargin = Ini->ReadInteger("Font","CellLineMargin", 0);
    MainGrid->Font->Color = (TColor)Ini->ReadInteger("Font","FgColor",MainGrid->Font->Color);
    MainGrid->Canvas->Font = MainGrid->Font;
    MainGrid->Color       = (TColor)Ini->ReadInteger("Font","BgColor",MainGrid->Color);
    MainGrid->UrlColor    = (TColor)Ini->ReadInteger("Font","UrlColor",clBlue);
    MainGrid->FixFgColor  = (TColor)Ini->ReadInteger("Font","FixFgColor",MainGrid->Font->Color);
    MainGrid->FixedColor  = (TColor)Ini->ReadInteger("Font","FixedColor",MainGrid->FixedColor);
    MainGrid->CurrentRowBgColor = (TColor)Ini->ReadInteger("Font","CurrentRowBgColor",MainGrid->Color);
    MainGrid->CurrentColBgColor  = (TColor)Ini->ReadInteger("Font","CurrentColBgColor",MainGrid->Color);
    MainGrid->DummyBgColor  = (TColor)Ini->ReadInteger("Font","DummyBgColor",clCream);
    MainGrid->CalcFgColor  = (TColor)Ini->ReadInteger("Font","CalcFgColor",MainGrid->Font->Color);
    MainGrid->CalcBgColor  = (TColor)Ini->ReadInteger("Font","CalcBgColor",clAqua);
    MainGrid->CalcErrorFgColor  = (TColor)Ini->ReadInteger("Font","CalcErrorFgColor",MainGrid->Font->Color);
    MainGrid->CalcErrorBgColor  = (TColor)Ini->ReadInteger("Font","CalcErrorBgColor",clRed);
    MainGrid->WordWrap = Ini->ReadBool("Font","WordWrap", false);
    MainGrid->TextAlignment = Ini->ReadInteger("Font","TextAlign", MainGrid->TextAlignment);
    MainGrid->NumberComma = Ini->ReadInteger("Font","NumberComma", 0);
    MainGrid->DecimalDigits = Ini->ReadInteger("Font","DecimalDigits", -1);
    MainGrid->MinColWidth = Ini->ReadInteger("Font","MinColWidth", MainGrid->MinColWidth);

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
    if(FixedRows + 1 >= MainGrid->RowCount){
      MainGrid->ChangeRowCount(FixedRows + 2);
    }
    MainGrid->Row = FixedRows + 1;
    if(FixedCols + 1 >= MainGrid->ColCount){
      MainGrid->ChangeColCount(FixedCols + 2);
    }
    MainGrid->Col = FixedCols + 1;
    mnFixUpLeftClick(this);

    bool STB = Ini->ReadBool("Mode","ShowToolbar",true);
      if(STB != mnShowToolbar->Checked) mnShowToolbarClick(this);
    bool SSB = Ini->ReadBool("Mode","ShowStatusbar",true);
      if(SSB != mnShowStatusbar->Checked) mnShowStatusbarClick(this);
    Application->HintPause
      = Ini->ReadInteger("Mode","HintPause",Application->HintPause);
    Application->HintHidePause
      = Ini->ReadInteger("Mode","HintHidePause",Application->HintHidePause);

    MainGrid->LeftArrowInCell = Ini->ReadBool("Mode","LeftArrowInCell",false);
    MainGrid->WheelMoveCursol = Ini->ReadInteger("Mode","WheelMoveCursol",0);
    MainGrid->WheelScrollStep = Ini->ReadInteger("Mode","WheelScrollStep",1);
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
    SortByNumber = Ini->ReadBool("Mode","SortByNumber",true);
    SortIgnoreCase = Ini->ReadBool("Mode", "SortIgnoreCase", false);
    SortIgnoreZenhan = Ini->ReadBool("Mode", "SortIgnoreZenhan", false);
    MainGrid->CheckKanji = Ini->ReadBool("Mode","CheckKanji",false);
    MainGrid->DefaultViewMode = Ini->ReadBool("Mode","DefaultViewMode", 0);
    MainGrid->CalcWidthForAllRow = Ini->ReadBool("Mode","CalcWidthForAllRow", 0);
    StopMacroCount = Ini->ReadInteger("Mode","StopMacro", 0);

    FindCase = Ini->ReadBool("Search", "Case", true);
    FindWordSearch = Ini->ReadBool("Search", "Word", false);
    FindRegex = Ini->ReadBool("Search", "Regex", false);

    AnsiString LaunchName[3];
    mnAppli0->Hint = Ini->ReadString("Application", "E0", "");
    mnAppli0->Tag  = Ini->ReadBool("Application", "Q0", true);
    mnAppli0->Enabled = (mnAppli0->Hint != "");
    LaunchName[0]  = Ini->ReadString("Application", "N0", "未設定");
    mnAppli1->Hint = Ini->ReadString("Application", "E1", "");
    mnAppli1->Tag  = Ini->ReadBool("Application", "Q1", true);
    mnAppli1->Enabled = (mnAppli1->Hint != "");
    LaunchName[1]  = Ini->ReadString("Application", "N1", "未設定");
    mnAppli2->Hint = Ini->ReadString("Application", "E2", "");
    mnAppli2->Tag  = Ini->ReadBool("Application", "Q2", true);
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

  if(FileExists(Pref->Path + "AutoKey.csv"))
  {
    fmKey = new TfmKey(Application);
      fmKey->MakeTree();
      if(fmKey->LoadKey(Pref->Path + "AutoKey.csv"))
        fmKey->MenuUpDate();
    delete fmKey;
  }

  mnAppli0->Caption = (AnsiString)"&0: " + LaunchName[0];
  mnAppli1->Caption = (AnsiString)"&1: " + LaunchName[1];
  mnAppli2->Caption = (AnsiString)"&2: " + LaunchName[2];

  SetHistory("");
}
//---------------------------------------------------------------------------
void TfmMain::WriteIni(bool PosSlide)
{
  try {
    TIniFile *Ini = Pref->GetInifile();
    Ini->WriteInteger("Position","Mode",WindowState);
    if(WindowState == wsNormal){
      int Slide = PosSlide ? 32 : 0;
      Ini->WriteInteger("Position","Left",Left+Slide);
      Ini->WriteInteger("Position","Top",Top+Slide);
      Ini->WriteInteger("Position","Width",Width);
      Ini->WriteInteger("Position","Height",Height);
    }
    Ini->WriteString("Font","Name",MainGrid->Font->Name);
    Ini->WriteInteger("Font","Size",MainGrid->Font->Size);
    Ini->WriteInteger("Font","TBMargin",MainGrid->TBMargin);
    Ini->WriteInteger("Font","LRMargin",MainGrid->LRMargin);
    Ini->WriteInteger("Font","CellLineMargin",MainGrid->CellLineMargin);
    Ini->WriteInteger("Font","FgColor",MainGrid->Font->Color);
    Ini->WriteInteger("Font","UrlColor",MainGrid->UrlColor);
    Ini->WriteInteger("Font","BgColor",MainGrid->Color);
    Ini->WriteInteger("Font","FixFgColor",MainGrid->FixFgColor);
    Ini->WriteInteger("Font","FixedColor",MainGrid->FixedColor);
    Ini->WriteInteger("Font","CurrentRowBgColor",MainGrid->CurrentRowBgColor);
    Ini->WriteInteger("Font","CurrentColBgColor",MainGrid->CurrentColBgColor);
    Ini->WriteInteger("Font","DummyBgColor",MainGrid->DummyBgColor);
    Ini->WriteInteger("Font","CalcFgColor",MainGrid->CalcFgColor);
    Ini->WriteInteger("Font","CalcBgColor",MainGrid->CalcBgColor);
    Ini->WriteInteger("Font","CalcErrorFgColor",MainGrid->CalcErrorFgColor);
    Ini->WriteInteger("Font","CalcErrorBgColor",MainGrid->CalcErrorBgColor);
    Ini->WriteBool("Font","WordWrap",MainGrid->WordWrap);
    Ini->WriteInteger("Font","TextAlign", MainGrid->TextAlignment);
    Ini->WriteInteger("Font","NumberComma", MainGrid->NumberComma);
    Ini->WriteInteger("Font","DecimalDigits", MainGrid->DecimalDigits);
    Ini->WriteInteger("Font","MinColWidth", MainGrid->MinColWidth);

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
    Ini->WriteBool("Mode","ShowStatusbar",mnShowStatusbar->Checked);
    Ini->WriteInteger("Mode","HintPause",Application->HintPause);
    Ini->WriteInteger("Mode","HintHidePause",Application->HintHidePause);
    Ini->WriteBool("Mode","LeftArrowInCell",MainGrid->LeftArrowInCell);
    Ini->WriteInteger("Mode","WheelMoveCursol",MainGrid->WheelMoveCursol);
    Ini->WriteInteger("Mode","WheelScrollStep",MainGrid->WheelScrollStep);
    Ini->WriteBool("Mode","AlwaysShowEditor", MainGrid->AlwaysShowEditor);
    Ini->WriteBool("Mode","SortAll", SortAll);
    Ini->WriteInteger("Mode","UseURL", MainGrid->DblClickOpenURL);

    Ini->WriteBool("Mode","NewWindow",MakeNewWindow);
    Ini->WriteBool("Mode","TitleFullPath",TitleFullPath);
    Ini->WriteInteger("Mode","LockFile",LockFile);
    Ini->WriteBool("Mode","CheckTimeStamp",CheckTimeStamp);
    Ini->WriteBool("Mode","SortByNumber", SortByNumber);
    Ini->WriteBool("Mode", "SortIgnoreCase", SortIgnoreCase);
    Ini->WriteBool("Mode", "SortIgnoreZenhan", SortIgnoreZenhan);
    Ini->WriteBool("Mode","CheckKanji",MainGrid->CheckKanji);
    Ini->WriteBool("Mode","DefaultViewMode", MainGrid->DefaultViewMode);
    Ini->WriteBool("Mode","CalcWidthForAllRow", MainGrid->CalcWidthForAllRow);
    Ini->WriteInteger("Mode","StopMacro", StopMacroCount);

    Ini->WriteBool("Search", "Case", fmFind->cbCase->Checked);
    Ini->WriteBool("Search", "Word", fmFind->cbWordSearch->Checked);
    Ini->WriteBool("Search", "Regex", fmFind->cbRegex->Checked);

    Ini->WriteString("Application", "E0", mnAppli0->Hint);
    Ini->WriteString("Application", "N0", mnAppli0->Caption.c_str() + 4);
    Ini->WriteBool("Application", "Q0", mnAppli0->Tag);
    Ini->WriteString("Application", "E1", mnAppli1->Hint);
    Ini->WriteString("Application", "N1", mnAppli1->Caption.c_str() + 4);
    Ini->WriteBool("Application", "Q1", mnAppli1->Tag);
    Ini->WriteString("Application", "E2", mnAppli2->Hint);
    Ini->WriteString("Application", "N2", mnAppli2->Caption.c_str() + 4);
    Ini->WriteBool("Application", "Q2", mnAppli2->Tag);
    Ini->WriteString("Application", "Browser", MainGrid->BrowserFileName);

    for(int i=0; i<History->Count; i++)
      Ini->WriteString("History", (AnsiString)i,History->Strings[i]);
    for(int i=History->Count; i<10; i++)
      Ini->DeleteKey("History", (AnsiString)i);

    delete Ini;
  }catch(...){}
}
//---------------------------------------------------------------------------
void TfmMain::ReadToolBar()
{
  String toolbarcsv = Pref->Path + "ToolBar.csv";
  if(FileExists(toolbarcsv)){
    ::SendMessage(CoolBar->Handle, WM_SETREDRAW, 0, 0);
    TFileStream *file = new TFileStream(toolbarcsv, fmOpenRead|fmShareDenyNone);
    __int64 len = file->Size;
    char *buf = new char[len];
    file->Read(buf, len);
    delete file;

    tbarNormal->Visible = false;
    tbarAdditional->Visible = false;

    TTypeOption typeOption("CSV");
    CsvReader reader(&typeOption, AnsiString(buf, len));
    TStringList *list = new TStringList();
    reader.ReadLine(list);
    if(list->Count == 0 || list->Strings[0] != "(Cassava-ToolBarSetting)") {
      delete list;
      delete[] buf;
      return;
    }
    TToolBar *toolBar = NULL;
    int width = 0;
    int tbarTop = 0;
    int tbarLeft = 0;
    while(reader.ReadLine(list)){
      while(list->Count < 3) { list->Add(""); }
      String str0 = list->Strings[0];
      String name = list->Strings[1];
      String action = list->Strings[2];

      if(str0 != "" && str0[1] == '='){
        tbarTop += tbarNormal->Height;
        tbarLeft = -1;
      }else if(str0 != "" && str0[1] == '#'){
        if(toolBar){
          toolBar->Tag = width;
          toolBar->Width = width;
          if(tbarLeft >= 0){ tbarLeft += width; }
        }
        if(tbarLeft < 0){ tbarLeft = 0; }
        if(str0 == "#1"){
          toolBar = tbarNormal;
          toolBar->Visible = true;
          width = toolBar->Tag;
        }else if(str0 == "#2"){
          toolBar = tbarAdditional;
          toolBar->Visible = true;
          width = toolBar->Tag;
        }else{
          toolBar = new TToolBar(CoolBar);
          toolBar->Parent = CoolBar;
          width = 0;
        }
        toolBar->Top = tbarTop;
        toolBar->Left = tbarLeft;
        String toolbarbmp = Pref->Path + name;
        if(name == "#1"){
          toolBar->Images = imlNormal;
        }else if(name == "#2"){
          toolBar->Images = imlMenu;
        }else if(name != "" && FileExists(toolbarbmp)){
          TCustomImageList *images = new TCustomImageList(16, 16);
          images->FileLoad(rtBitmap, toolbarbmp, clSilver);
          toolBar->Images = images;
        }
      }else if(name == "-"){
        TToolButton *button = new TToolButton(toolBar);
        button->Style = tbsSeparator;
        button->Left = width;
        button->Parent = toolBar;
        button->Width = 8;
        width += button->Width;
      }else if(name != ""){
        TToolButton *button = new TToolButton(toolBar);
        button->Left = width;
        button->Parent = toolBar;
        for(int i=0; i<ActionList->ActionCount; i++){
          TContainedAction *ac = ActionList->Actions[i];
          if(ac->Name == "ac" + action){
            button->Action = ac;
            break;
          }
        }
        name = StringReplace(name, "|", "_", TReplaceFlags()<<rfReplaceAll);
        button->Hint = name + "|" + action;
        button->ImageIndex = str0.ToIntDef(0);
        if(! button->Action){
          button->OnClick = UserToolBarAction;
        }
        if(action == "OpenHistory"){
          button->Hint = name + "|Open";
          button->Style = tbsDropDown;
          button->DropdownMenu = PopMenuOpen;
        }
        width += button->Width;
      }
    }
    if(toolBar){
      toolBar->Tag = width;
      toolBar->Width = width;
    }
    delete list;
    delete[] buf;
    ::SendMessage(CoolBar->Handle, WM_SETREDRAW, 1, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::UserToolBarAction(TObject *Sender)
{
#ifdef CssvMacro
  TControl *control = static_cast<TControl*>(Sender);
  String action = control->Hint;
  int pos = action.Pos("|");
  if(pos > 0){
    action = action.SubString(pos + 1, action.Length() - pos).Trim();
  }
  if(action != ""){
    AnsiString CmsFile = Pref->UserPath + "Macro\\" + action;
    if(!FileExists(CmsFile)){
      CmsFile = Pref->SharedPath + "Macro\\" + action;
    }
    if(FileExists(CmsFile)){
      // 一致するファイルが存在する場合、そのファイルを実行
      MacroExec(CmsFile, NULL);
    }else{
      // ファイルが存在しない場合、スクリプトとして実行
      TCHAR c = *(action.LastChar());
      if(c != ';' && c != '}'){
        if(c != ')'){
          action += "();";
        }else{
          action += ";";
        }
      }
      MacroScriptExec("$@ToolBar", action);
    }
  }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::CoolBarResize(TObject *Sender)
{
  for(int i=0; i<CoolBar->ControlCount; i++){
    TControl *tbar = CoolBar->Controls[i];
    if(tbar->Tag > 0){
      tbar->Width = tbar->Tag;
    }
  }
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
inline TCHAR hex(int digit)
{
  return ((digit < 10) ? ('0' + digit) : ('A' + digit - 10));
}
//---------------------------------------------------------------------------
inline String ToHex(TCHAR ch)
{
  String result = "";
  result += hex(ch / 4096); ch %= 4096;
  result += hex(ch / 256); ch %= 256;
  result += hex(ch / 16); ch %= 16;
  result += hex(ch);
  return result;
}
//---------------------------------------------------------------------------
String TfmMain::MakeId(String prefix, String caption, int i)
{
  String id = prefix + "_";
  for(int i=1; i<=caption.Length(); i++){
    if(caption[i] >= 'A' && caption[i] <= 'Z' ||
       caption[i] >= 'a' && caption[i] <= 'z' ||
       caption[i] >= '0' && caption[i] <= '9'){
      id += caption[i];
    }else{
      id += "_" + ToHex(caption[i]);
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
  acSave->Enabled = Enab;
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
    if(pnlSearch->Visible){
      pnlSearch->Visible = false;
    }
  }else if(Key == VK_CANCEL){
    StopAllMacros();
  }else{
    MainGrid->KeyDownSub(Sender, Key, Shift);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridKeyUp(System::TObject* Sender,
                               Word &Key, Classes::TShiftState Shift)
{
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridMouseMove(System::TObject* Sender,
                               Classes::TShiftState Shift, int X, int Y)
{
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridMouseUp(System::TObject* Sender,
                               Controls::TMouseButton Button,
                               Classes::TShiftState Shift, int X, int Y)
{
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::ApplicationActivate(System::TObject* Sender)
{
  if(!CheckTimeStamp || FileName == "" || !FileExists(FileName)){ return; }
  TDateTime age;
  if(FileAge(FileName, age)){
    if(age > TimeStamp){
      if(Application->MessageBox(
        TEXT("他のアプリケーションによってファイルが更新されました。\n再読み込みしますか？"),
        FileName.c_str(), MB_YESNO + MB_ICONQUESTION) == IDYES){
        mnReloadClick(Sender);
      }
    }
    TimeStamp = age;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::ApplicationHint(TObject *Sender)
{
  StatusBar->Panels->Items[0]->Text = Application->Hint;
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnNewClick(TObject *Sender)
{
  if(MakeNewWindow){
    WriteIni(true);
    _wspawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(), NULL);
  }else{
    if(IfModifiedThenSave())
    {
      MainGrid->Clear();
      if(mnFixFirstRow->Checked || mnFixFirstCol->Checked) {
        // Clear() でファイル読み込みを中断した後で固定解除する必要がある
        if(mnFixFirstRow->Checked){ mnFixFirstRowClick(this); }
        if(mnFixFirstCol->Checked){ mnFixFirstColClick(this); }
        // 1列目・1行目の幅を初期値に戻すため、再度 Clear()
        MainGrid->Clear();
      }
      FileName = "";
      Caption = "Cassava";
      Application->Title = Caption;
      dlgSave->FilterIndex = 0;
      mnReloadCode->Enabled = false;
      UpdateStatusbar();

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
    String W = wd;
    String H = ht;
    WriteIni(true);
    _wspawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
      TEXT("-w"), W.c_str(), TEXT("-h"), H.c_str(), NULL);
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
void TfmMain::OpenFile (String OpenFileName, int KCode)
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
  if(!MainGrid->LoadFromFile(OpenFileName, KCode, ExecOpenMacro)){
    return;
  }
  FileName = OpenFileName;
  Caption = TitleFullPath ? FileName : ExtractFileName(FileName);
  Application->Title = Caption;
  SetHistory(FileName);
  FileAge(FileName, TimeStamp);
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
        String BuFN = FormattedFileName(BuFileNameT, FileName);
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
        _wspawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
		  ((String)("\"") + files->Strings[i] + "\"").c_str(), NULL);
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
                                      String *DropFileNames){
  if(MakeNewWindow){
    WriteIni(true);
    for(int i=0; i<iFiles; i++){
      _wspawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
        ((String)("\"") + DropFileNames[i] + "\"").c_str(), NULL);
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
      TEXT("Cassava"),MB_YESNO + MB_ICONQUESTION);
    if(A == IDNO) return;
  }

  OpenFile(FileName);
}
//---------------------------------------------------------------------------
bool TfmMain::IfModifiedThenSave()
{
  if(MainGrid->Modified)
  {
    String FN;
    if(FileName != ""){ FN = Caption; }else{ FN=TEXT("無題"); }
    int A = Application->MessageBox(
              (FN + " への変更を保存しますか？").c_str(),
			  TEXT("Cassava"), MB_YESNOCANCEL + MB_ICONQUESTION);
	if(A == IDYES){
	  if(MainGrid->FileOpenThread){
		Application->MessageBox(
		  TEXT("ファイルの読み込みが完了していないため保存できません。"),
          TEXT("Cassava"), MB_ICONERROR);
        return false;
      }
      acSaveExecute(this);
    }else if(A == IDCANCEL){
      return false;
    }
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

  return true;
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
  if(MainGrid->FileOpenThread){
    Application->MessageBox(
      TEXT("ファイルの読み込みが完了していないため保存できません。"),
	  TEXT("Cassava"), MB_ICONERROR);
    return;
  }

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
    FileAge(FileName, TimeStamp);

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
void __fastcall TfmMain::acSaveExecute(TObject *Sender)
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

    String Ext = ExtractFileExt(FileName);
    String DefExt = "." + Format->DefExt();

    if(Ext == "" || (Format->ForceExt && Ext != DefExt)){
      FileName += DefExt;
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
      if(MainGrid->FileOpenThread){
        return;
      }
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
  if(MainGrid->FileOpenThread){
    Application->MessageBox(
	  TEXT("ファイルの読み込みが完了していないためエクスポートできません。"),
      TEXT("Cassava"), MB_ICONERROR);
    return;
  }

  UpdateKCode();
  AnsiString strFilter = "";
  AnsiString strOrgFilter = dlgSave->Filter;
  TStringList *types = new TStringList;
  TSearchRec sr;
  if(FindFirst(Pref->UserPath+"Export\\*.cms", faAnyFile, sr) == 0){
    do{
      AnsiString type = ChangeFileExt(sr.Name,"");
      AnsiString ext = ExtractFileExt(type);
      if(ext == ""){ ext = (AnsiString)"." + type; }
      strFilter += type.UpperCase() + " 形式 (*" + ext + ")|*" + ext + "|";
      types->Add(type);
    }while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if(FindFirst(Pref->SharedPath+"Export\\*.cms", faAnyFile, sr) == 0){
    do{
      AnsiString type = ChangeFileExt(sr.Name,"");
      AnsiString ext = ExtractFileExt(type);
      if(ext == ""){ ext = (AnsiString)"." + type; }
      strFilter += type.UpperCase() + " 形式 (*" + ext + ")|*" + ext + "|";
      types->Add(type);
    }while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if(strFilter == ""){
	Application->MessageBox(TEXT("エクスポート可能な形式はありません。"),
							TEXT("Cassava Export"), 0);
    delete types;
    return;
  }
  dlgSave->Filter = strFilter;
  if(dlgSave->Execute()){
    dlgSave->Filter = strOrgFilter;
    AnsiString type = types->Strings[dlgSave->FilterIndex - 1];
    if(ExtractFileExt(dlgSave->FileName) == ""){
      AnsiString ext = ExtractFileExt(type);
      if(ext == ""){ ext = (AnsiString)"." + type; }
      dlgSave->FileName = dlgSave->FileName + ext;
    }
    Export(dlgSave->FileName, type);
  }
  dlgSave->Filter = strOrgFilter;
  delete types;
#endif
}
//---------------------------------------------------------------------------
void TfmMain::Export(String filename, String type)
{
#ifdef CssvMacro
    AnsiString CmsFile = Pref->SharedPath + "Export\\" + type + ".cms";
    if(FileExists(Pref->UserPath + "Export\\" + type + ".cms")){
      CmsFile = Pref->UserPath + "Export\\" + type + ".cms";
    }
    if(!FileExists(CmsFile)){
      Application->MessageBox(
          (type + " 形式ではエクスポートできません。").c_str(),
          TEXT("Cassava Export"), 0);
      return;
    }

    TStream *out = NULL;
    EncodedWriter *ew = NULL;
    TStringList *checkedMenus = NULL;
    try{
      out = new TFileStream(filename, fmCreate | fmShareDenyWrite);
      ew = new EncodedWriter(out, MainGrid->KanjiCode);
      checkedMenus = new TStringList();
      GetCheckedMenus(checkedMenus);
      MacroExec(CmsFile, ew);
      RestoreCheckedMenus(checkedMenus);
    }catch(Exception *e){
	  Application->MessageBox(e->Message.c_str(),
                              TEXT("Cassava Macro Interpreter"), 0);
    }
    if(checkedMenus) { delete checkedMenus; }
    if(ew) { delete ew; }
    if(out) { delete out; }
#endif
}
//---------------------------------------------------------------------------
void TfmMain::SetHistory(String S)
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
  String FN = History->Strings[Num];

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
    _wspawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(),
		  ((String)("\"") + FN + "\"").c_str(), NULL);
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
      TEXT("Cassava"), MB_YESNO + MB_ICONQUESTION);
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
  if(isUrl(FileName)){
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
    StopAllMacros();
    CanClose = IfModifiedThenSave();
    if(CanClose){
      WriteIni();
    }
  }catch(...){}
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::PopMenuPopup(TObject *Sender)
{
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
void __fastcall TfmMain::acUndoExecute(TObject *Sender)
{
  MainGrid->Undo();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acUndoUpdate(TObject *Sender)
{
  acUndo->Enabled = MainGrid->CanUndo();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acRedoExecute(TObject *Sender)
{
  MainGrid->Redo();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acRedoUpdate(TObject *Sender)
{
  acRedo->Enabled = MainGrid->CanRedo();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acCutExecute(TObject *Sender)
{
  if(edFindText->Focused()){
    edFindText->CutToClipboard();
  }else if(fmFind->edFindText->Focused()){
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
void __fastcall TfmMain::acCutUpdate(TObject *Sender)
{
  if(! MainGrid->EditorMode){
    if(!MainGrid->Dragging){ acCut->Enabled = true; }
  }else if(MainGrid->InplaceEditor){
    acCut->Enabled = (MainGrid->InplaceEditor->SelLength > 0);
  }else{
    acCut->Enabled = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acCopyExecute(TObject *Sender)
{
  if(edFindText->Focused()){
    edFindText->CopyToClipboard();
  }else if(fmFind->edFindText->Focused()){
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
void __fastcall TfmMain::acCopyUpdate(TObject *Sender)
{
  if(! MainGrid->EditorMode){
    if(!MainGrid->Dragging){ acCopy->Enabled = true; }
  }else if(MainGrid->InplaceEditor){
    acCopy->Enabled = (MainGrid->InplaceEditor->SelLength > 0);
  }else{
    acCopy->Enabled = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acPasteExecute(TObject *Sender)
{
  if(edFindText->Focused()){
    edFindText->PasteFromClipboard();
  }else if(fmFind->edFindText->Focused()){
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
void __fastcall TfmMain::acPasteUpdate(TObject *Sender)
{
  if((! MainGrid->EditorMode) || MainGrid->InplaceEditor){
    acPaste->Enabled = Clipboard()->HasFormat(CF_TEXT);
  }else{
    acPaste->Enabled = false;
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
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnShowAllColumnClick(TObject *Sender)
{
  MainGrid->Cut();
  MainGrid->ReNum();
  MainGrid->ShowAllColumn();
  MainGrid->SetHeight();
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void TfmMain::SetGridFont(TFont *AFont)
{
  MainGrid->Font = AFont;
  MainGrid->UpdateDefaultRowHeight();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFontClick(TObject *Sender)
{
  dlgFont->Font = MainGrid->Font;
  if(dlgFont->Execute())
  {
    SetGridFont(dlgFont->Font);
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
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnShowStatusbarClick(TObject *Sender)
{
  bool Visible = !(mnShowStatusbar->Checked);
  mnShowStatusbar->Checked = Visible;
  StatusBar->Visible = Visible;
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFixFirstRowClick(TObject *Sender)
{
  if(MainGrid->FileOpenThread){
    mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
    tsbFixFirstRow->Down   = !MainGrid->ShowColCounter;
    Application->MessageBox(
      TEXT("ファイルの読み込み中は固定セルを変更できません。"),
      TEXT("Cassava"), MB_ICONERROR);
    return;
  }

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
  if(MainGrid->FileOpenThread){
    mnFixFirstCol->Checked = !MainGrid->ShowRowCounter;
    tsbFixFirstCol->Down   = !MainGrid->ShowRowCounter;
    Application->MessageBox(
	  TEXT("ファイルの読み込み中は固定セルを変更できません。"),
	  TEXT("Cassava"), MB_ICONERROR);
    return;
  }

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
  if(MainGrid->FileOpenThread){
    mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
    tsbFixFirstRow->Down   = !MainGrid->ShowColCounter;
    mnFixFirstCol->Checked = !MainGrid->ShowRowCounter;
    tsbFixFirstCol->Down   = !MainGrid->ShowRowCounter;
    Application->MessageBox(
	  TEXT("ファイルの読み込み中は固定セルを変更できません。"),
	  TEXT("Cassava"), MB_ICONERROR);
    return;
  }

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
  if(MainGrid->FileOpenThread){
    mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
    tsbFixFirstRow->Down   = !MainGrid->ShowColCounter;
    mnFixFirstCol->Checked = !MainGrid->ShowRowCounter;
    tsbFixFirstCol->Down   = !MainGrid->ShowRowCounter;
    Application->MessageBox(
	  TEXT("ファイルの読み込み中は固定セルを変更できません。"),
	  TEXT("Cassava"), MB_ICONERROR);
    return;
  }

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
    TMenuItem *menuItem = static_cast<TMenuItem*>(Sender);
    String exe = menuItem->Hint;
    String arg0 = (String)("\"") + exe + "\"";
    if(FileName == ""){
      _wspawnl(P_NOWAITO, exe.c_str(), arg0.c_str(), NULL);
    }else{
      String arg1 = (String)("\"") + FileName + "\"";
      _wspawnl(P_NOWAITO, exe.c_str(), arg0.c_str(), arg1.c_str(), NULL);
    }
    if(menuItem->Tag){
      Close();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroSearchClick(TObject *Sender)
{
#ifdef CssvMacro
  std::map<String, TMenuItem *> oldItems;
  for(int i=mnMacro->Count - 1; i>=6; i--){
    TMenuItem *oldItem = mnMacro->Items[i];
    mnMacro->Delete(i);
    oldItems[oldItem->Name] = oldItem;
  }

  for(int i=1; i>=0; i--){
    String path = (i ? Pref->UserPath+"Macro\\*.cms"
                     : Pref->SharedPath+"Macro\\*.cms");
    TNotifyEvent onClick = (i ? &mnMacroUserExecClick : &mnMacroExecClick);

    TSearchRec sr;
    if(FindFirst(path, faAnyFile, sr) == 0){
      do{
        if(sr.Name.Length() > 0 && sr.Name[1] != '!'){
          String macroname = ChangeFileExt(sr.Name,"");
          String id = MakeId("macro", macroname, i);
          TMenuItem *newItem = oldItems[id];
          if(newItem){
            oldItems.erase(id);
          }else{
            newItem = new TMenuItem(mnMacro->Owner);
            newItem->Hint = macroname;
            newItem->Caption = macroname;
            newItem->Name = id;
            newItem->OnClick = onClick;
          }
          mnMacro->Add(newItem);
        }
      } while (FindNext(sr) == 0);
      FindClose(sr);
    }
  }

  for(std::map<String, TMenuItem *>::iterator it = oldItems.begin();
      it != oldItems.end(); ++it){
    delete it->second;
  }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroOpenUserFolderClick(TObject *Sender)
{
#ifdef CssvMacro
  AnsiString path = Pref->UserPath + "Macro";
  if(!DirectoryExists(path)){
    ForceDirectories(path);
  }
  spawnlp(P_NOWAITO, "Explorer.exe", "/idlist", path.c_str(), NULL);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroOpenFolderClick(TObject *Sender)
{
#ifdef CssvMacro
  spawnlp(P_NOWAITO, "Explorer.exe", "/idlist",
         (Pref->SharedPath + "Macro").c_str(), NULL);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroExecuteClick(TObject *Sender)
{
#ifdef CssvMacro
  AnsiString path = Pref->UserPath + "Macro";
  if(!DirectoryExists(path)){
    path = Pref->SharedPath + "Macro";
  }
  dlgOpenMacro->InitialDir = path;
  if(dlgOpenMacro->Execute()){
    AnsiString CmsFile = dlgOpenMacro->FileName;
    MacroExec(CmsFile, NULL);
  }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroUserExecClick(TObject *Sender)
{
#ifdef CssvMacro
  TMenuItem *Menu = static_cast<TMenuItem *>(Sender);
  AnsiString CmsFile = Pref->UserPath + "Macro\\" + Menu->Hint + ".cms";
  MacroExec(CmsFile, NULL);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroExecClick(TObject *Sender)
{
#ifdef CssvMacro
  TMenuItem *Menu = static_cast<TMenuItem *>(Sender);
  AnsiString CmsFile = Pref->SharedPath + "Macro\\" + Menu->Hint + ".cms";
  MacroExec(CmsFile, NULL);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acMacroTerminateExecute(TObject *Sender)
{
#ifdef CssvMacro
  StopAllMacros();
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acMacroTerminateUpdate(TObject *Sender)
{
  acMacroTerminate->Enabled = (GetRunningMacroCount() > 0);
}
//---------------------------------------------------------------------------
void TfmMain::MacroExec(String CmsFile, EncodedWriter *io)
{
#ifdef CssvMacro
  AnsiString InName = ChangeFileExt(ExtractFileName(CmsFile),"");
  TStringList *Modules = new TStringList;
  TStringList *InPaths = new TStringList;
  AnsiString inpath = ExtractFilePath(CmsFile);
  if(*(inpath.AnsiLastChar()) != '\\') inpath += "\\";
  InPaths->Add(inpath);
  InPaths->Add(Pref->UserPath + "Macro\\");
  InPaths->Add(Pref->SharedPath + "Macro\\");
  bool C = MacroCompile(CmsFile, InPaths, Modules, true);
  if(C){
    MainGrid->SetUndoCsv();
    MainGrid->UndoSetLock++;
    MainGrid->Invalidate();
    acMacroTerminate->Enabled = true;
    ExecMacro(InName, StopMacroCount, Modules, -1, -1, io);
    MainGrid->Invalidate();
    MainGrid->UndoSetLock--;
  }
  for(int i=Modules->Count-1; i>=0; i--){
    if(Modules->Objects[i]) delete Modules->Objects[i];
  }
  delete Modules;
  delete InPaths;
#endif
}
//---------------------------------------------------------------------------
void TfmMain::UpdateStatusbar()
{
#ifdef CssvMacro
  if(mnShowStatusbar->Checked && StatusbarCmsFile != ""){
    try{
      ExecMacro(StatusbarCmsFile, StopMacroCount, StatusbarMacroCache,
                -1, -1, NULL, true);
    }catch(...){}
  }
#endif
}
//---------------------------------------------------------------------------
void TfmMain::MacroScriptExec(String cmsname, String script)
{
#ifdef CssvMacro
  TStream *fs = new TMemoryStream();
  AnsiString ansiScript = script;
  fs->Write(ansiScript.c_str(), ansiScript.Length());
  fs->Position = 0;
  TStringList *modules = new TStringList;
  TStringList *inPaths = new TStringList;
  inPaths->Add(Pref->UserPath + "Macro\\");
  inPaths->Add(Pref->SharedPath + "Macro\\");
  bool C = MacroCompile(fs, inPaths, "", cmsname, ".cms", modules, true);
  if(C){
    acMacroTerminate->Enabled = true;
    ExecMacro(cmsname, StopMacroCount, modules, -1, -1);
  }
  delete fs;
  for(int i=modules->Count-1; i>=0; i--){
    if(modules->Objects[i]) delete modules->Objects[i];
  }
  delete modules;
  delete inPaths;
#endif
}
//---------------------------------------------------------------------------
String TfmMain::GetCalculatedCell(String Str, int ACol, int ARow)
{
  String ResultCell = (String)CALC_NG + Str;
#ifdef CssvMacro
  String CmsName = "";

  if(Str.Length() == 0 || Str[1] != '='){ return (AnsiString)CALC_NOTEXPR + Str; }
  Str.Delete(1,1);
  TStream *fs = NULL;
  TStringList *modules = NULL;
  TStringList *inPaths = NULL;
  try{
    CmsName = (AnsiString)"$@cell_" + ACol + "_" + ARow;
	AnsiString Formula = (AnsiString) Str;
	fs = new TMemoryStream();
    fs->Write("return ", 7);
	fs->Write(Formula.c_str(), Formula.Length());
    fs->Write(";", 1);
    fs->Position = 0;
    modules = new TStringList;
    inPaths = new TStringList;
    inPaths->Add(Pref->UserPath + "Macro\\");
    inPaths->Add(Pref->SharedPath + "Macro\\");
    bool C = MacroCompile(fs, inPaths, "", CmsName, ".cms", modules, false);
    if(C){
      AnsiString Result = ExecMacro(CmsName, StopMacroCount, modules, ACol, ARow, NULL, true);
      ResultCell = (AnsiString)CALC_OK + Result;
    }
  }catch(...){
    // エラー用のResultCell文字列は設定済み
  }
  if(fs){ delete fs; }
  if(modules){
    for(int i=modules->Count-1; i>=0; i--){
      if(modules->Objects[i]) delete modules->Objects[i];
    }
    delete modules;
  }
  if(inPaths){ delete inPaths; }
#endif
  return ResultCell;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnQuickFindClick(TObject *Sender)
{
  edFindText->Text = fmFind->edFindText->Text;
  pnlSearch->Visible = true;
  edFindText->SetFocus();
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
void __fastcall TfmMain::btnSearchCancelClick(TObject *Sender)
{
  pnlSearch->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::edFindTextKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if(Key == VK_RETURN){
    btnNextClick(Sender);
  }else if(Key == VK_ESCAPE){
    btnSearchCancelClick(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnNextClick(TObject *Sender)
{
  fmFind->rgDirection->ItemIndex = 1;
  fmFind->edFindText->Text = edFindText->Text;
  fmFind->btnNext->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnPreviousClick(TObject *Sender)
{
  fmFind->rgDirection->ItemIndex = 0;
  fmFind->edFindText->Text = edFindText->Text;
  fmFind->btnNext->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnSearchOptionClick(TObject *Sender)
{
  fmFind->edFindText->Text = edFindText->Text;
  fmFind->Show();
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

  fmSort->udSortLeft->Position   = R.Left;
  fmSort->udSortTop->Position    = R.Top;
  fmSort->udSortRight->Position  = R.Right;
  fmSort->udSortBottom->Position = R.Bottom;
  fmSort->udSortCol->Position    = sortcol;

  fmSort->Show();
  fmSort->seSortCol->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnpSortClick(TObject *Sender)
{
  int sortcol = MainGrid->Selection.Left;
  fmSort->udSortLeft->Position   = MainGrid->DataLeft;
  fmSort->udSortTop->Position    = MainGrid->DataTop;
  fmSort->udSortRight->Position  = MainGrid->DataRight;
  fmSort->udSortBottom->Position = MainGrid->DataBottom;
  fmSort->udSortCol->Position = sortcol;

  fmSort->Show();
  fmSort->seSortCol->SetFocus();

  MainGrid->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::sbSortClick(TObject *Sender)
{
  mnSortClick(this);
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
void __fastcall TfmMain::mnCheckUpdateClick(TObject *Sender)
{
  Version::UpdateCheck();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnAboutClick(TObject *Sender)
{
  String message = "Cassava Editor\n   Ver. " + Version::CurrentText()
    + "\n   by あすかぜ\n\t" + Version::CurrentDate();
  Application->MessageBox(message.c_str(), TEXT("バージョン情報"), 0);
}
//---------------------------------------------------------------------------

