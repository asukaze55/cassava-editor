//---------------------------------------------------------------------------
#include <vcl.h>
#include <Vcl.clipbrd.hpp>
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
#include "FormattedFileName.h"
#include "Macro.h"
#include "Compiler.h"
#include "Find.h"
#include "EncodedWriter.h"
#include "Version.h"
#include "Sort.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfmMain *fmMain;
//---------------------------------------------------------------------------
static String MaybeCompileMacro(String fileName, Preference *pref,
                                TStringList *modules)
{
  String cmsFile = pref->UserPath + "Macro\\" + fileName;
  if (!FileExists(cmsFile)) {
    cmsFile = pref->SharedPath + "Macro\\" + fileName;
  }
  if (!FileExists(cmsFile)) {
    return "";
  }
  TStringList *macroDirs = new TStringList;
  macroDirs->Add(pref->UserPath + "Macro\\");
  macroDirs->Add(pref->SharedPath + "Macro\\");
  bool compiled = MacroCompile(cmsFile, macroDirs, modules, true);
  delete macroDirs;
  return compiled ? cmsFile : (String)"";
}
//---------------------------------------------------------------------------
__fastcall TfmMain::TfmMain(TComponent* Owner)
  : TForm(Owner)
{
  FullPath = ExtractFilePath(ParamStr(0));
  if(*(FullPath.LastChar()) != '\\') FullPath += "\\";
  Pref = new Preference(FullPath);

  MainGrid = new TMainGrid(this);
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
  MainGrid->TabStop = false;
  Application->OnActivate = ApplicationActivate;
  Application->OnHint = ApplicationHint;

  History = new TStringList;
  ScreenDpi = Screen->PixelsPerInch;

  TTypeList defaultTypeList;
  defaultTypeList.Add(TTypeOption());
  defaultTypeList.Add(TTypeOption("CSV"));
  defaultTypeList.Add(TTypeOption("TSV"));
  MainGrid->TypeOption = defaultTypeList.DefItem();
  TypeList = defaultTypeList;

  ReadIni();
  ReadToolBar();

  bool FileOpening = false;
  StartupMacroDone = false;
  StatusbarCmsFile = "";
  ParamCmsFile = "";
  TimeStamp = 0;
  int wd = MainGrid->FixedCols;
  int ht = MainGrid->FixedRows;
  int positionShift = 0;
  int newWindowCount = 0;
  wchar_t **newWindowArgs = new wchar_t*[ParamCount() + 3];

  for (int i = 1; i <= ParamCount(); i++) {
    if (ParamStr(i)[1] == L'-') {
      if (ParamStr(i) == "-w") {
        wd = ParamStr(++i).ToIntDef(wd);
      } else if (ParamStr(i) == "-h") {
        ht = ParamStr(++i).ToIntDef(ht);
      } else if (ParamStr(i) == "-i") {
        positionShift = ParamStr(++i).ToIntDef(0);
      } else if (ParamStr(i) == "-m") {
        ParamCmsFile = ParamStr(++i);
      }
    } else {
      if (!FileOpening) {
        FileName = ParamStr(i);
        OpenFile(FileName);
        FileOpening = true;
      } else {
        newWindowCount++;
        newWindowArgs[newWindowCount] = new wchar_t[ParamStr(i).Length() + 3];
        wcscpy(newWindowArgs[newWindowCount],
            ((String)("\"") + ParamStr(i) + "\"").c_str());
      }
    }
  }

  if (newWindowCount > 0) {
    newWindowArgs[0] = ParamStr(0).c_str();
    newWindowArgs[newWindowCount + 1] = const_cast<wchar_t*>(L"-i");
    newWindowArgs[newWindowCount + 2] =((String)(positionShift + 1)).c_str();
    newWindowArgs[newWindowCount + 3] = nullptr;
    _wspawnv(P_NOWAITO, ParamStr(0).c_str(), newWindowArgs);
    for (int i = 1; i <= newWindowCount; i++) {
      delete[] newWindowArgs[i];
    }
  }
  delete[] newWindowArgs;

  if (!FileOpening) {
    FileName = "";
    UpdateTitle();
    MainGrid->Clear(wd + 1, ht + 1, true);
  }

  if (MainGrid->AlwaysShowEditor) {
    MainGrid->SetFocus();
    MainGrid->ShowEditor();
  } else {
    MainGrid->Options >> goAlwaysShowEditor;
    MainGrid->SetFocus();
    MainGrid->EditorMode = false;
  }

  if (!FileOpening) {
    StartupMacroDone = true;
    ExecStartupMacro();
  }
  SystemMacroCache = new TStringList;
  FormatCmsFile = MaybeCompileMacro("!format.cms", Pref, SystemMacroCache);
  if (FormatCmsFile != "") {
    MainGrid->OnGetFormattedCell = GetFormattedCell;
  }

  String statusbarCmsFile =
      MaybeCompileMacro("!statusbar.cms", Pref, SystemMacroCache);
  if (statusbarCmsFile != "") {
    String statusbarInit =
        GetMacroModuleName(statusbarCmsFile, "init", "0", false);
    if (SystemMacroCache->IndexOf(statusbarInit) >= 0) {
      try {
        ExecMacro(statusbarInit, StopMacroCount, SystemMacroCache, -1, -1,
                  nullptr, true);
      } catch (...) {}
    }
    StatusbarCmsFile = statusbarCmsFile;
    if (!FileOpening) {
      UpdateStatusbar();
    }
  }
}
//---------------------------------------------------------------------------
void TfmMain::ExecStartupMacro()
{
  String CmsFile;
  CmsFile = Pref->SharedPath + "Macro\\!startup.cms";
  if (FileExists(CmsFile)) {
    MacroExec(CmsFile, nullptr);
  }
  CmsFile = Pref->UserPath + "Macro\\!startup.cms";
  if (FileExists(CmsFile)) {
    MacroExec(CmsFile, nullptr);
  }

  if (ParamCmsFile != "") {
    if (FileExists(ParamCmsFile)) {
      MacroExec(ParamCmsFile, nullptr);
    } else {
      Application->MessageBox(
          (ParamCmsFile + "\nファイルが見つかりません。").c_str(),
          CASSAVA_TITLE, 0);
    }
  }
}
//---------------------------------------------------------------------------
void TfmMain::ExecOpenMacro(System::TObject* Sender)
{
  if(!StartupMacroDone){
    StartupMacroDone = true;
    ExecStartupMacro();
  }

  String CmsFile;
  CmsFile = Pref->SharedPath + "Macro\\!open.cms";
  if(FileExists(CmsFile)){
    MacroExec(CmsFile, nullptr);
  }
  CmsFile = Pref->UserPath + "Macro\\!open.cms";
  if(FileExists(CmsFile)){
    MacroExec(CmsFile, nullptr);
  }

  UpdateStatusbar();
}
//---------------------------------------------------------------------------
__fastcall TfmMain::~TfmMain()
{
  delete MainGrid;
  delete History;
  delete Pref;
  if (LockingFile) { delete LockingFile; }
  if (SystemMacroCache) { delete SystemMacroCache; }
}
//---------------------------------------------------------------------------
void TfmMain::ReadIni()
{
  History->Clear();

  IniFile *Ini = Pref->GetInifile();

  Style = Ini->ReadString("Mode", "Style", "Windows");
  int iniScreenDpi = Ini->ReadInteger("Position", "Dpi", ScreenDpi);
  double dpiRatio = (double)ScreenDpi / iniScreenDpi;
  int screenWidth = GetSystemMetrics (SM_CXSCREEN);
  int screenHeight = GetSystemMetrics (SM_CYSCREEN);
  int virtualScreenLeft = GetSystemMetrics (SM_XVIRTUALSCREEN);
  int virtualScreenTop = GetSystemMetrics (SM_YVIRTUALSCREEN);
  int virtualScreenRight =
      virtualScreenLeft + GetSystemMetrics (SM_CXVIRTUALSCREEN);
  int virtualScreenBottom =
      virtualScreenTop + GetSystemMetrics (SM_CYVIRTUALSCREEN);

  int iniWidth =
      Ini->ReadInteger("Position", "Width", Width) * dpiRatio + 0.5;
  Width = iniWidth <= screenWidth ? iniWidth : screenWidth;
  int iniHeight =
      Ini->ReadInteger("Position", "Height", Height) * dpiRatio + 0.5;
  Height = iniHeight <= screenHeight ? iniHeight : screenHeight;
  int iniLeft = Ini->ReadInteger("Position", "Left", -1);
  Left = (iniLeft >= virtualScreenLeft && iniLeft <= virtualScreenRight - Width)
      ? iniLeft : (screenWidth / 2 - Width / 2);
  int iniTop = Ini->ReadInteger("Position", "Top", -1);
  Top = (iniTop >= virtualScreenTop && iniTop <= virtualScreenBottom - Height)
      ? iniTop : (screenHeight / 2 - Height / 2);

  for (int i = 1; i <= ParamCount(); i++) {
    if (ParamStr(i) == "-i") {
      int ps =
          ParamStr(i + 1).ToIntDef(0) * (GetSystemMetrics(SM_CYCAPTION) + 8);
      Left += ps;
      Top += ps;
      break;
    }
  }

  if (Ini->ReadBool("Mode", "ShowToolbar", true) != mnShowToolbar->Checked) {
    mnShowToolbarClick(this);
  }
  if (Ini->ReadBool("Mode", "ShowStatusbar", true)
      != mnShowStatusbar->Checked) {
    mnShowStatusbarClick(this);
  }

  MainGrid->Color =
      (TColor) Ini->ReadInteger("Font", "BgColor", MainGrid->Color);
  MainGrid->CalcBgColor =
      (TColor) Ini->ReadInteger("Font", "CalcBgColor", clAqua);
  MainGrid->CalcErrorBgColor =
      (TColor) Ini->ReadInteger("Font", "CalcErrorBgColor", clRed);
  MainGrid->CalcErrorFgColor = (TColor) Ini->ReadInteger(
      "Font", "CalcErrorFgColor", MainGrid->Font->Color);
  MainGrid->CalcFgColor =
      (TColor) Ini->ReadInteger("Font", "CalcFgColor", MainGrid->Font->Color);
  MainGrid->CurrentRowBgColor =
      (TColor) Ini->ReadInteger("Font", "CurrentRowBgColor", MainGrid->Color);
  MainGrid->CurrentColBgColor =
      (TColor) Ini->ReadInteger("Font", "CurrentColBgColor", MainGrid->Color);
  MainGrid->DummyBgColor =
      (TColor) Ini->ReadInteger("Font", "DummyBgColor", clCream);
  MainGrid->EvenRowBgColor =
      (TColor) Ini->ReadInteger("Font", "EvenRowBgColor", MainGrid->Color);
  MainGrid->FixFgColor =
      (TColor) Ini->ReadInteger("Font", "FixFgColor", MainGrid->Font->Color);
  MainGrid->FixedColor =
      (TColor) Ini->ReadInteger("Font", "FixedColor", MainGrid->FixedColor);
  MainGrid->FoundBgColor =
      (TColor) Ini->ReadInteger("Font", "FoundBgColor", clYellow);
  MainGrid->UrlColor = (TColor) Ini->ReadInteger("Font", "UrlColor", clBlue);
  MainGrid->WordWrap = Ini->ReadBool("Font", "WordWrap", false);
  MainGrid->TextAlignment =
      Ini->ReadInteger("Font", "TextAlign", MainGrid->TextAlignment);
  MainGrid->NumberComma = Ini->ReadInteger("Font", "NumberComma", 0);
  MainGrid->DecimalDigits = Ini->ReadInteger("Font", "DecimalDigits", -1);
  MainGrid->MinColWidth =
      Ini->ReadInteger("Font", "MinColWidth", MainGrid->MinColWidth);

  Show();
  SearchMacro(mnMacro);
  dpiRatio = (double)ScreenDpi / iniScreenDpi;

  WindowState =
      Ini->ReadInteger("Position", "Mode", 0) == 2 ? wsMaximized : wsNormal;
  MainGrid->Font->Name = Ini->ReadString("Font", "Name",
      Screen->Fonts->IndexOf("Yu Gothic UI") >= 0 ? "Yu Gothic UI"
                                                  : "ＭＳ Ｐゴシック");
  MainGrid->Font->Size = Ini->ReadInteger("Font", "Size", 12) * dpiRatio + 0.5;
  if (Ini->ReadBool("Font", "Bold", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsBold;
  }
  if (Ini->ReadBool("Font", "Italic", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsItalic;
  }
  if (Ini->ReadBool("Font", "Underline", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsUnderline;
  }
  if (Ini->ReadBool("Font", "StrikeOut", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsStrikeOut;
  }
  MainGrid->TBMargin = Ini->ReadInteger("Font", "TBMargin", 2);
  MainGrid->LRMargin = Ini->ReadInteger("Font", "LRMargin", 4);
  MainGrid->CellLineMargin = Ini->ReadInteger("Font", "CellLineMargin", 0);
  MainGrid->Font->Color =
      (TColor) Ini->ReadInteger("Font", "FgColor", MainGrid->Font->Color);
  MainGrid->Canvas->Font = MainGrid->Font;

    PrintFontName = Ini->ReadString("Print","FontName",MainGrid->Font->Name);
    PrintFontSize = Ini->ReadInteger("Print","FontSize",MainGrid->Font->Size);
    PrintMargin[0] = Ini->ReadInteger("Print","MarginLeft",15);
    PrintMargin[1] = Ini->ReadInteger("Print","MarginRight",15);
    PrintMargin[2] = Ini->ReadInteger("Print","MarginTop",15);
    PrintMargin[3] = Ini->ReadInteger("Print","MarginBottom",15);
    PrintHeader = Ini->ReadString("Print", "Header", "%f");
    PrintHeaderPosition = Ini->ReadInteger("Print", "HeaderPosition", 2);
    PrintFooter = Ini->ReadString("Print", "Footer", "- %p -");
    PrintFooterPosition = Ini->ReadInteger("Print", "FooterPosition", 2);

    int TypeCount = Ini->ReadInteger("DataType","Count", 0);
    if(TypeCount > 0){
      TTypeList newTypeList;
      for(int i=0; i<TypeCount; i++){
        String Section = (String)"DataType:" + i;
        TTypeOption option;
        option.Name = Ini->ReadString(Section, "Name", "[新規]");
        String exts = Ini->ReadString(Section, "Exts", "csv");
        option.SetExts(exts);
        option.ForceExt = Ini->ReadBool(Section, "ForceExt", false);
        option.SepChars = Ascii2Ctrl(Ini->ReadString(Section, "SepChars", ",\\t"));
        option.WeakSepChars = Ascii2Ctrl(Ini->ReadString(Section, "WeakSepChars", "\\_"));
        option.QuoteChars = Ascii2Ctrl(Ini->ReadString(Section, "QuoteChars", "\""));
        option.QuoteOption = Ini->ReadInteger(Section, "Quote", 1);
        option.OmitComma = Ini->ReadBool(Section, "OmitComma", true);
        option.DummyEof = Ini->ReadBool(Section, "DummyEof", false);
        option.DummyEol = Ini->ReadBool(Section, "DummyEol", false);
        newTypeList.Add(option);
      }
      TypeList = newTypeList;
    }

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
    MainGrid->DragBehavior = (TDragBehavior)
        Ini->ReadInteger("Mode", "DragCounter", dbMoveIfSelected);
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

    MainGrid->ShowToolTipForLongCell
      = Ini->ReadBool("Mode", "ShowToolTipForLongCell", false);
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
    LockingFile = nullptr;
    CheckTimeStamp = Ini->ReadBool("Mode","CheckTimeStamp", true);
    SortByNumber = Ini->ReadBool("Mode","SortByNumber",true);
    SortIgnoreCase = Ini->ReadBool("Mode", "SortIgnoreCase", false);
    SortIgnoreZenhan = Ini->ReadBool("Mode", "SortIgnoreZenhan", false);
    SortDirection = Ini->ReadInteger("Mode", "SortDirection", 0);
    EncodingDetector.Enabled = Ini->ReadBool("Mode", "CheckKanji", true);
    EncodingDetector.DefaultEncoding =
        ToEncoding(Ini->ReadInteger("Mode", "DefaultCharCode", CHARCODE_UTF8));
    if (FileName == "") {
      MainGrid->Encoding = EncodingDetector.DefaultEncoding;
    }
    MainGrid->CompactColWidth =
        Ini->ReadBool("Mode","CompactColWidth", true);
    MainGrid->CalcWidthForAllRow =
        Ini->ReadBool("Mode","CalcWidthForAllRow", 0);
    MainGrid->MaxRowHeightLines =
        Ini->ReadFloat("Mode", "MaxRowHeightLines", 1.5);
    StopMacroCount = Ini->ReadInteger("Mode","StopMacro", 0);
    MainGrid->UndoList->MaxCount = Ini->ReadInteger("Mode", "UndoCount", 100);

    FindCase = Ini->ReadBool("Search", "Case", true);
    FindWordSearch = Ini->ReadBool("Search", "Word", false);
    FindRegex = Ini->ReadBool("Search", "Regex", false);
    FindRange = Ini->ReadInteger("Search", "Range", 3);

    String LaunchName[3];
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
  for (int i = 0; i < 10; i++) {
    String historyFile = Ini->ReadString("History", (String)i, "");
    if (historyFile != "") {
      History->Add(historyFile);
    } else {
      break;
    }
  }
  dlgOpenMacro->InitialDir = Ini->ReadString("History", "Macro", "");

  delete Ini;

  if(FileExists(Pref->Path + "AutoKey.csv"))
  {
    fmKey = new TfmKey(Application);
      fmKey->MakeTree();
      if(fmKey->LoadKey(Pref->Path + "AutoKey.csv"))
        fmKey->MenuUpDate();
    delete fmKey;
  }

  mnAppli0->Caption = (String)"&0: " + LaunchName[0];
  mnAppli1->Caption = (String)"&1: " + LaunchName[1];
  mnAppli2->Caption = (String)"&2: " + LaunchName[2];

  SetHistory("");
}
//---------------------------------------------------------------------------
void TfmMain::WriteIni(bool PosSlide)
{
  try {
    IniFile *Ini = Pref->GetInifile();
    Ini->WriteString("Mode", "Style", Style);
    Ini->WriteInteger("Position", "Mode", WindowState == wsMaximized ? 2 : 0);
    if(WindowState == wsNormal){
      int Slide = PosSlide ? 32 : 0;
      Ini->WriteInteger("Position","Left",Left+Slide);
      Ini->WriteInteger("Position","Top",Top+Slide);
      Ini->WriteInteger("Position","Width",Width);
      Ini->WriteInteger("Position","Height",Height);
    }
    Ini->WriteInteger("Position", "Dpi", ScreenDpi);
    Ini->WriteString("Font","Name",MainGrid->Font->Name);
    Ini->WriteInteger("Font","Size",MainGrid->Font->Size);
    Ini->WriteInteger("Font","Bold",MainGrid->Font->Style.Contains(fsBold));
    Ini->WriteInteger("Font","Italic",MainGrid->Font->Style.Contains(fsItalic));
    Ini->WriteInteger("Font","Underline",MainGrid->Font->Style.Contains(fsUnderline));
    Ini->WriteInteger("Font","StrikeOut",MainGrid->Font->Style.Contains(fsStrikeOut));
    Ini->WriteInteger("Font","TBMargin",MainGrid->TBMargin);
    Ini->WriteInteger("Font","LRMargin",MainGrid->LRMargin);
    Ini->WriteInteger("Font","CellLineMargin",MainGrid->CellLineMargin);
    Ini->WriteInteger("Font", "BgColor", MainGrid->Color);
    Ini->WriteInteger("Font", "CalcBgColor", MainGrid->CalcBgColor);
    Ini->WriteInteger("Font", "CalcErrorBgColor", MainGrid->CalcErrorBgColor);
    Ini->WriteInteger("Font", "CalcErrorFgColor", MainGrid->CalcErrorFgColor);
    Ini->WriteInteger("Font", "CalcFgColor", MainGrid->CalcFgColor);
    Ini->WriteInteger("Font", "CurrentColBgColor", MainGrid->CurrentColBgColor);
    Ini->WriteInteger("Font", "CurrentRowBgColor", MainGrid->CurrentRowBgColor);
    Ini->WriteInteger("Font", "DummyBgColor", MainGrid->DummyBgColor);
    Ini->WriteInteger("Font", "EvenRowBgColor", MainGrid->EvenRowBgColor);
    Ini->WriteInteger("Font", "FgColor", MainGrid->Font->Color);
    Ini->WriteInteger("Font", "FixFgColor", MainGrid->FixFgColor);
    Ini->WriteInteger("Font", "FixedColor", MainGrid->FixedColor);
    Ini->WriteInteger("Font", "FoundBgColor", MainGrid->FoundBgColor);
    Ini->WriteInteger("Font", "UrlColor", MainGrid->UrlColor);
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
    Ini->WriteString("Print", "Header", PrintHeader);
    Ini->WriteInteger("Print", "HeaderPosition", PrintHeaderPosition);
    Ini->WriteString("Print", "Footer", PrintFooter);
    Ini->WriteInteger("Print", "FooterPosition", PrintFooterPosition);

    int DataCount = TypeList.Count;
    Ini->WriteInteger("DataType", "Count", DataCount);
    for(int i=0; i<DataCount; i++){
      String Section = (String)"DataType:" + i;
      TTypeOption *TO = TypeList.Items(i);
      Ini->WriteString(Section, "Name", TO->Name);
      Ini->WriteString(Section, "Exts", TO->GetExtsStr(0));
      Ini->WriteBool(Section, "ForceExt", TO->ForceExt);
      Ini->WriteString(Section, "SepChars", Ctrl2Ascii(TO->SepChars));
      Ini->WriteString(Section, "WeakSepChars", Ctrl2Ascii(TO->WeakSepChars));
      Ini->WriteString(Section, "QuoteChars", Ctrl2Ascii(TO->QuoteChars));
      Ini->WriteInteger(Section, "Quote", TO->QuoteOption);
      Ini->WriteBool(Section, "OmitComma", TO->OmitComma);
      Ini->WriteBool(Section, "DummyEof", TO->DummyEof);
      Ini->WriteBool(Section, "DummyEol", TO->DummyEol);
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
    Ini->WriteInteger("Mode", "DragCounter", MainGrid->DragBehavior);
    Ini->WriteInteger("Mode","EnterMove",MainGrid->EnterMove);
    Ini->WriteInteger("Mode","FixTopRow",
      MainGrid->ShowColCounter ? 0 : MainGrid->FixedRows);
    Ini->WriteInteger("Mode","FixLeftCol",
      MainGrid->ShowRowCounter ? 0 : MainGrid->FixedCols);
    Ini->WriteBool("Mode","ShowToolbar",mnShowToolbar->Checked);
    Ini->WriteBool("Mode","ShowStatusbar",mnShowStatusbar->Checked);
    Ini->WriteBool("Mode", "ShowToolTipForLongCell",
      MainGrid->ShowToolTipForLongCell);
    Ini->WriteInteger("Mode","HintPause",Application->HintPause);
    Ini->WriteInteger("Mode","HintHidePause",Application->HintHidePause);
    Ini->WriteBool("Mode","LeftArrowInCell",MainGrid->LeftArrowInCell);
    Ini->WriteInteger("Mode","WheelMoveCursol",MainGrid->WheelMoveCursol);
    Ini->WriteInteger("Mode","WheelScrollStep",MainGrid->WheelScrollStep);
    Ini->WriteBool("Mode","AlwaysShowEditor", MainGrid->AlwaysShowEditor);
    Ini->WriteBool("Mode","SortAll", SortAll);
    Ini->WriteInteger("Mode","UseURL", MainGrid->DblClickOpenURL);

    Ini->WriteBool("Mode", "NewWindow", MakeNewWindow);
    Ini->WriteBool("Mode"," TitleFullPath", TitleFullPath);
    Ini->WriteInteger("Mode", "LockFile", LockFile);
    Ini->WriteBool("Mode", "CheckTimeStamp", CheckTimeStamp);
    Ini->WriteBool("Mode", "SortByNumber", SortByNumber);
    Ini->WriteBool("Mode", "SortIgnoreCase", SortIgnoreCase);
    Ini->WriteBool("Mode", "SortIgnoreZenhan", SortIgnoreZenhan);
    Ini->WriteBool("Mode", "SortDirection", SortDirection);
    Ini->WriteBool("Mode", "CheckKanji", EncodingDetector.Enabled);
    Ini->WriteInteger("Mode", "DefaultCharCode",
        ToCharCode(EncodingDetector.DefaultEncoding));
    Ini->WriteBool("Mode", "CompactColWidth", MainGrid->CompactColWidth);
    Ini->WriteBool("Mode", "CalcWidthForAllRow", MainGrid->CalcWidthForAllRow);
    Ini->WriteFloat("Mode", "MaxRowHeightLines", MainGrid->MaxRowHeightLines);
    Ini->WriteInteger("Mode", "StopMacro", StopMacroCount);
    Ini->WriteInteger("Mode", "UndoCount", MainGrid->UndoList->MaxCount);

    Ini->WriteBool("Search", "Case", fmFind->cbCase->Checked);
    Ini->WriteBool("Search", "Word", fmFind->cbWordSearch->Checked);
    Ini->WriteBool("Search", "Regex", fmFind->cbRegex->Checked);
    Ini->WriteInteger("Search", "Range", fmFind->rgRange->ItemIndex);

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

    for (int i = 0; i < History->Count; i++) {
      Ini->WriteString("History", (String)i, History->Strings[i]);
    }
    for (int i = History->Count; i < 10; i++) {
      Ini->DeleteKey("History", (String)i);
    }
    if (dlgOpenMacro->InitialDir != "" &&
        dlgOpenMacro->InitialDir != Pref->UserPath + "Macro") {
      Ini->WriteString("History", "Macro", dlgOpenMacro->InitialDir);
    } else {
      Ini->DeleteKey("History", "Macro");
    }

    delete Ini;
  }catch(...){}
}
//---------------------------------------------------------------------------
void TfmMain::ReadToolBar()
{
  CoolBarResize(nullptr);
  tbarAdditional->Left = tbarNormal->Left + tbarNormal->Width;

  String toolbarcsv = Pref->Path + "ToolBar.csv";
  if (!FileExists(toolbarcsv)) {
    return;
  }

  bool visible = CoolBar->Visible;
  CoolBar->Visible = false;
  tbarNormal->Visible = false;
  tbarAdditional->Visible = false;

  TTypeOption typeOption("CSV");
  CsvReader reader(&typeOption, toolbarcsv, TEncoding::Default);
  TStringList *list = new TStringList();
  reader.ReadLine(list);
  if (list->Count == 0 || list->Strings[0] != "(Cassava-ToolBarSetting)") {
    delete list;
    return;
  }
  TToolBar *toolBar = nullptr;
  int width = 0;
  int tbarTop = 0;
  int tbarLeft = 0;
  while (reader.ReadLine(list)) {
    while (list->Count < 3) {
      list->Add("");
    }
    String str0 = list->Strings[0];
    String name = list->Strings[1];
    String action = list->Strings[2];

    if (str0 != "" && str0[1] == '=') {
      tbarTop += tbarNormal->Height;
      tbarLeft = -1;
    } else if (str0 != "" && str0[1] == '#') {
      if (toolBar) {
        toolBar->Width = width;
        if (tbarLeft >= 0) {
          tbarLeft += width;
        }
      }
      if (tbarLeft < 0) {
        tbarLeft = 0;
      }
      if (str0 == "#1") {
        toolBar = tbarNormal;
        toolBar->Visible = true;
        width = toolBar->Width;
      } else if (str0 == "#2") {
        toolBar = tbarAdditional;
        toolBar->Visible = true;
        width = toolBar->Width;
      } else {
        toolBar = new TToolBar(CoolBar);
        toolBar->Parent = CoolBar;
        toolBar->Wrapable = false;
        toolBar->AutoSize = true;
        toolBar->Font->Height = 16 * PixelsPerInch / 96;
        width = 0;
      }
      toolBar->Top = tbarTop;
      toolBar->Left = tbarLeft;
      String toolbarbmp = Pref->Path + name;
      if (name == "#1") {
        if (Style == "Windows10 Dark") {
          toolBar->Images = imlNormalDark;
          toolBar->DisabledImages = imlNormalDarkDisabled;
        } else {
          toolBar->Images = imlNormal;
          toolBar->DisabledImages = imlNormalDisabled;
        }
      } else if (name == "#2") {
        if (Style == "Windows10 Dark") {
          toolBar->Images = imlAdditionalDark;
        } else {
          toolBar->Images = imlAdditional;
        }
      } else if (name != "" && FileExists(toolbarbmp)) {
        TCustomImageList *images = new TCustomImageList(16, 16);
        images->FileLoad(rtBitmap, toolbarbmp, clSilver);
        toolBar->Images = images;
      }
    } else if (name == "-") {
      TToolButton *button = new TToolButton(toolBar);
      button->Style = tbsSeparator;
      button->Left = width;
      button->Parent = toolBar;
      button->Width = 8;
      width += button->Width;
    } else if (name != "") {
      TToolButton *button = new TToolButton(toolBar);
      button->Left = width;
      button->Parent = toolBar;
      TMenuItem *menuItem = FindMenuItem(action);
      if (menuItem) {
        if (menuItem->Action) {
          button->Action = menuItem->Action;
        } else {
          button->OnClick = menuItem->OnClick;
        }
        button->Hint = name;
      } else {
        button->OnClick = UserToolBarAction;
        button->Hint = StringReplace(name, "|", "_",
            TReplaceFlags() << rfReplaceAll) + "|" + action;
      }
      int imageIndex = str0.ToIntDef(-1);
      if (imageIndex >= 0) {
        button->ImageIndex = imageIndex;
      } else {
        toolBar->AllowTextButtons = true;
        button->Style = tbsTextButton;
        button->Caption = str0 != "" ? str0 : name;
      }
      if (action == "OpenHistory") {
        button->OnClick = mnOpenClick;
        button->Hint = name;
        button->Style = tbsDropDown;
        button->DropdownMenu = PopMenuOpen;
      }
      width += button->Width;
    }
  }
  if (toolBar) {
    toolBar->Width = width;
  }
  delete list;
  CoolBar->Visible = visible;
}
//---------------------------------------------------------------------------
TMenuItem *FindMenuItemInternal(TMenuItem *root, String name)
{
  for (int i = 0; i < root->Count; i++) {
    if (root->Items[i]->Name == name) {
      return root->Items[i];
    } else if (root->Items[i]->Count > 0) {
      TMenuItem *menuItem = FindMenuItemInternal(root->Items[i], name);
      if (menuItem) {
        return menuItem;
      }
    }
  }
  return nullptr;
}
//---------------------------------------------------------------------------
TMenuItem *TfmMain::FindMenuItem(String name)
{
  return FindMenuItemInternal(MainMenu->Items, (String)"mn" + name);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::UserToolBarAction(TObject *Sender)
{
  TControl *control = static_cast<TControl*>(Sender);
  String action = control->Hint;
  int pos = action.Pos("|");
  if(pos > 0){
    action = action.SubString(pos + 1, action.Length() - pos).Trim();
  }
  if(action != ""){
    String CmsFile = Pref->UserPath + "Macro\\" + action;
    if(!FileExists(CmsFile)){
      CmsFile = Pref->SharedPath + "Macro\\" + action;
    }
    if(FileExists(CmsFile)){
      // 一致するファイルが存在する場合、そのファイルを実行
      MacroExec(CmsFile, nullptr);
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
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::CoolBarResize(TObject *Sender)
{
  CoolBar->RowSize = tbarNormal->Height + 4;

  for (int i = 0; i < CoolBar->ControlCount; i++) {
    TToolBar *toolbar = static_cast<TToolBar *>(CoolBar->Controls[i]);
    int width = 0;
    for (int j = 0; j < toolbar->ControlCount; j++) {
      width += toolbar->Controls[j]->Width;
    }
    toolbar->Width = width;
  }
}
//---------------------------------------------------------------------------
void TfmMain::SetTypeList(const TTypeList &TypeList)
{
  String name = MainGrid->TypeOption->Name;
  FTypeList = TypeList;
  MainGrid->TypeOption = FTypeList.Items(FTypeList.IndexOf(name));

  SetFilter();
  SetCutMenu(mnCutFormat);
  SetCutMenu(mnpCutFormat);
  SetCopyMenu(mnCopyFormat);
  SetCopyMenu(mnpCopyFormat);
  SetPasteMenu(mnPasteFormat);
  SetPasteMenu(mnpPasteFormat);
}
//---------------------------------------------------------------------------
void TfmMain::SetFilter()
{
  String OFilter = "";
  String SFilter = "";
  String FilterExt;
  TStringList *AllExts = new TStringList;
  for (int i = 0; i < TypeList.Count; i++) {
    TTypeOption *p = TypeList.Items(i);
    FilterExt = "";
    for(int j=0; j<p->Exts.size(); j++){
      if(j > 0) FilterExt += ";";
      String str = p->Exts[j];
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
  String CFilter = (String)"Cassava (" + FilterExt + ")|" + FilterExt + "|";
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
    if((caption[i] >= 'A' && caption[i] <= 'Z') ||
       (caption[i] >= 'a' && caption[i] <= 'z') ||
       (caption[i] >= '0' && caption[i] <= '9')){
      id += caption[i];
    }else{
      id += "_" + ToHex(caption[i]);
    }
  }
  return id + "_" + i;
}
//---------------------------------------------------------------------------
void TfmMain::SetCutMenu(TMenuItem *Item)
{
  Item->Clear();
  for (int i = 0; i < TypeList.Count; i++) {
    TMenuItem *MI = new TMenuItem(Item->Owner);
    MI->Caption = TypeList.Items(i)->Name;
    if(Item == mnCutFormat){
      MI->Name = MakeId("cutformat", MI->Caption, i);
    }
    MI->Tag = i;
    MI->OnClick = mnCutFormatDefaultClick;
    Item->Add(MI);
  }
}
//---------------------------------------------------------------------------
void TfmMain::SetCopyMenu(TMenuItem *Item)
{
  Item->Clear();
  for (int i = 0; i < TypeList.Count; i++) {
    TMenuItem *MI = new TMenuItem(Item->Owner);
    MI->Caption = TypeList.Items(i)->Name;
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
  for (int i = 0; i < TypeList.Count; i++) {
    TMenuItem *MI = new TMenuItem(Item->Owner);
    MI->Caption = TypeList.Items(i)->Name;
    if(Item == mnPasteFormat){
      MI->Name = MakeId("pasteformat", MI->Caption, i);
    }
    MI->Tag = i;
    MI->OnClick = mnPasteFormatDefaultClick;
    Item->Add(MI);
  }
}
//---------------------------------------------------------------------------
void TfmMain::SetStyle(String Value)
{
  if (Value == FStyle) {
    return;
  }

  FStyle = Value;
  TStyleManager::TrySetStyle(FStyle);
  if (Style == "Windows10 Dark") {
    tbarNormal->Images = imlNormalDark;
    tbarNormal->DisabledImages = imlNormalDarkDisabled;
    tbarAdditional->Images = imlAdditionalDark;
  } else {
    tbarNormal->Images = imlNormal;
    tbarNormal->DisabledImages = imlNormalDisabled;
    tbarAdditional->Images = imlAdditional;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridChangeModified(TObject *Sender)
{
  bool modified = MainGrid->Modified;
  acSave->Enabled = modified;
  if (modified && LockFile == cssv_lfEdit && LockingFile == nullptr &&
      FileName != "") {
    LockingFile = new TFileStream(FileName, fmOpenWrite|fmShareDenyWrite);
  }
  UpdateTitle();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridKeyDown(System::TObject* Sender,
                               Word &Key, Classes::TShiftState Shift)
{
  if (Key == VK_ESCAPE) {
    if (!MainGrid->AlwaysShowEditor) {
      MainGrid->Options >> goAlwaysShowEditor;
      MainGrid->EditorMode = false;
    }
    if (pnlSearch->Visible) {
      pnlSearch->Visible = false;
    }
  } else if (Key == VK_CANCEL) {
    StopAllMacros();
  } else if (Key == VK_MENU) {
    Menu = MainMenu;
  } else {
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
        L"他のアプリケーションによってファイルが更新されました。\n"
        L"再読み込みしますか？",
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
void __fastcall TfmMain::FormAfterMonitorDpiChanged(TObject *Sender, int OldDPI,
                                                    int NewDPI)
{
  ScreenDpi = NewDPI;
  MainGrid->Canvas->Font->Size = MainGrid->Font->Size;
  MainGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::FormShow(TObject *Sender)
{
  // Need to call DragAcceptFiles again when the window handle gets changed.
  MainGrid->SetDragAcceptFiles(true);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnNewClick(TObject *Sender)
{
  if(MakeNewWindow){
    WriteIni(true);
    _wspawnl(P_NOWAITO, ParamStr(0).c_str(), ParamStr(0).c_str(), nullptr);
  }else{
    if(IfModifiedThenSave())
    {
      MainGrid->Clear();
      if(mnFixFirstRow->Checked || mnFixFirstCol->Checked) {
        // Clear() でファイル読み込みを中断した後で固定解除する必要がある
        if (mnFixFirstRow->Checked) {
          acFixFirstRowExecute(this);
        }
        if (mnFixFirstCol->Checked) {
          acFixFirstColExecute(this);
        }
        // 1列目・1行目の幅を初期値に戻すため、再度 Clear()
        MainGrid->Clear();
      }
      FileName = "";
      UpdateTitle();
      dlgSave->FilterIndex = 0;
      mnReload->Enabled = false;
      mnReloadCode->Enabled = false;
      UpdateStatusbar();

      if(LockingFile){
        delete LockingFile;
        LockingFile = nullptr;
      }
    }
  }
}
//---------------------------------------------------------------------------
void TfmMain::OpenFile(String OpenFileName, int CharCode,
    const TTypeOption *Format)
{
  if(!FileExists(OpenFileName)){
    Application->MessageBox(
      ("ファイル " + OpenFileName + " は存在しません").c_str(),
      ExtractFileName(OpenFileName).c_str(), MB_ICONERROR);
    return;
  }
  if(LockingFile){
    delete LockingFile;
    LockingFile = nullptr;
  }
  if (Format == nullptr) {
    Format = TypeList.FindForFileName(OpenFileName);
  }
  if (CharCode == CHARCODE_AUTO) {
    MainGrid->LoadFromFile(OpenFileName, EncodingDetector.Detect(OpenFileName),
        /* isDetectedEncoding= */ true, Format, ExecOpenMacro);
  } else {
    MainGrid->LoadFromFile(OpenFileName, ToEncoding(CharCode),
        /* isDetectedEncoding= */ false, Format, ExecOpenMacro);
  }
  FileName = OpenFileName;
  UpdateTitle();
  SetHistory(FileName);
  FileAge(FileName, TimeStamp);
  dlgSave->FilterIndex = TypeList.IndexOf(MainGrid->TypeOption) + 1;
  tmAutoSaver->Enabled = false;
  mnReload->Enabled = true;
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
            ((String)"\"" + files->Strings[i] + "\"").c_str(), nullptr);
      }
    }
    dlgOpen->Options >> ofAllowMultiSelect;
  }else{
    if(IfModifiedThenSave()) {
      if(dlgOpen->Execute()) {
        int index = dlgOpen->FilterIndex - 1;
        const TTypeOption *format =
            (index > 0 && index < TypeList.Count)
                ? TypeList.Items(index) : nullptr;
        OpenFile(dlgOpen->FileName, CHARCODE_AUTO, format);
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
          ((String)"\"" + DropFileNames[i] + "\"").c_str(), nullptr);
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
  if (FileName == "") { return; }

  String fileNameToReload = FileName;
  if (MainGrid->Modified) {
    int answer = Application->MessageBox(
                     L"編集中の変更を別名で保存しますか？",
                     CASSAVA_TITLE, MB_YESNOCANCEL + MB_ICONQUESTION);
    if (answer == IDYES) {
      mnSaveAsClick(Sender);
    } else if (answer == IDCANCEL) {
      return;
    }
  }

  OpenFile(fileNameToReload);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnReloadCodeClick(TObject *Sender)
{
  int code = static_cast<TMenuItem *>(Sender)->Tag;

  String fileNameToReload = FileName;
  if (MainGrid->Modified) {
    int answer = Application->MessageBox(
                     L"編集中の変更を別名で保存しますか？",
                     CASSAVA_TITLE, MB_YESNOCANCEL + MB_ICONQUESTION);
    if (answer == IDYES) {
      mnSaveAsClick(Sender);
    } else if (answer == IDCANCEL) {
      return;
    }
  }

  OpenFile(fileNameToReload, code);
}
//---------------------------------------------------------------------------
String TfmMain::GetUiFileName()
{
  if (FileName == "") {
    return "無題";
  }
  return TitleFullPath ? FileName : ExtractFileName(FileName);
}
//---------------------------------------------------------------------------
void TfmMain::UpdateTitle()
{
  String title = MainGrid->Modified ? "* " : "";
  if (FileName == "") {
    title += CASSAVA_TITLE;
  } else {
    title += TitleFullPath ? FileName : ExtractFileName(FileName);
  }
  Caption = title;
  Application->Title = title;
}
//---------------------------------------------------------------------------
bool TfmMain::IfModifiedThenSave()
{
  if (MainGrid->Modified) {
    int a = Application->MessageBox(
                (GetUiFileName() + " への変更を保存しますか？").c_str(),
                CASSAVA_TITLE, MB_YESNOCANCEL + MB_ICONQUESTION);
    if (a == IDYES) {
      if (MainGrid->FileOpenThread) {
        Application->MessageBox(
            L"ファイルの読み込みが完了していないため保存できません。",
            CASSAVA_TITLE, MB_ICONERROR);
        return false;
      }
      acSaveExecute(this);
    } else if (a == IDCANCEL) {
      return false;
    }
  }

  try {
    if(FileName != ""){
      if(BackupOnSave && DelBuSExit){
        String BuFN = FormattedFileName(BuFileNameS, FileName);
        if(FileExists(BuFN)) DeleteFile(BuFN);
      }
      if(BackupOnTime && DelBuT){
        String BuFN = FormattedFileName(BuFileNameT, FileName);
        if(FileExists(BuFN)) DeleteFile(BuFN);
      }
    }
  }catch(...){}

  return true;
}
//---------------------------------------------------------------------------
void TfmMain::SaveFile(const TTypeOption *Format)
{
  if(MainGrid->FileOpenThread){
    Application->MessageBox(
        L"ファイルの読み込みが完了していないため保存できません。",
        CASSAVA_TITLE, MB_ICONERROR);
    return;
  }

  if(FileName == "")
    mnSaveAsClick(this);
  else {
    // ロックを解除
    if(LockingFile){
      delete LockingFile;
      LockingFile = nullptr;
    }

    // バックアップ前に、アクセス権限を確認
    if(FileExists(FileName)){
      TFileStream *AccessTest = nullptr;
      try {
        AccessTest = new TFileStream(FileName, fmOpenReadWrite|fmShareDenyWrite);
        delete AccessTest;
        AccessTest = nullptr;
      }catch(Exception &ex){
        if(AccessTest){
          delete AccessTest;
        }
        Application->ShowException(&ex);
        return;
      }
    }

    // 必要ならばバックアップ
    String NewFile = "";
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
  SaveFile(nullptr);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSaveAsClick(TObject *Sender)
{
  if (FileName == "") {
    const TTypeOption *typeOption = MainGrid->TypeOption;
    dlgSave->FileName = "無題." + typeOption->DefExt();
  } else {
    dlgSave->InitialDir = ExtractFilePath(FileName);
    dlgSave->FileName = ExtractFileName(FileName);
  }

  dlgSave->FilterIndex = TypeList.IndexOf(MainGrid->TypeOption) + 1;
  if (dlgSave->Execute()) {
    String fileName = dlgSave->FileName;
    int typeIndex = dlgSave->FilterIndex - 1;
    TTypeOption *typeOption = TypeList.Items(typeIndex);

    String ext = ExtractFileExt(fileName);
    String defExt = "." + typeOption->DefExt();
    if (ext == "" || (typeOption->ForceExt && ext != defExt)) {
      fileName += defExt;
    }
    SaveAs(fileName, typeOption);
  }
}
//---------------------------------------------------------------------------
void TfmMain::SaveAs(String AFileName, const TTypeOption *Format)
{
  FileName = AFileName;
  MainGrid->TypeOption = Format;
  UpdateTitle();
  SaveFile(Format);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::tmAutoSaverTimer(TObject *Sender)
{
  try{
    if(BackupOnTime && FileName != ""){
      if(MainGrid->FileOpenThread){
        return;
      }
      String BuFN = FormattedFileName(BuFileNameT, FileName);
      MainGrid->SaveToFile(BuFN, nullptr, false);
    }else{
      tmAutoSaver->Enabled = false;
    }
  }catch(...){}
}
//---------------------------------------------------------------------------
void TfmMain::GetCheckedMenus(TStringList *list)
{
  list->Clear();
  TMenuItem *items = MainMenu->Items;
  for(int i=0; i<items->Count; i++){
    AddCheckedMenus(list, items->Items[i]);
  }
}
//---------------------------------------------------------------------------
void TfmMain::AddCheckedMenus(TStringList *list, TMenuItem* item)
{
   if (item == mnFile || item->Caption == "-") {
     return;
   }

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
  TMenuItem *items = MainMenu->Items;
  for(int i=0; i<items->Count; i++){
    RestoreCheckedMenus(list, items->Items[i]);
  }
}
//---------------------------------------------------------------------------
void TfmMain::RestoreCheckedMenus(TStringList *list, TMenuItem* item)
{
  if (item == mnFile || item->Caption == "-") {
    return;
  }

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
  if(MainGrid->FileOpenThread){
    Application->MessageBox(
      L"ファイルの読み込みが完了していないためエクスポートできません。",
      CASSAVA_TITLE, MB_ICONERROR);
    return;
  }

  String strFilter = "";
  String strOrgFilter = dlgSave->Filter;
  TStringList *types = new TStringList;
  TSearchRec sr;
  if(FindFirst(Pref->UserPath+"Export\\*.cms", faAnyFile, sr) == 0){
    do{
      String type = ChangeFileExt(sr.Name,"");
      String ext = ExtractFileExt(type);
      if(ext == ""){ ext = (String)"." + type; }
      strFilter += type.UpperCase() + " 形式 (*" + ext + ")|*" + ext + "|";
      types->Add(type);
    }while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if(FindFirst(Pref->SharedPath+"Export\\*.cms", faAnyFile, sr) == 0){
    do{
      String type = ChangeFileExt(sr.Name,"");
      String ext = ExtractFileExt(type);
      if(ext == ""){ ext = (String)"." + type; }
      strFilter += type.UpperCase() + " 形式 (*" + ext + ")|*" + ext + "|";
      types->Add(type);
    }while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if(strFilter == ""){
    Application->MessageBox(
        L"エクスポート可能な形式はありません。", L"Cassava Export", 0);
    delete types;
    return;
  }
  dlgSave->Filter = strFilter;
  dlgSave->InitialDir = ExtractFilePath(dlgSave->FileName);
  dlgSave->FileName = ChangeFileExt(ExtractFileName(dlgSave->FileName), "");
  if(dlgSave->Execute()){
    dlgSave->Filter = strOrgFilter;
    String type = types->Strings[dlgSave->FilterIndex - 1];
    if(ExtractFileExt(dlgSave->FileName) == ""){
      String ext = ExtractFileExt(type);
      if(ext == ""){ ext = (String)"." + type; }
      dlgSave->FileName = dlgSave->FileName + ext;
    }
    Export(dlgSave->FileName, type);
  }
  dlgSave->Filter = strOrgFilter;
  delete types;
}
//---------------------------------------------------------------------------
void TfmMain::Export(String filename, String type)
{
    String CmsFile = Pref->SharedPath + "Export\\" + type + ".cms";
    if(FileExists(Pref->UserPath + "Export\\" + type + ".cms")){
      CmsFile = Pref->UserPath + "Export\\" + type + ".cms";
    }
    if(!FileExists(CmsFile)){
      Application->MessageBox(
          (type + " 形式ではエクスポートできません。").c_str(),
          L"Cassava Export", 0);
      return;
    }

    TStream *out = nullptr;
    EncodedWriter *ew = nullptr;
    TStringList *checkedMenus = nullptr;
    try{
      out = new TFileStream(filename, fmCreate | fmShareDenyWrite);
      TEncoding *encoding = MainGrid->Encoding;
      TReturnCode returnCode = MainGrid->ReturnCode;
      TReturnCode inCellReturnCode = MainGrid->InCellReturnCode;
      bool addBom = MainGrid->AddBom;
      ew = new EncodedWriter(out, encoding, addBom);
      checkedMenus = new TStringList();
      GetCheckedMenus(checkedMenus);

      MacroExec(CmsFile, ew);

      RestoreCheckedMenus(checkedMenus);
      MainGrid->Encoding = encoding;
      MainGrid->ReturnCode = returnCode;
      MainGrid->InCellReturnCode = inCellReturnCode;
      MainGrid->AddBom = addBom;
    }catch(Exception *e){
      Application->MessageBox(e->Message.c_str(),
                              L"Cassava Macro Interpreter", 0);
    }
    if(checkedMenus) { delete checkedMenus; }
    if(ew) { delete ew; }
    if(out) { delete out; }
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
      MnHist[i]->Caption = (String)"&" + i + ": " + History->Strings[i];
      MnHist[i]->Enabled = true;
      MnHist[i]->Visible = true;
    } else {
      MnHist[i]->Visible = false;
      MnHist[i]->Caption = (String)"&" + i + ": (なし)";
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
        ((String)"\"" + FN + "\"").c_str(), nullptr);
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
    NewItem->Caption = (String)"&" + i + ": " + History->Strings[i];
    NewItem->Tag = i;
    NewItem->OnClick = mnOpenHistorysClick;
    mi->Add(NewItem);
  }
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
  String CellFileName = MainGrid->Cells[MainGrid->Col][MainGrid->Row];
  if(isUrl(CellFileName)){
    MainGrid->OpenURL(CellFileName);
  }else{
    AutoOpen(CellFileName, ExtractFilePath(FileName));
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
  mnpPaste->Visible = true;
  mnpPasteInsert->Visible = false;
  const TGridRect &Sel = MainGrid->Selection;

  bool isRowSelected = MainGrid->IsRowSelected();
  bool isRowHeaderSelected = MainGrid->IsRowHeaderSelected();
  bool isColSelected = MainGrid->IsColSelected();
  bool isColHeaderSelected = MainGrid->IsColHeaderSelected();

  if (isRowHeaderSelected) {
    mnpPaste->Visible = false;
    mnpPasteInsert->Enabled = Clipboard()->HasFormat(CF_TEXT);
    mnpPasteInsert->Visible = true;
    mnpKugiri->Visible = true;
    mnpInsRow->Visible = true;
    mnpCutRow->Visible = true;
    mnpDefWidth->Visible = true;

    if (!isColHeaderSelected) {
      for (int y = Sel.Top; y <= Sel.Bottom; y++) {
        if (MainGrid->RowHeights[y] > 8) {
          mnpNarrow->Visible = true;
          break;
        }
      }
    }
  } else if (isRowSelected && !isColSelected) {
    mnpKugiri->Visible = true;
    mnpInsRow->Visible = true;
    mnpCutRow->Visible = true;
  }

  if (isColHeaderSelected) {
    mnpPaste->Visible = false;
    mnpPasteInsert->Enabled = Clipboard()->HasFormat(CF_TEXT);
    mnpPasteInsert->Visible = true;
    mnpKugiri->Visible = true;
    mnpInsCol->Visible = true;
    mnpCutCol->Visible = true;
    mnpDefWidth->Visible = true;

    if (!isRowHeaderSelected) {
      for (int x = Sel.Left; x <= Sel.Right; x++) {
        if (MainGrid->ColWidths[x] > 16) {
          mnpNarrow->Visible = true;
          break;
        }
      }
    }
    if (Sel.Left == Sel.Right) {
      mnpSort->Visible = true;
      if (MainGrid->ColWidths[Sel.Left] > 16) {
        mnpNarrow->Default = true;
      } else {
        mnpDefWidth->Default = true;
      }
    }
  } else if (isColSelected && !isRowSelected) {
    mnpKugiri->Visible = true;
    mnpInsCol->Visible = true;
    mnpCutCol->Visible = true;
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
  if (edFindText->Focused()) {
    edFindText->CutToClipboard();
  } else if (fmFind->edFindText->Focused()) {
    fmFind->edFindText->CutToClipboard();
  } else if (fmFind->edReplaceText->Focused()) {
    fmFind->edReplaceText->CutToClipboard();
  } else if (fmFind->edMin->Focused()) {
    fmFind->edMin->CutToClipboard();
  } else if(fmFind->edMax->Focused()) {
    fmFind->edMax->CutToClipboard();
  } else {
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
  if (edFindText->Focused()) {
    edFindText->CopyToClipboard();
  } else if (fmFind->edFindText->Focused()) {
    fmFind->edFindText->CopyToClipboard();
  } else if (fmFind->edReplaceText->Focused()) {
    fmFind->edReplaceText->CopyToClipboard();
  } else if (fmFind->edMin->Focused()) {
    fmFind->edMin->CopyToClipboard();
  } else if (fmFind->edMax->Focused()) {
    fmFind->edMax->CopyToClipboard();
  } else {
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
  if (edFindText->Focused()) {
    edFindText->PasteFromClipboard();
  } else if(fmFind->edFindText->Focused()) {
    fmFind->edFindText->PasteFromClipboard();
  } else if(fmFind->edReplaceText->Focused()) {
    fmFind->edReplaceText->PasteFromClipboard();
  } else if(fmFind->edMin->Focused()) {
    fmFind->edMin->PasteFromClipboard();
  } else if(fmFind->edMax->Focused()) {
    fmFind->edMax->PasteFromClipboard();
  } else {
    MainGrid->PasteFromClipboard(PASTE_OPTION_UNKNOWN);
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
void __fastcall TfmMain::mnpPasteInsertClick(TObject *Sender)
{
  if (MainGrid->IsColHeaderSelected()) {
    MainGrid->PasteFromClipboard(PASTE_OPTION_INSERT_COL);
  } else {
    MainGrid->PasteFromClipboard(PASTE_OPTION_INSERT_ROW);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCutFormatDefaultClick(TObject *Sender)
{
  const TTypeOption *Format =
      TypeList.Items(static_cast<TMenuItem *>(Sender)->Tag);
  MainGrid->CutToClipboard(Format);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnCopyFormatDefaultClick(TObject *Sender)
{
  const TTypeOption *Format =
      TypeList.Items(static_cast<TMenuItem *>(Sender)->Tag);
  MainGrid->CopyToClipboard(Format);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnPasteFormatDefaultClick(TObject *Sender)
{
  const TTypeOption *Format =
      TypeList.Items(static_cast<TMenuItem *>(Sender)->Tag);
  MainGrid->PasteFromClipboard(PASTE_OPTION_UNKNOWN, Format);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSelectAllClick(TObject *Sender)
{
  if (edFindText->Focused()) {
    edFindText->SelectAll();
  } else if (fmFind->edFindText->Focused()) {
    fmFind->edFindText->SelectAll();
  } else if (fmFind->edReplaceText->Focused()) {
    fmFind->edReplaceText->SelectAll();
  } else if (fmFind->edMin->Focused()) {
    fmFind->edMin->SelectAll();
  } else if(fmFind->edMax->Focused()) {
    fmFind->edMax->SelectAll();
  } else {
    MainGrid->SelectAll();
  }
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
  MainGrid->Invalidate();
  MainGrid->TransChar(static_cast<TMenuItem*>(Sender)->Tag);
  MainGrid->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnTransKanaClick(TObject *Sender)
{
  MainGrid->Invalidate();
  MainGrid->TransKana(static_cast<TMenuItem*>(Sender)->Tag);
  MainGrid->Repaint();
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
  MainGrid->Invalidate();
  MainGrid->Sequence(false);
  MainGrid->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnSequenceSClick(TObject *Sender)
{
  MainGrid->Invalidate();
  MainGrid->Sequence(true);
  MainGrid->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnInsRowClick(TObject *Sender)
{
  int T = MainGrid->Selection.Top;
  MainGrid->InsertRow(T, MainGrid->Selection.Bottom);

  if (mnFixFirstCol->Checked) {
    acFixFirstColExecute(this);
  }
  MainGrid->Row = T;
  MainGrid->Col = MainGrid->FixedCols;
  MainGrid->Options << goEditing << goAlwaysShowEditor;
  MainGrid->EditorMode = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnInsColClick(TObject *Sender)
{
  int left = MainGrid->Selection.Left;
  MainGrid->InsertColumn(left, MainGrid->Selection.Right);

  if (mnFixFirstRow->Checked) {
    acFixFirstRowExecute(this);
  }
  MainGrid->Row = MainGrid->FixedRows;
  MainGrid->Col = left;
  MainGrid->Options << goEditing << goAlwaysShowEditor;
  MainGrid->EditorMode = true;
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
  const TGridRect& sel = MainGrid->Selection;
  if (MainGrid->IsRowHeaderSelected()) {
    for (int y = sel.Top; y <= sel.Bottom; y++) {
      MainGrid->RowHeights[y] = MainGrid->DefaultRowHeight;
    }
  }
  if (MainGrid->IsColHeaderSelected()) {
    for (int x = sel.Left; x <= sel.Right; x++) {
      MainGrid->SetWidth(x);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnpNarrowClick(TObject *Sender)
{
  const TGridRect& sel = MainGrid->Selection;
  if (MainGrid->IsRowHeaderSelected()) {
    for (int y = sel.Top; y <= sel.Bottom; y++) {
      MainGrid->RowHeights[y] = 8;
    }
  }
  if (MainGrid->IsColHeaderSelected()) {
    for (int x = sel.Left; x <= sel.Right; x++) {
      MainGrid->ColWidths[x] = 16;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnRefreshClick(TObject *Sender)
{
  MainGrid->Cut();
  MainGrid->SetWidth();
  MainGrid->SetHeight();
  MainGrid->Invalidate();
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void TfmMain::SetGridFont(TFont *AFont)
{
  MainGrid->Font = AFont;
  MainGrid->Canvas->Font = AFont;
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
void __fastcall TfmMain::acCalcExpressionExecute(TObject *Sender)
{
  MainGrid->ExecCellMacro = !MainGrid->ExecCellMacro;
  MainGrid->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acCalcExpressionUpdate(TObject *Sender)
{
  acCalcExpression->Checked = MainGrid->ExecCellMacro;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnHideMenuBarClick(TObject *Sender)
{
  Menu = nullptr;
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
void __fastcall TfmMain::StatusBarContextPopup(TObject *Sender,
    TPoint &MousePos, bool &Handled)
{
  int x = MousePos.X;
  int panelIndex = 0;
  while (panelIndex < StatusBar->Panels->Count) {
    int panelWidth = StatusBar->Panels->Items[panelIndex]->Width;
    if (x <= panelWidth) {
      break;
    }
    x -= panelWidth;
    panelIndex++;
  }
  if (panelIndex >= StatusBar->Panels->Count ||
      StatusBarPopUp.count(panelIndex) == 0) {
    PopMenuStatusBar->AutoPopup = false;
    return;
  }

  PopMenuStatusBar->Items->Clear();
  TStringList *items = new TStringList;
  items->Text = StatusBarPopUp[panelIndex].Label;
  for (int i = 0; i < items->Count; i++) {
    TMenuItem *newItem = new TMenuItem(PopMenuStatusBar);
    newItem->Caption = items->Strings[i];
    newItem->Tag = (panelIndex << 16) + i;
    newItem->OnClick = StatusBarPopUpClick;
    PopMenuStatusBar->Items->Add(newItem);
  }
  delete items;
  PopMenuStatusBar->AutoPopup = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::StatusBarPopUpClick(TObject *Sender)
{
  int tag = static_cast<TMenuItem*>(Sender)->Tag;
  int panelIndex = tag >> 16;
  if (StatusBarPopUp.count(panelIndex) == 0) {
    return;
  }
  TStringList *arguments = new TStringList;
  arguments->Text = StatusBarPopUp[panelIndex].Label;
  String label = arguments->Strings[tag & 0xffff];
  arguments->Clear();
  arguments->Add(label);
  ExecMacro(StatusBarPopUp[panelIndex].Handler, StopMacroCount,
            SystemMacroCache, -1, -1, nullptr, false, arguments);
  delete arguments;
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acFixFirstRowExecute(TObject *Sender)
{
  if(MainGrid->FileOpenThread){
    mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
    tsbFixFirstRow->Down   = !MainGrid->ShowColCounter;
    Application->MessageBox(
        L"ファイルの読み込み中は固定セルを変更できません。",
        CASSAVA_TITLE, MB_ICONERROR);
    return;
  }

  MainGrid->UndoList->Lock();
  MainGrid->ShowColCounter = !MainGrid->ShowColCounter;
  MainGrid->UndoList->Unlock();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acFixFirstRowUpdate(TObject *Sender)
{
  acFixFirstRow->Checked = !MainGrid->ShowColCounter;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acFixFirstColExecute(TObject *Sender)
{
  if(MainGrid->FileOpenThread){
    Application->MessageBox(
        L"ファイルの読み込み中は固定セルを変更できません。",
        CASSAVA_TITLE, MB_ICONERROR);
    return;
  }

  MainGrid->UndoList->Lock();
  MainGrid->ShowRowCounter = !MainGrid->ShowRowCounter;
  MainGrid->UndoList->Unlock();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acFixFirstColUpdate(TObject *Sender)
{
  acFixFirstCol->Checked = !MainGrid->ShowRowCounter;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnFixUpLeftClick(TObject *Sender)
{
  if(MainGrid->FileOpenThread){
    Application->MessageBox(
        L"ファイルの読み込み中は固定セルを変更できません。",
        CASSAVA_TITLE, MB_ICONERROR);
    return;
  }

  MainGrid->UndoList->Lock();

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

  MainGrid->Col = MainGrid->FixedCols;
  MainGrid->Row = MainGrid->FixedRows;

  MainGrid->UndoList->Unlock();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnUnFixClick(TObject *Sender)
{
  if(MainGrid->FileOpenThread){
    Application->MessageBox(
        L"ファイルの読み込み中は固定セルを変更できません。",
        CASSAVA_TITLE, MB_ICONERROR);
    return;
  }

  MainGrid->UndoList->Lock();
  if (!MainGrid->ShowColCounter) {
    MainGrid->ShowColCounter = true;
  }
  if (!MainGrid->ShowRowCounter) {
    MainGrid->ShowRowCounter = true;
  }
  MainGrid->UndoList->Unlock();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOptionDlgClick(TObject *Sender)
{
  fmOption = new TfmOption(Application);
  fmOption->ShowModal();
  UpdateTitle();
  delete fmOption;
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
      _wspawnl(P_NOWAITO, exe.c_str(), arg0.c_str(), nullptr);
    }else{
      String arg1 = (String)("\"") + FileName + "\"";
      _wspawnl(P_NOWAITO, exe.c_str(), arg0.c_str(), arg1.c_str(), nullptr);
    }
    if(menuItem->Tag){
      Close();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroClick(TObject *Sender)
{
  SearchMacro(static_cast<TMenuItem *>(Sender));
}
//---------------------------------------------------------------------------
void TfmMain::SearchMacro(TMenuItem *Parent)
{
  std::map<String, TMenuItem *> oldItems;
  std::map<String, TMenuItem *> addedDirs;
  int itemsToKeep = Parent == mnMacro ? 5 : 1;
  for (int i = Parent->Count - 1; i >= itemsToKeep; i--){
    TMenuItem *oldItem = Parent->Items[i];
    Parent->Delete(i);
    oldItems[oldItem->Name] = oldItem;
  }

  String parentPath = Parent->Hint;
  for (int i = 1; i >= 0; i--) {
    String path = (i ? Pref->UserPath + "Macro\\" + parentPath + "*"
                     : Pref->SharedPath + "Macro\\" + parentPath + "*");
    TSearchRec sr;
    if (FindFirst(path, faDirectory, sr) == 0) {
      do {
        if ((sr.Attr & faDirectory) && sr.Name != "lib" && sr.Name != "tests" &&
            sr.Name.Length() > 0 && sr.Name[1] != '.') {
          String dirName = parentPath + sr.Name;
          String id = MakeId("macro", dirName, 2);
          if (addedDirs[id]) {
            continue;
          }
          TMenuItem *newItem = oldItems[id];
          if (newItem) {
            oldItems.erase(id);
          } else {
            newItem = new TMenuItem(Parent->Owner);
            newItem->Hint = dirName + "\\";
            newItem->Caption = sr.Name;
            newItem->Name = id;
            newItem->OnClick = mnMacroClick;
            TMenuItem *dummyChild = new TMenuItem(Parent->Owner);
            dummyChild->Caption = "(empty)";
            dummyChild->Enabled = false;
            newItem->Add(dummyChild);
          }
          Parent->Add(newItem);
          newItem->Break = (newItem->MenuIndex % 25 == 0) ? mbBarBreak : mbNone;
          addedDirs[id] = newItem;
        }
      } while (FindNext(sr) == 0);
      FindClose(sr);
    }
  }

  for (int i = 1; i >= 0; i--) {
    String path = (i ? Pref->UserPath + "Macro\\" + parentPath + "*.cms"
                     : Pref->SharedPath + "Macro\\" + parentPath + "*.cms");
    TNotifyEvent onClick = (i ? &mnMacroUserExecClick : &mnMacroExecClick);

    TSearchRec sr;
    if (FindFirst(path, faAnyFile, sr) == 0) {
      do {
        if (sr.Name.Length() > 0 && sr.Name[1] != '!') {
          String macroName = parentPath + ChangeFileExt(sr.Name, "");
          String id = MakeId("macro", macroName, i);
          TMenuItem *newItem = oldItems[id];
          if(newItem){
            oldItems.erase(id);
          }else{
            newItem = new TMenuItem(Parent->Owner);
            newItem->Hint = macroName;
            newItem->Caption = ChangeFileExt(sr.Name, "");
            newItem->Name = id;
            newItem->OnClick = onClick;
          }
          Parent->Add(newItem);
          newItem->Break = (newItem->MenuIndex % 25 == 0) ? mbBarBreak : mbNone;
        }
      } while (FindNext(sr) == 0);
      FindClose(sr);
    }
  }

  Parent->Items[0]->Visible = Parent == mnMacro || Parent->Count == 1;

  for(std::map<String, TMenuItem *>::iterator it = oldItems.begin();
      it != oldItems.end(); ++it){
    delete it->second;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroOpenUserFolderClick(TObject *Sender)
{
  String path = Pref->UserPath + "Macro";
  if(!DirectoryExists(path)){
    ForceDirectories(path);
  }
  _wspawnlp(P_NOWAITO, L"Explorer.exe", L"/idlist", path.c_str(), nullptr);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroOpenFolderClick(TObject *Sender)
{
  _wspawnlp(P_NOWAITO, L"Explorer.exe", L"/idlist",
            (Pref->SharedPath + "Macro").c_str(), nullptr);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroExecuteClick(TObject *Sender)
{
  if (dlgOpenMacro->InitialDir == "") {
    String path = Pref->UserPath + "Macro";
    if (!DirectoryExists(path)) {
      path = Pref->SharedPath + "Macro";
    }
    dlgOpenMacro->InitialDir = path;
  }
  if (dlgOpenMacro->Execute()) {
    String CmsFile = dlgOpenMacro->FileName;
    dlgOpenMacro->InitialDir = ExtractFilePath(CmsFile);
    dlgOpenMacro->FileName = ExtractFileName(CmsFile);
    MacroExec(CmsFile, nullptr);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroUserExecClick(TObject *Sender)
{
  TMenuItem *Menu = static_cast<TMenuItem *>(Sender);
  String CmsFile = Pref->UserPath + "Macro\\" + Menu->Hint + ".cms";
  MacroExec(CmsFile, nullptr);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroExecClick(TObject *Sender)
{
  TMenuItem *Menu = static_cast<TMenuItem *>(Sender);
  String CmsFile = Pref->SharedPath + "Macro\\" + Menu->Hint + ".cms";
  MacroExec(CmsFile, nullptr);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acMacroTerminateExecute(TObject *Sender)
{
  StopAllMacros();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acMacroTerminateUpdate(TObject *Sender)
{
  acMacroTerminate->Enabled = (GetRunningMacroCount() > 0);
}
//---------------------------------------------------------------------------
void TfmMain::MacroExec(String CmsFile, EncodedWriter *io)
{
  MainGrid->Cursor = crAppStart;
  MainGrid->Hint = "マクロを実行中です。";
  MainGrid->ShowHint = true;
  Application->Hint = MainGrid->Hint;
  ApplicationHint(nullptr);

  TStringList *Modules = new TStringList;
  TStringList *InPaths = new TStringList;
  String inPath = ExtractFilePath(CmsFile);
  if (inPath != "" && *(inPath.LastChar()) != '\\') {
    inPath += "\\";
  }
  InPaths->Add(inPath);
  InPaths->Add(Pref->UserPath + "Macro\\");
  InPaths->Add(Pref->SharedPath + "Macro\\");
  bool C = MacroCompile(CmsFile, InPaths, Modules, true);
  if(C){
    MainGrid->UndoList->Push();
    MainGrid->Invalidate();
    acMacroTerminate->Enabled = true;
    ExecMacro(CmsFile, StopMacroCount, Modules, -1, -1, io);
    MainGrid->Invalidate();
    MainGrid->UndoList->Pop();
  }
  for(int i=Modules->Count-1; i>=0; i--){
    if(Modules->Objects[i]) delete Modules->Objects[i];
  }
  delete Modules;
  delete InPaths;

  MainGrid->Cursor = crDefault;
  MainGrid->Hint = "";
  MainGrid->ShowHint = false;
  Application->CancelHint();
}
//---------------------------------------------------------------------------
void TfmMain::UpdateStatusbar()
{
  if (mnShowStatusbar->Checked && StatusbarCmsFile != "") {
    StatusBarPopUp.clear();
    try {
      ExecMacro(StatusbarCmsFile, StopMacroCount, SystemMacroCache, -1, -1,
                nullptr, true);
    } catch(...) {}
  }
}
//---------------------------------------------------------------------------
void TfmMain::MacroScriptExec(String cmsname, String script)
{
  TStringList *modules = new TStringList;
  TStringList *inPaths = new TStringList;
  inPaths->Add(Pref->UserPath + "Macro\\");
  inPaths->Add(Pref->SharedPath + "Macro\\");
  bool ok = MacroCompile(&script, cmsname, inPaths, modules, true);
  if (ok) {
    acMacroTerminate->Enabled = true;
    ExecMacro(cmsname, StopMacroCount, modules, -1, -1);
  }
  for(int i=modules->Count-1; i>=0; i--){
    if(modules->Objects[i]) delete modules->Objects[i];
  }
  delete modules;
  delete inPaths;
}
//---------------------------------------------------------------------------
TCalculatedCell TfmMain::GetCalculatedCell(String Str, int ACol, int ARow)
{
  if (Str.Length() == 0 || Str[1] != '=') {
    return TCalculatedCell(Str, ctNotExpr);
  }
  TCalculatedCell result = TCalculatedCell(Str, ctError);
  Str.Delete(1,1);
  TStringList *modules = new TStringList;
  TStringList *inPaths = new TStringList;
  inPaths->Add(Pref->UserPath + "Macro\\");
  inPaths->Add(Pref->SharedPath + "Macro\\");
  String cmsName = (String)"$@cell_" + ACol + "_" + ARow;
  try {
    String formula = "return " + Str + ";";
    bool ok = MacroCompile(&formula, cmsName, inPaths, modules, false);
    if (ok){
      TMacroValue macroResult = ExecMacro(
          cmsName, StopMacroCount, modules, ACol, ARow, nullptr, true);
      result = TCalculatedCell(macroResult.string, ctOk);
    }
  }catch(...){
    // エラー用のResultCell文字列は設定済み
  }
  for(int i=modules->Count-1; i>=0; i--){
    if(modules->Objects[i]) delete modules->Objects[i];
  }
  delete modules;
  delete inPaths;
  return result;
}
//---------------------------------------------------------------------------
static int HexToInt(wchar_t c)
{
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return (c - 'A') + 10;
  } else if (c >= 'a' && c <= 'f') {
    return (c - 'a') + 10;
  }
  return 0;
}
//---------------------------------------------------------------------------
static TColor ParseColor(String Value, TColor defaultColor)
{
  if (Value.Length() == 7 && Value[1] == '#') {
    return TColor(
        (HexToInt(Value[6]) << 20) + (HexToInt(Value[7]) << 16) +
        (HexToInt(Value[4]) << 12) + (HexToInt(Value[5]) << 8) +
        (HexToInt(Value[2]) << 4) + HexToInt(Value[3]));
  } else if (Value.Length() == 4 && Value[1] == '#') {
    return TColor(
        (HexToInt(Value[4]) << 20) + (HexToInt(Value[4]) << 16) +
        (HexToInt(Value[3]) << 12) + (HexToInt(Value[3]) << 8) +
        (HexToInt(Value[2]) << 4) + HexToInt(Value[2]));
  }
  return defaultColor;
}
//---------------------------------------------------------------------------
TFormattedCell TfmMain::GetFormattedCell(TCalculatedCell Cell, int AX, int AY)
{
  if (FormatCmsFile != "") {
    try {
      TMacroValue result =
          ExecMacro(FormatCmsFile, StopMacroCount, SystemMacroCache, AX, AY,
                    nullptr, true);
      if (result.object.size() > 0) {
        if (result.object["text"] != "") {
          Cell.text = result.object["text"];
        }
        TFormattedCell formattedCell = MainGrid->GetStyledCell(Cell, AX, AY);
        formattedCell.fgColor =
            ParseColor(result.object["color"], formattedCell.fgColor);
        formattedCell.bgColor =
            ParseColor(result.object["background"], formattedCell.bgColor);
        if (result.object["align"] == "left") {
          formattedCell.alignment = taLeftJustify;
        } else if (result.object["align"] == "right") {
          formattedCell.alignment = taRightJustify;
        } else if (result.object["align"] == "center") {
          formattedCell.alignment = taCenter;
        }
        return formattedCell;
      }
      if (result.string != "") {
        Cell.text = result.string;
      }
    } catch (...) {}
  }
  return MainGrid->GetStyledCell(Cell, AX, AY);
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
  MainGrid->Invalidate();
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
void __fastcall TfmMain::mnCharCodeClick(TObject *Sender)
{
  switch (MainGrid->Encoding->CodePage) {
    case CODE_PAGE_JIS:
      mnJis->Checked = true;
      mnReloadCodeJIS->Checked = true;
      break;
    case CODE_PAGE_EUC:
      mnEuc->Checked = true;
      mnReloadCodeEUC->Checked = true;
      break;
    case CODE_PAGE_UTF8:
      mnUtf8->Checked = true;
      mnReloadCodeUTF8->Checked = true;
      break;
    case CODE_PAGE_UTF16BE:
      mnUtf16be->Checked = true;
      mnReloadCodeUTF16BE->Checked = true;
      break;
    case CODE_PAGE_UTF16LE:
      mnUnicode->Checked = true;
      mnReloadCodeUnicode->Checked = true;
      break;
    default:
      mnSjis->Checked = true;
      mnReloadCodeShiftJIS->Checked = true;
      break;
  }

  if (MainGrid->ReturnCode == MainGrid->InCellReturnCode) {
    switch (MainGrid->ReturnCode) {
      case LF: mnLf->Checked = true;   break;
      case CR: mnCr->Checked = true;   break;
      default: mnLfcr->Checked = true; break;
    }
  } else if (MainGrid->ReturnCode == CRLF && MainGrid->InCellReturnCode == LF) {
    mnLfInCell->Checked = true;
  } else {
    mnLfcr->Checked = false;
    mnLf->Checked = false;
    mnCr->Checked = false;
    mnLfInCell->Checked = false;
  }

  mnBom->Checked = MainGrid->AddBom;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnKCodeClick(TObject *Sender)
{
  TMenuItem *Menu = static_cast<TMenuItem *>(Sender);

  if (ToCharCode(MainGrid->Encoding) != Menu->Tag) {
    MainGrid->Encoding = ToEncoding(Menu->Tag);
    MainGrid->Modified = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnReturnCodeClick(TObject *Sender)
{
  TMenuItem *menu = static_cast<TMenuItem *>(Sender);

  TReturnCode returnCode;
  TReturnCode inCellReturnCode;
  if (menu == mnLf) {
    returnCode = LF;
    inCellReturnCode = LF;
  } else if (menu == mnLfInCell) {
    returnCode = CRLF;
    inCellReturnCode = LF;
  } else if (menu == mnCr) {
    returnCode = CR;
    inCellReturnCode = CR;
  } else {
    returnCode = CRLF;
    inCellReturnCode = CRLF;
  }

  if (MainGrid->ReturnCode != returnCode
      || MainGrid->InCellReturnCode != inCellReturnCode) {
    MainGrid->ReturnCode = returnCode;
    MainGrid->InCellReturnCode = inCellReturnCode;
    MainGrid->Modified = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnBomClick(TObject *Sender)
{
  MainGrid->AddBom = !MainGrid->AddBom;
  int cp = MainGrid->Encoding->CodePage;
  if (cp == CODE_PAGE_UTF8 || cp == CODE_PAGE_UTF16LE ||
      cp == CODE_PAGE_UTF16BE) {
    MainGrid->Modified = true;
  }
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
  AutoOpen("Help\\index.html", FullPath);
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
    + "\n   by あすかぜ\n                " + Version::CurrentDate();
  Application->MessageBox(message.c_str(), L"バージョン情報", 0);
}
//---------------------------------------------------------------------------

