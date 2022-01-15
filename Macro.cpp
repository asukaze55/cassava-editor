//---------------------------------------------------------------------------
#ifdef CssvMacro
//---------------------------------------------------------------------------
#include <cmath>
#include <map>
#include <process.h>
#pragma hdrstop
#include "Macro.h"
#include "MacroOpeCode.h"
#include "MainForm.h"
#include "MultiLineInputBox.h"
#include "AutoOpen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace std;
//---------------------------------------------------------------------------
#define etErr 0
#define etAtom 1
#define etVar 2
#define etCell 3
#define etSystem 4
//---------------------------------------------------------------------------
#define ME_HIKISU 1
#define ME_SECURITY 2
//---------------------------------------------------------------------------
#define TVar  map<String, Element>
#define MGCol (fmMain->MainGrid->Col - fmMain->MainGrid->DataLeft + 1)
#define MGRow (fmMain->MainGrid->Row - fmMain->MainGrid->DataTop  + 1)
#define RXtoAX(x)                 (x - fmMain->MainGrid->DataLeft + 1)
#define RYtoAY(y)                 (y - fmMain->MainGrid->DataTop  + 1)
#define AXtoRX(x)                 (x + fmMain->MainGrid->DataLeft - 1)
#define AYtoRY(y)                 (y + fmMain->MainGrid->DataTop  - 1)
//---------------------------------------------------------------------------
class TMacro;
//---------------------------------------------------------------------------
class Element {
private:
  String st;
  double vl;
  bool Num;
  TMacro *macro;
  Element &GetVar(String name);
public:
  int Type;
  bool isNum();
  int X, Y;
  Element() : Type(etErr), Num(true), vl(0), st(""), macro(NULL) {};
  Element(double d, TMacro *m) : Type(etAtom), Num(true), vl(d), macro(m) {};
  Element(String s, TMacro *m) : Type(etAtom), Num(false), st(s), macro(m) {};
  Element(String s, int t, TMacro *m) : Type(t), Num(false), st(s), macro(m) {};
  Element(String s, int t, bool nm, TMacro *m) : Type(t), Num(nm), st(s), macro(m) {};
  Element(int cl, int rw, TMacro *m);
  Element(int cl, int rw, bool nm, TMacro *m)
  : Type(etCell), Num(nm), X(cl), Y(rw), macro(m) {};
  Element(const Element &e)
  : st(e.st), vl(e.vl), Type(e.Type), Num(e.Num), X(e.X), Y(e.Y), macro(e.macro) {};
  double Val();
  String Str();
  void Sbst(Element e);
  void Pntr(Element e);
  Element Points();
};
//---------------------------------------------------------------------------
class TMacro{
private:
  TStream *fs;
  String ReadString(TStream *fs);
  TStream *GetStreamFor(String FileName);
  void ExecOpe(char c);
  void ExecFnc(String s);
  TList *Stack;
  int LoopCount;
  String FileName;
  bool canReadFile;
  bool canWriteFile;
  EncodedWriter *fs_io;
  TStringList *modules;
public:
  TMacro(EncodedWriter *io, int ml, TStringList *md, bool cm);
  TVar Var;
  int MaxLoop;
  bool IsCellMacro;
  Element *Do(String FileName, TList *AStack, int x=-1, int y=-1);
};
//---------------------------------------------------------------------------
class MacroException{
public:
  String Message;
  int Type;
  MacroException(String Mes, int t = 0) : Message(Mes), Type(t) {}
};
//---------------------------------------------------------------------------
static int RunningCount = 0;
static bool RunningOk = true;
//---------------------------------------------------------------------------
void CsvGridGoTo(TMainGrid *g, int x, int y){
  x += g->DataLeft - 1;
  y += g->DataTop  - 1;
  if(x < g->FixedCols){
    x = g->FixedCols;
  }else if(x >= g->ColCount){
    g->ChangeColCount(x+1);
  }
  if(y < g->FixedRows){
    y = g->FixedRows;
  }else if(y >= g->RowCount){
    g->ChangeRowCount(y+1);
  }
  g->Col = x;
  g->Row = y;
}
//---------------------------------------------------------------------------
void Write(int x, int y, String str, bool IsCellMacro)
{
  if(IsCellMacro){
	throw MacroException("Cell Macro Can't Update Cells.", ME_SECURITY);
  }
  if(x < 1 || y < 1) return;
  TMainGrid *g = fmMain->MainGrid;
  g->ACells[x][y] = str;
  g->Modified = true;
}
//---------------------------------------------------------------------------
double ToDouble(String str, double def)
{
  try{
    String temp = str;
    for(int i=temp.Length(); i>0; i--){
      if(temp[i] == ','){
        temp.Delete(i,1);
      }
    }
    return temp.ToDouble();
  }catch(...){
    return def;
  }
}
//---------------------------------------------------------------------------
Element::Element(int cl, int rw, TMacro *m)
{
  Type = etCell; X = cl; Y = rw;
  Num = fmMain->MainGrid->IsNumberAtACell(cl,rw);
  macro = m;
}
//---------------------------------------------------------------------------
Element &Element::GetVar(String name)
{
	return macro->Var[name];
}
//---------------------------------------------------------------------------
double Element::Val()
{
  if(Type == etCell){
    return ToDouble(fmMain->MainGrid->ACells[X][Y], 0);
  }else if(Type == etVar){
    return GetVar(st).Val();
  }else if(Type == etSystem){
    if     (st == "Col")       { return MGCol; }
    else if(st == "Row")       { return MGRow; }
    else if(st == "Right")     { return RXtoAX(fmMain->MainGrid->DataRight); }
    else if(st == "Bottom")    { return RYtoAY(fmMain->MainGrid->DataBottom); }
    else if(st == "SelLeft")   { return RXtoAX(fmMain->MainGrid->SelLeft); }
    else if(st == "SelTop")    { return RYtoAY(fmMain->MainGrid->SelTop); }
    else if(st == "SelRight")  { return RXtoAX(fmMain->MainGrid->Selection.Right); }
    else if(st == "SelBottom") { return RYtoAY(fmMain->MainGrid->Selection.Bottom); }
  }else if(Num){
    return vl;
  }
  return ToDouble(st, 0);
}
//---------------------------------------------------------------------------
String Element::Str()
{
  if(Type == etCell) return fmMain->MainGrid->ACells[X][Y];
  else if(Type == etVar) return GetVar(st).Str();
  else if(Type == etSystem) return String(Val());
  else if(Num) return String(vl);
  else return st;
}
//---------------------------------------------------------------------------
bool Element::isNum()
{
  if(Type == etVar){
	return GetVar(st).isNum();
  }else if(Type == etCell){
    return fmMain->MainGrid->IsNumberAtACell(X,Y);
  }
  return Num;
}
//---------------------------------------------------------------------------
void Element::Sbst(Element e)
{
  if(e.Type == etVar){
	GetVar(st) = GetVar(e.st);
  }else if(e.Type == etCell){
	GetVar(st) = (e.Num ? Element(e.Val(), macro) : Element(e.Str(), macro));
  }else if(e.Type == etSystem){
	GetVar(st) = Element(e.Val(), macro);
  }else{
    GetVar(st) = e;
  }

  if(Type == etSystem){
    int v = GetVar(st).Val();
    TMainGrid *g = fmMain->MainGrid;
    if     (st == "Col")       { CsvGridGoTo(g, v, MGRow); }
    else if(st == "Row")       { CsvGridGoTo(g, MGCol, v); }
    else if(st == "Right")     { g->ChangeColCount(v + g->DataLeft); }
    else if(st == "Bottom")    { g->ChangeRowCount(v + g->DataTop);  }
    else {
      TGridRect Sel = g->Selection;
      if(st == "SelLeft") {
        Sel.Left = v - 1 + g->DataLeft;
        if(Sel.Right < Sel.Left) { Sel.Right = Sel.Left; }
      }else if(st == "SelTop") {
        Sel.Top = v - 1 + g->DataTop;
        if(Sel.Bottom < Sel.Top) { Sel.Bottom = Sel.Top; }
      }else if(st == "SelRight") {
        Sel.Right = v - 1 + g->DataLeft;
        if(Sel.Left > Sel.Right) { Sel.Left = Sel.Right; }
      }else if(st == "SelBottom") {
        Sel.Bottom = v - 1 + g->DataTop;
        if(Sel.Top > Sel.Bottom) { Sel.Top = Sel.Bottom; }
      }
      g->Selection = Sel;
    }
  }else if(Type == etCell){
	Write(X, Y, GetVar(st).Str(), macro->IsCellMacro);
  }
}
//---------------------------------------------------------------------------
void Element::Pntr(Element e)
{
  GetVar(st) = e;
}
//---------------------------------------------------------------------------
Element Element::Points()
{
  if(Type != etVar) return *this;
  return GetVar(st);
}
//---------------------------------------------------------------------------
void Clear(TList *L)
{
  for(int i=L->Count-1; i>=0; i--){
    if(L->Items[i]) delete L->Items[i];
  }
  L->Clear();
}
//---------------------------------------------------------------------------
TMacro::TMacro(EncodedWriter *io, int ml, TStringList *md, bool cm)
{
  fs_io = io;
  if(io){
	canReadFile = false;
	canWriteFile = true;
  }else{
	canReadFile = false;
	canWriteFile = false;
  }
  MaxLoop = ml;
  modules = md;
  IsCellMacro = cm;

}
//---------------------------------------------------------------------------
String TMacro::ReadString(TStream *fs)
{
  char str[260];
  unsigned char c;
  if(fs->Read(&c, 1) > 0){
    if(c == 0) return "";
    fs->Read(str, c);
    str[c] = '\0';
  }else{
    return "";
  }

  if(str[0] != '\0'){
    return (AnsiString)str;
  }else{
    char *p = str+1;
    AnsiString st = (AnsiString)"#" + p;
    st[1] = '\0';
    return st;
  }
}
//---------------------------------------------------------------------------
TMenuItem *MenuSearch(TMenuItem *m, String s)
{
  for(int i=0; i<m->Count; i++){
    if(m->Items[i]->Name == (String)"mn" + s) return (m->Items[i]);
    else if(m->Items[i]->Count > 0){
      TMenuItem *sub = MenuSearch(m->Items[i], s);
      if(sub) return sub;
    }
  }
  return NULL;
}
//---------------------------------------------------------------------------
String NormalizeNewLine(String Val)
{
  int len = Val.Length();
  for(int i=len; i > 0; i--){
    if(Val[i] == '\r'){
      if(i == len || Val[i+1] != '\n'){
        Val[i] = '\n';
      }else{
        Val.Delete(i, 1);
      }
    }
  }
  return Val;
}
//---------------------------------------------------------------------------
#define STR0 ope[0]->Str()
#define VAL0 ope[0]->Val()
#define STR1 ope[1]->Str()
#define VAL1 ope[1]->Val()
#define STR2 ope[2]->Str()
#define VAL2 ope[2]->Val()
#define STR3 ope[3]->Str()
#define VAL3 ope[3]->Val()
#define VAL4 ope[4]->Val()
#define VAL5 ope[5]->Val()
#define VAL6 ope[6]->Val()
#define VAL7 ope[7]->Val()
#define VAL8 ope[8]->Val()
//---------------------------------------------------------------------------
void TMacro::ExecFnc(String s){
  int H = s[1];
  s.Delete(1,1);

  Element **ope = new Element*[H];
  for(int i=H-1; i>=0; i--){
    ope[i] = (Element *)(Stack->Last()); Stack->Delete(Stack->Count-1);
  }

  try{

    if(s[1] == '$'){
      s.Delete(1,1);
      TList *ArgStack = new TList();
      for(int i=0; i<H; i++){
        if(ope[i]->isNum()){
          ArgStack->Add(new Element(ope[i]->Val(), NULL));
        }else{
          ArgStack->Add(new Element(ope[i]->Str(), NULL));
        }
      }
      int ml = ((MaxLoop > 0) ? MaxLoop-LoopCount : 0);
      TMacro mcr(fs_io, ml, modules, IsCellMacro);
      Element *r = mcr.Do(s + "$" + (String)H, ArgStack);
      if(r){
        Stack->Add(r);
      }else{
        Clear(Stack);
      }
    }else if(s == "func="){
      int count = H / 2;
      for(int i=0; i<count; i++){
        ope[i+count]->Sbst(*(ope[i]));
      }
    }else if(s == "swap" && H == 2){
      Element t0 = ((ope[0]->isNum() && ope[0]->Type != etCell)
        ? Element(VAL0, this) : Element(STR0, this));
      Element t1 = ((ope[1]->isNum() && ope[1]->Type != etCell)
        ? Element(VAL1, this) : Element(STR1, this));
      ope[0]->Sbst(t1);
      ope[1]->Sbst(t0);
    }else if(s == "MessageBox"){
      const wchar_t *Text = (H >= 1) ? STR0.c_str() : TEXT("ブレークポイントです");
      int Flag = (H >= 2) ? (ope[H-1]->Val()) : MB_OK;
      const wchar_t *Caption = (H >= 3) ? STR1.c_str() : TEXT("Cassava Macro");
      Stack->Add(new Element(::MessageBox(NULL, Text, Caption, Flag), this));
    }else if(s == "InputBox"){
      String Text = (H >= 1) ? STR0 : (String)"";
      String Def = (H >= 2) ? (ope[H-1]->Str()) : (String)"";
      String Caption = (H >= 3) ? STR1 : (String)"Cassava Macro";
      Stack->Add(new Element(InputBox(Caption, Text, Def), this));
    }else if(s == "InputBoxMultiLine"){
      String Text = (H >= 1) ? STR0 : (String)"";
      String Def = (H >= 2) ? (ope[H-1]->Str()) : (String)"";
      String Caption = (H >= 3) ? STR1 : (String)"Cassava Macro";
      String Cancel = (H >= 4) ? STR2 : Def;
      String Result = InputBoxMultiLine(Caption, Text, Cancel, Def);
      Stack->Add(new Element(NormalizeNewLine(Result), this));
    }else if(s == "GetRowHeight" && H == 0){
      Stack->Add(new Element(fmMain->MainGrid->DefaultRowHeight, this));
    }else if(s == "GetRowHeight" && H == 1){
      Stack->Add(new Element(fmMain->MainGrid->RowHeights[VAL0], this));
    }else if(s == "SetRowHeight" && H == 1){
      fmMain->MainGrid->DefaultRowHeight = VAL0;
    }else if(s == "SetRowHeight" && H == 2){
      fmMain->MainGrid->RowHeights[VAL0] = VAL1;
    }else if(s == "AdjustRowHeight" && H == 1){
      fmMain->MainGrid->SetHeight(VAL0);
    }else if(s == "GetColWidth" && H == 0){
      Stack->Add(new Element(fmMain->MainGrid->DefaultColWidth, this));
    }else if(s == "GetColWidth" && H == 1){
      Stack->Add(new Element(fmMain->MainGrid->ColWidths[VAL0], this));
    }else if(s == "SetColWidth" && H == 1){
      fmMain->MainGrid->DefaultColWidth = VAL0;
    }else if(s == "SetColWidth" && H == 2){
      fmMain->MainGrid->ColWidths[VAL0] = VAL1;
    }else if(s == "AdjustColWidth" && H == 1){
      fmMain->MainGrid->SetWidth(VAL0);
    }else if(s == "GetYear" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack->Add(new Element((int)year, this));
    }else if(s == "GetMonth" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack->Add(new Element((int)month, this));
    }else if(s == "GetDate" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack->Add(new Element((int)day, this));
    }else if(s == "GetHours" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack->Add(new Element((int)hours, this));
    }else if(s == "GetMinutes" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack->Add(new Element((int)minutes, this));
    }else if(s == "GetSeconds" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack->Add(new Element((int)seconds, this));
    }else if(s == "GetFilePath" && H == 0){
      String Path = ExtractFilePath(fmMain->FileName);
      if(Path != "" && (*(Path.LastChar()) != '\\')){ Path += '\\'; }
      Stack->Add(new Element(Path, this));
    }else if(s == "GetFileName" && H == 0){
      Stack->Add(new Element(ExtractFileName(fmMain->FileName), this));
    }else if(s == "GetStatusBarCount" && H == 0){
      Stack->Add(new Element(fmMain->StatusBar->Panels->Count - 1, this));
    }else if(s == "SetStatusBarCount" && H == 1){
      int val = VAL0 + 1;
      if(val < 1){ val = 1; }
      TStatusPanels *panels = fmMain->StatusBar->Panels;
      while(panels->Count > val){
        panels->Delete(panels->Count - 1);
      }
      while(panels->Count < val){
        panels->Add();
      }
    }else if(s == "GetStatusBarText" && H == 1){
      int val = VAL0;
      String data = "";
      if(val >= 0 && val < fmMain->StatusBar->Panels->Count){
        data = fmMain->StatusBar->Panels->Items[val]->Text;
      }
      Stack->Add(new Element(data, this));
    }else if(s == "SetStatusBarText" && H == 2){
      int val = VAL0;
      if(val >= 0 && val < fmMain->StatusBar->Panels->Count){
        fmMain->StatusBar->Panels->Items[val]->Text = STR1;
      }
    }else if(s == "GetStatusBarWidth" && H == 1){
      int val = VAL0;
      int data = 0;
      if(val >= 0 && val < fmMain->StatusBar->Panels->Count){
        data = fmMain->StatusBar->Panels->Items[val]->Width;
      }
      Stack->Add(new Element(data, this));
    }else if(s == "SetStatusBarWidth" && H == 2){
      int val = VAL0;
      if(val >= 0 && val < fmMain->StatusBar->Panels->Count){
        fmMain->StatusBar->Panels->Items[val]->Width = VAL1;
      }
    }else if(s == "LoadIniSetting" && H == 0){
      fmMain->ReadIni();
    }else if(s == "SaveIniSetting" && H == 0){
      fmMain->WriteIni(false);
    }else if(s == "GetIniSetting" && H ==  2){
      TIniFile *Ini = fmMain->Pref->GetInifile();
      Stack->Add(new Element(Ini->ReadString(STR0, STR1, ""), this));
      delete Ini;
    }else if(s == "SetIniSetting" && H == 3){
      TIniFile *Ini = fmMain->Pref->GetInifile();
      Ini->WriteString(STR0, STR1, STR2);
      delete Ini;
    }else if(s == "GetFontName" && H == 0){
      Stack->Add(new Element(fmMain->MainGrid->Font->Name, this));
    }else if(s == "SetFontName" && H == 1){
      fmMain->MainGrid->Font->Name = STR0;
    }else if(s == "GetFontSize" && H == 0){
      Stack->Add(new Element(fmMain->MainGrid->Font->Size, this));
    }else if(s == "SetFontSize" && H == 1){
      fmMain->MainGrid->Font->Size = VAL0;
    }else if(s == "write" || s == "writeln"){
      if(canWriteFile){
        if(fs_io){
          if(fmMain->mnSjis->Checked){
            fs_io->SetEncode(CHARCODE_SJIS);
          }else if(fmMain->mnEuc->Checked){
            fs_io->SetEncode(CHARCODE_EUC);
          }else if(fmMain->mnJis->Checked){
            fs_io->SetEncode(CHARCODE_JIS);
          }else if(fmMain->mnUtf8->Checked){
            fs_io->SetEncode(CHARCODE_UTF8);
          }else if(fmMain->mnUnicode->Checked){
            fs_io->SetEncode(CHARCODE_UTF16LE);
          }else if(fmMain->mnUtf16be->Checked){
            fs_io->SetEncode(CHARCODE_UTF16BE);
          }
        }
        for(int i=0; i<H; i++){
          fs_io->Write(ope[i]->Str());
        }
        if(s == "writeln"){
          if(fmMain->mnLf->Checked){
            fs_io->Write(TEXT("\x0A"));
          }else if(fmMain->mnCr->Checked){
            fs_io->Write(TEXT("\x0D"));
          }else{
            fs_io->Write(TEXT("\x0D\x0A"));
          }
        }
      }else{
        int wx = MGCol, wy = MGRow;
        for(int i=0; i<H; i++){
          Write(wx, wy, ope[i]->Str(), IsCellMacro);
          wx++;
        }
        if(s == "writeln"){ CsvGridGoTo(fmMain->MainGrid, 1, wy+1); }
        else{ CsvGridGoTo(fmMain->MainGrid, wx, wy); }
      }
    }else if(H == 0){
      TMenuItem *Menu = MenuSearch(fmMain->Menu->Items, s);
      if(Menu && Menu->OnClick){
        Menu->OnClick(Menu);
      }else{
        throw MacroException("定義されていない関数です:" + s + "/0");
      }
    }else if(s == "int" && H == 1){
      Stack->Add(new Element((int)VAL0, this));
    }else if(s == "double" && H == 1){
      Stack->Add(new Element(VAL0, this));
    }else if(s == "str" && H == 1){
      Stack->Add(new Element(STR0, this));
    }else if(s == "max"){
      double maxval = ope[0]->Val();
      for(int i=1; i<H; i++){
        maxval = max(maxval, ope[i]->Val());
      }
      Stack->Add(new Element(maxval, this));
    }else if(s == "min"){
      double minval = ope[0]->Val();
      for(int i=1; i<H; i++){
        minval = min(minval, ope[i]->Val());
      }
      Stack->Add(new Element(minval, this));
    }else if(s == "sqrt" && H == 1){
      Stack->Add(new Element(sqrt(VAL0), this));
    }else if(s == "sin" && H == 1){
      Stack->Add(new Element(sin(VAL0), this));
    }else if(s == "cos" && H == 1){
      Stack->Add(new Element(cos(VAL0), this));
    }else if(s == "tan" && H == 1){
      Stack->Add(new Element(tan(VAL0), this));
    }else if(s == "pow" && H == 2){
      Stack->Add(new Element(pow(VAL0, VAL1), this));
    }else if(s == "len" && H == 1){
      Stack->Add(new Element(STR0.Length(), this));
    }else if(s == "lenB" && H == 1){
      Stack->Add(new Element(((AnsiString)STR0).Length(), this));
    }else if(s == "InsertRow" && H == 1){
      fmMain->MainGrid->InsertRow(VAL0 + fmMain->MainGrid->DataTop - 1);
    }else if(s == "DeleteRow" && H == 1){
      fmMain->MainGrid->DeleteRow(VAL0 + fmMain->MainGrid->DataTop - 1);
    }else if(s == "InsertCol" && H == 1){
      fmMain->MainGrid->InsertColumn(VAL0 + fmMain->MainGrid->DataLeft - 1);
    }else if(s == "DeleteCol" && H == 1){
      fmMain->MainGrid->DeleteColumn(VAL0 + fmMain->MainGrid->DataLeft - 1);
    }else if(s == "random" && H == 1){
      Stack->Add(new Element(random(VAL0), this));
    }else if(s == "cell" && H == 2){
      Stack->Add(new Element(VAL0, VAL1, this));
    }else if(s == "left" && H == 2){
      Stack->Add(new Element(STR0.SubString(1, VAL1), this));
    }else if(s == "right" && H == 2){
      String st = STR0;
      int v1 = VAL1;
      Stack->Add(new Element(st.SubString(st.Length()-v1+1, v1), this));
    }else if(s == "pos" && H == 2){
      Stack->Add(new Element(STR0.Pos(STR1), this));
    }else if(s == "posB" && H == 2){
      Stack->Add(new Element(((AnsiString)STR0).AnsiPos(STR1), this));
    }else if(s == "asc" && H == 1){
      AnsiString str = STR0;
      if(str == ""){
        Stack->Add(new Element(0, this));
      }else if(str.IsLeadByte(1)){
        Stack->Add(new Element((unsigned char)(str[1]) * 256
          + (unsigned char)(str[2]), this));
      }else{
        Stack->Add(new Element((unsigned char)(str[1]), this));
      }
    }else if(s == "ascW" && H == 1){
      String str = STR0;
      if(str == ""){
        Stack->Add(new Element(0, this));
      }else{
        Stack->Add(new Element(str[1], this));
      }
    }else if(s == "chr" && H == 1){
      int val = VAL0;
      char buf[3];
      if(val < 256){
        buf[0] = val;
        buf[1] = '\0';
      }else{
        buf[0] = val / 256;
        buf[1] = val % 256;
        buf[2] = '\0';
      }
      Stack->Add(new Element((AnsiString)buf, this));
    }else if(s == "chrW" && H == 1){
      wchar_t buf[2];
      buf[0] = VAL0;
      buf[1] = '\0';
      Stack->Add(new Element((String)buf, this));
    }else if((s == "move" || s == "moveto") && H == 2){
      int c = VAL0;
      int r = VAL1;
      if(s == "move"){ c += MGCol; r += MGRow; }
      CsvGridGoTo(fmMain->MainGrid, c, r);
    }else if(s == "mid" && H == 2){
      String st = STR0;
      Stack->Add(new Element(st.SubString(VAL1, st.Length()-VAL1+1), this));
    }else if(s == "mid" && H == 3){
      Stack->Add(new Element(((String)STR0).SubString(VAL1, VAL2), this));
    }else if(s == "mid=" && H == 4){
      String st = STR0;
      int r = VAL1 + VAL2;
      st = st.SubString(1, VAL1-1) + STR3 + st.SubString(r, st.Length() - r + 1);
      ope[0]->Sbst(Element(st, this));
    }else if(s == "replace" && H == 3){
      String fr = STR0;
      String to = "";
      int hitlen = STR1.Length();
      int p;
      while((p = fr.Pos(STR1)) > 0){
        to += fr.SubString(1, p-1) + STR2;
        fr.Delete(1, hitlen + p - 1);
      }
      to += fr;
      Stack->Add(new Element(to, this));
    }else if(s == "sum" && H == 4){
      int l = VAL0;
      int t = VAL1;
      int r = VAL2;
      int b = VAL3;
      double result = fmMain->MainGrid->GetSum(l,t,r,b);
      Stack->Add(new Element(result, this));
    }else if(s == "avr" && H == 4){
      int l = VAL0;
      int t = VAL1;
      int r = VAL2;
      int b = VAL3;
      double result = fmMain->MainGrid->GetAvr(l,t,r,b);
      Stack->Add(new Element(result, this));
    }else if(s == "ShellOpen" && H == 1){
      String FileName = STR0;
      if(isUrl(FileName)){
        fmMain->MainGrid->OpenURL(FileName);
      }else{
        AutoOpen(FileName, ExtractFilePath(fmMain->FileName));
      }
    }else if(s == "ShellOpen" && H > 1){
      wchar_t **argv = new wchar_t*[H+1];
      for(int i=0; i<H; i++){
        argv[i] = ope[i]->Str().c_str();
      }
      argv[H] = NULL;
      int result = _wspawnvp(P_NOWAITO, argv[0], argv);
      delete[] argv;

      if(result == -1){
        if(errno == ENOENT){
          throw MacroException(ope[0]->Str() + "\nファイルが見つかりません。");
        }else{
          throw MacroException(ope[0]->Str() + "\n実行に失敗しました。");
        }
      }
    }else if(s == "Open" && H == 1){
      String filename = STR0;
      fmMain->MainGridDropFiles(NULL, 1, &filename);
      while (fmMain->MainGrid->FileOpenThread) {
        Sleep(100);
        Application->ProcessMessages();
      }
    }else if(s == "SaveAs" && (H == 1 || H == 2)){
      TTypeOption *format = NULL;
      if(H == 1){
        format = fmMain->MainGrid->TypeOption;
      }else{
        int count = fmMain->MainGrid->TypeList.Count;
        for(int i=0; i<count; i++){
          TTypeOption *p = fmMain->MainGrid->TypeList.Items(i);
          if(p->Name == STR1){
            format = p;
            break;
          }
        }
        if(!format) {
          throw MacroException("保存形式が不明です:" + STR1);
        }
      }
      fmMain->UpdateKCode();
      fmMain->MainGrid->SaveToFile(STR0, format);
    }else if(s == "Export" && H == 2){
      fmMain->UpdateKCode();
      fmMain->Export(STR0, STR1);
    }else if(s == "Sort" && H >= 1 && H <= 9) {
      int left = VAL0;
      int top = (H > 1 ? VAL1 : 1);
      int right = (H > 2 ? VAL2 : fmMain->MainGrid->DataRight);
      int bottom = (H > 3 ? VAL3 : fmMain->MainGrid->DataBottom);
      int col = (H > 4 ? VAL4 : left);
      bool direction = (H > 5 ? VAL5 : false);
      bool numSort = (H > 6 ? VAL6 : false);
      bool ignoreCase = (H > 7 ? VAL7 : false);
      bool ignoreZenhan = (H > 8 ? VAL8 : false);
      fmMain->MainGrid->Sort(AXtoRX(left), AYtoRY(top), AXtoRX(right),
          AYtoRY(bottom), AXtoRX(col), !direction, numSort, ignoreCase,
          ignoreZenhan);
    }else{
      throw MacroException("定義されていない関数です:" + s + "/" + H);
    }

  }catch(MacroException e){
    for(int i=0; i<H; i++){
      delete ope[i];
    }
    delete[] ope;
    throw e;
  }

  for(int i=0; i<H; i++){
    delete ope[i];
  }
  delete[] ope;
}
//---------------------------------------------------------------------------
void TMacro::ExecOpe(char c){
  if(c == 'P' || c == 'm' || c == 'g' ||
     c == CMO_Inc || c == CMO_Dec || c == '!'){
    if(Stack->Count < 1){ throw MacroException(c, ME_HIKISU); }
    Element *ope = (Element *)(Stack->Last()); Stack->Delete(Stack->Count-1);
    if(c == 'g') {
      fs->Position = ope->Val();
      Clear(Stack);
      LoopCount++;
      Application->ProcessMessages();
      if(MaxLoop > 0 && LoopCount > MaxLoop){
        throw MacroException((String)"ループ回数が" + MaxLoop +
              "に達しました。処理を中断します。");
      }
      if(!RunningOk){ throw MacroException("中断しました"); }
    }
    else if(c == 'P') Stack->Add(new Element(ope->Points()));
	else if(c == 'm') Stack->Add(new Element(-(ope->Val()), this));
	else if(c == CMO_Inc) { ope->Sbst(Element(ope->Val() + 1, this)); }
	else if(c == CMO_Dec) { ope->Sbst(Element(ope->Val() - 1, this)); }
    else if(c == '!') Stack->Add(new Element(((ope->Val() == 0) ? 1 : 0), this));
    delete ope;
  }else{
    if(Stack->Count < 2){ throw MacroException(c, ME_HIKISU); }
    Element *ope2 = (Element *)(Stack->Last()); Stack->Delete(Stack->Count-1);
    Element *ope1 = (Element *)(Stack->Last()); Stack->Delete(Stack->Count-1);

    if(c == '=' || c == 'p'){
      if(ope1->Type == etVar){
        if(c == '=') ope1->Sbst(*ope2);
        if(c == 'p') ope1->Pntr(*ope2);
      }else if(ope1->Type == etSystem){
        ope1->Sbst(*ope2);
      }else if(ope1->Type == etCell){
        Write(ope1->X, ope1->Y, ope2->Str(), IsCellMacro);
      }else{
        delete ope1; delete ope2;
        throw MacroException("「=」の左が左辺値ではありません");
      }
    }else if(c == '+'){
      if(ope1->isNum() && ope2->isNum()){
		Stack->Add(new Element(ope1->Val() + ope2->Val(), this));
      }else{
		Stack->Add(new Element(ope1->Str() + ope2->Str(), this));
      }
    }else if(c == '-'){
	  Stack->Add(new Element(ope1->Val() - ope2->Val(), this));
    }else if(c == '*'){
	  Stack->Add(new Element(ope1->Val() * ope2->Val(), this));
    }else if(c == '/'){
	  Stack->Add(new Element(ope1->Val() / ope2->Val(), this));
    }else if(c == '%'){
	  Stack->Add(new Element((int)(ope1->Val()) % (int)(ope2->Val()), this));
    }else if(c == CMO_Eq){
	  Stack->Add(new Element(((ope1->Str() == ope2->Str()) ? 1 : 0), this));
    }else if(c == CMO_NEq){
	  Stack->Add(new Element(((ope1->Str() != ope2->Str()) ? 1 : 0), this));
    }else if(c == '<'){
	  Stack->Add(new Element(((ope1->Val() < ope2->Val()) ? 1 : 0), this));
    }else if(c == '>'){
	  Stack->Add(new Element(((ope1->Val() > ope2->Val()) ? 1 : 0), this));
    }else if(c == CMO_LEq){
	  Stack->Add(new Element(((ope1->Val() <= ope2->Val()) ? 1 : 0), this));
    }else if(c == CMO_GEq){
	  Stack->Add(new Element(((ope1->Val() >= ope2->Val()) ? 1 : 0), this));
    }else if(c == '&'){
      Stack->Add(new Element((
		((ope1->Val() != 0) && (ope2->Val() != 0)) ? 1 : 0), this));
    }else if(c == '|'){
      Stack->Add(new Element((
		((ope1->Val() != 0) || (ope2->Val() != 0)) ? 1 : 0), this));
    }else if(c == '?'){
      if(ope1->Val() == 0){
        fs->Position = ope2->Val();
      }
      Clear(Stack);
    }else if(c == ']'){
      Stack->Add(new Element(ope1->Val(), ope2->Val(), this));
    }

    delete ope1;
    delete ope2;
  }
}
//---------------------------------------------------------------------------
TStream *TMacro::GetStreamFor(String FileName){
  TStream *result;
  try{
    int index = modules->IndexOf(FileName);
    if (index < 0) {
      throw MacroException("ユーザー関数が見つかりません:");
    }
    TObject *obj = modules->Objects[index];
    result = static_cast<TStream *>(obj);
  }catch(...){
    String FuncName = FileName;
    int pos;
    pos = FuncName.LastDelimiter("$");
    if(pos > 0){
      FuncName[pos] = '/';
    }
    pos = FuncName.LastDelimiter("$");
    if(pos > 0){
      FuncName[pos] = '.';
    }
    throw MacroException("ユーザー関数が見つかりません:" + FuncName);
  }
  return result;
}
//---------------------------------------------------------------------------
Element *TMacro::Do(String FileName, TList *AStack, int x, int y){
  Element *ReturnValue = NULL;
  // 再帰呼び出し対応のため、メソッド終了時にStreamのPositionを元に戻す
  long oldpc = 0L;
  if(AStack){
    Stack = AStack;
  }else{
    Stack = new TList();
  }
  fs = NULL;
  try{
    fs = GetStreamFor(FileName);
    oldpc = fs->Position;
    fs->Position = 0;

    Var.clear();
    Var["x"] = Element(((x >= 0) ? x : MGCol), this);
    Var["y"] = Element(((y >= 0) ? y : MGRow), this);
    Var["Left"] = Element(fmMain->MainGrid->ShowRowCounter ?
        1 : fmMain->MainGrid->FixedCols + 1, this);
    Var["Top"] = Element(fmMain->MainGrid->ShowColCounter ?
        1 : fmMain->MainGrid->FixedRows + 1, this);
    char Type;
    LoopCount = 0;
    while(fs->Read(&Type, 1) > 0) {
      if(Type == '$'){
        Stack->Add(new Element(ReadString(fs), this));
      }else if(Type == '*'){
        ExecFnc(ReadString(fs));
      }else if(Type == '~'){
        Stack->Add(new Element(ReadString(fs), etVar, this));
      }else if(Type == '!'){
        Stack->Add(new Element(ReadString(fs), etSystem, true, this));
      }else if(Type == '-'){
        char c;
        fs->Read(&c, 1);
        if(c == '}'){
          Element *L = (Element *)(Stack->Last());
          if(L->isNum()){
            ReturnValue = new Element(L->Val(), NULL);
          }else{
            ReturnValue = new Element(L->Str(), NULL);
          }
          break;
        }else{
          ExecOpe(c);
        }
      }else if(Type == 'i'){
        int i;
        fs->Read(&i, sizeof(int));
        Stack->Add(new Element((double)i, this));
      }else if(Type == 'd' || Type == 'I'){
        double d;
        fs->Read(&d, sizeof(double));
        Stack->Add(new Element(d, this));
      }
    }

  }catch(MacroException e){
    if(IsCellMacro){
      if(fs){
        fs->Position = oldpc;
      }
      Clear(Stack); delete Stack;
      throw e;
    }else{
      if(e.Type == ME_HIKISU){
        e.Message = "引数の数が足りません:" + e.Message;
      }
      ::MessageBox(NULL, e.Message.c_str(), TEXT("Cassava Macro Interpreter"), 0);
    }
  }catch(Exception *e){
    if(IsCellMacro){
      if(fs){
        fs->Position = oldpc;
      }
      Clear(Stack); delete Stack;
      throw e;
    }else{
      Application->MessageBox(e->Message.c_str(),
                              TEXT("Cassava Macro Interpreter"), 0);
    }
  }

  if(fs){
    fs->Position = oldpc;
  }
  Clear(Stack); delete Stack;
  return ReturnValue;
}
//---------------------------------------------------------------------------
String ExecMacro(String FileName, int MaxLoop, TStringList *Modules,
                 int x, int y, EncodedWriter *IO, bool IsCellMacro)
{
  if(!RunningOk){ return ""; }
  if(!IsCellMacro){ RunningCount++; }
  randomize();
  TMacro mcr(IO, MaxLoop, Modules, IsCellMacro);
  Element *r = mcr.Do(FileName, NULL, x, y);
  if(!IsCellMacro){
    RunningCount--;
    if(RunningCount == 0){
      RunningOk = true;
    }
  }

  String ReturnValue = "";
  if(r){
    ReturnValue = r->Str();
    delete r;
  }
  return ReturnValue;
}
//---------------------------------------------------------------------------
void StopAllMacros()
{
  if(RunningCount > 0){
    RunningOk = false;
  }
}
//---------------------------------------------------------------------------
int GetRunningMacroCount()
{
  return RunningCount;
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

