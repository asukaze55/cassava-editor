//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include <Vcl.clipbrd.hpp>
#include <process.h>
#include <stdio.h>
#include <map>

#include "Letter.h"
#include "AutoOpen.h"
#include "KeyCustomize.h"
#include "Option.h"
#include "OptionColor.h"
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
static String MaybeCompileMacro(String fileName, const Preference &pref,
                                TMacroContext *context)
{
  String cmsFile = pref.UserPath + "Macro\\" + fileName;
  if (!FileExists(cmsFile)) {
    cmsFile = pref.SharedPath + "Macro\\" + fileName;
  }
  if (!FileExists(cmsFile)) {
    return "";
  }
  bool compiled = CompileMacro(cmsFile, context, true);
  return compiled ? cmsFile : (String)"";
}
//---------------------------------------------------------------------------
__fastcall TfmMain::TfmMain(TComponent* Owner)
  : TForm(Owner)
{
  FullPath = ExtractFilePath(ParamStr(0));
  if(*(FullPath.LastChar()) != '\\') FullPath += "\\";
  Pref = std::make_unique<Preference>(FullPath);

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

  ScreenDpi = Screen->PixelsPerInch;

  TTypeList defaultTypeList;
  defaultTypeList.Add(TTypeOption());
  defaultTypeList.Add(TTypeOption("CSV"));
  defaultTypeList.Add(TTypeOption("TSV"));
  MainGrid->TypeOption = defaultTypeList.DefItem();
  TypeList = defaultTypeList;

  if (!fmFind) {
    fmFind = new TfmFind(this);
  }

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
  std::vector<String> newWindowArgs;
  newWindowArgs.push_back(ParamStr(0));

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
        newWindowArgs.push_back(ParamStr(i));
      }
    }
  }

  if (newWindowCount > 0) {
    newWindowArgs.push_back("-i");
    newWindowArgs.push_back((String)(positionShift + 1));
    SpawnProcess(newWindowArgs);
  }

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
  SystemMacroContext.AddDirectory(Pref->UserPath + "Macro\\");
  SystemMacroContext.AddDirectory(Pref->SharedPath + "Macro\\");
  FormatCmsFile = MaybeCompileMacro("!format.cms", *Pref, &SystemMacroContext);
  if (FormatCmsFile != "") {
    MainGrid->OnGetFormattedCell = GetFormattedCell;
  }

  String statusbarCmsFile =
      MaybeCompileMacro("!statusbar.cms", *Pref, &SystemMacroContext);
  if (statusbarCmsFile != "") {
    String statusbarInit =
        GetMacroModuleName(statusbarCmsFile, "init", "0", false);
    if (SystemMacroContext.HasModule(statusbarInit)) {
      try {
        RunMacro(statusbarInit, StopMacroCount, SystemMacroContext, -1, -1,
            /* ReadOnly= */ true);
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
          (ParamCmsFile + L"\nファイルが見つかりません。").c_str(),
          CASSAVA_TITLE, 0);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::ExecOpenMacro(System::TObject* Sender)
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
void TfmMain::ReadIni()
{
  History.clear();

  IniFile ini = Pref->GetInifile();

  FStyle = IsDarkMode(ini.ReadString("Mode", "Style", ""))
      ? DARK_MODE_STYLE_NAME : LIGHT_MODE_STYLE_NAME;
  TStyleManager::TrySetStyle(FStyle);

  int iniScreenDpi = ini.ReadInteger("Position", "Dpi", ScreenDpi);
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
      ini.ReadInteger("Position", "Width", Width) * dpiRatio + 0.5;
  Width = iniWidth <= screenWidth ? iniWidth : screenWidth;
  int iniHeight =
      ini.ReadInteger("Position", "Height", Height) * dpiRatio + 0.5;
  Height = iniHeight <= screenHeight ? iniHeight : screenHeight;
  int iniLeft = ini.ReadInteger("Position", "Left", -1);
  int left =
      (iniLeft >= virtualScreenLeft && iniLeft <= virtualScreenRight - Width)
          ? iniLeft : (screenWidth / 2 - Width / 2);
  int iniTop = ini.ReadInteger("Position", "Top", -1);
  int top =
      (iniTop >= virtualScreenTop && iniTop <= virtualScreenBottom - Height)
          ? iniTop : (screenHeight / 2 - Height / 2);

  for (int i = 1; i <= ParamCount(); i++) {
    if (ParamStr(i) == "-i") {
      int positionShift = ParamStr(i + 1).ToIntDef(0)
          * (GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME));
      left += positionShift;
      top += positionShift;
      break;
    }
  }
  Left =  left;
  Top = top;

  if (ini.ReadBool("Mode", "ShowToolbar", true) != mnShowToolbar->Checked) {
    mnShowToolbarClick(this);
  }
  FToolBarSize = ini.ReadInteger("Mode", "ToolbarSize", 16 * ScreenDpi / 96);
  if (ini.ReadBool("Mode", "ShowStatusbar", true)
      != mnShowStatusbar->Checked) {
    mnShowStatusbarClick(this);
  }

  MainGrid->Color =
      (TColor) ini.ReadInteger("Font", "BgColor", MainGrid->Color);
  MainGrid->CalcBgColor =
      (TColor) ini.ReadInteger("Font", "CalcBgColor", clAqua);
  MainGrid->CalcErrorBgColor =
      (TColor) ini.ReadInteger("Font", "CalcErrorBgColor", clRed);
  MainGrid->CalcErrorFgColor = (TColor) ini.ReadInteger(
      "Font", "CalcErrorFgColor", MainGrid->Font->Color);
  MainGrid->CalcFgColor =
      (TColor) ini.ReadInteger("Font", "CalcFgColor", MainGrid->Font->Color);
  MainGrid->CurrentRowBgColor =
      (TColor) ini.ReadInteger("Font", "CurrentRowBgColor", MainGrid->Color);
  MainGrid->CurrentColBgColor =
      (TColor) ini.ReadInteger("Font", "CurrentColBgColor", MainGrid->Color);
  MainGrid->DummyBgColor =
      (TColor) ini.ReadInteger("Font", "DummyBgColor", clCream);
  MainGrid->EvenRowBgColor =
      (TColor) ini.ReadInteger("Font", "EvenRowBgColor", MainGrid->Color);
  MainGrid->FixFgColor =
      (TColor) ini.ReadInteger("Font", "FixFgColor", MainGrid->Font->Color);
  MainGrid->FixedColor =
      (TColor) ini.ReadInteger("Font", "FixedColor", MainGrid->FixedColor);
  MainGrid->FoundBgColor =
      (TColor) ini.ReadInteger("Font", "FoundBgColor", clYellow);
  MainGrid->UrlColor = (TColor) ini.ReadInteger("Font", "UrlColor", clBlue);
  MainGrid->WordWrap = ini.ReadBool("Font", "WordWrap", false);
  MainGrid->TextAlignment =
      ini.ReadInteger("Font", "TextAlign", MainGrid->TextAlignment);
  MainGrid->NumberComma = ini.ReadInteger("Font", "NumberComma", 0);
  MainGrid->DecimalDigits = ini.ReadInteger("Font", "DecimalDigits", -1);
  MainGrid->MinColWidth =
      ini.ReadInteger("Font", "MinColWidth", MainGrid->MinColWidth);

  Show();
  SearchMacro(mnMacro);
  dpiRatio = (double)ScreenDpi / iniScreenDpi;

  // Show() may reset the Left and Top in dark mode.
  Left = left;
  Top = top;

  WindowState =
      ini.ReadInteger("Position", "Mode", 0) == 2 ? wsMaximized : wsNormal;
  MainGrid->Font->Name = ini.ReadString("Font", "Name",
      Screen->Fonts->IndexOf(L"Yu Gothic UI") >= 0 ? L"Yu Gothic UI"
                                                   : L"ＭＳ Ｐゴシック");
  MainGrid->Font->Size = ini.ReadInteger("Font", "Size", 12) * dpiRatio + 0.5;
  if (ini.ReadBool("Font", "Bold", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsBold;
  }
  if (ini.ReadBool("Font", "Italic", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsItalic;
  }
  if (ini.ReadBool("Font", "Underline", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsUnderline;
  }
  if (ini.ReadBool("Font", "StrikeOut", false)) {
    MainGrid->Font->Style = MainGrid->Font->Style << fsStrikeOut;
  }
  MainGrid->TBMargin = ini.ReadInteger("Font", "TBMargin", 2);
  MainGrid->LRMargin = ini.ReadInteger("Font", "LRMargin", 4);
  MainGrid->CellLineMargin = ini.ReadInteger("Font", "CellLineMargin", 0);
  MainGrid->Font->Color =
      (TColor) ini.ReadInteger("Font", "FgColor", MainGrid->Font->Color);
  MainGrid->Canvas->Font = MainGrid->Font;

  PrintFontName = ini.ReadString("Print", "FontName", MainGrid->Font->Name);
  PrintFontSize = ini.ReadInteger("Print", "FontSize", MainGrid->Font->Size);
  PrintMargin[0] = ini.ReadInteger("Print", "MarginLeft", 15);
  PrintMargin[1] = ini.ReadInteger("Print", "MarginRight", 15);
  PrintMargin[2] = ini.ReadInteger("Print", "MarginTop", 15);
  PrintMargin[3] = ini.ReadInteger("Print", "MarginBottom", 15);
  PrintHeader = ini.ReadString("Print", "Header", "%f");
  PrintHeaderPosition = ini.ReadInteger("Print", "HeaderPosition", 2);
  PrintFooter = ini.ReadString("Print", "Footer", "- %p -");
  PrintFooterPosition = ini.ReadInteger("Print", "FooterPosition", 2);

  int TypeCount = ini.ReadInteger("DataType", "Count", 0);
  if (TypeCount > 0) {
    TTypeList newTypeList;
    for (int i = 0; i < TypeCount; i++) {
      String Section = (String)"DataType:" + i;
      TTypeOption option;
      option.Name = ini.ReadString(Section, "Name", L"[新規]");
      String exts = ini.ReadString(Section, "Exts", "csv");
      option.SetExts(exts);
      option.ForceExt = ini.ReadBool(Section, "ForceExt", false);
      option.SepChars =
          Ascii2Ctrl(ini.ReadString(Section, "SepChars", ",\\t"));
      option.WeakSepChars =
          Ascii2Ctrl(ini.ReadString(Section, "WeakSepChars", "\\_"));
      option.QuoteChars =
          Ascii2Ctrl(ini.ReadString(Section, "QuoteChars", "\""));
      option.QuoteOption =
          (TQuoteOption) ini.ReadInteger(Section, "Quote", QUOTE_NORMAL);
      option.QuoteExpression =
          ini.ReadString(Section, "QuoteExpression", "");
      option.OmitComma = ini.ReadBool(Section, "OmitComma", true);
      option.DummyEof = ini.ReadBool(Section, "DummyEof", false);
      option.DummyEol = ini.ReadBool(Section, "DummyEol", false);
      newTypeList.Add(option);
    }
    TypeList = newTypeList;
  }

  BackupOnSave = ini.ReadBool("Backup", "OnSave", true);
  BackupOnTime = ini.ReadBool("Backup", "OnTime", false);
  BackupOnOpen = ini.ReadBool("Backup", "OnOpen", true);
  BuFileNameS = ini.ReadString("Backup", "FileNameS", "%f.%x~");
  BuFileNameT = ini.ReadString("Backup", "FileNameT", "#%f.%x#");
  DelBuSSaved = ini.ReadBool(
      "Backup", "DeleteSSaved", !(ini.ValueExists("Backup", "OnSave")));
  DelBuSExit = ini.ReadBool("Backup", "DeleteS", false);
  DelBuT = ini.ReadBool("Backup", "DeleteT", true);
  BuInterval = ini.ReadInteger("Backup", "Interval", 5);

  int pasteOption = ini.ReadInteger("Mode", "Paste", -1);
  MainGrid->PasteOption = pasteOption;
  mnPasteNormal->Checked = (pasteOption < 0);
  mnPasteOption0->Checked = (pasteOption == 0);
  mnPasteOption1->Checked = (pasteOption == 1);
  mnPasteOption2->Checked = (pasteOption == 2);
  mnPasteOption3->Checked = (pasteOption == 3);
  mnPasteOption4->Checked = (pasteOption == 4);
  mnPasteOption5->Checked = (pasteOption == 5);
  MainGrid->DragBehavior =
      (TDragBehavior) ini.ReadInteger("Mode", "DragCounter", dbMoveIfSelected);
  MainGrid->EnterMove = ini.ReadInteger("Mode", "EnterMove", 0);

  int FixedRows = ini.ReadInteger("Mode", "FixTopRow", 0);
  int FixedCols = ini.ReadInteger("Mode", "FixLeftCol", 0);
  if (FixedRows + 1 >= MainGrid->RowCount) {
    MainGrid->ChangeRowCount(FixedRows + 2);
  }
  MainGrid->Row = FixedRows + 1;
  if (FixedCols + 1 >= MainGrid->ColCount) {
    MainGrid->ChangeColCount(FixedCols + 2);
  }
  MainGrid->Col = FixedCols + 1;
  mnFixUpLeftClick(this);

  MainGrid->ShowToolTipForLongCell =
      ini.ReadBool("Mode", "ShowToolTipForLongCell", false);
  Application->HintPause =
      ini.ReadInteger("Mode", "HintPause", Application->HintPause);
  Application->HintHidePause =
      ini.ReadInteger("Mode", "HintHidePause", Application->HintHidePause);

  MainGrid->LeftArrowInCell = ini.ReadBool("Mode", "LeftArrowInCell", false);
  MainGrid->WheelMoveCursol = ini.ReadInteger("Mode", "WheelMoveCursol", 0);
  MainGrid->WheelScrollStep = ini.ReadInteger("Mode", "WheelScrollStep", 1);
  MainGrid->AlwaysShowEditor = ini.ReadBool("Mode", "AlwaysShowEditor", true);
  SortAll = ini.ReadBool("Mode", "SortAll", false);
  int useURL = ini.ReadInteger("Mode", "UseURL", 1);
  MainGrid->ShowURLBlue = useURL;
  MainGrid->DblClickOpenURL = useURL;
  MakeNewWindow = ini.ReadBool("Mode", "NewWindow", false);
  TitleFullPath = ini.ReadBool("Mode", "TitleFullPath", false);
  LockFile = ini.ReadInteger("Mode", "LockFile", 0);
  LockingFile = nullptr;
  CheckTimeStamp = ini.ReadBool("Mode", "CheckTimeStamp", true);
  SortByNumber = ini.ReadBool("Mode", "SortByNumber", true);
  SortIgnoreCase = ini.ReadBool("Mode", "SortIgnoreCase", false);
  SortIgnoreZenhan = ini.ReadBool("Mode", "SortIgnoreZenhan", false);
  SortDirection = ini.ReadInteger("Mode", "SortDirection", 0);
  EncodingDetector.Enabled = ini.ReadBool("Mode", "CheckKanji", true);
  EncodingDetector.DefaultEncoding =
      ToEncoding(ini.ReadInteger("Mode", "DefaultCharCode", CHARCODE_UTF8));
  if (FileName == "") {
    MainGrid->Encoding = EncodingDetector.DefaultEncoding;
  }
  MainGrid->CompactColWidth = ini.ReadBool("Mode","CompactColWidth", true);
  MainGrid->CalcWidthForAllRow = ini.ReadBool("Mode","CalcWidthForAllRow", 0);
  MainGrid->MaxRowHeightLines = ini.ReadFloat("Mode", "MaxRowHeightLines", 1.5);
  StopMacroCount = ini.ReadInteger("Mode", "StopMacro", 0);
  MainGrid->UndoList->MaxCount = ini.ReadInteger("Mode", "UndoCount", 100);

  fmFind->cbCase->Checked = ini.ReadBool("Search", "Case", false);
  fmFind->cbWordSearch->Checked = ini.ReadBool("Search", "Word", false);
  fmFind->cbRegex->Checked = ini.ReadBool("Search", "Regex", false);
  fmFind->rgRange->ItemIndex = ini.ReadInteger("Search", "Range", 3);

  String LaunchName[3];
  mnAppli0->Hint = ini.ReadString("Application", "E0", "");
  mnAppli0->Tag = ini.ReadBool("Application", "Q0", true);
  mnAppli0->Enabled = (mnAppli0->Hint != "");
  LaunchName[0] = ini.ReadString("Application", "N0", L"未設定");
  mnAppli1->Hint = ini.ReadString("Application", "E1", "");
  mnAppli1->Tag  = ini.ReadBool("Application", "Q1", true);
  mnAppli1->Enabled = (mnAppli1->Hint != "");
  LaunchName[1]  = ini.ReadString("Application", "N1", L"未設定");
  mnAppli2->Hint = ini.ReadString("Application", "E2", "");
  mnAppli2->Tag  = ini.ReadBool("Application", "Q2", true);
  mnAppli2->Enabled = (mnAppli2->Hint != "");
  LaunchName[2]  = ini.ReadString("Application", "N2", L"未設定");
  MainGrid->BrowserFileName = ini.ReadString("Application", "Browser", "");
  History.clear();
  for (int i = 0; i < 10; i++) {
    String historyFile = ini.ReadString("History", (String)i, "");
    if (historyFile != "") {
      History.push_back(historyFile);
    } else {
      break;
    }
  }
  dlgOpenMacro->InitialDir = ini.ReadString("History", "Macro", "");

  if (FileExists(Pref->Path + "AutoKey.csv")) {
    std::unique_ptr<TfmKey> fmKey = std::make_unique<TfmKey>(nullptr);
    fmKey->MakeTree();
    if (fmKey->LoadKey(Pref->Path + "AutoKey.csv")) {
      fmKey->MenuUpDate();
    }
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
    IniFile ini = Pref->GetInifile();
    // Use the historical style names to be compatible with older versions.
    ini.WriteString(
        "Mode", "Style", IsDarkMode(Style) ? "Windows10 Dark": "Windows");
    ini.WriteInteger("Position", "Mode", WindowState == wsMaximized ? 2 : 0);
    if (WindowState == wsNormal) {
      int Slide = PosSlide ? 32 : 0;
      ini.WriteInteger("Position", "Left", Left+Slide);
      ini.WriteInteger("Position", "Top", Top+Slide);
      ini.WriteInteger("Position", "Width", Width);
      ini.WriteInteger("Position", "Height", Height);
    }
    ini.WriteInteger("Position", "Dpi", ScreenDpi);
    ini.WriteString("Font", "Name", MainGrid->Font->Name);
    ini.WriteInteger("Font", "Size", MainGrid->Font->Size);
    ini.WriteInteger("Font", "Bold", MainGrid->Font->Style.Contains(fsBold));
    ini.WriteInteger(
        "Font", "Italic", MainGrid->Font->Style.Contains(fsItalic));
    ini.WriteInteger(
        "Font", "Underline", MainGrid->Font->Style.Contains(fsUnderline));
    ini.WriteInteger(
        "Font", "StrikeOut", MainGrid->Font->Style.Contains(fsStrikeOut));
    ini.WriteInteger("Font", "TBMargin", MainGrid->TBMargin);
    ini.WriteInteger("Font", "LRMargin", MainGrid->LRMargin);
    ini.WriteInteger("Font", "CellLineMargin", MainGrid->CellLineMargin);
    ini.WriteInteger("Font", "BgColor", MainGrid->Color);
    ini.WriteInteger("Font", "CalcBgColor", MainGrid->CalcBgColor);
    ini.WriteInteger("Font", "CalcErrorBgColor", MainGrid->CalcErrorBgColor);
    ini.WriteInteger("Font", "CalcErrorFgColor", MainGrid->CalcErrorFgColor);
    ini.WriteInteger("Font", "CalcFgColor", MainGrid->CalcFgColor);
    ini.WriteInteger("Font", "CurrentColBgColor", MainGrid->CurrentColBgColor);
    ini.WriteInteger("Font", "CurrentRowBgColor", MainGrid->CurrentRowBgColor);
    ini.WriteInteger("Font", "DummyBgColor", MainGrid->DummyBgColor);
    ini.WriteInteger("Font", "EvenRowBgColor", MainGrid->EvenRowBgColor);
    ini.WriteInteger("Font", "FgColor", MainGrid->Font->Color);
    ini.WriteInteger("Font", "FixFgColor", MainGrid->FixFgColor);
    ini.WriteInteger("Font", "FixedColor", MainGrid->FixedColor);
    ini.WriteInteger("Font", "FoundBgColor", MainGrid->FoundBgColor);
    ini.WriteInteger("Font", "UrlColor", MainGrid->UrlColor);
    ini.WriteBool("Font", "WordWrap", MainGrid->WordWrap);
    ini.WriteInteger("Font", "TextAlign", MainGrid->TextAlignment);
    ini.WriteInteger("Font", "NumberComma", MainGrid->NumberComma);
    ini.WriteInteger("Font", "DecimalDigits", MainGrid->DecimalDigits);
    ini.WriteInteger("Font", "MinColWidth", MainGrid->MinColWidth);

    ini.WriteString("Print", "FontName", PrintFontName);
    ini.WriteInteger("Print", "FontSize", PrintFontSize);
    ini.WriteInteger("Print", "MarginLeft", PrintMargin[0]);
    ini.WriteInteger("Print", "MarginRight", PrintMargin[1]);
    ini.WriteInteger("Print", "MarginTop", PrintMargin[2]);
    ini.WriteInteger("Print", "MarginBottom", PrintMargin[3]);
    ini.WriteString("Print", "Header", PrintHeader);
    ini.WriteInteger("Print", "HeaderPosition", PrintHeaderPosition);
    ini.WriteString("Print", "Footer", PrintFooter);
    ini.WriteInteger("Print", "FooterPosition", PrintFooterPosition);

    int DataCount = TypeList.Count;
    ini.WriteInteger("DataType", "Count", DataCount);
    for (int i = 0; i < DataCount; i++) {
      String Section = (String)"DataType:" + i;
      TTypeOption *TO = TypeList.Items(i);
      ini.WriteString(Section, "Name", TO->Name);
      ini.WriteString(Section, "Exts", TO->GetExtsStr(0));
      ini.WriteBool(Section, "ForceExt", TO->ForceExt);
      ini.WriteString(Section, "SepChars", Ctrl2Ascii(TO->SepChars));
      ini.WriteString(Section, "WeakSepChars", Ctrl2Ascii(TO->WeakSepChars));
      ini.WriteString(Section, "QuoteChars", Ctrl2Ascii(TO->QuoteChars));
      ini.WriteInteger(Section, "Quote", TO->QuoteOption);
      ini.WriteString(Section, "QuoteExpression", TO->QuoteExpression);
      ini.WriteBool(Section, "OmitComma", TO->OmitComma);
      ini.WriteBool(Section, "DummyEof", TO->DummyEof);
      ini.WriteBool(Section, "DummyEol", TO->DummyEol);
    }

    ini.WriteBool("Backup", "OnSave", BackupOnSave);
    ini.WriteBool("Backup", "OnTime", BackupOnTime);
    ini.WriteBool("Backup", "OnOpen", BackupOnOpen);
    ini.WriteString("Backup", "FileNameS", BuFileNameS);
    ini.WriteString("Backup", "FileNameT", BuFileNameT);
    ini.WriteBool("Backup", "DeleteSSaved", DelBuSSaved);
    ini.WriteBool("Backup", "DeleteS", DelBuSExit);
    ini.WriteBool("Backup", "DeleteT", DelBuT);
    ini.WriteInteger("Backup", "Interval", BuInterval);

    ini.WriteInteger("Mode", "Paste", MainGrid->PasteOption);
    ini.WriteInteger("Mode", "DragCounter", MainGrid->DragBehavior);
    ini.WriteInteger("Mode", "EnterMove", MainGrid->EnterMove);
    ini.WriteInteger("Mode", "FixTopRow",
        MainGrid->ShowColCounter ? 0 : MainGrid->FixedRows);
    ini.WriteInteger("Mode", "FixLeftCol",
        MainGrid->ShowRowCounter ? 0 : MainGrid->FixedCols);
    ini.WriteBool("Mode", "ShowToolbar", mnShowToolbar->Checked);
    ini.WriteInteger("Mode", "ToolbarSize", ToolBarSize);
    ini.WriteBool("Mode", "ShowStatusbar", mnShowStatusbar->Checked);
    ini.WriteBool("Mode", "ShowToolTipForLongCell",
        MainGrid->ShowToolTipForLongCell);
    ini.WriteInteger("Mode", "HintPause", Application->HintPause);
    ini.WriteInteger("Mode", "HintHidePause", Application->HintHidePause);
    ini.WriteBool("Mode", "LeftArrowInCell", MainGrid->LeftArrowInCell);
    ini.WriteInteger("Mode", "WheelMoveCursol", MainGrid->WheelMoveCursol);
    ini.WriteInteger("Mode", "WheelScrollStep", MainGrid->WheelScrollStep);
    ini.WriteBool("Mode", "AlwaysShowEditor", MainGrid->AlwaysShowEditor);
    ini.WriteBool("Mode", "SortAll", SortAll);
    ini.WriteInteger("Mode", "UseURL", MainGrid->DblClickOpenURL);

    ini.WriteBool("Mode", "NewWindow", MakeNewWindow);
    ini.WriteBool("Mode"," TitleFullPath", TitleFullPath);
    ini.WriteInteger("Mode", "LockFile", LockFile);
    ini.WriteBool("Mode", "CheckTimeStamp", CheckTimeStamp);
    ini.WriteBool("Mode", "SortByNumber", SortByNumber);
    ini.WriteBool("Mode", "SortIgnoreCase", SortIgnoreCase);
    ini.WriteBool("Mode", "SortIgnoreZenhan", SortIgnoreZenhan);
    ini.WriteBool("Mode", "SortDirection", SortDirection);
    ini.WriteBool("Mode", "CheckKanji", EncodingDetector.Enabled);
    ini.WriteInteger("Mode", "DefaultCharCode",
        ToCharCode(EncodingDetector.DefaultEncoding));
    ini.WriteBool("Mode", "CompactColWidth", MainGrid->CompactColWidth);
    ini.WriteBool("Mode", "CalcWidthForAllRow", MainGrid->CalcWidthForAllRow);
    ini.WriteFloat("Mode", "MaxRowHeightLines", MainGrid->MaxRowHeightLines);
    ini.WriteInteger("Mode", "StopMacro", StopMacroCount);
    ini.WriteInteger("Mode", "UndoCount", MainGrid->UndoList->MaxCount);

    ini.WriteBool("Search", "Case", fmFind->cbCase->Checked);
    ini.WriteBool("Search", "Word", fmFind->cbWordSearch->Checked);
    ini.WriteBool("Search", "Regex", fmFind->cbRegex->Checked);
    ini.WriteInteger("Search", "Range", fmFind->rgRange->ItemIndex);

    ini.WriteString("Application", "E0", mnAppli0->Hint);
    ini.WriteString("Application", "N0", mnAppli0->Caption.c_str() + 4);
    ini.WriteBool("Application", "Q0", mnAppli0->Tag);
    ini.WriteString("Application", "E1", mnAppli1->Hint);
    ini.WriteString("Application", "N1", mnAppli1->Caption.c_str() + 4);
    ini.WriteBool("Application", "Q1", mnAppli1->Tag);
    ini.WriteString("Application", "E2", mnAppli2->Hint);
    ini.WriteString("Application", "N2", mnAppli2->Caption.c_str() + 4);
    ini.WriteBool("Application", "Q2", mnAppli2->Tag);
    ini.WriteString("Application", "Browser", MainGrid->BrowserFileName);

    for (int i = 0; i < History.size(); i++) {
      ini.WriteString("History", (String)i, History[i]);
    }
    for (int i = History.size(); i < 10; i++) {
      ini.DeleteKey("History", (String)i);
    }
    if (dlgOpenMacro->InitialDir != "" &&
        dlgOpenMacro->InitialDir != Pref->UserPath + "Macro") {
      ini.WriteString("History", "Macro", dlgOpenMacro->InitialDir);
    } else {
      ini.DeleteKey("History", "Macro");
    }
  } catch(...) {}
}
//---------------------------------------------------------------------------
TToolButton *TfmMain::AddToolButton(String Label, String Name, String Action,
    int Left, TToolBar *ToolBar)
{
  TToolButton *button = new TToolButton(ToolBar);
  button->Left = Left;
  button->Parent = ToolBar;

  if (Name == "-") {
    button->Style = tbsSeparator;
    button->Width = 8;
    return button;
  }

  TMenuItem *menuItem = FindMenuItem(Action);
  if (menuItem) {
    if (menuItem->Action) {
      button->Action = menuItem->Action;
    } else {
      button->OnClick = menuItem->OnClick;
    }
    button->Hint = Name;
  } else {
    button->OnClick = UserToolBarAction;
    button->Hint = StringReplace(
        Name, "|", "_", TReplaceFlags() << rfReplaceAll) + "|" + Action;
  }

  int imageIndex = Label.ToIntDef(-1);
  if (imageIndex >= 0) {
    button->ImageIndex = imageIndex;
  } else {
    ToolBar->AllowTextButtons = true;
    button->Style = tbsTextButton;
    button->Caption = Label != "" ? Label : Name;
  }

  if (Action == "OpenHistory") {
    button->OnClick = mnOpenClick;
    button->Hint = Name;
    button->Style = tbsDropDown;
    button->DropdownMenu = PopMenuOpen;
  }
  return button;
}
//---------------------------------------------------------------------------
void AddToImageCollection(TBitmap *Bitmap, TImageCollection *ImageCollection)
{
  std::unique_ptr<TBitmap> maskBitmap = std::make_unique<TBitmap>();
  maskBitmap->Assign(Bitmap);
  maskBitmap->Mask(clSilver);

  TIconInfo iconInfo;
  iconInfo.fIcon = true;
  iconInfo.xHotspot = 0;
  iconInfo.yHotspot = 0;
  iconInfo.hbmMask = maskBitmap->Handle;
  iconInfo.hbmColor = Bitmap->Handle;

  std::unique_ptr<TIcon> icon = std::make_unique<TIcon>();
  icon->Handle = CreateIconIndirect(&iconInfo);

  std::unique_ptr<TMemoryStream> stream = std::make_unique<TMemoryStream>();
  icon->SaveToStream(stream.get());

  ImageCollection->Images->Add()->SourceImages->Add()->Image
      ->LoadFromStream(stream.get());
}
//---------------------------------------------------------------------------
static inline int GetToolBarButtonSize(int ToolBarSize, int ScreenDpi)
{
  int minSize = (24 * ScreenDpi / 96) - 8;
  return ToolBarSize < minSize ? minSize : ToolBarSize;
}
//---------------------------------------------------------------------------
TToolBar *TfmMain::AddToolBar(String Label, String ImageList, int Top, int Left)
{
  TToolBar *toolBar = new TToolBar(CoolBar);
  toolBar->Parent = CoolBar;
  toolBar->Wrapable = false;
  toolBar->AutoSize = true;
  toolBar->Font->Height = GetToolBarButtonSize(ToolBarSize, ScreenDpi);
  toolBar->Top = Top;
  toolBar->Left = Left;

  TVirtualImageList *images = nullptr;
  TVirtualImageList *disabledImages = nullptr;
  String imageListFileName = Pref->Path + ImageList;
  if (Label == "#1" || ImageList == "#1") {
    if (IsDarkMode(Style)) {
      images = imlNormalDark;
      disabledImages = imlNormalDarkDisabled;
    } else {
      images = imlNormal;
      disabledImages = imlNormalDisabled;
    }
  } else if (Label == "#2" || ImageList == "#2") {
    if (IsDarkMode(Style)) {
      images = imlAdditionalDark;
    } else {
      images = imlAdditional;
    }
  } else if (ImageList != "" && FileExists(imageListFileName)) {
    std::unique_ptr<TBitmap> imageListBitmap = std::make_unique<TBitmap>();
    imageListBitmap->LoadFromFile(imageListFileName);

    TImageCollection *imageCollection = new TImageCollection(this);
    for (int x = 0; x < imageListBitmap->Width; x += 16) {
      std::unique_ptr<TBitmap> bitmap = std::make_unique<TBitmap>(16, 16);
      bitmap->Canvas->Draw(-x, 0, imageListBitmap.get());
      AddToImageCollection(bitmap.get(), imageCollection);
    }

    images = new TVirtualImageList(this);
    images->AutoFill = true;
    images->ImageCollection = imageCollection;
  }
  if (images) {
    images->SetSize(ToolBarSize, ToolBarSize);
    toolBar->Images = images;
  }
  if (disabledImages) {
    disabledImages->SetSize(ToolBarSize, ToolBarSize);
    toolBar->DisabledImages = disabledImages;
  }

  if (Label == "#1") {
    int width = 0;
    width += AddToolButton("6", L"新規作成", "New", width, toolBar)->Width;
    width += AddToolButton("7", L"開く", "OpenHistory", width, toolBar)->Width;
    width += AddToolButton("8", L"上書き保存", "Save", width, toolBar)->Width;
    width += AddToolButton("", "-", "", width, toolBar)->Width;
    width += AddToolButton("0", L"切り取り", "Cut", width, toolBar)->Width;
    width += AddToolButton("1", L"コピー", "Copy", width, toolBar)->Width;
    width += AddToolButton("2", L"貼り付け", "Paste", width, toolBar)->Width;
    width += AddToolButton("", "-", "", width, toolBar)->Width;
    width += AddToolButton("3", L"元に戻す", "Undo", width, toolBar)->Width;
    width += AddToolButton("4", L"やり直し", "Redo", width, toolBar)->Width;
    toolBar->Width = width;
  } else if (Label == "#2") {
    int width = 0;
    width += AddToolButton("0", L"ソート", "Sort", width, toolBar)->Width;
    width += AddToolButton("", "-", "", width, toolBar)->Width;
    width += AddToolButton("1", L"１行挿入", "InsRow", width, toolBar)->Width;
    width += AddToolButton("2", L"１列挿入", "InsCol", width, toolBar)->Width;
    width += AddToolButton("3", L"１行削除", "CutRow", width, toolBar)->Width;
    width += AddToolButton("4", L"１列削除", "CutCol", width, toolBar)->Width;
    width += AddToolButton("", "-", "", width, toolBar)->Width;
    width += AddToolButton("5", L"検索・置換", "Find", width, toolBar)->Width;
    width += AddToolButton("", "-", "", width, toolBar)->Width;
    width +=
        AddToolButton("6", L"表示の更新", "Refresh", width, toolBar)->Width;
    width += AddToolButton("8", L"フォント", "Font", width, toolBar)->Width;
    width += AddToolButton(
        "12", L"セル内計算式を処理", "CalcExpression", width, toolBar)->Width;
    width += AddToolButton("", "-", "", width, toolBar)->Width;
    width += AddToolButton(
        "9", L"カーソル位置までを固定", "FixUpLeft", width, toolBar)->Width;
    width += AddToolButton(
        "10", L"１行目を固定", "FixFirstRow", width, toolBar)->Width;
    width += AddToolButton(
        "11", L"１列目を固定", "FixFirstCol", width, toolBar)->Width;
    toolBar->Width = width;
  } else {
    toolBar->Width = 0;
  }
  return toolBar;
}
//---------------------------------------------------------------------------
void TfmMain::ReadToolBar()
{
  bool visible = CoolBar->Visible;
  CoolBar->Visible = false;

  for (int i = CoolBar->ControlCount - 1; i >= 0; i--) {
    TToolBar *toolbar = static_cast<TToolBar *>(CoolBar->Controls[i]);
    toolbar->Parent = nullptr;
    delete toolbar;
  }
  CoolBar->RowSize = GetToolBarButtonSize(ToolBarSize, ScreenDpi) + 10;

  String toolbarcsv = Pref->Path + "ToolBar.csv";
  if (!FileExists(toolbarcsv)) {
    TToolBar *tbarNormal = AddToolBar("#1", "", 0, 0);
    AddToolBar("#2", "", 0, tbarNormal->Width);
    CoolBar->Visible = visible;
    return;
  }

  TTypeOption typeOption("CSV");
  CsvReader reader(
      &typeOption, toolbarcsv, EncodingDetector.Detect(toolbarcsv));
  std::vector<String> row;
  reader.ReadLine(row);
  if (row.size() == 0 || row[0] != "(Cassava-ToolBarSetting)") {
    return;
  }
  TToolBar *toolBar = nullptr;
  int width = 0;
  int tbarTop = 0;
  int tbarLeft = 0;
  while (reader.ReadLine(row)) {
    row.resize(3);
    String str0 = row[0];
    String name = row[1];
    String action = row[2];

    if (str0 != "" && str0[1] == '=') {
      tbarTop += CoolBar->RowSize;
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
      toolBar = AddToolBar(str0, name, tbarTop, tbarLeft);
      width = toolBar->Width;
    } else if (name != "") {
      width += AddToolButton(str0, name, action, width, toolBar)->Width;
    }
  }
  if (toolBar) {
    toolBar->Width = width;
  }
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
void TfmMain::SetToolBarSize(int Size)
{
  FToolBarSize = Size;
  ReadToolBar();
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
  std::vector<String> allExts;
  for (int i = 0; i < TypeList.Count; i++) {
    TTypeOption *p = TypeList.Items(i);
    FilterExt = "";
    for (int j = 0; j < p->Exts.size(); j++){
      if (j > 0) FilterExt += ";";
      String str = p->Exts[j];
      FilterExt += "*." + str;
      if (std::find(allExts.begin(), allExts.end(), str) == allExts.end()) {
        allExts.push_back(str);
      }
    }
    if (i > 0) {
      OFilter += p->Name + " (" + FilterExt + ")|" + FilterExt + "|";
    }
    SFilter += p->Name + " (*." + p->DefExt() + ")|*." + p->DefExt() + "|";
  }
  FilterExt = "";
  for (int i = 0; i < allExts.size(); i++){
    if (i > 0) FilterExt += ";";
    FilterExt += "*." + allExts[i];
  }
  String CFilter = (String)"Cassava (" + FilterExt + ")|" + FilterExt + "|";
  dlgOpen->Filter = CFilter + OFilter + L"すべてのファイル (*.*)|*.*";
  dlgSave->Filter = SFilter;
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
  ReadToolBar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::MainGridChangeModified(TObject *Sender)
{
  bool modified = MainGrid->Modified;
  acSave->Enabled = modified;
  if (modified && LockFile == cssv_lfEdit && LockingFile == nullptr &&
      FileName != "") {
    LockingFile =
        std::make_unique<TFileStream>(FileName, fmOpenWrite|fmShareDenyWrite);
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
  if (MakeNewWindow) {
    WriteIni(true);
    SpawnProcess({ParamStr(0)});
  } else {
    if (IfModifiedThenSave()) {
      Clear();
    }
  }
}
//---------------------------------------------------------------------------
void TfmMain::Clear()
{
  MainGrid->Clear();
  if (mnFixFirstRow->Checked || mnFixFirstCol->Checked) {
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
  pnlSearch->Visible = false;
  UpdateStatusbar();

  if (LockingFile) {
    LockingFile.reset();
  }
}
//---------------------------------------------------------------------------
void TfmMain::OpenFile(String OpenFileName, int CharCode,
    const TTypeOption *Format)
{
  if(!FileExists(OpenFileName)){
    Application->MessageBox(
      (L"ファイル " + OpenFileName + L" は存在しません").c_str(),
      ExtractFileName(OpenFileName).c_str(), MB_ICONERROR);
    return;
  }
  if(LockingFile){
    LockingFile.reset();
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
      LockingFile =
          std::make_unique<TFileStream>(FileName, fmOpenWrite|fmShareDenyWrite);
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
        SpawnProcess({ParamStr(0), files->Strings[i]});
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
      SpawnProcess({ParamStr(0), DropFileNames[i]});
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
    return L"無題";
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
                (GetUiFileName() + L" への変更を保存しますか？").c_str(),
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
      LockingFile.reset();
    }

    // バックアップ前に、アクセス権限を確認
    if (FileExists(FileName)) {
      std::unique_ptr<TFileStream> accessTest;
      try {
        accessTest = std::make_unique<TFileStream>(
            FileName, fmOpenReadWrite|fmShareDenyWrite);
      } catch(Exception &ex) {
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
      LockingFile =
          std::make_unique<TFileStream>(FileName, fmOpenWrite|fmShareDenyWrite);
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
    dlgSave->FileName = L"無題." + typeOption->DefExt();
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
std::set<String> TfmMain::GetCheckedMenus()
{
  std::set<String> checkedMenus;
  TMenuItem *items = MainMenu->Items;
  for (int i = 0; i < items->Count; i++) {
    AddCheckedMenus(checkedMenus, items->Items[i]);
  }
  return checkedMenus;
}
//---------------------------------------------------------------------------
void TfmMain::AddCheckedMenus(std::set<String>& checkedMenus, TMenuItem* item)
{
   if (item == mnFile || item->Caption == "-") {
     return;
   }

   if (item->Checked) {
     checkedMenus.insert(item->Name);
   }

   if (item->Count > 0) {
     for (int i = 0; i < item->Count; i++) {
       AddCheckedMenus(checkedMenus, item->Items[i]);
     }
   }
}
//---------------------------------------------------------------------------
void TfmMain::RestoreCheckedMenus(const std::set<String>& checkedMenus)
{
  TMenuItem *items = MainMenu->Items;
  for (int i = 0; i < items->Count; i++){
    RestoreCheckedMenus(checkedMenus, items->Items[i]);
  }
}
//---------------------------------------------------------------------------
void TfmMain::RestoreCheckedMenus(const std::set<String>& checkedMenus,
    TMenuItem* item)
{
  if (item == mnFile || item->Caption == "-") {
    return;
  }

  bool isChecked = item->Checked;
  bool toChecked = (checkedMenus.count(item->Name) > 0);
  bool toChange;
  if (item->GroupIndex > 0) {
    toChange = (!isChecked && toChecked);
  } else {
    toChange = (isChecked != toChecked);
  }
  if (toChange) {
    if (item->OnClick) {
      item->OnClick(item);
    }
    item->Checked = toChecked;
  }

  if (item->Count > 0) {
    for (int i = 0; i < item->Count; i++) {
      RestoreCheckedMenus(checkedMenus, item->Items[i]);
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
  std::vector<String> types;
  TSearchRec sr;
  if(FindFirst(Pref->UserPath+"Export\\*.cms", faAnyFile, sr) == 0){
    do{
      String type = ChangeFileExt(sr.Name,"");
      String ext = ExtractFileExt(type);
      if(ext == ""){ ext = (String)"." + type; }
      strFilter += type.UpperCase() + L" 形式 (*" + ext + ")|*" + ext + "|";
      types.push_back(type);
    }while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if(FindFirst(Pref->SharedPath+"Export\\*.cms", faAnyFile, sr) == 0){
    do{
      String type = ChangeFileExt(sr.Name,"");
      String ext = ExtractFileExt(type);
      if(ext == ""){ ext = (String)"." + type; }
      strFilter += type.UpperCase() + L" 形式 (*" + ext + ")|*" + ext + "|";
      types.push_back(type);
    }while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if(strFilter == ""){
    Application->MessageBox(
        L"エクスポート可能な形式はありません。", L"Cassava Export", 0);
    return;
  }
  dlgSave->Filter = strFilter;
  dlgSave->InitialDir = ExtractFilePath(dlgSave->FileName);
  dlgSave->FileName = ChangeFileExt(ExtractFileName(dlgSave->FileName), "");
  if(dlgSave->Execute()){
    dlgSave->Filter = strOrgFilter;
    String type = types[dlgSave->FilterIndex - 1];
    if(ExtractFileExt(dlgSave->FileName) == ""){
      String ext = ExtractFileExt(type);
      if(ext == ""){ ext = (String)"." + type; }
      dlgSave->FileName = dlgSave->FileName + ext;
    }
    Export(dlgSave->FileName, type);
  }
  dlgSave->Filter = strOrgFilter;
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
          (type + L" 形式ではエクスポートできません。").c_str(),
          L"Cassava Export", 0);
      return;
    }

    try {
      std::unique_ptr<TStream> out =
          std::make_unique<TFileStream>(filename, fmCreate | fmShareDenyWrite);
      TEncoding *encoding = MainGrid->Encoding;
      TReturnCode returnCode = MainGrid->ReturnCode;
      TReturnCode inCellReturnCode = MainGrid->InCellReturnCode;
      bool addBom = MainGrid->AddBom;
      EncodedWriter encodedWriter(out.get(), encoding, addBom);
      std::set<String> checkedMenus = GetCheckedMenus();

      MacroExec(CmsFile, &encodedWriter);

      RestoreCheckedMenus(checkedMenus);
      MainGrid->Encoding = encoding;
      MainGrid->ReturnCode = returnCode;
      MainGrid->InCellReturnCode = inCellReturnCode;
      MainGrid->AddBom = addBom;
    } catch(Exception *e) {
      Application->MessageBox(e->Message.c_str(),
                              L"Cassava Macro Interpreter", 0);
    }
}
//---------------------------------------------------------------------------
void TfmMain::SetHistory(String S)
{
  if (S != "") {
    std::erase_if(History, [S](String value) { return value == S; });
    History.insert(History.begin(), S);
  }
  if (History.size() > 10) {
    History.resize(10);
  }

  TMenuItem *MnHist[10] = {
    mnOpenHistory0, mnOpenHistory1, mnOpenHistory2, mnOpenHistory3,
    mnOpenHistory4, mnOpenHistory5, mnOpenHistory6, mnOpenHistory7,
    mnOpenHistory8, mnOpenHistory9 };

  for (int i = 0; i < 10; i++) {
    if (i < History.size()){
      MnHist[i]->Caption = (String)"&" + i + ": " + History[i];
      MnHist[i]->Enabled = true;
      MnHist[i]->Visible = true;
    } else {
      MnHist[i]->Visible = false;
      MnHist[i]->Caption = (String)"&" + i + L": (なし)";
    }
  }
  if (History.size() == 0) {
    mnOpenHistory0->Enabled = false;
    mnOpenHistory0->Visible = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOpenHistoryClick(TObject *Sender)
{
  int Num = static_cast<TMenuItem *>(Sender)->Tag;
  String FN = History[Num];

  if (!FileExists(FN)) {
    History.erase(History.begin() + Num);
    SetHistory("");
    Application->MessageBox(
      (L"ファイル " + FN + L" は存在しません").c_str(),
      ExtractFileName(FN).c_str(), MB_ICONERROR);
    return;
  }

  if(MakeNewWindow){
    WriteIni(true);
    SpawnProcess({ParamStr(0), FN});
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

  for (int i = 0; i < History.size(); i++) {
    TMenuItem *NewItem = new TMenuItem(mi->Owner);
    NewItem->Caption = (String)"&" + i + ": " + History[i];
    NewItem->Tag = i;
    NewItem->OnClick = mnOpenHistoryClick;
    mi->Add(NewItem);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnOpenHistoryClearClick(TObject *Sender)
{
  History.clear();
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
  std::unique_ptr<TfmPrint> fmPrint = std::make_unique<TfmPrint>(nullptr);
  fmPrint->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnLetterPrintClick(TObject *Sender)
{
  std::unique_ptr<TfmLetter> fmLetter = std::make_unique<TfmLetter>(nullptr);
  fmLetter->ShowModal();
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
    SendMessage(edFindText->Handle, WM_CUT, 0, 0);
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
    SendMessage(edFindText->Handle, WM_COPY, 0, 0);
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
    SendMessage(edFindText->Handle, WM_PASTE, 0, 0);
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
  std::unique_ptr<TStringList> items = std::make_unique<TStringList>();
  items->Text = StatusBarPopUp[panelIndex].Label;
  for (int i = 0; i < items->Count; i++) {
    TMenuItem *newItem = new TMenuItem(PopMenuStatusBar);
    newItem->Caption = items->Strings[i];
    newItem->Tag = (panelIndex << 16) + i;
    newItem->OnClick = StatusBarPopUpClick;
    PopMenuStatusBar->Items->Add(newItem);
  }
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
  std::unique_ptr<TStringList> arguments = std::make_unique<TStringList>();
  arguments->Text = StatusBarPopUp[panelIndex].Label;
  String label = arguments->Strings[tag & 0xffff];
  arguments->Clear();
  arguments->Add(label);
  RunMacro(StatusBarPopUp[panelIndex].Handler, StopMacroCount,
      SystemMacroContext, -1, -1, /* ReadOnly= */ false, /* IO= */ nullptr,
      arguments.get());
  UpdateStatusbar();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::acFixFirstRowExecute(TObject *Sender)
{
  if(MainGrid->FileOpenThread){
    mnFixFirstRow->Checked = !MainGrid->ShowColCounter;
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
  std::unique_ptr<TfmOption> fmOption = std::make_unique<TfmOption>(nullptr);
  fmOption->ShowModal();
  UpdateTitle();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnAppliClick(TObject *Sender)
{
  if(IfModifiedThenSave()){
    if(MainGrid->Modified) return;
    TMenuItem *menuItem = static_cast<TMenuItem*>(Sender);
    String exe = menuItem->Hint;
    if(FileName == ""){
      SpawnProcess({exe});
    }else{
      SpawnProcess({exe, FileName});
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
  ShellOpen({L"Explorer.exe", path});
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnMacroOpenFolderClick(TObject *Sender)
{
  ShellOpen({L"Explorer.exe", Pref->SharedPath + "Macro"});
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
  MainGrid->Hint = L"マクロを実行中です。";
  MainGrid->ShowHint = true;
  Application->Hint = MainGrid->Hint;
  ApplicationHint(nullptr);

  TMacroContext macroContext;
  String inPath = ExtractFilePath(CmsFile);
  if (inPath != "" && *(inPath.LastChar()) != '\\') {
    inPath += "\\";
  }
  macroContext.AddDirectory(inPath);
  macroContext.AddDirectory(Pref->UserPath + "Macro\\");
  macroContext.AddDirectory(Pref->SharedPath + "Macro\\");
  bool ok = CompileMacro(CmsFile, &macroContext, true);
  if (ok) {
    MainGrid->UndoList->Push();
    MainGrid->Invalidate();
    acMacroTerminate->Enabled = true;
    RunMacro(CmsFile, StopMacroCount, macroContext, -1, -1,
        /* ReadOnly= */ false, io);
    MainGrid->Invalidate();
    MainGrid->UndoList->Pop();
  }

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
      RunMacro(StatusbarCmsFile, StopMacroCount, SystemMacroContext, -1, -1,
          /* ReadOnly= */ true);
    } catch(...) {}
  }
}
//---------------------------------------------------------------------------
void TfmMain::MacroScriptExec(String cmsname, String script)
{
  TMacroContext macroContext;
  macroContext.AddDirectory(Pref->UserPath + "Macro\\");
  macroContext.AddDirectory(Pref->SharedPath + "Macro\\");
  bool ok = CompileMacro(&script, cmsname, &macroContext, true);
  if (ok) {
    acMacroTerminate->Enabled = true;
    RunMacro(cmsname, StopMacroCount, macroContext, -1, -1);
  }
}
//---------------------------------------------------------------------------
TCalculatedCell TfmMain::GetCalculatedCell(String Str, int ACol, int ARow)
{
  if (Str.Length() == 0 || Str[1] != '=') {
    return TCalculatedCell(Str, ctNotExpr);
  }
  TCalculatedCell result = TCalculatedCell(Str, ctError);
  Str.Delete(1,1);
  TMacroContext macroContext;
  macroContext.AddDirectory(Pref->UserPath + "Macro\\");
  macroContext.AddDirectory(Pref->SharedPath + "Macro\\");
  String cmsName = (String)"$@cell_" + ACol + "_" + ARow;
  try {
    String formula = "return " + Str + ";";
    bool ok = CompileMacro(&formula, cmsName, &macroContext, false);
    if (ok){
      TMacroValue macroResult = RunMacro(cmsName, StopMacroCount, macroContext,
          ACol, ARow, /* ReadOnly= */ true);
      result = TCalculatedCell(macroResult.string, ctOk);
    }
  }catch(...){
    // エラー用のResultCell文字列は設定済み
  }
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
      TMacroValue result = RunMacro(FormatCmsFile, StopMacroCount,
          SystemMacroContext, AX, AY, /* ReadOnly= */ true);
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
void TfmMain::UpdateQuickFindPanel()
{
  String text = fmFind->edFindText->Text;
  if (text != "") {
    int i = edFindText->Items->IndexOf(text);
    if (i >= 0) {
      edFindText->Items->Delete(i);
    }
    edFindText->Items->Insert(0, text);
  }
  edFindText->Text = text;
  btnCase->Down = fmFind->cbCase->Checked;
  btnWordSearch->Down = fmFind->cbWordSearch->Checked;
  btnRegex->Down = fmFind->cbRegex->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mnQuickFindClick(TObject *Sender)
{
  UpdateQuickFindPanel();
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
void __fastcall TfmMain::btnCaseClick(TObject *Sender)
{
  fmFind->cbCase->Checked = btnCase->Down;
  MainGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnWordSearchClick(TObject *Sender)
{
  fmFind->cbWordSearch->Checked = btnWordSearch->Down;
  MainGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnRegexClick(TObject *Sender)
{
  fmFind->cbRegex->Checked = btnRegex->Down;
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
      R.Top    = MainGrid->Selection.Top;
      R.Right  = MainGrid->DataRight;
      R.Bottom = MainGrid->DataBottom;
      sortcol = MainGrid->Selection.Left;
    }else{
      R = MainGrid->Selection;
      sortcol = MainGrid->Selection.Left;
    }
  }else{
    R.Left   = 1;
    R.Top    = MainGrid->Row;
    R.Right  = MainGrid->DataRight;
    R.Bottom = MainGrid->DataBottom;
    sortcol = MainGrid->Col;
  }

  if (!fmSort) {
    fmSort = new TfmSort(this);
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
  if (!fmSort) {
    fmSort = new TfmSort(this);
  }
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
  std::unique_ptr<TfmKey> fmKey = std::make_unique<TfmKey>(nullptr);
  fmKey->ShowModal();
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
    + L"\n   by あすかぜ\n                " + Version::CurrentDate();
  Application->MessageBox(message.c_str(), L"バージョン情報", 0);
}
//---------------------------------------------------------------------------

