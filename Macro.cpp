//---------------------------------------------------------------------------
#ifdef CssvMacro
//---------------------------------------------------------------------------
#include <cmath>
#include <dialogs.hpp>
#include <map>
#include <process.h>
#include <vector>
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
#define etObject 5
#define etFunction 6
//---------------------------------------------------------------------------
#define ME_HIKISU 1
#define ME_SECURITY 2
#define ME_CANCELED 3
//---------------------------------------------------------------------------
#define MGCol (fmMain->MainGrid->Col - fmMain->MainGrid->DataLeft + 1)
#define MGRow (fmMain->MainGrid->Row - fmMain->MainGrid->DataTop  + 1)
#define RXtoAX(x)               ((x) - fmMain->MainGrid->DataLeft + 1)
#define RYtoAY(y)               ((y) - fmMain->MainGrid->DataTop  + 1)
#define AXtoRX(x)               ((x) + fmMain->MainGrid->DataLeft - 1)
#define AYtoRY(y)               ((y) + fmMain->MainGrid->DataTop  - 1)
//---------------------------------------------------------------------------
class TEnvironment;
//---------------------------------------------------------------------------
class Element {
private:
  String st;
  double vl;
  bool Num;
  TEnvironment *env;
  Element &GetVar() const;
public:
  int Type;
  bool isNum() const;
  int X, Y;
  Element() : Type(etErr), Num(true), vl(0), st(""), env(NULL) {};
  Element(double d) :
      Type(etAtom), Num(true), vl(d), env(NULL) {};
  Element(double d, String s, int t, TEnvironment *e) :
      Type(t), Num(true), st(s), vl(d), env(e) {};
  Element(String s) :
      Type(etAtom), Num(false), st(s), env(NULL) {};
  Element(String s, int t, TEnvironment *e) :
      Type(t), Num(false), st(s), env(e) {};
  Element(String s, int t, bool nm, TEnvironment *e) :
      Type(t), Num(nm), st(s), env(e) {};
  Element(int cl, int rw, TEnvironment *e) :
      Type(etCell), X(cl), Y(rw), env(e),
      Num(fmMain->MainGrid->IsNumberAtACell(cl, rw)) {};
  Element(int cl, int rw, bool nm, TEnvironment *e) :
      Type(etCell), Num(nm), X(cl), Y(rw), env(e) {};
  Element(const Element &e) :
      st(e.st), vl(e.vl), Type(e.Type), Num(e.Num), X(e.X), Y(e.Y),
      env(e.env) {};
  double Val() const;
  String Str() const;
  String Name() const { return st; }
  Element Value() const;
  void Sbst(const Element &e);
};
//---------------------------------------------------------------------------
class TEnvironment {
private:
  TEnvironment *global;
  std::vector<TEnvironment*> *objects;
public:
  std::map<String, Element> Vars;
  bool IsCellMacro;

  TEnvironment(bool cm, TEnvironment *gl) :
      Vars(), IsCellMacro(cm), global(gl), objects(NULL) {}
  ~TEnvironment() {
    if (objects) {
      for (size_t i = 0; i < objects->size(); i++) {
        delete objects->at(i);
      }
      delete objects;
    }
  }
  TEnvironment *GetGlobal() { return global ? global : this; }
  TEnvironment CreateSubEnvironment() {
    return TEnvironment(IsCellMacro, GetGlobal());
  }
  std::vector<TEnvironment*> *GetObjects() {
    TEnvironment *gl = GetGlobal();
    if (!gl->objects) {
      gl->objects = new std::vector<TEnvironment*>();
    }
    return gl->objects;
  }
  TEnvironment *GetObject(int id) {
    return GetObjects()->at(id);
  }
};
//---------------------------------------------------------------------------
class TMacro{
private:
  TStream *fs;
  String ReadString(TStream *fs);
  TStream *GetStreamFor(String FileName);
  void ExecOpe(char c);
  void ExecMethod(String name, int H, const std::vector<Element>& ope,
                  bool isLambda);
  void ExecFnc(String s);
  std::vector<Element> Stack;
  int LoopCount;
  String FileName;
  bool canWriteFile;
  EncodedWriter *fs_io;
  TStringList *modules;
  TEnvironment env;
public:
  int MaxLoop;
  Element Do(String FileName, const std::vector<Element> &AStack,
             int x = -1, int y = -1, Element *thisPtr = NULL);

  TMacro(EncodedWriter *io, int ml, TStringList *md, TEnvironment e) :
      fs_io(io), canWriteFile(io), MaxLoop(ml), modules(md), env(e) {}
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
  g->SetSelection(x, x, y, y);
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
Element &Element::GetVar() const
{
  return env->Vars[st];
}
//---------------------------------------------------------------------------
double Element::Val() const
{
  if (Type == etErr && st != "") {
    throw MacroException(st);
  } else if (Type == etCell) {
    return ToDouble(fmMain->MainGrid->ACells[X][Y], 0);
  } else if(Type == etVar) {
    Element &e = GetVar();
    if (e.Type == etErr) {
      if (e.st != "") { throw MacroException(e.st); }
      throw MacroException("�l���������Ă��Ȃ��ϐ��E�t�B�[���h�ł��F" + st);
    }
    return e.Val();
  } else if (Type == etSystem) {
    if     (st == "Col")       { return MGCol; }
    else if(st == "Row")       { return MGRow; }
    else if(st == "Right")     { return RXtoAX(fmMain->MainGrid->DataRight); }
    else if(st == "Bottom")    { return RYtoAY(fmMain->MainGrid->DataBottom); }
    else if(st == "SelLeft")   { return RXtoAX(fmMain->MainGrid->SelLeft); }
    else if(st == "SelTop")    { return RYtoAY(fmMain->MainGrid->SelTop); }
    else if(st == "SelRight")  { return RXtoAX(fmMain->MainGrid->Selection.Right); }
    else if(st == "SelBottom") { return RYtoAY(fmMain->MainGrid->Selection.Bottom); }
  } else if (Num) {
    return vl;
  }
  return ToDouble(st, 0);
}
//---------------------------------------------------------------------------
String Element::Str() const
{
  if (Type == etErr && st != "") {
    throw MacroException(st);
  } else if (Type == etCell){
    return fmMain->MainGrid->ACells[X][Y];
  } else if (Type == etVar) {
    Element &e = GetVar();
    if (e.Type == etErr) {
      if (e.st != "") { throw MacroException(e.st); }
      throw MacroException("�l���������Ă��Ȃ��ϐ��E�t�B�[���h�ł��F" + st);
    }
    return e.Str();
  } else if (Type == etSystem) {
    return String(Val());
  } else if (Type == etObject) {
    String s = "{";
    std::map<String, Element>& vars = env->GetObject(vl)->Vars;
    for (std::map<String, Element>::iterator it = vars.begin();
         it != vars.end(); it++) {
      if (it != vars.begin()) {
        s += ", ";
      }
      s += it->first + ": ";
      if (it->second.Type == etObject) {
        s += "{...}";
      } else if (it->second.Num) {
        s += it->second.Str();
      } else if (it->second.Str().Pos("\n") > 0) {
        s += "...";
      } else {
        s += "\"" + it->second.Str() + "\"";
      }
    }
    return s + "}";
  } else if (st != "") {
    return st;
  } else if (Num) {
    return String(vl);
  }
  return st;
}
//---------------------------------------------------------------------------
Element Element::Value() const
{
  if (Type == etVar) {
    Element& e = GetVar();
    if (e.Type == etErr) {
      if (e.st != "") { throw MacroException(e.st); }
      throw MacroException("�l���������Ă��Ȃ��ϐ��E�t�B�[���h�ł��F" + st);
    }
    return e;
  } else if (Type == etCell) {
    if (Num) {
      return Element(Val(), Str(), etAtom, NULL);
    }
    return Element(Str());
  } else if (Type == etSystem) {
    return Element(Val());
  }
  return *this;
}
//---------------------------------------------------------------------------
bool Element::isNum() const
{
  if(Type == etVar){
    return GetVar().isNum();
  }else if(Type == etCell){
    return fmMain->MainGrid->IsNumberAtACell(X,Y);
  }
  return Num;
}
//---------------------------------------------------------------------------
void Element::Sbst(const Element &e)
{
  if (Type == etVar) {
    GetVar() = e.Value();
  } else if (Type == etSystem) {
    int v = e.Value().Val();
    TMainGrid *g = fmMain->MainGrid;
    if (st == "Col") {
      CsvGridGoTo(g, v, MGRow);
    } else if(st == "Row") {
      CsvGridGoTo(g, MGCol, v);
    } else if(st == "Right") {
      if (MGCol > v) { CsvGridGoTo(g, v, MGRow); }
      int dataRight = v + g->DataLeft - 1;
      g->ChangeColCount(dataRight + 1);
      g->SetDataRightBottom(dataRight, g->DataBottom, true);
    } else if(st == "Bottom") {
      if (MGRow > v) { CsvGridGoTo(g, MGCol, v); }
      int dataBottom = v + g->DataTop - 1;
      g->ChangeRowCount(dataBottom + 1);
      g->SetDataRightBottom(g->DataRight, dataBottom, true);
    } else {
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
      g->SetSelection(Sel.Left, Sel.Right, Sel.Top, Sel.Bottom);
    }
  } else if (Type == etCell) {
    Write(X, Y, e.Value().Str(), env->IsCellMacro);
  } else {
    throw MacroException("����悪���Ӓl�ł͂���܂���F" + Str());
  }
}
//---------------------------------------------------------------------------
String TMacro::ReadString(TStream *fs)
{
  wchar_t str[260];
  unsigned char c;
  if(fs->Read(&c, 1) > 0){
    if(c == 0) return "";
    fs->Read(str, c * sizeof(wchar_t));
  }else{
    return "";
  }
  return String(str, c);
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
  TReplaceFlags replaceAll = TReplaceFlags() << rfReplaceAll;
  return StringReplace(Val, "\r\n", "\n", replaceAll);
}
//---------------------------------------------------------------------------
#define STR0 ope[0].Str()
#define VAL0 ope[0].Val()
#define STR1 ope[1].Str()
#define VAL1 ope[1].Val()
#define STR2 ope[2].Str()
#define VAL2 ope[2].Val()
#define STR3 ope[3].Str()
#define VAL3 ope[3].Val()
#define VAL4 ope[4].Val()
#define VAL5 ope[5].Val()
#define VAL6 ope[6].Val()
#define VAL7 ope[7].Val()
#define VAL8 ope[8].Val()
//---------------------------------------------------------------------------
void TMacro::ExecMethod(String name, int H, const std::vector<Element>& ope,
                        bool isLambda)
{
  Element obj = ope[0].Value();
  String objName = (ope[0].Type == etVar ? ope[0].Name() : ope[0].Str());
  if (!isLambda && obj.Type != etObject) {
    throw MacroException("�u.�v�̍����I�u�W�F�N�g�ł͂���܂���F" + objName);
  }
  Element funcPtr = obj.Type != etObject ? obj
                  : Element(name, etVar, env.GetObject(obj.Val()));
  while (funcPtr.Value().Type == etObject) {
    obj = funcPtr.Value();
    funcPtr = Element("", etVar, env.GetObject(obj.Val()));
  }
  String funcName = funcPtr.Str();
  try {
    GetStreamFor(funcPtr.Str());
  } catch (...) {
    throw MacroException(isLambda
        ? "�֐��I�u�W�F�N�g�ł͂���܂���F" + objName
        : "���\�b�h�ł͂���܂���F" + objName + "." + name);
  }
  int funcArity = funcName.SubString(funcName.LastDelimiter("/") + 1,
                                     funcName.Length()).ToIntDef(0);
  if (funcArity != H - 1) {
    throw MacroException("�����̐�����v���܂���B\n"
        + (isLambda ? objName : name) + "/" + funcArity + " �Ɉ����� "
        + (H - 1) + " �n����Ă��܂��B");
  }

  std::vector<Element> argStack;
  for (int i = 1; i < H; i++) {
    argStack.push_back(Element(ope[i].Value()));
  }
  int ml = (MaxLoop > 0) ? MaxLoop-LoopCount : 0;
  TMacro mcr(fs_io, ml, modules, env.CreateSubEnvironment());
  const Element &r = mcr.Do(funcName, argStack, -1, -1, &obj);
  if (r.Type != etErr) {
    Stack.push_back(r);
  } else {
    Stack.push_back(Element(isLambda
        ? "�֐��I�u�W�F�N�g���l��Ԃ��܂���F" + objName
        : "���\�b�h�͒l��Ԃ��܂���F" + name + "/" + (H - 1), etErr, NULL));
  }
}
//---------------------------------------------------------------------------
void TMacro::ExecFnc(String s)
{
  int H = s[1];
  s.Delete(1,1);

  std::vector<Element> ope;
  ope.assign(Stack.end() - H, Stack.end());
  Stack.erase(Stack.end() - H, Stack.end());

  String notFoundMessage = "��`����Ă��Ȃ��֐��ł�:" + s + "/" + H;
  if (s[1] == '.' || s[1] == '>') {
    bool isLambda = (s[1] == '>');
    s.Delete(1,1);
    try {
      ExecMethod(isLambda ? (String)"" : s, H, ope, isLambda);
      return;
    } catch (MacroException e) {
      if (!isLambda) {
        throw;
      }
      notFoundMessage = e.Message;
    } catch (...) {
      if (!isLambda) {
        throw;
      }
      notFoundMessage = "�֐��I�u�W�F�N�g�ł͂���܂���F" + ope[0].Name();
    }
    H--;
    ope.erase(ope.begin());
  }
    if(s[1] == '$') {
      s.Delete(1,1);
      std::vector<Element> argStack;
      for (int i = 0; i < H; i++) {
        argStack.push_back(Element(ope[i].Value()));
      }
      int ml = ((MaxLoop > 0) ? MaxLoop-LoopCount : 0);
      TMacro mcr(fs_io, ml, modules, env.CreateSubEnvironment());
      const Element &r = mcr.Do(s + "/" + H, argStack);
      if (r.Type != etErr) {
        Stack.push_back(r);
      } else {
        Stack.push_back(
            Element("�֐��͒l��Ԃ��܂���F" + s + "/" + H, etErr, NULL));
      }
    }else if(s == "{}") {
      std::vector<TEnvironment*> *objects = env.GetObjects();
      Stack.push_back(Element(objects->size(), "", etObject, env.GetGlobal()));
      objects->push_back(new TEnvironment(env.IsCellMacro, env.GetGlobal()));
    }else if(s == "(constructor)") {
      Element obj = Stack.back();
      Element funcPtr("constructor", etVar, env.GetObject(obj.Val()));
      Stack.pop_back();
      int ml = ((MaxLoop > 0) ? MaxLoop-LoopCount : 0);
      TMacro mcr(fs_io, ml, modules, env.CreateSubEnvironment());
      mcr.Do(funcPtr.Str(), Stack, -1, -1, &obj);
      Stack.push_back(obj);
    }else if(s == "func="){
      int count = H / 2;
      for(int i=0; i<count; i++){
        ope[i+count].Sbst(ope[i]);
      }
    }else if(s == "swap" && H == 2){
      Element t0 = ((ope[0].isNum() && ope[0].Type != etCell)
        ? Element(VAL0) : Element(STR0));
      Element t1 = ((ope[1].isNum() && ope[1].Type != etCell)
        ? Element(VAL1) : Element(STR1));
      ope[0].Sbst(t1);
      ope[1].Sbst(t0);
    }else if(s == "MessageBox"){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't show dialogs.", ME_SECURITY);
      }
      const wchar_t *text =
          H >= 1 ? STR0.c_str() : TEXT("�u���[�N�|�C���g�ł�");
      int flag = (H >= 2 ? ope[H - 1].Val() : MB_OK);
      const wchar_t *caption = (H >= 3 ? STR1.c_str() : TEXT("Cassava Macro"));
      Stack.push_back(
          Element(MessageBoxW(Application->Handle, text, caption, flag)));
    }else if(s == "InputBox"){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't show dialogs.", ME_SECURITY);
      }
      String Text = (H >= 1) ? STR0 : (String)"";
      String Value = (H >= 2) ? (ope[H-1].Str()) : (String)"";
      String Caption = (H >= 3) ? STR1 : (String)"Cassava Macro";
      bool isOk = InputQuery(Caption, Text, Value);
      if (isOk) {
        Stack.push_back(Element(Value));
      } else {
        throw MacroException("�L�����Z������܂����B", ME_CANCELED);
      }
    }else if(s == "InputBoxMultiLine"){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't show dialogs.", ME_SECURITY);
      }
      String Text = (H >= 1) ? STR0 : (String)"";
      String Value = (H >= 2) ? (ope[H-1].Str()) : (String)"";
      String Caption = (H >= 3) ? STR1 : (String)"Cassava Macro";
      bool isOk = InputBoxMultiLine(Caption, Text, Value);
      if (isOk) {
        Stack.push_back(Element(NormalizeNewLine(Value)));
      } else {
        throw MacroException("�L�����Z������܂����B", ME_CANCELED);
      }
    }else if(s == "GetRowHeight" && H == 0){
      Stack.push_back(Element(fmMain->MainGrid->DefaultRowHeight));
    }else if(s == "GetRowHeight" && H == 1){
      Stack.push_back(Element(fmMain->MainGrid->RowHeights[VAL0]));
    }else if(s == "SetRowHeight" && H == 1){
      fmMain->MainGrid->DefaultRowHeight = VAL0;
    }else if(s == "SetRowHeight" && H == 2){
      fmMain->MainGrid->RowHeights[VAL0] = VAL1;
    }else if(s == "AdjustRowHeight" && H == 1){
      fmMain->MainGrid->SetHeight(VAL0, false);
    }else if(s == "GetColWidth" && H == 0){
      Stack.push_back(Element(fmMain->MainGrid->DefaultColWidth));
    }else if(s == "GetColWidth" && H == 1){
      Stack.push_back(Element(fmMain->MainGrid->ColWidths[VAL0]));
    }else if(s == "SetColWidth" && H == 1){
      fmMain->MainGrid->DefaultColWidth = VAL0;
    }else if(s == "SetColWidth" && H == 2){
      fmMain->MainGrid->ColWidths[VAL0] = VAL1;
    }else if(s == "AdjustColWidth" && H == 1){
      fmMain->MainGrid->SetWidth(VAL0);
    }else if(s == "GetYear" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack.push_back(Element((int)year));
    }else if(s == "GetMonth" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack.push_back(Element((int)month));
    }else if(s == "GetDate" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack.push_back(Element((int)day));
    }else if(s == "GetHours" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack.push_back(Element((int)hours));
    }else if(s == "GetMinutes" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack.push_back(Element((int)minutes));
    }else if(s == "GetSeconds" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack.push_back(Element((int)seconds));
    }else if(s == "GetFilePath" && H == 0){
      String Path = ExtractFilePath(fmMain->FileName);
      if(Path != "" && (*(Path.LastChar()) != '\\')){ Path += '\\'; }
      Stack.push_back(Element(Path));
    }else if(s == "GetFileName" && H == 0){
      Stack.push_back(Element(ExtractFileName(fmMain->FileName)));
    }else if(s == "GetStatusBarCount" && H == 0){
      Stack.push_back(Element(fmMain->StatusBar->Panels->Count - 1));
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
      Stack.push_back(Element(data));
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
      Stack.push_back(Element(data));
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
      IniFile *Ini = fmMain->Pref->GetInifile();
      Stack.push_back(Element(Ini->ReadString(STR0, STR1, "")));
      delete Ini;
    }else if(s == "SetIniSetting" && H == 3){
      IniFile *Ini = fmMain->Pref->GetInifile();
      Ini->WriteString(STR0, STR1, STR2);
      delete Ini;
    }else if(s == "GetFontName" && H == 0){
      Stack.push_back(Element(fmMain->MainGrid->Font->Name));
    }else if(s == "SetFontName" && H == 1){
      fmMain->MainGrid->Font->Name = STR0;
      fmMain->SetGridFont(fmMain->MainGrid->Font);
    }else if(s == "GetFontSize" && H == 0){
      Stack.push_back(Element(fmMain->MainGrid->Font->Size));
    }else if(s == "SetFontSize" && H == 1){
      fmMain->MainGrid->Font->Size = VAL0;
      fmMain->SetGridFont(fmMain->MainGrid->Font);
    }else if(s == "write" || s == "writeln"){
      if (canWriteFile) {
        fs_io->SetEncode(fmMain->MainGrid->KanjiCode);
        for (int i = 0; i < H; i++) {
          fs_io->Write(ope[i].Str());
        }
        if (s == "writeln") {
          if (fmMain->MainGrid->ReturnCode == '\x0A') {
            fs_io->Write(TEXT("\x0A"));
          } else if(fmMain->MainGrid->ReturnCode == '\x0D'){
            fs_io->Write(TEXT("\x0D"));
          } else {
            fs_io->Write(TEXT("\x0D\x0A"));
          }
        }
      }else{
        int wx = MGCol, wy = MGRow;
        for(int i=0; i<H; i++){
          Write(wx, wy, ope[i].Str(), env.IsCellMacro);
          wx++;
        }
        if(s == "writeln"){ CsvGridGoTo(fmMain->MainGrid, 1, wy+1); }
        else{ CsvGridGoTo(fmMain->MainGrid, wx, wy); }
      }
    }else if(H == 0){
      TMenuItem *menu = NULL;
      if (!env.IsCellMacro) {
        menu = MenuSearch(fmMain->Menu->Items, s);
      }
      if (menu && menu->OnClick) {
        menu->OnClick(menu);
        if (s.SubString(1, 4) == "Open") {
          do {
            Sleep(100);
            Application->ProcessMessages();
          } while (fmMain->MainGrid->FileOpenThread);
        }
      } else {
        throw MacroException(notFoundMessage);
      }
    }else if(s == "int" && H == 1){
      Stack.push_back(Element((int)VAL0));
    }else if(s == "double" && H == 1){
      Stack.push_back(Element(VAL0));
    }else if(s == "str" && H == 1){
      Stack.push_back(Element(STR0));
    }else if(s == "max"){
      double maxVal = ope[0].Val();
      for(int i=1; i<H; i++){
        if (ope[i].Val() > maxVal) {
          maxVal = ope[i].Val();
        }
      }
      Stack.push_back(Element(maxVal));
    }else if(s == "min"){
      double minVal = ope[0].Val();
      for(int i=1; i<H; i++){
        if (ope[i].Val() < minVal) {
          minVal = ope[i].Val();
        }
      }
      Stack.push_back(Element(minVal));
    }else if(s == "sqrt" && H == 1){
      Stack.push_back(Element(sqrt(VAL0)));
    }else if(s == "sin" && H == 1){
      Stack.push_back(Element(sin(VAL0)));
    }else if(s == "cos" && H == 1){
      Stack.push_back(Element(cos(VAL0)));
    }else if(s == "tan" && H == 1){
      Stack.push_back(Element(tan(VAL0)));
    }else if(s == "pow" && H == 2){
      Stack.push_back(Element(pow(VAL0, VAL1)));
    }else if(s == "len" && H == 1){
      Stack.push_back(Element(STR0.Length()));
    }else if(s == "lenB" && H == 1){
      Stack.push_back(Element(((AnsiString)STR0).Length()));
    }else if(s == "InsertRow" && (H == 1 || H == 2)){
      int top = VAL0;
      int bottom = (H > 1 ? VAL1 : top);
      fmMain->MainGrid->InsertRow(AYtoRY(top), AYtoRY(bottom));
    }else if(s == "DeleteRow" && (H == 1 || H == 2)){
      int top = VAL0;
      int bottom = (H > 1 ? VAL1 : top);
      fmMain->MainGrid->DeleteRow(AYtoRY(top), AYtoRY(bottom));
    }else if(s == "InsertCol" && (H == 1 || H == 2)){
      int left = VAL0;
      int right = (H > 1 ? VAL1 : left);
      fmMain->MainGrid->InsertColumn(AXtoRX(left), AXtoRX(right));
    }else if(s == "DeleteCol" && (H == 1 || H == 2)){
      int left = VAL0;
      int right = (H > 1 ? VAL1 : left);
      fmMain->MainGrid->DeleteColumn(AXtoRX(left), AXtoRX(right));
    }else if(s == "random" && H == 1){
      Stack.push_back(Element(random(VAL0)));
    }else if(s == "cell" && H == 2){
      Stack.push_back(Element(VAL0, VAL1, &env));
    }else if(s == "left" && H == 2){
      Stack.push_back(Element(STR0.SubString(1, VAL1)));
    }else if(s == "right" && H == 2){
      String st = STR0;
      int v1 = VAL1;
      Stack.push_back(Element(st.SubString(st.Length() - v1 + 1, v1)));
    }else if(s == "pos" && H == 2){
      Stack.push_back(Element(STR0.Pos(STR1)));
    }else if(s == "posB" && H == 2){
      Stack.push_back(Element(((AnsiString)STR0).AnsiPos(STR1)));
    }else if(s == "asc" && H == 1){
      AnsiString str = STR0;
      if(str == ""){
        Stack.push_back(Element(0));
      }else if(str.IsLeadByte(1)){
        Stack.push_back(Element((unsigned char)(str[1]) * 256
          + (unsigned char)(str[2])));
      }else{
        Stack.push_back(Element((unsigned char)(str[1])));
      }
    }else if(s == "ascW" && H == 1){
      String str = STR0;
      if(str == ""){
        Stack.push_back(Element(0));
      }else{
        Stack.push_back(Element(str[1]));
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
      Stack.push_back(Element((AnsiString)buf));
    }else if(s == "chrW" && H == 1){
      wchar_t buf[2];
      buf[0] = VAL0;
      buf[1] = '\0';
      Stack.push_back(Element((String)buf));
    }else if((s == "move" || s == "moveto") && H == 2){
      int c = VAL0;
      int r = VAL1;
      if(s == "move"){ c += MGCol; r += MGRow; }
      CsvGridGoTo(fmMain->MainGrid, c, r);
    }else if(s == "mid" && H == 2){
      String st = STR0;
      Stack.push_back(Element(st.SubString(VAL1, st.Length()-VAL1+1)));
    }else if(s == "mid" && H == 3){
      Stack.push_back(Element(((String)STR0).SubString(VAL1, VAL2)));
    }else if(s == "mid=" && H == 4){
      String st = STR0;
      int r = VAL1 + VAL2;
      st = st.SubString(1, VAL1-1) + STR3 + st.SubString(r, st.Length() - r + 1);
      ope[0].Sbst(Element(st));
    }else if(s == "replace" && (H == 3 || H == 5)){
      bool ignoreCase = (H > 3 && VAL3 != 0);
      bool regex = (H > 4 && VAL4 != 0);
      Stack.push_back(Element(fmMain->MainGrid->ReplaceAll(
          STR0, STR1, STR2, !ignoreCase, regex, /* Word= */ false)));
    }else if(s == "sum" && H == 4){
      int l = VAL0;
      int t = VAL1;
      int r = VAL2;
      int b = VAL3;
      double result = fmMain->MainGrid->GetSum(l,t,r,b);
      Stack.push_back(Element(result));
    }else if(s == "avr" && H == 4){
      int l = VAL0;
      int t = VAL1;
      int r = VAL2;
      int b = VAL3;
      double result = fmMain->MainGrid->GetAvr(l,t,r,b);
      Stack.push_back(Element(result));
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
        argv[i] = ope[i].Str().c_str();
      }
      argv[H] = NULL;
      int result = _wspawnvp(P_NOWAITO, argv[0], argv);
      delete[] argv;

      if(result == -1){
        if(errno == ENOENT){
          throw MacroException(ope[0].Str() + "\n�t�@�C����������܂���B");
        }else{
          throw MacroException(ope[0].Str() + "\n���s�Ɏ��s���܂����B");
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
          throw MacroException("�ۑ��`�����s���ł�:" + STR1);
        }
      }
      fmMain->MainGrid->SaveToFile(STR0, format);
    }else if(s == "Export" && H == 2){
      fmMain->Export(STR0, STR1);
    }else if(s == "Sort" && H >= 1 && H <= 9) {
      int left = AXtoRX(VAL0);
      int top = AYtoRY(H > 1 ? VAL1 : 1);
      int right = (H > 2 ? AXtoRX(VAL2) : fmMain->MainGrid->DataRight);
      int bottom = (H > 3 ? AYtoRY(VAL3) : fmMain->MainGrid->DataBottom);
      int col = (H > 4 ? AXtoRX(VAL4) : left);
      bool direction = (H > 5 ? VAL5 : false);
      bool numSort = (H > 6 ? VAL6 : false);
      bool ignoreCase = (H > 7 ? VAL7 : false);
      bool ignoreZenhan = (H > 8 ? VAL8 : false);
      fmMain->MainGrid->Sort(left, top, right, bottom, col, !direction, numSort,
          ignoreCase, ignoreZenhan);
    }else if(s == "ReplaceAll" && (H == 2 || H == 5 || H == 9)) {
      String find = STR0;
      String replace = STR1;
      bool ignoreCase = (H > 2 ? VAL2 : false);
      bool word = (H > 3 ? VAL3 : false);
      bool regex = (H > 4 ? VAL4 : false);
      int left = (H > 5 ? AXtoRX(VAL5) : fmMain->MainGrid->FixedCols);
      int top = (H > 6 ? AYtoRY(VAL6) : fmMain->MainGrid->FixedRows);
      int right = (H > 7 ? AXtoRX(VAL7) : fmMain->MainGrid->DataRight);
      int bottom = (H > 8 ? AYtoRY(VAL8) : fmMain->MainGrid->DataBottom);
      int count = fmMain->MainGrid->ReplaceAll(
          find, replace, left, top, right, bottom, !ignoreCase, regex, word);
      Stack.push_back(Element(count));
    }else{
      throw MacroException(notFoundMessage);
    }
}
//---------------------------------------------------------------------------
void TMacro::ExecOpe(char c){
  if (c == 'P' || c == 'm' || c == 'g' ||
      c == CMO_Inc || c == CMO_Dec || c == '!') {
    if (Stack.size() < 1) { throw MacroException(c, ME_HIKISU); }
    Element ope = Stack.back();
    Stack.pop_back();
    if (c == 'g') {
      fs->Position = ope.Val();
      Stack.clear();
      LoopCount++;
      Application->ProcessMessages();
      if (MaxLoop > 0 && LoopCount > MaxLoop) {
        throw MacroException((String)"���[�v�񐔂�" + MaxLoop +
              "�ɒB���܂����B�����𒆒f���܂��B");
      }
      if (!RunningOk) { throw MacroException("���f���܂���"); }
    }
    else if (c == 'm') Stack.push_back(Element(-(ope.Val())));
    else if (c == CMO_Inc) { ope.Sbst(Element(ope.Val() + 1)); }
    else if (c == CMO_Dec) { ope.Sbst(Element(ope.Val() - 1)); }
    else if (c == '!') Stack.push_back(Element(((ope.Val() == 0) ? 1 : 0)));
  } else if (c == ':') {
    if (Stack.size() < 3) { throw MacroException(c, ME_HIKISU); }
    Element val = Stack.back(); Stack.pop_back();
    Element key = Stack.back(); Stack.pop_back();
    Element obj = Stack.back();
    String keyString = (key.Type == etVar ? key.Name() : key.Str());
    Element(keyString, etVar, env.GetObject(obj.Val())).Sbst(val);
  } else {
    if (Stack.size() < 2) { throw MacroException(c, ME_HIKISU); }
    Element ope2 = Stack.back(); Stack.pop_back();
    Element ope1 = Stack.back(); Stack.pop_back();

    if (c == '=' || c == CMO_Add || c == CMO_Subtract || c == CMO_Multiply
        || c == CMO_Divide) {
        if (c == '=') {
          ope1.Sbst(ope2);
        } else if (c == CMO_Add) {
          if (ope1.isNum() && ope2.isNum()) {
            ope1.Sbst(ope1.Val() + ope2.Val());
          } else {
            ope1.Sbst(ope1.Str() + ope2.Str());
          }
        } else if (c == CMO_Subtract) {
          ope1.Sbst(ope1.Val() - ope2.Val());
        } else if (c == CMO_Multiply) {
          ope1.Sbst(ope1.Val() * ope2.Val());
        } else if (c == CMO_Divide) {
          ope1.Sbst(ope1.Val() / ope2.Val());
        }
    } else if(c == '+') {
      if (ope1.isNum() && ope2.isNum()) {
        Stack.push_back(Element(ope1.Val() + ope2.Val()));
      } else {
        Stack.push_back(Element(ope1.Str() + ope2.Str()));
      }
    } else if (c == '-') {
      Stack.push_back(Element(ope1.Val() - ope2.Val()));
    } else if (c == '*') {
      Stack.push_back(Element(ope1.Val() * ope2.Val()));
    } else if (c == '/') {
      Stack.push_back(Element(ope1.Val() / ope2.Val()));
    } else if (c == '%') {
      Stack.push_back(Element((int)(ope1.Val()) % (int)(ope2.Val())));
    } else if (c == CMO_Eq) {
      Stack.push_back(Element(((ope1.Str() == ope2.Str()) ? 1 : 0)));
    } else if (c == CMO_NEq) {
      Stack.push_back(Element(((ope1.Str() != ope2.Str()) ? 1 : 0)));
    } else if (c == '<') {
      Stack.push_back(Element(((ope1.Val() < ope2.Val()) ? 1 : 0)));
    } else if (c == '>') {
      Stack.push_back(Element(((ope1.Val() > ope2.Val()) ? 1 : 0)));
    } else if (c == CMO_LEq) {
      Stack.push_back(Element(((ope1.Val() <= ope2.Val()) ? 1 : 0)));
    } else if (c == CMO_GEq) {
      Stack.push_back(Element(((ope1.Val() >= ope2.Val()) ? 1 : 0)));
    } else if (c == '&') {
      if (ope1.Val() == 0) {
        Stack.push_back(ope1);
        fs->Position = ope2.Val();
        return;
      }
    } else if (c == '|') {
      if (ope1.Val() != 0) {
        Stack.push_back(ope1);
        fs->Position = ope2.Val();
        return;
      }
    } else if (c == '.') {
      if (ope1.Value().Type != etObject) {
        throw MacroException("�u.�v�̍����I�u�W�F�N�g�ł͂���܂���F"
            + (ope1.Type == etVar ? ope1.Name() : ope1.Str()));
      }
      Stack.push_back(Element(ope2.Str(), etVar, env.GetObject(ope1.Val())));
    } else if (c == CMO_In) {
      if (ope2.Value().Type != etObject) {
        throw MacroException("in �̉E���I�u�W�F�N�g�ł͂���܂���F"
            + (ope2.Type == etVar ? ope2.Name() : ope2.Str()));
      }
      map<String, Element>& vars = env.GetObject(ope2.Val())->Vars;
      Stack.push_back(Element((vars.find(ope1.Str()) != vars.end()) ? 1 : 0));
    } else if (c == '?') {
      if (ope1.Val() == 0) {
        fs->Position = ope2.Val();
      }
      Stack.clear();
    } else if (c == ']') {
      Stack.push_back(Element(ope1.Val(), ope2.Val(), &env));
    }
  }
}
//---------------------------------------------------------------------------
TStream *TMacro::GetStreamFor(String funcName){
  TStream *result;
  try {
    int index = modules->IndexOf(funcName);
    if (index < 0) {
      throw MacroException("���[�U�[�֐���������܂���B");
    }
    TObject *obj = modules->Objects[index];
    result = static_cast<TStream *>(obj);
  } catch (...) {
    throw MacroException(funcName + "\n���[�U�[�֐���������܂���B");
  }
  return result;
}
//---------------------------------------------------------------------------
Element TMacro::Do(String FileName, const std::vector<Element> &AStack,
                   int x, int y, Element *thisPtr) {
  Element ReturnValue;
  // �ċA�Ăяo���Ή��̂��߁A���\�b�h�I������Stream��Position�����ɖ߂�
  long oldpc = 0L;
  Stack = AStack;
  fs = NULL;
  try{
    fs = GetStreamFor(FileName);
    oldpc = fs->Position;
    fs->Position = 0;

    env.Vars.clear();
    env.Vars["x"] = Element(((x >= 0) ? x : MGCol));
    env.Vars["y"] = Element(((y >= 0) ? y : MGRow));
    env.Vars["Left"] = Element(fmMain->MainGrid->ShowRowCounter ?
        1 : fmMain->MainGrid->FixedCols + 1);
    env.Vars["Top"] = Element(fmMain->MainGrid->ShowColCounter ?
        1 : fmMain->MainGrid->FixedRows + 1);
    if (thisPtr) {
      env.Vars["this"] = *thisPtr;
    }
    char Type;
    LoopCount = 0;
    while(fs->Read(&Type, 1) > 0) {
      if(Type == '$'){
        Stack.push_back(Element(ReadString(fs)));
      }else if(Type == '*'){
        ExecFnc(ReadString(fs));
      }else if(Type == '~'){
        String name = ReadString(fs);
        Stack.push_back(Element(name, etVar, &env));
      }else if(Type == '!'){
        Stack.push_back(Element(ReadString(fs), etSystem, true, &env));
      }else if(Type == '-'){
        char c;
        fs->Read(&c, 1);
        if(c == '}'){
          ReturnValue = Stack.back().Value();
          break;
        }else{
          ExecOpe(c);
        }
      }else if(Type == 'i'){
        int i;
        fs->Read(&i, sizeof(int));
        Stack.push_back(Element(i));
      }else if(Type == 'd' || Type == 'I'){
        double d;
        fs->Read(&d, sizeof(double));
        Stack.push_back(Element(d));
      }
    }

  } catch (MacroException e) {
    if(fs){
      fs->Position = oldpc;
    }
    throw e;
  } catch(Exception *e) {
    if(fs){
      fs->Position = oldpc;
    }
    throw e;
  }

  if(fs){
    fs->Position = oldpc;
  }
  return ReturnValue;
}
//---------------------------------------------------------------------------
String ExecMacro(String FileName, int MaxLoop, TStringList *Modules,
                 int x, int y, EncodedWriter *IO, bool IsCellMacro)
{
  if(!RunningOk){ return ""; }
  if(!IsCellMacro){ RunningCount++; }
  randomize();
  TMacro mcr(IO, MaxLoop, Modules, TEnvironment(IsCellMacro, NULL));
  Element r;
  try {
    std::vector<Element> stack;
    r = mcr.Do(FileName, stack, x, y);
  } catch (MacroException e) {
    if (IsCellMacro) {
      RunningCount--;
      throw e;
    } else if (e.Type != ME_CANCELED) {
      if (e.Type == ME_HIKISU) {
        e.Message = "�����̐�������܂���:" + e.Message;
      }
      Application->MessageBox((FileName + "\n" + e.Message).c_str(),
                              TEXT("Cassava Macro Interpreter"), 0);
    }
  } catch (Exception *e) {
    if (IsCellMacro) {
      RunningCount--;
      throw e;
    } else {
      Application->MessageBox((FileName + "\n" + e->Message).c_str(),
                              TEXT("Cassava Macro Interpreter"), 0);
    }
  }
  if(!IsCellMacro){
    RunningCount--;
    if(RunningCount == 0){
      RunningOk = true;
    }
  }

  String ReturnValue = "";
  if (r.Type != etErr) {
    ReturnValue = r.Str();
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

