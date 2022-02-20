//---------------------------------------------------------------------------
#include <boost\regex.hpp>
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
enum ElementType {
  etErr,
  etAtom,
  etVar,
  etCell,
  etSystem,
  etObject,
  etFunction
};
//---------------------------------------------------------------------------
enum MacroExceptionType {
  ME_GENERIC,
  ME_HIKISU,
  ME_SECURITY,
  ME_CANCELED
};
//---------------------------------------------------------------------------
class MacroException{
public:
  String Message;
  MacroExceptionType Type;
  MacroException(String Mes, MacroExceptionType t = ME_GENERIC)
      : Message(Mes), Type(t) {}
};
//---------------------------------------------------------------------------
class GridProxy {
private:
  int deletingTop;
  int deletingRows;
  TMainGrid *grid;
  bool isCellMacro;

  int RXtoAX(int x) const { return x - grid->DataLeft + 1; }
  int RYtoAY(int y) const { return y - grid->DataTop + 1; }
  int AXtoRX(int x) const { return x + grid->DataLeft - 1; }
  int AYtoRY(int y) const { return y + grid->DataTop - 1; }
  int ax(int x) const { return x; }
  int rx(int x) const { return AXtoRX(ax(x)); }
  int ay(int y) const { return y >= deletingTop ? y + deletingRows : y; }
  int ry(int y) const { return AYtoRY(ay(y)); }

public:
  GridProxy(TMainGrid *Grid, bool cm)
      : deletingTop(0), deletingRows(0), grid(Grid), isCellMacro(cm) {}

  ~GridProxy() {
    ApplyPendingChanges();
  }

  void ApplyPendingChanges() {
    if (deletingRows > 0) {
      grid->DeleteRow(AYtoRY(deletingTop),
                      AYtoRY(deletingTop + deletingRows - 1));
      deletingTop = 0;
      deletingRows = 0;
    }
  }

  TMainGrid *Raw() {
    ApplyPendingChanges();
    return grid;
  }

  String GetCell(int x, int y) {
    return grid->ACells[ax(x)][ay(y)];
  }

  void SetCell(int x, int y, String value) {
    if (isCellMacro) {
      throw MacroException("Cell Macro Can't Update Cells.", ME_SECURITY);
    }
    if (x < 1 || y < 1) {
      return;
    }
    grid->ACells[ax(x)][ay(y)] = value;
    grid->Modified = true;
  }

  bool IsNumberAtACell(int x, int y) {
    return grid->IsNumberAtACell(ax(x), ay(y));
  }

  void MoveTo(int x, int y) {
    x = rx(x);
    y = ry(y);
    if (x < grid->FixedCols) {
      x = grid->FixedCols;
    } else if (x >= grid->ColCount) {
      grid->ChangeColCount(x + 1);
    }
    if (y < grid->FixedRows) {
      y = grid->FixedRows;
    } else if (y >= grid->RowCount) {
      grid->ChangeRowCount(y + 1);
    }
    grid->SetSelection(x, x, y, y);
  }

  bool RowCounterShown() {
    return grid->ShowRowCounter;
  }

  bool ColCounterShown() {
    return grid->ShowColCounter;
  }

  int GetCol() {
    return RXtoAX(grid->Col);
  }

  int GetRow() {
    int row = RYtoAY(grid->Row);
    if (row > deletingTop + deletingRows) {
      return row - deletingRows;
    } else if (row > deletingTop) {
      return deletingTop;
    } else {
      return row;
    }
  }

  int GetLeft() {
    return grid->ShowRowCounter ? 1 : grid->FixedCols + 1;
  }

  int GetTop() {
    return grid->ShowColCounter ? 1 : grid->FixedRows + 1;
  }

  int GetRight() {
    return RXtoAX(grid->DataRight);
  }

  int GetBottom() {
    return RYtoAY(grid->DataBottom) - deletingRows;
  }

  int GetSelLeft() {
    ApplyPendingChanges();
    return RXtoAX(grid->SelLeft);
  }

  int GetSelTop() {
    ApplyPendingChanges();
    return RYtoAY(grid->SelTop);
  }

  int GetSelRight() {
    ApplyPendingChanges();
    return RXtoAX(grid->Selection.Right);
  }

  int GetSelBottom() {
    ApplyPendingChanges();
    return RYtoAY(grid->Selection.Bottom);
  }

  void DeleteRow(int top, int bottom) {
    if (top == deletingTop) {
      deletingRows += bottom - top + 1;
    } else if (bottom == deletingTop - 1) {
      deletingTop = top;
      deletingRows += bottom - top + 1;
    } else {
      ApplyPendingChanges();
      deletingTop = top;
      deletingRows = bottom - top + 1;
    }
    if (deletingRows >= 10000) {
      ApplyPendingChanges();
    }
  }

  void InsertRow(int top, int bottom) {
    ApplyPendingChanges();
    grid->InsertRow(ry(top), ry(bottom));
  }

  void MoveRow(int from, int to) {
    ApplyPendingChanges();
    grid->MoveRow(ry(from), ry(to));
  }

  void ChangeRowCount(int count) {
    ApplyPendingChanges();
    grid->ChangeRowCount(count);
  }

  void DeleteCol(int left, int right) {
    ApplyPendingChanges();
    grid->DeleteColumn(rx(left), rx(right));
  }

  void InsertCol(int left, int right) {
    ApplyPendingChanges();
    grid->InsertColumn(rx(left), rx(right));
  }

  void MoveCol(int from, int to) {
    ApplyPendingChanges();
    grid->MoveColumn(rx(from), rx(to));
  }

  void ChangeColCount(int count) {
    ApplyPendingChanges();
    grid->ChangeColCount(count);
  }

  void Sort(int left, int top, int right, int bottom, int col,
            bool direction, bool numSort, bool ignoreCase, bool ignoreZenhan) {
    ApplyPendingChanges();
    grid->Sort(rx(left), ry(top), rx(right), ry(bottom), rx(col),
               !direction, numSort, ignoreCase, ignoreZenhan);
  }

  int ReplaceAll(String find, String replace, int left, int top, int right,
                 int bottom, bool ignoreCase, bool regex, bool word) {
    ApplyPendingChanges();
    return grid->ReplaceAll(find, replace, rx(left), ry(top), rx(right),
                            ry(bottom), !ignoreCase, regex, word);
  }

  void Select(int left, int top, int right, int bottom) {
    ApplyPendingChanges();
    grid->SetSelection(rx(min(left, right)), rx(max(left, right)),
                       ry(min(top, bottom)), ry(max(top, bottom)));
  }
};
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
  ElementType Type;
  bool isNum() const;
  int X, Y;
  Element() : Type(etErr), Num(true), vl(0), st(""), env(NULL) {};
  Element(double d) :
      Type(etAtom), Num(true), vl(d), env(NULL) {};
  Element(double d, String s, ElementType t, TEnvironment *e) :
      Type(t), Num(true), st(s), vl(d), env(e) {};
  Element(String s) :
      Type(etAtom), Num(false), st(s), env(NULL) {};
  Element(String s, ElementType t, TEnvironment *e) :
      Type(t), Num(false), st(s), env(e) {};
  Element(String s, ElementType t, bool nm, TEnvironment *e) :
      Type(t), Num(nm), st(s), env(e) {};
  Element(int cl, int rw, TEnvironment *e);
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
  Element GetMember(String name);
};
//---------------------------------------------------------------------------
class TEnvironment {
private:
  TEnvironment *global;
  std::vector<TEnvironment*> *objects;
public:
  std::map<String, Element> Vars;
  bool IsCellMacro;
  GridProxy *Grid;

  TEnvironment(bool cm, GridProxy *grid,  TEnvironment *gl) :
      Vars(), IsCellMacro(cm), Grid(grid), global(gl), objects(NULL) {}
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
    return TEnvironment(IsCellMacro, Grid, GetGlobal());
  }
  TEnvironment *NewObject() {
    return new TEnvironment(IsCellMacro, Grid, GetGlobal());
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
  void ExecPrimitiveMethod(String s, int H, const std::vector<Element>& ope);
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
static int RunningCount = 0;
static bool RunningOk = true;
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
Element::Element(int cl, int rw, TEnvironment *e)
    : Type(etCell), X(cl), Y(rw), env(e)
{
  Num = env->Grid->IsNumberAtACell(cl, rw);
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
    return ToDouble(env->Grid->GetCell(X, Y), 0);
  } else if(Type == etVar) {
    Element &e = GetVar();
    if (e.Type == etErr) {
      if (e.st != "") { throw MacroException(e.st); }
      throw MacroException("値が代入されていない変数・フィールドです：" + st);
    }
    return e.Val();
  } else if (Type == etSystem) {
    if     (st == "Col")       { return env->Grid->GetCol(); }
    else if(st == "Row")       { return env->Grid->GetRow(); }
    else if(st == "Right")     { return env->Grid->GetRight(); }
    else if(st == "Bottom")    { return env->Grid->GetBottom(); }
    else if(st == "SelLeft")   { return env->Grid->GetSelLeft(); }
    else if(st == "SelTop")    { return env->Grid->GetSelTop(); }
    else if(st == "SelRight")  { return env->Grid->GetSelRight(); }
    else if(st == "SelBottom") { return env->Grid->GetSelBottom(); }
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
    return env->Grid->GetCell(X, Y);
  } else if (Type == etVar) {
    Element &e = GetVar();
    if (e.Type == etErr) {
      if (e.st != "") { throw MacroException(e.st); }
      throw MacroException("値が代入されていない変数・フィールドです：" + st);
    }
    return e.Str();
  } else if (Type == etSystem) {
    return String(Val());
  } else if (Type == etObject) {
    String s = "{";
    std::map<String, Element>& vars = env->GetObject(vl)->Vars;
    for (std::map<String, Element>::iterator it = vars.begin();
         it != vars.end(); it++) {
      if (it->second.Type == etErr) {
        continue;
      }
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
      throw MacroException("値が代入されていない変数・フィールドです：" + st);
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
    return env->Grid->IsNumberAtACell(X,Y);
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
    GridProxy *g = env->Grid;
    if (st == "Col") {
      g->MoveTo(v, g->GetRow());
    } else if(st == "Row") {
      g->MoveTo(g->GetCol(), v);
    } else if(st == "Right") {
      int newRight = v;
      int currentRight = g->GetRight();
      if (newRight < currentRight) {
        g->DeleteCol(newRight + 1, currentRight);
      } else if (newRight > currentRight) {
        g->InsertCol(currentRight + 1, newRight);
      }
      if (g->GetCol() > v) { g->MoveTo(v, g->GetRow()); }
      g->ChangeColCount(newRight + (g->RowCounterShown() ? 2 : 1));
    } else if(st == "Bottom") {
      int newBottom = v;
      int currentBottom = g->GetBottom();
      if (newBottom < currentBottom) {
        g->DeleteRow(newBottom + 1, currentBottom);
      } else if (newBottom > currentBottom) {
        g->InsertRow(currentBottom + 1, newBottom);
      }
      if (g->GetRow() > v) { env->Grid->MoveTo(g->GetCol(), v); }
      g->ChangeRowCount(newBottom + (g->ColCounterShown() ? 2 : 1));
    } else {
      TGridRect Sel;
      Sel.Left = g->GetSelLeft();
      Sel.Top = g->GetSelTop();
      Sel.Right = g->GetSelRight();
      Sel.Bottom = g->GetSelBottom();
      if(st == "SelLeft") {
        Sel.Left = v;
        if(Sel.Right < Sel.Left) { Sel.Right = Sel.Left; }
      }else if(st == "SelTop") {
        Sel.Top = v;
        if(Sel.Bottom < Sel.Top) { Sel.Bottom = Sel.Top; }
      }else if(st == "SelRight") {
        Sel.Right = v;
        if(Sel.Left > Sel.Right) { Sel.Left = Sel.Right; }
      }else if(st == "SelBottom") {
        Sel.Bottom = v;
        if(Sel.Top > Sel.Bottom) { Sel.Top = Sel.Bottom; }
      }
      g->Select(Sel.Left, Sel.Top, Sel.Right, Sel.Bottom);
    }
  } else if (Type == etCell) {
    env->Grid->SetCell(X, Y, e.Value().Str());
  } else {
    throw MacroException("代入先が左辺値ではありません：" + Str());
  }
}
//---------------------------------------------------------------------------
Element Element::GetMember(String name)
{
  return Element(name, etVar, env->GetObject(Val()));
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
    ExecPrimitiveMethod(name, H, ope);
    return;
  }
  Element funcPtr = obj.Type != etObject ? obj : obj.GetMember(name);
  try {
    while (funcPtr.Value().Type == etObject) {
      obj = funcPtr.Value();
      funcPtr = obj.GetMember("");
    }
  } catch (...) {
    if (name == "toString") {
      Stack.push_back(Element(obj.Str()));
      return;
    }
    throw;
  }
  String funcName = funcPtr.Str();
  try {
    GetStreamFor(funcPtr.Str());
  } catch (...) {
    throw MacroException(isLambda
        ? "関数オブジェクトではありません：" + objName
        : "メソッドではありません：" + objName + "." + name);
  }
  int slash = funcName.LastDelimiter("/");
  bool isVarArg = (funcName[slash + 1] == '+');
  int funcArity = funcName.SubString(slash + 1, funcName.Length()).ToIntDef(0);
  if (!isVarArg && funcArity != H - 1) {
    throw MacroException("引数の数が一致しません。\n"
        + (isLambda ? objName : name) + "/" + funcArity + " に引数が "
        + (H - 1) + " 個渡されています。");
  } else if (isVarArg && funcArity > H - 1) {
    throw MacroException("引数の数が一致しません。\n"
        + (isLambda ? objName : name) + " には引数が " + funcArity
        + " 個以上必要です。");
  }

  std::vector<Element> argStack;
  for (int i = 1; i <= funcArity; i++) {
    argStack.push_back(Element(ope[i].Value()));
  }
  if (isVarArg) {
    TEnvironment *varArg = env.NewObject();
    for (int i = funcArity + 1; i < H; i++) {
      varArg->Vars[(String)(i - funcArity - 1)] = Element(ope[i].Value());
    }
    varArg->Vars["length"] = Element(H - funcArity - 1);
    std::vector<TEnvironment*> *objects = env.GetObjects();
    argStack.push_back(Element(objects->size(), "", etObject, env.GetGlobal()));
    objects->push_back(varArg);
  }
  int ml = (MaxLoop > 0) ? MaxLoop-LoopCount : 0;
  TMacro mcr(fs_io, ml, modules, env.CreateSubEnvironment());
  const Element &r = mcr.Do(funcName, argStack, -1, -1, &obj);
  if (r.Type != etErr) {
    Stack.push_back(r);
  } else {
    Stack.push_back(Element(isLambda
        ? "関数オブジェクトが値を返しません：" + objName
        : "メソッドは値を返しません：" + name + "/" + (H - 1), etErr, NULL));
  }
}
//---------------------------------------------------------------------------
void TMacro::ExecPrimitiveMethod(String s, int H,
                                 const std::vector<Element>& ope)
{
  if (s == "endsWith" && (H == 2 || H == 3)) {
    String target = STR0;
    String search = STR1;
    int length = (H > 2 ? VAL2 : target.Length());
    Stack.push_back(Element(target.SubString(
        length - search.Length() + 1, search.Length()) == search ? 1 : 0));
  } else if (s == "includes" && (H == 2 || H == 3)) {
    String target = STR0;
    String search = STR1;
    int from = (H > 2 ? VAL2 : 0);
    if (search == "") {
      Stack.push_back(Element(1));
      return;
    }
    int pos = target.SubString(from + 1, target.Length() - from).Pos(search);
    Stack.push_back(Element(pos > 0 ? 1 : 0));
  } else if (s == "indexOf" && (H == 2 || H == 3)) {
    String target = STR0;
    String search = STR1;
    int from = (H > 2 ? VAL2 : 0);
    if (from < 0) {
      from = 0;
    }
    if (search == "") {
      Stack.push_back(Element(min(from, target.Length())));
      return;
    }
    int pos = target.SubString(from + 1, target.Length() - from).Pos(search);
    if (pos == 0) {
      Stack.push_back(Element(-1));
      return;
    }
    Stack.push_back(Element(pos + from - 1));
  } else if (s == "lastIndexOf" && (H == 2 || H == 3)) {
    String target = STR0;
    String search = STR1;
    int from = (H > 2 ? VAL2 : target.Length());
    if (from < 0) {
      from = 0;
    }
    if (search == "") {
      Stack.push_back(Element(min(from, target.Length())));
      return;
    }
    int lastPos = 0;
    while (true) {
      int pos =
          target.SubString(lastPos + 1, target.Length() - lastPos).Pos(search);
      if (pos == 0 || pos > from - lastPos + 1) {
        break;
      }
      lastPos += pos;
    }
    Stack.push_back(Element(lastPos - 1));
  } else if (s == "padEnd" && (H == 2 || H == 3)) {
    String target = STR0;
    int length = VAL1;
    String pad = (H > 2 ? STR2 : (String)" ");
    if (target.Length() >= length) {
      Stack.push_back(Element(target));
      return;
    }
    while (target.Length() < length) {
      target += pad;
    }
    Stack.push_back(Element(target.SubString(1, length)));
  } else if (s == "padStart" && (H == 2 || H == 3)) {
    String target = STR0;
    int length = VAL1;
    String pad = (H > 2 ? STR2 : (String)" ");
    if (target.Length() >= length) {
      Stack.push_back(Element(target));
      return;
    }
    int lengthToPad = length - target.Length();
    String prefix = "";
    while (lengthToPad >= pad.Length()) {
      prefix += pad;
      lengthToPad -= pad.Length();
    }
    Stack.push_back(Element(prefix + pad.SubString(1, lengthToPad) + target));
  } else if (s == "repeat" && H == 2) {
    String target = STR0;
    String result = "";
    int count = VAL1;
    for (int i = 0; i < count; i++) {
      result += target;
    }
    Stack.push_back(Element(result));
  } else if (s == "replaceAll" && H == 3) {
    Stack.push_back(Element(fmMain->MainGrid->ReplaceAll(STR0, STR1, STR2,
        /* Case= */ true, /* Regex= */ false, /* Word= */ false)));
  } else if (s == "search" && H == 2) {
    wchar_t *target = STR0.c_str();
    boost::wcmatch match;
    if (boost::regex_search(target, match, boost::wregex(STR1.c_str()))) {
      Stack.push_back(Element(match[0].first - target));
    } else {
      Stack.push_back(Element(-1));
    }
  } else if (s == "startsWith" && (H == 2 || H == 3)) {
    String search = STR1;
    int position = (H > 2 ? VAL2 : 0);
    Stack.push_back(Element(
        STR0.SubString(position + 1, search.Length()) == search ? 1 : 0));
  } else if (s == "substring" && (H == 2 || H == 3)) {
    String target = STR0;
    int arg1 = VAL1;
    int arg2 = (H > 2 ? VAL2 : target.Length());
    int start = min(arg1, arg2);
    if (start < 0) {
      start = 0;
    }
    if (start > target.Length()) {
      start = target.Length();
    }
    int end = max(arg1, arg2);
    if (end < 0) {
      end = 0;
    }
    if (end > target.Length()) {
      end = target.Length();
    }
    Stack.push_back(Element(target.SubString(start + 1, end - start)));
  } else if (s == "toLowerCase" && H == 1) {
    Stack.push_back(Element(STR0.LowerCase()));
  } else if (s == "toString" && H == 1) {
    Stack.push_back(Element(STR0));
  } else if (s == "toUpperCase" && H == 1) {
    Stack.push_back(Element(STR0.UpperCase()));
  } else if (s == "trim" && H == 1) {
    Stack.push_back(Element(STR0.Trim()));
  } else if (s == "trimEnd" && H == 1) {
    Stack.push_back(Element(STR0.TrimRight()));
  } else if (s == "trimStart" && H == 1) {
    Stack.push_back(Element(STR0.TrimLeft()));
  } else {
    throw MacroException("定義されていないメソッドです：" + s + "/" + (H - 1));
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

  String notFoundMessage = "定義されていない関数です:" + s + "/" + H;
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
      notFoundMessage = "関数オブジェクトではありません：" + ope[0].Name();
    }
    H--;
    ope.erase(ope.begin());
  }

  if(s[1] == '$') {
    s.Delete(1,1);
    int ml = ((MaxLoop > 0) ? MaxLoop-LoopCount : 0);
    TMacro mcr(fs_io, ml, modules, env.CreateSubEnvironment());
    String funcName;
    std::vector<Element> argStack;
    if (modules->IndexOf(s + "/" + H) >= 0) {
      for (int i = 0; i < H; i++) {
        argStack.push_back(Element(ope[i].Value()));
      }
      funcName = s + "/" + H;
    } else {
      int normalArgs;
      for (normalArgs = H; normalArgs >= 0; normalArgs--) {
        if (modules->IndexOf(s + "/+" + normalArgs) >= 0) {
          funcName = s + "/+" + normalArgs;
          break;
        }
      }
      if (normalArgs < 0) {
        throw MacroException(s + "/" + H + "\nユーザー関数が見つかりません。");
      }
      for (int i = 0; i < normalArgs; i++) {
        argStack.push_back(Element(ope[i].Value()));
      }
      TEnvironment *varArg = env.NewObject();
      for (int i = normalArgs; i < H; i++) {
        varArg->Vars[(String)(i - normalArgs)] = Element(ope[i].Value());
      }
      varArg->Vars["length"] = Element(H - normalArgs);
      std::vector<TEnvironment*> *objects = env.GetObjects();
      argStack.push_back(
          Element(objects->size(), "", etObject, env.GetGlobal()));
      objects->push_back(varArg);
    }
    const Element &r = mcr.Do(funcName, argStack);
    if (r.Type != etErr) {
      Stack.push_back(r);
    } else {
      Stack.push_back(
          Element("関数は値を返しません：" + s + "/" + H, etErr, NULL));
    }
  }else if(s == "{}") {
      std::vector<TEnvironment*> *objects = env.GetObjects();
      Stack.push_back(Element(objects->size(), "", etObject, env.GetGlobal()));
      objects->push_back(env.NewObject());
    }else if(s == "(constructor)") {
      Element obj = Stack.back();
      Element funcPtr = obj.GetMember("constructor");
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
      Element t0 = ope[0].Value();
      Element t1 = ope[1].Value();
      ope[0].Sbst(t1);
      ope[1].Sbst(t0);
    }else if(s == "MessageBox"){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't show dialogs.", ME_SECURITY);
      }
      env.Grid->ApplyPendingChanges();
      const wchar_t *text =
          H >= 1 ? STR0.c_str() : L"ブレークポイントです";
      int flag = (H >= 2 ? ope[H - 1].Val() : MB_OK);
      const wchar_t *caption = (H >= 3 ? STR1.c_str() : L"Cassava Macro");
      Stack.push_back(
          Element(MessageBoxW(Application->Handle, text, caption, flag)));
    }else if(s == "InputBox"){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't show dialogs.", ME_SECURITY);
      }
      env.Grid->ApplyPendingChanges();
      String Text = (H >= 1) ? STR0 : (String)"";
      String Value = (H >= 2) ? (ope[H-1].Str()) : (String)"";
      String Caption = (H >= 3) ? STR1 : (String)"Cassava Macro";
      bool isOk = InputQuery(Caption, Text, Value);
      if (isOk) {
        Stack.push_back(Element(Value));
      } else {
        throw MacroException("キャンセルされました。", ME_CANCELED);
      }
    }else if(s == "InputBoxMultiLine"){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't show dialogs.", ME_SECURITY);
      }
      env.Grid->ApplyPendingChanges();
      String Text = (H >= 1) ? STR0 : (String)"";
      String Value = (H >= 2) ? (ope[H-1].Str()) : (String)"";
      String Caption = (H >= 3) ? STR1 : (String)"Cassava Macro";
      bool isOk = InputBoxMultiLine(Caption, Text, Value);
      if (isOk) {
        Stack.push_back(Element(NormalizeNewLine(Value)));
      } else {
        throw MacroException("キャンセルされました。", ME_CANCELED);
      }
    }else if(s == "GetRowHeight" && H == 0){
      Stack.push_back(Element(env.Grid->Raw()->DefaultRowHeight));
    }else if(s == "GetRowHeight" && H == 1){
      Stack.push_back(Element(env.Grid->Raw()->RowHeights[VAL0]));
    }else if(s == "SetRowHeight" && H == 1){
      env.Grid->Raw()->DefaultRowHeight = VAL0;
    }else if(s == "SetRowHeight" && H == 2){
      env.Grid->Raw()->RowHeights[VAL0] = VAL1;
    }else if(s == "AdjustRowHeight" && H == 1){
      env.Grid->Raw()->SetHeight(VAL0, false);
    }else if(s == "GetColWidth" && H == 0){
      Stack.push_back(Element(env.Grid->Raw()->DefaultColWidth));
    }else if(s == "GetColWidth" && H == 1){
      Stack.push_back(Element(env.Grid->Raw()->ColWidths[VAL0]));
    }else if(s == "SetColWidth" && H == 1){
      env.Grid->Raw()->DefaultColWidth = VAL0;
    }else if(s == "SetColWidth" && H == 2){
      env.Grid->Raw()->ColWidths[VAL0] = VAL1;
    }else if(s == "AdjustColWidth" && H == 1){
      env.Grid->Raw()->SetWidth(VAL0);
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
    }else if(s == "GetSettingPath" && H == 0){
      Stack.push_back(Element(fmMain->Pref->Path));
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
      Stack.push_back(Element(env.Grid->Raw()->Font->Name));
    }else if(s == "SetFontName" && H == 1){
      env.Grid->Raw()->Font->Name = STR0;
      fmMain->SetGridFont(env.Grid->Raw()->Font);
    }else if(s == "GetFontSize" && H == 0){
      Stack.push_back(Element(env.Grid->Raw()->Font->Size));
    }else if(s == "SetFontSize" && H == 1){
      env.Grid->Raw()->Font->Size = VAL0;
      fmMain->SetGridFont(env.Grid->Raw()->Font);
    }else if(s == "write" || s == "writeln"){
      if (canWriteFile) {
        fs_io->SetEncode(env.Grid->Raw()->KanjiCode);
        for (int i = 0; i < H; i++) {
          fs_io->Write(ope[i].Str());
        }
        if (s == "writeln") {
          fs_io->Write(ReturnCodeString(env.Grid->Raw()->ReturnCode));
        }
      }else{
        int wx = env.Grid->GetCol();
        int wy = env.Grid->GetRow();
        for(int i=0; i<H; i++){
          env.Grid->SetCell(wx, wy, ope[i].Str());
          wx++;
        }
        if (s == "writeln") {
          env.Grid->MoveTo(1, wy + 1);
        } else {
          env.Grid->MoveTo(wx, wy);
        }
      }
    }else if(s == "StartMacroRecording" && H == 0) {
      env.Grid->Raw()->UndoList->StartMacroRecording();
    }else if(s == "StopMacroRecording" && H == 0) {
      env.Grid->Raw()->UndoList->StopMacroRecording();
    }else if(s == "GetRecordedMacro" && H == 0) {
      Stack.push_back(Element(env.Grid->Raw()->UndoList->GetRecordedMacro()));
    }else if(H == 0){
      env.Grid->ApplyPendingChanges();
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
          } while (env.Grid->Raw()->FileOpenThread);
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
    } else if (s == "asin" && H == 1) {
      Stack.push_back(Element(asin(VAL0)));
    } else if (s == "acos" && H == 1) {
      Stack.push_back(Element(acos(VAL0)));
    } else if (s == "atan" && H == 1) {
      Stack.push_back(Element(atan(VAL0)));
    } else if (s == "atan2" && H == 2) {
      double y = VAL0;
      double x = VAL1;
      if (x == 0 && y == 0) {
        Stack.push_back(Element(0));
      } else {
        Stack.push_back(Element(atan2(VAL0, VAL1)));
      }
    }else if(s == "pow" && H == 2){
      Stack.push_back(Element(pow(VAL0, VAL1)));
    }else if(s == "len" && H == 1){
      Stack.push_back(Element(STR0.Length()));
    }else if(s == "lenB" && H == 1){
      Stack.push_back(Element(((AnsiString)STR0).Length()));
    }else if(s == "InsertRow" && (H == 1 || H == 2)){
      int top = VAL0;
      int bottom = (H > 1 ? VAL1 : top);
      env.Grid->InsertRow(top, bottom);
    }else if(s == "DeleteRow" && (H == 1 || H == 2)){
      int top = VAL0;
      int bottom = (H > 1 ? VAL1 : top);
      env.Grid->DeleteRow(top, bottom);
    }else if(s == "InsertCol" && (H == 1 || H == 2)){
      int left = VAL0;
      int right = (H > 1 ? VAL1 : left);
      env.Grid->InsertCol(left, right);
    }else if(s == "DeleteCol" && (H == 1 || H == 2)){
      int left = VAL0;
      int right = (H > 1 ? VAL1 : left);
      env.Grid->DeleteCol(left, right);
    }else if(s == "MoveRow" && H == 2) {
      env.Grid->MoveRow(VAL0, VAL1);
    }else if(s == "MoveCol" && H == 2) {
      env.Grid->MoveCol(VAL0, VAL1);
    }else if(s == "Paste" && H == 1) {
      env.Grid->Raw()->PasteFromClipboard(VAL0);
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
      if (s == "move") {
        c += env.Grid->GetCol();
        r += env.Grid->GetRow();
      }
      env.Grid->MoveTo(c, r);
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
      double result = env.Grid->Raw()->GetSum(l,t,r,b);
      Stack.push_back(Element(result));
    }else if(s == "avr" && H == 4){
      int l = VAL0;
      int t = VAL1;
      int r = VAL2;
      int b = VAL3;
      double result = env.Grid->Raw()->GetAvr(l,t,r,b);
      Stack.push_back(Element(result));
    }else if(s == "ShellOpen" && H == 1){
      String FileName = STR0;
      if(isUrl(FileName)){
        env.Grid->Raw()->OpenURL(FileName);
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
          throw MacroException(ope[0].Str() + "\nファイルが見つかりません。");
        }else{
          throw MacroException(ope[0].Str() + "\n実行に失敗しました。");
        }
      }
    }else if(s == "Open" && H == 1){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't open files.", ME_SECURITY);
      }
      env.Grid->ApplyPendingChanges();
      String filename = STR0;
      fmMain->MainGridDropFiles(NULL, 1, &filename);
      while (env.Grid->Raw()->FileOpenThread) {
        Sleep(100);
        Application->ProcessMessages();
      }
    }else if(s == "SaveAs" && (H == 1 || H == 2)){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't write to files.", ME_SECURITY);
      }
      env.Grid->ApplyPendingChanges();
      int typeIndex = -1;
      if (H == 1) {
        typeIndex = env.Grid->Raw()->TypeIndex;
      } else {
        int count = env.Grid->Raw()->TypeList.Count;
        for (int i = 0; i < count; i++) {
          if (env.Grid->Raw()->TypeList.Items(i)->Name == STR1) {
            typeIndex = i;
            break;
          }
        }
        if (typeIndex < 0) {
          throw MacroException("保存形式が不明です:" + STR1);
        }
      }
      fmMain->SaveAs(STR0, typeIndex);
    }else if(s == "Export" && H == 2){
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't write to files.", ME_SECURITY);
      }
      env.Grid->ApplyPendingChanges();
      fmMain->Export(STR0, STR1);
    }else if(s == "FileExists" && H == 1) {
      Stack.push_back(Element(FileExists(STR0) ? 1 : 0));
    }else if(s == "WriteToFile" && H == 2) {
      if (env.IsCellMacro) {
        throw MacroException("Cell Macro can't write to files.", ME_SECURITY);
      }
      String fileName = STR1;
      String dirName = ExtractFilePath(fileName);
      if(!DirectoryExists(dirName)){
        ForceDirectories(dirName);
      }
      TFileStream *fs = new TFileStream(fileName, fmCreate | fmShareDenyWrite);
      DynamicArray<System::Byte> array = TEncoding::UTF8->GetBytes(STR0);
      fs->Write(&(array[0]), array.Length);
      delete fs;
    }else if(s == "Sort" && H >= 1 && H <= 9) {
      int left = VAL0;
      int top = (H > 1 ? VAL1 : 1);
      int right = (H > 2 ? VAL2 : env.Grid->GetRight());
      int bottom = (H > 3 ? VAL3 : env.Grid->GetBottom());
      int col = (H > 4 ? VAL4 : left);
      bool direction = (H > 5 ? VAL5 : false);
      bool numSort = (H > 6 ? VAL6 : false);
      bool ignoreCase = (H > 7 ? VAL7 : false);
      bool ignoreZenhan = (H > 8 ? VAL8 : false);
      env.Grid->Sort(left, top, right, bottom, col, direction, numSort,
          ignoreCase, ignoreZenhan);
    }else if(s == "ReplaceAll" && (H == 2 || H == 5 || H == 9)) {
      String find = STR0;
      String replace = STR1;
      bool ignoreCase = (H > 2 ? VAL2 : false);
      bool word = (H > 3 ? VAL3 : false);
      bool regex = (H > 4 ? VAL4 : false);
      int left = (H > 5 ? VAL5 : env.Grid->GetLeft());
      int top = (H > 6 ? VAL6 : env.Grid->GetTop());
      int right = (H > 7 ? VAL7 : env.Grid->GetRight());
      int bottom = (H > 8 ? VAL8 : env.Grid->GetBottom());
      int count = env.Grid->ReplaceAll(
          find, replace, left, top, right, bottom, ignoreCase, regex, word);
      Stack.push_back(Element(count));
    }else if(s == "Select" && H == 4) {
      env.Grid->Select(VAL0, VAL1, VAL2, VAL3);
    }else{
      throw MacroException(notFoundMessage);
    }
}
//---------------------------------------------------------------------------
void TMacro::ExecOpe(char c){
  if (c == CMO_Goto || c == CMO_Jump || c == CMO_Minus || c == CMO_Inc ||
      c == CMO_Dec || c == '!') {
    if (Stack.size() < 1) { throw MacroException(c, ME_HIKISU); }
    Element ope = Stack.back();
    Stack.pop_back();
    if (c == CMO_Goto) {
      fs->Position = ope.Val();
      Stack.clear();
      LoopCount++;
      Application->ProcessMessages();
      if (MaxLoop > 0 && LoopCount > MaxLoop) {
        throw MacroException((String)"ループ回数が" + MaxLoop +
              "に達しました。処理を中断します。");
      }
      if (!RunningOk) { throw MacroException("中断しました"); }
    }
    else if (c == CMO_Jump) { fs->Position = ope.Val(); }
    else if (c == CMO_Minus) { Stack.push_back(Element(-(ope.Val()))); }
    else if (c == CMO_Inc) { ope.Sbst(Element(ope.Val() + 1)); }
    else if (c == CMO_Dec) { ope.Sbst(Element(ope.Val() - 1)); }
    else if (c == '!') { Stack.push_back(Element(((ope.Val() == 0) ? 1 : 0))); }
  } else if (c == CMO_ObjKey) {
    if (Stack.size() < 3) { throw MacroException(c, ME_HIKISU); }
    Element val = Stack.back(); Stack.pop_back();
    Element key = Stack.back(); Stack.pop_back();
    Element obj = Stack.back();
    String keyString = (key.Type == etVar ? key.Name() : key.Str());
    obj.GetMember(keyString).Sbst(val);
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
        if (ope2.Str() == "length") {
          Stack.push_back(Element(ope1.Str().Length()));
          return;
        } else if (ope2.isNum()) {
          String target = ope1.Str();
          int index = ope2.Val();
          if (index >= 0 && index < target.Length()) {
            Stack.push_back(Element(target.SubString(ope2.Val() + 1, 1)));
          } else {
            Stack.push_back(Element(""));
          }
          return;
        }
        throw MacroException("「.」の左がオブジェクトではありません："
            + (ope1.Type == etVar ? ope1.Name() : ope1.Str()));
      }
      Stack.push_back(ope1.GetMember(ope2.Str()));
    } else if (c == CMO_In) {
      if (ope2.Value().Type != etObject) {
        throw MacroException("in の右がオブジェクトではありません："
            + (ope2.Type == etVar ? ope2.Name() : ope2.Str()));
      }
      map<String, Element>& vars = env.GetObject(ope2.Val())->Vars;
      Stack.push_back(Element((vars.find(ope1.Str()) != vars.end()) ? 1 : 0));
    } else if (c == CMO_IfThen) {
      if (ope1.Val() == 0) {
        fs->Position = ope2.Val();
      }
    } else if (c == CMO_Cell) {
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
      throw MacroException("ユーザー関数が見つかりません。");
    }
    TObject *obj = modules->Objects[index];
    result = static_cast<TStream *>(obj);
  } catch (...) {
    throw MacroException(funcName + "\nユーザー関数が見つかりません。");
  }
  return result;
}
//---------------------------------------------------------------------------
Element TMacro::Do(String FileName, const std::vector<Element> &AStack,
                   int x, int y, Element *thisPtr) {
  Element ReturnValue;
  // 再帰呼び出し対応のため、メソッド終了時にStreamのPositionを元に戻す
  long oldpc = 0L;
  Stack = AStack;
  fs = NULL;
  try{
    fs = GetStreamFor(FileName);
    oldpc = fs->Position;
    fs->Position = 0;

    env.Vars.clear();
    env.Vars["x"] = Element(((x >= 0) ? x : env.Grid->GetCol()));
    env.Vars["y"] = Element(((y >= 0) ? y : env.Grid->GetRow()));
    env.Vars["Left"] = Element(env.Grid->GetLeft());
    env.Vars["Top"] = Element(env.Grid->GetTop());
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
        if(c == CMO_Return){
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
  GridProxy grid(fmMain->MainGrid, IsCellMacro);
  TMacro mcr(IO, MaxLoop, Modules, TEnvironment(IsCellMacro, &grid, NULL));
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
        e.Message = "引数の数が足りません:" + e.Message;
      }
      Application->MessageBox((FileName + "\n" + e.Message).c_str(),
                              L"Cassava Macro Interpreter", 0);
    }
  } catch (Exception *e) {
    if (IsCellMacro) {
      RunningCount--;
      throw e;
    } else {
      Application->MessageBox((FileName + "\n" + e->Message).c_str(),
                              L"Cassava Macro Interpreter", 0);
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
