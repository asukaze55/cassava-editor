//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include <boost\regex.hpp>
#include <cmath>
#include <dialogs.hpp>
#include <map>
#include <process.h>
#include <vector>

#include "Find.h"
#include "Macro.h"
#include "MultiLineInputBox.h"
#include "AutoOpen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
enum ElementType {
  etErr,
  etString,
  etNumber,
  etVar,
  etCell,
  etSystem,
  etObject,
  etFunction,
  etRegExp
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
  MacroException(const String& Mes, MacroExceptionType t = ME_GENERIC)
      : Message(Mes), Type(t) {}
};
//---------------------------------------------------------------------------
class GridProxy {
private:
  int deletingTop;
  int deletingRows;
  TMainGrid *grid;
  bool readOnly;

  int RXtoAX(int x) const { return x - grid->DataLeft + 1; }
  int RYtoAY(int y) const { return y - grid->DataTop + 1; }
  int AXtoRX(int x) const { return x + grid->DataLeft - 1; }
  int AYtoRY(int y) const { return y + grid->DataTop - 1; }
  int ax(int x) const { return x; }
  int rx(int x) const { return AXtoRX(ax(x)); }
  int ay(int y) const { return y >= deletingTop ? y + deletingRows : y; }
  int ry(int y) const { return AYtoRY(ay(y)); }

public:
  GridProxy(TMainGrid *Grid, bool ReadOnly)
      : deletingTop(0), deletingRows(0), grid(Grid), readOnly(ReadOnly) {}

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

  void SetCell(int x, int y, const String& value) {
    if (readOnly) {
      throw MacroException("Can't Update Cells.", ME_SECURITY);
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
    return RXtoAX(grid->Selection.Left);
  }

  int GetSelTop() {
    ApplyPendingChanges();
    return RYtoAY(grid->Selection.Top);
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

  int ReplaceAll(const String& find, const String& replace, int left, int top,
      int right, int bottom, bool ignoreCase, bool regex, bool word) {
    ApplyPendingChanges();
    return grid->ReplaceAll(find, replace, rx(left), ry(top), rx(right),
                            ry(bottom), !ignoreCase, regex, word);
  }

  void Select(int left, int top, int right, int bottom) {
    ApplyPendingChanges();
    grid->SetSelection(rx(std::min(left, right)), rx(std::max(left, right)),
                       ry(std::min(top, bottom)), ry(std::max(top, bottom)));
  }
};
//---------------------------------------------------------------------------
class TEnvironment;
//---------------------------------------------------------------------------
class Element {
private:
  String st = "";
  union {
    double vl;
    struct { int x; int y; } cell;
  };
  TEnvironment *env = nullptr;
  Element &GetVar() const;
public:
  ElementType Type;
  bool isNum() const;

  Element() : Type(etErr) {}
  explicit Element(double d) : vl(d), Type(etNumber) {}
  explicit Element(double d, const String& s, ElementType t, TEnvironment *e)
      : st(s), vl(d), env(e), Type(t) {}
  explicit Element(const String& s) : st(s), Type(etString) {}
  explicit Element(const String& s, ElementType t, TEnvironment *e)
      : st(s), env(e), Type(t) {}
  explicit Element(int x, int y, TEnvironment *e)
      : cell({x, y}), env(e), Type(etCell) {}

  double Val() const;
  String Str() const;
  String Name() const { return st; }
  Element Value() const;
  void Assign(const Element &e);
  Element GetMember(const String& name) const;
  std::map<String, Element> GetMembers() const;
  bool HasMember(const String& name) const;
};
//---------------------------------------------------------------------------
class TEnvironment {
private:
  TEnvironment *global;
  std::vector<std::unique_ptr<TEnvironment>> objects;
public:
  std::map<String, Element> Vars;
  bool ReadOnly;
  GridProxy *Grid;

  TEnvironment(bool readOnly, GridProxy *grid,  TEnvironment *gl)
      : global(gl), Vars(), ReadOnly(readOnly), Grid(grid) {}
  TEnvironment(TEnvironment&& env) = default;

  TEnvironment *GetGlobal() { return global ? global : this; }
  TEnvironment CreateSubEnvironment() {
    return TEnvironment(ReadOnly, Grid, GetGlobal());
  }
  std::unique_ptr<TEnvironment> NewObject() {
    return std::make_unique<TEnvironment>(ReadOnly, Grid, GetGlobal());
  }
  std::vector<std::unique_ptr<TEnvironment>>& GetObjects() {
    return GetGlobal()->objects;
  }
  TEnvironment *GetObject(int id) {
    return GetObjects()[id].get();
  }
};
//---------------------------------------------------------------------------
class TMacro{
private:
  const TByteVector& GetByteVector(const String& FileName);
  void ExecOpe(TMacroOperator c, size_t &p);
  void ExecMethod(const String& name, int paramCount,
      const std::vector<Element>& params, bool isLambda);
  void ExecPrimitiveMethod(const String& s, int paramCount,
      const std::vector<Element>& params);
  void ExecFnc(String s);
  std::vector<Element> Stack;
  int LoopCount;
  bool canWriteFile;
  EncodedWriter *fs_io;
  const TMacroContext &Context;
  TEnvironment env;
public:
  int MaxLoop;
  Element Do(const String& FileName, const std::vector<Element>& AStack,
             int x = -1, int y = -1, Element *thisPtr = nullptr);

  TMacro(EncodedWriter *io, int ml, const TMacroContext &context,
          TEnvironment&& e)
      : canWriteFile(io), fs_io(io), Context(context), env(std::move(e)),
        MaxLoop(ml) {}
};
//---------------------------------------------------------------------------
static int RunningCount = 0;
static bool RunningOk = true;
//---------------------------------------------------------------------------
static double ToDouble(const String& str, double def)
{
  try {
    String temp = str;
    for (int i = temp.Length(); i > 0; i--) {
      if (temp[i] == ',') {
        temp.Delete(i,1);
      }
    }
    return temp.ToDouble();
  } catch(...) {
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
    return ToDouble(env->Grid->GetCell(cell.x, cell.y), 0);
  } else if (Type == etVar) {
    Element &e = GetVar();
    if (e.Type == etErr) {
      if (e.st != "") { throw MacroException(e.st); }
      throw MacroException(L"値が代入されていない変数・フィールドです：" + st);
    }
    return e.Val();
  } else if (Type == etSystem) {
    if (st == "Col") {
      return env->Grid->GetCol();
    } else if (st == "Row") {
      return env->Grid->GetRow();
    } else if (st == "Right") {
      return env->Grid->GetRight();
    } else if (st == "Bottom") {
      return env->Grid->GetBottom();
    } else if (st == "SelLeft") {
      return env->Grid->GetSelLeft();
    } else if (st == "SelTop") {
      return env->Grid->GetSelTop();
    } else if (st == "SelRight") {
      return env->Grid->GetSelRight();
    } else if (st == "SelBottom") {
      return env->Grid->GetSelBottom();
    }
  } else if (Type == etNumber || Type == etObject) {
    return vl;
  }
  return ToDouble(st, 0);
}
//---------------------------------------------------------------------------
String Element::Str() const
{
  if (Type == etErr && st != "") {
    throw MacroException(st);
  } else if (Type == etCell) {
    return env->Grid->GetCell(cell.x, cell.y);
  } else if (Type == etVar) {
    Element &e = GetVar();
    if (e.Type == etErr) {
      if (e.st != "") { throw MacroException(e.st); }
      throw MacroException(L"値が代入されていない変数・フィールドです：" + st);
    }
    return e.Str();
  } else if (Type == etSystem) {
    return String(Val());
  } else if (Type == etObject) {
    String s = "{";
    std::map<String, Element>& vars = env->GetObject(vl)->Vars;
    bool first = true;
    for (const auto& [key, value] : vars) {
      if (value.Type == etErr) {
        continue;
      }
      if (first) {
        first = false;
      } else {
        s += ", ";
      }
      s += key + ": ";
      if (value.Type == etObject) {
        s += "{...}";
      } else if (value.isNum()) {
        s += value.Str();
      } else if (value.Str().Pos("\n") > 0) {
        s += "...";
      } else {
        s += "\"" + value.Str() + "\"";
      }
    }
    return s + "}";
  } else if (st != "") {
    return st;
  } else if (Type == etNumber) {
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
      throw MacroException(L"値が代入されていない変数・フィールドです：" + st);
    }
    return e;
  } else if (Type == etCell) {
    if (isNum()) {
      return Element(Val(), Str(), etNumber, nullptr);
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
  if (Type == etVar) {
    return GetVar().isNum();
  } else if (Type == etCell) {
    return env->Grid->IsNumberAtACell(cell.x, cell.y);
  }
  return Type == etNumber || Type == etSystem;
}
//---------------------------------------------------------------------------
void Element::Assign(const Element &e)
{
  if (Type == etVar) {
    GetVar() = e.Value();
  } else if (Type == etSystem) {
    int v = e.Value().Val();
    GridProxy *g = env->Grid;
    if (st == "Col") {
      g->MoveTo(v, g->GetRow());
    } else if (st == "Row") {
      g->MoveTo(g->GetCol(), v);
    } else if (st == "Right") {
      int newRight = v;
      int currentRight = g->GetRight();
      if (newRight < currentRight) {
        g->DeleteCol(newRight + 1, currentRight);
      } else if (newRight > currentRight) {
        g->InsertCol(currentRight + 1, newRight);
      }
      if (g->GetCol() > v) { g->MoveTo(v, g->GetRow()); }
      g->ChangeColCount(newRight + (g->RowCounterShown() ? 2 : 1));
    } else if (st == "Bottom") {
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
      if (st == "SelLeft") {
        Sel.Left = v;
        if (Sel.Right < Sel.Left) { Sel.Right = Sel.Left; }
      } else if (st == "SelTop") {
        Sel.Top = v;
        if (Sel.Bottom < Sel.Top) { Sel.Bottom = Sel.Top; }
      } else if (st == "SelRight") {
        Sel.Right = v;
        if (Sel.Left > Sel.Right) { Sel.Left = Sel.Right; }
      } else if (st == "SelBottom") {
        Sel.Bottom = v;
        if (Sel.Top > Sel.Bottom) { Sel.Top = Sel.Bottom; }
      }
      g->Select(Sel.Left, Sel.Top, Sel.Right, Sel.Bottom);
    }
  } else if (Type == etCell) {
    env->Grid->SetCell(cell.x, cell.y, e.Value().Str());
  } else {
    throw MacroException(L"代入先が左辺値ではありません：" + Str());
  }
}
//---------------------------------------------------------------------------
Element Element::GetMember(const String& name) const
{
  return Element(name, etVar, env->GetObject(Val()));
}
//---------------------------------------------------------------------------
std::map<String, Element> Element::GetMembers() const
{
  if (Type != etObject) {
    return {};
  }
  return env->GetObject(vl)->Vars;
}
//---------------------------------------------------------------------------
bool Element::HasMember(const String& name) const
{
  if (Type != etObject) {
    return false;
  }
  return env->GetObject(vl)->Vars.count(name) > 0;
}
//---------------------------------------------------------------------------
String NormalizeNewLine(const String& Val)
{
  TReplaceFlags replaceAll = TReplaceFlags() << rfReplaceAll;
  return StringReplace(Val, "\r\n", "\n", replaceAll);
}
//---------------------------------------------------------------------------
void TMacro::ExecMethod(const String& name, int paramCount,
    const std::vector<Element>& params, bool isLambda)
{
  Element obj = params[0].Value();
  String objName =
      (params[0].Type == etVar ? params[0].Name() : params[0].Str());
  if (!isLambda && obj.Type != etObject) {
    ExecPrimitiveMethod(name, paramCount, params);
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
      Stack.emplace_back(obj.Str());
      return;
    }
    throw;
  }
  String funcName = funcPtr.Str();
  try {
    GetByteVector(funcPtr.Str());
  } catch (...) {
    throw MacroException(isLambda
        ? L"関数オブジェクトではありません：" + objName
        : L"メソッドではありません：" + objName + "." + name);
  }
  int slash = funcName.LastDelimiter("/");
  bool isVarArg = (funcName[slash + 1] == '+');
  int funcArity = funcName.SubString(slash + 1, funcName.Length()).ToIntDef(0);
  if (!isVarArg && funcArity != paramCount - 1) {
    throw MacroException(L"引数の数が一致しません。\n"
        + (isLambda ? objName : name) + L"/" + funcArity + L" に引数が "
        + (paramCount - 1) + L" 個渡されています。");
  } else if (isVarArg && funcArity > paramCount - 1) {
    throw MacroException(L"引数の数が一致しません。\n"
        + (isLambda ? objName : name) + L" には引数が " + funcArity
        + L" 個以上必要です。");
  }

  std::vector<Element> argStack;
  for (int i = 1; i < paramCount; i++) {
    argStack.emplace_back(params[i].Value());
  }
  int ml = (MaxLoop > 0) ? MaxLoop-LoopCount : 0;
  TMacro mcr(fs_io, ml, Context, env.CreateSubEnvironment());
  const Element &r = mcr.Do(funcName, argStack, -1, -1, &obj);
  if (r.Type != etErr) {
    Stack.push_back(r);
  } else {
    Stack.emplace_back(isLambda
            ? L"関数オブジェクトが値を返しません：" + objName
            : L"メソッドは値を返しません：" + name + "/" + (paramCount - 1),
        etErr, nullptr);
  }
}
//---------------------------------------------------------------------------
class RegExp {
 public:
  String pattern;
  String flags;
  bool isRegExp;
  bool isReplaceAll;
  bool isIgnoreCase;

  RegExp(const String& Pattern, const String& Flags, bool IsRegExp)
      : pattern(Pattern), flags(Flags), isRegExp(IsRegExp),
        isReplaceAll(flags.Pos("g") > 0), isIgnoreCase(flags.Pos("i") > 0) {}

  String Replace(const String& Target, const String& ReplaceText) {
    if (!isRegExp) {
      TReplaceFlags replaceFlags;
      if (isReplaceAll) { replaceFlags << rfReplaceAll; }
      if (isIgnoreCase) { replaceFlags << rfIgnoreCase; }
      return StringReplace(Target, pattern, ReplaceText, replaceFlags);
    }

    boost::regex_constants::match_flag_type boostFlags =
        boost::format_perl | boost::match_single_line | boost::match_not_null;
    if (!isReplaceAll) { boostFlags |= boost::format_first_only; }
    return boost::regex_replace(
        std::wstring(Target.c_str()), BoostRegEx(),
        std::wstring(ReplaceText.c_str()), boostFlags).c_str();
  }

  int Search(const String& Target) {
    wchar_t *target = Target.c_str();
    boost::wregex regex = BoostRegEx();
    boost::wcmatch match;
    if (boost::regex_search(target, match, regex)) {
      return match[0].first - target;
    }
    return -1;
  }

 private:
  boost::wregex BoostRegEx() const {
    return isIgnoreCase ? boost::wregex(pattern.c_str(), boost::regex::icase)
                        : boost::wregex(pattern.c_str());
  }
};
//---------------------------------------------------------------------------
static RegExp ParseRegExp(const Element& e)
{
  Element obj = e.Value();
  String str = obj.Str();
  if (obj.Type != etRegExp) {
    return RegExp(str, "", false);
  }
  int p = str.LastDelimiter("/");
  String pattern = str.SubString(2, p - 2);
  String flags = str.SubString(p + 1, str.Length());
  return RegExp(pattern, flags, true);
}
//---------------------------------------------------------------------------
void TMacro::ExecPrimitiveMethod(const String& s, int paramCount,
                                 const std::vector<Element>& params)
{
  if (s == "endsWith" && (paramCount == 2 || paramCount == 3)) {
    String target = params[0].Str();
    String search = params[1].Str();
    int length = (paramCount > 2 ? params[2].Val() : target.Length());
    Stack.emplace_back(target.SubString(
        length - search.Length() + 1, search.Length()) == search ? 1 : 0);
  } else if (s == "includes" && (paramCount == 2 || paramCount == 3)) {
    String target = params[0].Str();
    String search = params[1].Str();
    int from = (paramCount > 2 ? params[2].Val() : 0);
    if (search == "") {
      Stack.emplace_back(1);
      return;
    }
    int pos = target.SubString(from + 1, target.Length() - from).Pos(search);
    Stack.emplace_back(pos > 0 ? 1 : 0);
  } else if (s == "indexOf" && (paramCount == 2 || paramCount == 3)) {
    String target = params[0].Str();
    String search = params[1].Str();
    int from = (paramCount > 2 ? params[2].Val() : 0);
    if (from < 0) {
      from = 0;
    }
    if (search == "") {
      Stack.emplace_back(std::min(from, target.Length()));
      return;
    }
    int pos = target.SubString(from + 1, target.Length() - from).Pos(search);
    if (pos == 0) {
      Stack.emplace_back(-1);
      return;
    }
    Stack.emplace_back(pos + from - 1);
  } else if (s == "lastIndexOf" && (paramCount == 2 || paramCount == 3)) {
    String target = params[0].Str();
    String search = params[1].Str();
    int from = (paramCount > 2 ? params[2].Val() : target.Length());
    if (from < 0) {
      from = 0;
    }
    if (search == "") {
      Stack.emplace_back(std::min(from, target.Length()));
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
    Stack.emplace_back(lastPos - 1);
  } else if (s == "padEnd" && (paramCount == 2 || paramCount == 3)) {
    String target = params[0].Str();
    int length = params[1].Val();
    String pad = (paramCount > 2 ? params[2].Str() : L" ");
    if (target.Length() >= length) {
      Stack.emplace_back(target);
      return;
    }
    while (target.Length() < length) {
      target += pad;
    }
    Stack.emplace_back(target.SubString(1, length));
  } else if (s == "padStart" && (paramCount == 2 || paramCount == 3)) {
    String target = params[0].Str();
    int length = params[1].Val();
    String pad = (paramCount > 2 ? params[2].Str() : L" ");
    if (target.Length() >= length) {
      Stack.emplace_back(target);
      return;
    }
    int lengthToPad = length - target.Length();
    String prefix = "";
    while (lengthToPad >= pad.Length()) {
      prefix += pad;
      lengthToPad -= pad.Length();
    }
    Stack.emplace_back(prefix + pad.SubString(1, lengthToPad) + target);
  } else if (s == "repeat" && paramCount == 2) {
    String target = params[0].Str();
    String result = "";
    int count = params[1].Val();
    for (int i = 0; i < count; i++) {
      result += target;
    }
    Stack.emplace_back(result);
  } else if (s == "replace" && paramCount == 3) {
    Stack.emplace_back(
        ParseRegExp(params[1]).Replace(params[0].Str(), params[2].Str()));
  } else if (s == "replaceAll" && paramCount == 3) {
    RegExp regExp = ParseRegExp(params[1]);
    if (regExp.isRegExp && !regExp.isReplaceAll) {
      throw MacroException(
          L"replaceAll で使用する正規表現には g フラグが必要です：" +
          params[1].Str());
    }
    regExp.isReplaceAll = true;
    Stack.emplace_back(regExp.Replace(params[0].Str(), params[2].Str()));
  } else if (s == "search" && paramCount == 2) {
    Stack.emplace_back(ParseRegExp(params[1]).Search(params[0].Str()));
  } else if (s == "startsWith" && (paramCount == 2 || paramCount == 3)) {
    String search = params[1].Str();
    int position = (paramCount > 2 ? params[2].Val() : 0);
    Stack.emplace_back(
        params[0].Str().SubString(position + 1, search.Length()) == search
            ? 1 : 0);
  } else if (s == "substring" && (paramCount == 2 || paramCount == 3)) {
    String target = params[0].Str();
    int arg1 = params[1].Val();
    int arg2 = (paramCount > 2 ? params[2].Val() : target.Length());
    int start = std::min(arg1, arg2);
    if (start < 0) {
      start = 0;
    }
    if (start > target.Length()) {
      start = target.Length();
    }
    int end = std::max(arg1, arg2);
    if (end < 0) {
      end = 0;
    }
    if (end > target.Length()) {
      end = target.Length();
    }
    Stack.emplace_back(target.SubString(start + 1, end - start));
  } else if (s == "toLowerCase" && paramCount == 1) {
    Stack.emplace_back(params[0].Str().LowerCase());
  } else if (s == "toString" && paramCount == 1) {
    Stack.emplace_back(params[0].Str());
  } else if (s == "toUpperCase" && paramCount == 1) {
    Stack.emplace_back(params[0].Str().UpperCase());
  } else if (s == "trim" && paramCount == 1) {
    Stack.emplace_back(params[0].Str().Trim());
  } else if (s == "trimEnd" && paramCount == 1) {
    Stack.emplace_back(params[0].Str().TrimRight());
  } else if (s == "trimStart" && paramCount == 1) {
    Stack.emplace_back(params[0].Str().TrimLeft());
  } else {
    throw MacroException(
        L"定義されていないメソッドです：" + s + "/" + (paramCount - 1));
  }
}
//---------------------------------------------------------------------------
void CallOnClick(TMenuItem *menuItem)
{
  if (menuItem && menuItem->OnClick) {
    menuItem->OnClick(menuItem);
  }
}
//---------------------------------------------------------------------------
class UserDialog {
 private:
  std::unique_ptr<TForm> Form;
  std::map<TObject *, Element> ElementMap;

  TControl *ConvertDialogControl(
      const Element& element, TWinControl *parent, bool *hasDefault);
  void __fastcall ButtonClick(TObject *Sender);
  void __fastcall EditChange(TObject *Sender);
 public:
  String ReturnValue;

  UserDialog(const Element& element, const String& caption);
  void ShowModal() {
    Form->ShowModal();
  }
};
//---------------------------------------------------------------------------
UserDialog::UserDialog(const Element& element, const String& caption)
{
  Form = std::make_unique<TForm>(static_cast<TComponent*>(nullptr));
  Form->BorderStyle = bsDialog;
  Form->Caption = caption;
  Form->Position = poMainFormCenter;
  bool hasDefault = false;
  TControl *control =
      ConvertDialogControl(element.Value(), Form.get(), &hasDefault);
  control->Left = 8;
  control->Top = 8;
  Form->ClientHeight = control->Height + 16;
  Form->ClientWidth = control->Width + 16;
  ReturnValue = "";
}
//---------------------------------------------------------------------------
TControl *UserDialog::ConvertDialogControl(
    const Element& element, TWinControl *parent, bool *hasDefault)
{
  if (!element.HasMember("tagName")) {
    TLabel *label = new TLabel(Form.get());
    label->Parent = parent;
    label->Caption = element.Str();
    label->Height = Form->Canvas->TextHeight(label->Caption) + 12;
    label->Width = Form->Canvas->TextWidth(label->Caption);
    return label;
  }

  String tagName = element.GetMember("tagName").Str().UpperCase();
  Element childNodes = element.GetMember("childNodes").Value();
  if (tagName == "BUTTON") {
    TButton *button = new TButton(Form.get());
    button->Parent = parent;
    button->Caption =  childNodes.HasMember("0")
        ? childNodes.GetMember("0").Str(): L" ";
    button->ClientHeight = Form->Canvas->TextHeight(button->Caption) + 16;
    button->ClientWidth = Form->Canvas->TextWidth(button->Caption) + 32;
    if (!*hasDefault) {
      button->Default = true;
      *hasDefault = true;
    }
    button->OnClick = ButtonClick;
    ElementMap[button] = element;
    return button;
  } else if (tagName == "INPUT") {
    TEdit *edit = new TEdit(Form.get());
    edit->Parent = parent;
    int size = element.HasMember("size") ? element.GetMember("size").Val() : 20;
    edit->Width = 16 * size;
    if (element.HasMember("value")) {
      edit->Text = element.GetMember("value").Str();
    } else {
      element.GetMember("value").Assign(Element(""));
    }
    edit->OnChange = EditChange;
    ElementMap[edit] = element;
    return edit;
  } else if (tagName == "TEXTAREA") {
    TMemo *memo = new TMemo(Form.get());
    memo->Parent = parent;
    int cols = element.HasMember("cols") ? element.GetMember("cols").Val() : 20;
    memo->Width = 16 * cols;
    int rows = element.HasMember("rows") ? element.GetMember("rows").Val() : 2;
    memo->Height = 24 * rows;
    String text = "";
    if (element.HasMember("value")) {
      text = element.GetMember("value").Str();
    } else if (childNodes.HasMember("0")) {
      text = childNodes.GetMember("0").Str();
      element.GetMember("value").Assign(Element(text));
    } else {
      element.GetMember("value").Assign(Element(""));
    }
    memo->Text =
        StringReplace(text, "\n", "\r\n", TReplaceFlags() << rfReplaceAll);
    memo->OnChange = EditChange;
    ElementMap[memo] = element;
    return memo;
  } else {
    TPanel *panel = new TPanel(Form.get());
    panel->BevelOuter = bvNone;
    panel->Parent = parent;
    int height = 0;
    int left = 0;
    int top = 0;
    int width = 0;
    for (int i = 0; i < childNodes.GetMember("length").Val(); i++) {
      const Element &childNode = childNodes.GetMember(String(i)).Value();
      bool isBlock = childNode.HasMember("tagName") &&
          childNode.GetMember("tagName").Str().UpperCase() == "DIV";
      TControl *child = ConvertDialogControl(childNode, panel, hasDefault);
      child->Left = left;
      child->Top = top;
      if (top + child->Height > height) {
        height = top + child->Height;
      }
      if (left + child->Width > width) {
        width = left + child->Width;
      }
      if (isBlock) {
        left = 0;
        top = height;
      } else {
        left = left + child->Width;
      }
    }
    panel->ClientWidth = width;
    panel->ClientHeight = height;
    return panel;
  }
}
//---------------------------------------------------------------------------
void __fastcall UserDialog::ButtonClick(TObject *Sender)
{
  const Element &element = ElementMap[Sender];
  if (element.HasMember("value")) {
    ReturnValue = element.GetMember("value").Str();
  }
  Form->Close();
}
//---------------------------------------------------------------------------
void __fastcall UserDialog::EditChange(TObject *Sender)
{
  ElementMap[Sender].GetMember("value").Assign(
      Element(NormalizeNewLine(static_cast<TCustomEdit *>(Sender)->Text)));
}
//---------------------------------------------------------------------------
void TMacro::ExecFnc(String s)
{
  int paramCount = s[1];
  s.Delete(1,1);

  std::vector<Element> params;
  params.assign(Stack.end() - paramCount, Stack.end());
  Stack.erase(Stack.end() - paramCount, Stack.end());

  String notFoundMessage = L"定義されていない関数です:" + s + "/" + paramCount;
  if (s[1] == '.' || s[1] == '>') {
    bool isLambda = (s[1] == '>');
    s.Delete(1,1);
    try {
      ExecMethod(isLambda ? L"" : s, paramCount, params, isLambda);
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
      notFoundMessage = L"関数オブジェクトではありません：" + params[0].Name();
    }
    paramCount--;
    params.erase(params.begin());
  }

  if (s[1] == '$') {
    s.Delete(1,1);
    int ml = ((MaxLoop > 0) ? MaxLoop-LoopCount : 0);
    TMacro mcr(fs_io, ml, Context, env.CreateSubEnvironment());
    String funcName;
    if (Context.HasModule(s + "/" + paramCount)) {
      funcName = s + "/" + paramCount;
    } else {
      int minArgs;
      for (minArgs = paramCount; minArgs >= 0; minArgs--) {
        if (Context.HasModule(s + "/+" + minArgs)) {
          funcName = s + "/+" + minArgs;
          break;
        }
      }
      if (minArgs < 0) {
        throw MacroException(
            s + "/" + paramCount + L"\nユーザー関数が見つかりません。");
      }
    }
    std::vector<Element> argStack;
    for (int i = 0; i < paramCount; i++) {
      argStack.emplace_back(params[i].Value());
    }
    const Element &r = mcr.Do(funcName, argStack);
    if (r.Type != etErr) {
      Stack.push_back(r);
    } else {
      Stack.emplace_back(
          L"関数は値を返しません：" + s + "/" + paramCount, etErr, nullptr);
    }
  } else if (s == "{}") {
    std::vector<std::unique_ptr<TEnvironment>>& objects = env.GetObjects();
    Stack.emplace_back(objects.size(), "", etObject, env.GetGlobal());
    objects.push_back(env.NewObject());
  } else if (s == "(constructor)") {
    Element obj = Stack.back();
    Element funcPtr = obj.GetMember("constructor");
    Stack.pop_back();
    int ml = ((MaxLoop > 0) ? MaxLoop-LoopCount : 0);
    TMacro mcr(fs_io, ml, Context, env.CreateSubEnvironment());
    mcr.Do(funcPtr.Str(), Stack, -1, -1, &obj);
    Stack.push_back(obj);
  } else if (s == "func=") {
    int count = paramCount / 2;
    for (int i = 0; i < count; i++) {
      params[i+count].Assign(params[i]);
    }
  } else if (s == "swap" && paramCount == 2) {
    Element t0 = params[0].Value();
    Element t1 = params[1].Value();
    params[0].Assign(t1);
    params[1].Assign(t0);
  } else if (s == "MessageBox") {
    if (env.ReadOnly) {
      throw MacroException("Can't show dialogs.", ME_SECURITY);
    }
    env.Grid->ApplyPendingChanges();
    String text = (paramCount >= 1 ? params[0].Str() : L"ブレークポイントです");
    int flag = (paramCount >= 2 ? params[paramCount - 1].Val() : MB_OK);
    String caption = (paramCount >= 3 ? params[1].Str() : L"Cassava Macro");
    Stack.emplace_back(
        Application->MessageBox(text.c_str(), caption.c_str(), flag));
  } else if (s == "InputBox") {
    if (env.ReadOnly) {
      throw MacroException("Can't show dialogs.", ME_SECURITY);
    }
    env.Grid->ApplyPendingChanges();
    String Text = (paramCount >= 1) ? params[0].Str() : L"";
    String Value = (paramCount >= 2) ? (params[paramCount-1].Str()) : L"";
    String Caption = (paramCount >= 3) ? params[1].Str() : L"Cassava Macro";
    bool isOk = InputQuery(Caption, Text, Value);
    if (isOk) {
      Stack.emplace_back(Value);
    } else {
      throw MacroException(L"キャンセルされました。", ME_CANCELED);
    }
  } else if (s == "InputBoxMultiLine") {
    if (env.ReadOnly) {
      throw MacroException("Can't show dialogs.", ME_SECURITY);
    }
    env.Grid->ApplyPendingChanges();
    String Text = (paramCount >= 1) ? params[0].Str() : L"";
    String Value = (paramCount >= 2) ? (params[paramCount-1].Str()) : L"";
    String Caption = (paramCount >= 3) ? params[1].Str() : L"Cassava Macro";
    bool isOk = InputBoxMultiLine(Caption, Text, Value);
    if (isOk) {
      Stack.emplace_back(NormalizeNewLine(Value));
    } else {
      throw MacroException(L"キャンセルされました。", ME_CANCELED);
    }
  } else if (s == "GetRowHeight" && paramCount == 0) {
    Stack.emplace_back(env.Grid->Raw()->DefaultRowHeight);
  } else if (s == "GetRowHeight" && paramCount == 1) {
    Stack.emplace_back(env.Grid->Raw()->RowHeights[params[0].Val()]);
  } else if (s == "SetRowHeight" && paramCount == 1) {
    env.Grid->Raw()->DefaultRowHeight = params[0].Val();
  } else if (s == "SetRowHeight" && paramCount == 2) {
    env.Grid->Raw()->RowHeights[params[0].Val()] = params[1].Val();
  } else if (s == "AdjustRowHeight" && paramCount == 1) {
    env.Grid->Raw()->SetHeight(params[0].Val(), false);
  } else if (s == "GetColWidth" && paramCount == 0) {
    Stack.emplace_back(env.Grid->Raw()->DefaultColWidth);
  } else if (s == "GetColWidth" && paramCount == 1) {
    Stack.emplace_back(env.Grid->Raw()->ColWidths[params[0].Val()]);
  } else if (s == "SetColWidth" && paramCount == 1) {
    env.Grid->Raw()->DefaultColWidth = params[0].Val();
  } else if (s == "SetColWidth" && paramCount == 2) {
    env.Grid->Raw()->ColWidths[params[0].Val()] = params[1].Val();
  } else if (s == "AdjustColWidth" && paramCount == 1) {
    env.Grid->Raw()->SetWidth(params[0].Val());
  } else if (s == "GetYear" && paramCount == 0) {
    unsigned short year, month, day;
    Date().DecodeDate(&year, &month, &day);
    Stack.emplace_back(year);
  } else if (s == "GetMonth" && paramCount == 0) {
    unsigned short year, month, day;
    Date().DecodeDate(&year, &month, &day);
    Stack.emplace_back(month);
  } else if (s == "GetDate" && paramCount == 0) {
    unsigned short year, month, day;
    Date().DecodeDate(&year, &month, &day);
    Stack.emplace_back(day);
  } else if (s == "GetHours" && paramCount == 0) {
    unsigned short hours, minutes, seconds, msec;
    Time().DecodeTime(&hours, &minutes, &seconds, &msec);
    Stack.emplace_back(hours);
  } else if (s == "GetMinutes" && paramCount == 0) {
    unsigned short hours, minutes, seconds, msec;
    Time().DecodeTime(&hours, &minutes, &seconds, &msec);
    Stack.emplace_back(minutes);
  } else if (s == "GetSeconds" && paramCount == 0) {
    unsigned short hours, minutes, seconds, msec;
    Time().DecodeTime(&hours, &minutes, &seconds, &msec);
    Stack.emplace_back(seconds);
  } else if (s == "GetFilePath" && paramCount == 0) {
    String Path = ExtractFilePath(fmMain->FileName);
    if (Path != "" && (*(Path.LastChar()) != '\\')) { Path += '\\'; }
    Stack.emplace_back(Path);
  } else if (s == "GetFileName" && paramCount == 0) {
    Stack.emplace_back(ExtractFileName(fmMain->FileName));
  } else if (s == "GetSettingPath" && paramCount == 0) {
    Stack.emplace_back(fmMain->Pref->Path);
  } else if (s == "GetStatusBarCount" && paramCount == 0) {
    Stack.emplace_back(fmMain->StatusBar->Panels->Count - 1);
  } else if (s == "SetStatusBarCount" && paramCount == 1) {
    int val = params[0].Val() + 1;
    if (val < 1) {
      val = 1;
    }
    TStatusPanels *panels = fmMain->StatusBar->Panels;
    while (panels->Count > val) {
      panels->Delete(panels->Count - 1);
    }
    while (panels->Count < val) {
      panels->Add();
    }
  } else if (s == "GetStatusBarText" && paramCount == 1) {
    int val = params[0].Val();
    String data = "";
    if (val >= 0 && val < fmMain->StatusBar->Panels->Count) {
      data = fmMain->StatusBar->Panels->Items[val]->Text;
    }
    Stack.emplace_back(data);
  } else if (s == "SetStatusBarText" && paramCount == 2) {
    int val = params[0].Val();
    if (val >= 0 && val < fmMain->StatusBar->Panels->Count) {
      fmMain->StatusBar->Panels->Items[val]->Text = params[1].Str();
    }
  } else if (s == "GetStatusBarWidth" && paramCount == 1) {
    int val = params[0].Val();
    int data = 0;
    if (val >= 0 && val < fmMain->StatusBar->Panels->Count) {
      data = fmMain->StatusBar->Panels->Items[val]->Width;
    }
    Stack.emplace_back(data);
  } else if (s == "SetStatusBarWidth" && paramCount == 2) {
    int val = params[0].Val();
    if (val >= 0 && val < fmMain->StatusBar->Panels->Count) {
      fmMain->StatusBar->Panels->Items[val]->Width = params[1].Val();
    }
  } else if (s == "SetStatusBarPopUp" && paramCount == 3) {
    const Element &value2 = params[2].Value();
    if (value2.Type == etObject) {
      throw MacroException(
          s + L" の第 3 引数にはキャプチャを使用しないラムダ式が必要です。");
    }
    fmMain->StatusBarPopUp[params[0].Val()] = {params[1].Str(), value2.Str()};
  } else if (s == "LoadIniSetting" && paramCount == 0) {
    fmMain->ReadIni();
  } else if (s == "SaveIniSetting" && paramCount == 0) {
    fmMain->WriteIni(false);
  } else if (s == "GetIniSetting" && paramCount ==  2) {
    IniFile ini = fmMain->Pref->GetInifile();
    Stack.emplace_back(ini.ReadString(params[0].Str(), params[1].Str(), ""));
  } else if (s == "SetIniSetting" && paramCount == 3) {
    IniFile ini = fmMain->Pref->GetInifile();
    ini.WriteString(params[0].Str(), params[1].Str(), params[2].Str());
  } else if (s == "GetFontName" && paramCount == 0) {
    Stack.emplace_back(env.Grid->Raw()->Font->Name);
  } else if (s == "SetFontName" && paramCount == 1) {
    env.Grid->Raw()->Font->Name = params[0].Str();
    fmMain->SetGridFont(env.Grid->Raw()->Font);
  } else if (s == "GetFontSize" && paramCount == 0) {
    Stack.emplace_back(env.Grid->Raw()->Font->Size);
  } else if (s == "SetFontSize" && paramCount == 1) {
    env.Grid->Raw()->Font->Size = params[0].Val();
    fmMain->SetGridFont(env.Grid->Raw()->Font);
  } else if (s == "GetCharCode" && paramCount == 0) {
    String encoding;
    switch (fmMain->MainGrid->Encoding->CodePage) {
      case CODE_PAGE_SJIS: encoding = "Shift-JIS"; break;
      case CODE_PAGE_EUC: encoding = "EUC"; break;
      case CODE_PAGE_JIS: encoding = "JIS"; break;
      case CODE_PAGE_UTF8: encoding = "UTF-8"; break;
      case CODE_PAGE_UTF16LE: encoding = "UTF-16LE"; break;
      case CODE_PAGE_UTF16BE: encoding = "UTF-16BE"; break;
      default: encoding = ""; break;
    }
    Stack.emplace_back(encoding);
  } else if (s == "SetCharCode" && paramCount == 1) {
    String encoding = params[0].Str();
    if (encoding == "Shift-JIS") {
      CallOnClick(fmMain->mnSjis);
    } else if (encoding == "EUC") {
      CallOnClick(fmMain->mnEuc);
    } else if (encoding == "JIS") {
      CallOnClick(fmMain->mnJis);
    } else if (encoding == "UTF-8") {
      CallOnClick(fmMain->mnUtf8);
    } else if (encoding == "UTF-16LE") {
      CallOnClick(fmMain->mnUnicode);
    } else if (encoding == "UTF-16BE") {
      CallOnClick(fmMain->mnUtf16be);
    } else {
      throw MacroException(L"文字コードが不正です：" + params[0].Str());
    }
  } else if (s == "GetDataTypes" && paramCount == 0) {
    TTypeList &typeList = fmMain->TypeList;
    String dataTypes = "";
    for (int i = 0; i < typeList.Count; i++) {
      if (i > 0) {
        dataTypes += "\n";
      }
      dataTypes += typeList.Items(i)->Name;
    }
    Stack.emplace_back(dataTypes);
  } else if (s == "GetActiveDataType" && paramCount == 0) {
    Stack.emplace_back(fmMain->MainGrid->TypeOption->Name);
  } else if (s == "SetActiveDataType" && paramCount == 1) {
    int index = fmMain->TypeList.IndexOf(params[0].Str(), -1);
    if (index == -1) {
      throw MacroException(L"データ形式名が不正です：" + params[0].Str());
    }
    fmMain->MainGrid->TypeOption = fmMain->TypeList.Items(index);
  } else if (s == "write" || s == "writeln") {
    if (canWriteFile) {
      fs_io->SetEncoding(env.Grid->Raw()->Encoding);
      for (int i = 0; i < paramCount; i++) {
        fs_io->Write(params[i].Str());
      }
      if (s == "writeln") {
        fs_io->Write(ReturnCodeString(env.Grid->Raw()->ReturnCode));
      }
    } else {
      int wx = env.Grid->GetCol();
      int wy = env.Grid->GetRow();
      for (int i = 0; i < paramCount; i++) {
        env.Grid->SetCell(wx, wy, params[i].Str());
        wx++;
      }
      if (s == "writeln") {
        env.Grid->MoveTo(1, wy + 1);
      } else {
        env.Grid->MoveTo(wx, wy);
      }
    }
  } else if (s == "StartMacroRecording" && paramCount == 0) {
    env.Grid->Raw()->UndoList->StartMacroRecording();
  } else if (s == "StopMacroRecording" && paramCount == 0) {
    env.Grid->Raw()->UndoList->StopMacroRecording();
  } else if (s == "GetRecordedMacro" && paramCount == 0) {
    Stack.emplace_back(env.Grid->Raw()->UndoList->GetRecordedMacro());
  } else if (s == "MacroTerminate" && paramCount == 0) {
    throw MacroException(L"中断されました。", ME_CANCELED);
  } else if (s == "Clear" && paramCount == 0) {
    fmMain->Clear();
  } else if (s == "End" && paramCount == 0) {
    fmMain->Close();
    throw MacroException(L"中断されました。", ME_CANCELED);
  } else if (paramCount == 0) {
    env.Grid->ApplyPendingChanges();
    TMenuItem *menu = nullptr;
    if (!env.ReadOnly) {
      menu = fmMain->FindMenuItem(s);
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
  } else if (s == "int" && paramCount == 1) {
    Stack.emplace_back(static_cast<int>(params[0].Val()));
  } else if (s == "double" && paramCount == 1) {
    Stack.emplace_back(params[0].Val());
  } else if (s == "str" && paramCount == 1) {
    Stack.emplace_back(params[0].Str());
  } else if (s == "max") {
    double maxVal = params[0].Val();
    for (int i = 1; i < paramCount; i++) {
      if (params[i].Val() > maxVal) {
        maxVal = params[i].Val();
      }
    }
    Stack.emplace_back(maxVal);
  } else if (s == "min") {
    double minVal = params[0].Val();
    for (int i = 1; i < paramCount; i++) {
      if (params[i].Val() < minVal) {
        minVal = params[i].Val();
      }
    }
    Stack.emplace_back(minVal);
  } else if (s == "sqrt" && paramCount == 1) {
    Stack.emplace_back(sqrt(params[0].Val()));
  } else if (s == "sin" && paramCount == 1) {
    Stack.emplace_back(sin(params[0].Val()));
  } else if (s == "cos" && paramCount == 1) {
    Stack.emplace_back(cos(params[0].Val()));
  } else if (s == "tan" && paramCount == 1) {
    Stack.emplace_back(tan(params[0].Val()));
  } else if (s == "asin" && paramCount == 1) {
    Stack.emplace_back(asin(params[0].Val()));
  } else if (s == "acos" && paramCount == 1) {
    Stack.emplace_back(acos(params[0].Val()));
  } else if (s == "atan" && paramCount == 1) {
    Stack.emplace_back(atan(params[0].Val()));
  } else if (s == "atan2" && paramCount == 2) {
    double y = params[0].Val();
    double x = params[1].Val();
    if (x == 0 && y == 0) {
      Stack.emplace_back(0);
    } else {
      Stack.emplace_back(atan2(params[0].Val(), params[1].Val()));
    }
  } else if (s == "pow" && paramCount == 2) {
    Stack.emplace_back(pow(params[0].Val(), params[1].Val()));
  } else if (s == "len" && paramCount == 1) {
    Stack.emplace_back(params[0].Str().Length());
  } else if (s == "lenB" && paramCount == 1) {
    Stack.emplace_back(AnsiString(params[0].Str()).Length());
  } else if (s == "InsertRow" && (paramCount == 1 || paramCount == 2)) {
    int top = params[0].Val();
    int bottom = (paramCount > 1 ? params[1].Val() : top);
    env.Grid->InsertRow(top, bottom);
  } else if (s == "DeleteRow" && (paramCount == 1 || paramCount == 2)) {
    int top = params[0].Val();
    int bottom = (paramCount > 1 ? params[1].Val() : top);
    env.Grid->DeleteRow(top, bottom);
  } else if (s == "InsertCol" && (paramCount == 1 || paramCount == 2)) {
    int left = params[0].Val();
    int right = (paramCount > 1 ? params[1].Val() : left);
    env.Grid->InsertCol(left, right);
  } else if (s == "DeleteCol" && (paramCount == 1 || paramCount == 2)) {
    int left = params[0].Val();
    int right = (paramCount > 1 ? params[1].Val() : left);
    env.Grid->DeleteCol(left, right);
  } else if (s == "MoveRow" && paramCount == 2) {
    env.Grid->MoveRow(params[0].Val(), params[1].Val());
  } else if (s == "MoveCol" && paramCount == 2) {
    env.Grid->MoveCol(params[0].Val(), params[1].Val());
  } else if (s == "Paste" && paramCount == 1) {
    env.Grid->Raw()->PasteFromClipboard(
        static_cast<TPasteOption>(params[0].Val()));
  } else if (s == "random" && paramCount == 1) {
    Stack.emplace_back(Random(params[0].Val()));
  } else if (s == "cell" && paramCount == 2) {
    Stack.emplace_back(params[0].Val(), params[1].Val(), &env);
  } else if (s == "left" && paramCount == 2) {
    Stack.emplace_back(params[0].Str().SubString(1, params[1].Val()));
  } else if (s == "right" && paramCount == 2) {
    String st = params[0].Str();
    int v1 = params[1].Val();
    Stack.emplace_back(st.SubString(st.Length() - v1 + 1, v1));
  } else if (s == "pos" && paramCount == 2) {
    Stack.emplace_back(params[0].Str().Pos(params[1].Str()));
  } else if (s == "posB" && paramCount == 2) {
    Stack.emplace_back(AnsiString(params[0].Str()).AnsiPos(params[1].Str()));
  } else if (s == "asc" && paramCount == 1) {
    AnsiString str = params[0].Str();
    if (str == "") {
      Stack.emplace_back(0);
    } else if (str.IsLeadByte(1)) {
      Stack.emplace_back(static_cast<unsigned char>(str[1]) * 256 +
          static_cast<unsigned char>(str[2]));
    } else {
      Stack.emplace_back(static_cast<unsigned char>(str[1]));
    }
  } else if (s == "ascW" && paramCount == 1) {
    String str = params[0].Str();
    if (str == "") {
      Stack.emplace_back(0);
    } else {
      Stack.emplace_back(str[1]);
    }
  } else if (s == "chr" && paramCount == 1) {
    AnsiString result;
    int val = params[0].Val();
    if (val < 256) {
      result.SetLength(1);
      result[1] = val;
    } else {
      result.SetLength(2);
      result[1] = val / 256;
      result[2] = val % 256;
    }
    Stack.emplace_back(result);
  } else if (s == "chrW" && paramCount == 1) {
    String result;
    result.SetLength(1);
    result[1] = params[0].Val();
    Stack.emplace_back(result);
  } else if ((s == "move" || s == "moveto") && paramCount == 2) {
    int c = params[0].Val();
    int r = params[1].Val();
    if (s == "move") {
      c += env.Grid->GetCol();
      r += env.Grid->GetRow();
    }
    env.Grid->MoveTo(c, r);
  } else if (s == "mid" && paramCount == 2) {
    String st = params[0].Str();
    Stack.emplace_back(
        st.SubString(params[1].Val(), st.Length() - params[1].Val() + 1));
  } else if (s == "mid" && paramCount == 3) {
    Stack.emplace_back(
        params[0].Str().SubString(params[1].Val(), params[2].Val()));
  } else if (s == "mid=" && paramCount == 4) {
    String st = params[0].Str();
    int r = params[1].Val() + params[2].Val();
    String result = st.SubString(1, params[1].Val() - 1) + params[3].Str() +
        st.SubString(r, st.Length() - r + 1);
    params[0].Assign(Element(result));
  } else if (s == "replace" && (paramCount == 3 || paramCount == 5)) {
    RegExp regExp = ParseRegExp(params[1]);
    regExp.isReplaceAll = true;
    if (paramCount > 3) { regExp.isIgnoreCase = params[3].Val(); }
    if (paramCount > 4) { regExp.isRegExp = params[4].Val(); }
    Stack.emplace_back(regExp.Replace(params[0].Str(), params[2].Str()));
  } else if (s == "sum" && paramCount == 4) {
    int l = params[0].Val();
    int t = params[1].Val();
    int r = params[2].Val();
    int b = params[3].Val();
    double result = env.Grid->Raw()->GetSum(l,t,r,b);
    Stack.emplace_back(result);
  } else if (s == "avr" && paramCount == 4) {
    int l = params[0].Val();
    int t = params[1].Val();
    int r = params[2].Val();
    int b = params[3].Val();
    double result = env.Grid->Raw()->GetAvr(l,t,r,b);
    Stack.emplace_back(result);
  } else if (s == "ShellOpen" && paramCount == 1) {
    String FileName = params[0].Str();
    if (isUrl(FileName)) {
      env.Grid->Raw()->OpenURL(FileName);
    } else {
      AutoOpen(FileName, ExtractFilePath(fmMain->FileName));
    }
  } else if (s == "ShellOpen" && paramCount > 1) {
    std::vector<String> shellParams;
    shellParams.reserve(paramCount);
    for (const Element& e : params) {
      shellParams.push_back(e.Str());
    }
    int result = ShellOpen(shellParams);

    if (result == -1) {
      if (errno == ENOENT) {
        throw MacroException(shellParams[0] + L"\nファイルが見つかりません。");
      } else {
        throw MacroException(shellParams[0] + L"\n実行に失敗しました。");
      }
    }
  } else if (s == "Open" && (paramCount == 1 || paramCount == 2)) {
    if (env.ReadOnly) {
      throw MacroException("Can't paramsn files.", ME_SECURITY);
    }
    env.Grid->ApplyPendingChanges();
    const TTypeOption *typeOption = nullptr;
    if (paramCount > 1) {
      int index = fmMain->TypeList.IndexOf(params[1].Str(), -1);
      if (index == -1) {
        throw MacroException(L"データ形式名が不正です：" + params[1].Str());
      }
      typeOption = fmMain->TypeList.Items(index);
    }
    fmMain->OpenFile(params[0].Str(), CHARCODE_AUTO, typeOption);
    while (env.Grid->Raw()->FileOpenThread) {
      Sleep(100);
      Application->ProcessMessages();
    }
  } else if (s == "SaveAs" && (paramCount == 1 || paramCount == 2)) {
    if (env.ReadOnly) {
      throw MacroException("Can't write to files.", ME_SECURITY);
    }
    env.Grid->ApplyPendingChanges();
    const TTypeOption *typeOption = nullptr;
    if (paramCount == 1) {
      typeOption = env.Grid->Raw()->TypeOption;
    } else {
      int index = fmMain->TypeList.IndexOf(params[1].Str(), -1);
      if (index == -1) {
        throw MacroException(L"保存形式が不明です：" + params[1].Str());
      }
      typeOption = fmMain->TypeList.Items(index);
    }
    fmMain->SaveAs(params[0].Str(), typeOption);
  } else if (s == "Export" && paramCount == 2) {
    if (env.ReadOnly) {
      throw MacroException("Can't write to files.", ME_SECURITY);
    }
    env.Grid->ApplyPendingChanges();
    fmMain->Export(params[0].Str(), params[1].Str());
  } else if (s == "FileExists" && paramCount == 1) {
    Stack.emplace_back(FileExists(params[0].Str()) ? 1 : 0);
  } else if (s == "WriteToFile" && paramCount == 2) {
    if (env.ReadOnly) {
      throw MacroException("Can't write to files.", ME_SECURITY);
    }
    String fileName = params[1].Str();
    String dirName = ExtractFilePath(fileName);
    if (!DirectoryExists(dirName)) {
      ForceDirectories(dirName);
    }
    std::unique_ptr<TFileStream> fs =
        std::make_unique<TFileStream>(fileName, fmCreate | fmShareDenyWrite);
    TBytes bytes = TEncoding::UTF8->GetBytes(params[0].Str());
    fs->Write(bytes, bytes.Length);
  } else if (s == "Sort" && paramCount >= 1 && paramCount <= 9) {
    int left = params[0].Val();
    int top = (paramCount > 1 ? params[1].Val() : 1);
    int right = (paramCount > 2 ? params[2].Val() : env.Grid->GetRight());
    int bottom = (paramCount > 3 ? params[3].Val() : env.Grid->GetBottom());
    int col = (paramCount > 4 ? params[4].Val() : left);
    bool direction = (paramCount > 5 ? params[5].Val() : false);
    bool numSort = (paramCount > 6 ? params[6].Val() : false);
    bool ignoreCase = (paramCount > 7 ? params[7].Val() : false);
    bool ignoreZenhan = (paramCount > 8 ? params[8].Val() : false);
    env.Grid->Sort(left, top, right, bottom, col, direction, numSort,
        ignoreCase, ignoreZenhan);
  } else if (s == "ReplaceAll" &&
      (paramCount == 2 || paramCount == 5 || paramCount == 9)) {
    RegExp regExp = ParseRegExp(params[0]);
    String find = regExp.pattern;
    String replace = params[1].Str();
    bool ignoreCase = (paramCount > 2 ? params[2].Val() : regExp.isIgnoreCase);
    bool word = (paramCount > 3 ? params[3].Val() : false);
    bool isRegex = (paramCount > 4 ? params[4].Val() : regExp.isRegExp);
    int left = (paramCount > 5 ? params[5].Val() : env.Grid->GetLeft());
    int top = (paramCount > 6 ? params[6].Val() : env.Grid->GetTop());
    int right = (paramCount > 7 ? params[7].Val() : env.Grid->GetRight());
    int bottom = (paramCount > 8 ? params[8].Val() : env.Grid->GetBottom());
    int count = env.Grid->ReplaceAll(
        find, replace, left, top, right, bottom, ignoreCase, isRegex, word);
    Stack.emplace_back(count);
  } else if (s == "QuickFind" && paramCount == 1) {
    fmFind->edFindText->Text = params[0].Str();
    fmMain->mnQuickFindClick(nullptr);
  } else if (s == "QuickFind" && paramCount == 4) {
    fmFind->edFindText->Text = params[0].Str();
    fmFind->cbCase->Checked = !params[1].Val();
    fmFind->cbWordSearch->Checked = params[2].Val();
    fmFind->cbRegex->Checked = params[3].Val();
    fmMain->mnQuickFindClick(nullptr);
  } else if (s == "Select" && paramCount == 4) {
    env.Grid->Select(
        params[0].Val(), params[1].Val(), params[2].Val(), params[3].Val());
  } else if (s == "RegExp" && (paramCount == 1 || paramCount == 2)) {
    RegExp original = ParseRegExp(params[0]);
    String flags = (paramCount > 1 ? params[1].Str() : original.flags);
    Stack.emplace_back(
        "/" + original.pattern  + "/" + flags, etRegExp, nullptr);
  } else if (s == "ShowDialog" && (paramCount == 1 || paramCount == 2)) {
    UserDialog dialog(
        params[0], paramCount > 1 ? params[1].Str() : L"Cassava Macro");
    dialog.ShowModal();
    Stack.emplace_back(dialog.ReturnValue);
  } else {
    throw MacroException(notFoundMessage);
  }
}
//---------------------------------------------------------------------------
void TMacro::ExecOpe(TMacroOperator c, size_t &p) {
  if (c == TMacroOperator::Goto || c == TMacroOperator::Jump ||
      c == TMacroOperator::Minus || c == TMacroOperator::Inc ||
      c == TMacroOperator::Dec || c == TMacroOperator::Not ||
      c == TMacroOperator::VarArg) {
    if (Stack.size() < 1) {
      throw MacroException(static_cast<char>(c), ME_HIKISU);
    }
    Element params = Stack.back();
    Stack.pop_back();
    if (c == TMacroOperator::Goto) {
      p = params.Val();
      Stack.clear();
      LoopCount++;
      Application->ProcessMessages();
      if (MaxLoop > 0 && LoopCount > MaxLoop) {
        throw MacroException(String(L"ループ回数が") + MaxLoop +
            L"に達しました。処理を中断します。");
      }
      if (!RunningOk) { throw MacroException(L"中断しました"); }
    } else if (c == TMacroOperator::Jump) {
      p = params.Val();
    } else if (c == TMacroOperator::Minus) {
      Stack.emplace_back(-(params.Val()));
    } else if (c == TMacroOperator::Inc) {
      params.Assign(Element(params.Val() + 1));
    } else if (c == TMacroOperator::Dec) {
      params.Assign(Element(params.Val() - 1));
    } else if (c == TMacroOperator::Not) {
      Stack.emplace_back(((params.Val() == 0) ? 1 : 0));
    } else if (c == TMacroOperator::VarArg) {
      std::unique_ptr<TEnvironment> varArg = env.NewObject();
      int fromIndex = params.Val();
      for (int i = fromIndex; i < Stack.size(); i++) {
        varArg->Vars[String(i - fromIndex)] = Element(Stack[i].Value());
      }
      varArg->Vars["length"] = Element(Stack.size() - fromIndex);
      std::vector<std::unique_ptr<TEnvironment>>& objects = env.GetObjects();
      Stack.erase(Stack.begin() + fromIndex, Stack.end());
      Stack.emplace_back(objects.size(), "", etObject, env.GetGlobal());
      objects.push_back(std::move(varArg));
    }
  } else if (c == TMacroOperator::ObjKey) {
    if (Stack.size() < 3) {
      throw MacroException(static_cast<char>(c), ME_HIKISU);
    }
    Element val = Stack.back(); Stack.pop_back();
    Element key = Stack.back(); Stack.pop_back();
    Element obj = Stack.back();
    String keyString = (key.Type == etVar ? key.Name() : key.Str());
    obj.GetMember(keyString).Assign(val);
  } else {
    if (Stack.size() < 2) {
      throw MacroException(static_cast<char>(c), ME_HIKISU);
    }
    Element params2 = Stack.back(); Stack.pop_back();
    Element params1 = Stack.back(); Stack.pop_back();

    if (c == static_cast<TMacroOperator>('=')) {
      params1.Assign(params2);
    } else if (c == TMacroOperator::Add) {
      if (params1.isNum() && params2.isNum()) {
        params1.Assign(Element(params1.Val() + params2.Val()));
      } else {
        params1.Assign(Element(params1.Str() + params2.Str()));
      }
    } else if (c == TMacroOperator::Subtract) {
      params1.Assign(Element(params1.Val() - params2.Val()));
    } else if (c == TMacroOperator::Multiply) {
      params1.Assign(Element(params1.Val() * params2.Val()));
    } else if (c == TMacroOperator::Divide) {
      params1.Assign(Element(params1.Val() / params2.Val()));
    } else if (c == static_cast<TMacroOperator>('+')) {
      if (params1.isNum() && params2.isNum()) {
        Stack.emplace_back(params1.Val() + params2.Val());
      } else {
        Stack.emplace_back(params1.Str() + params2.Str());
      }
    } else if (c == static_cast<TMacroOperator>('-')) {
      Stack.emplace_back(params1.Val() - params2.Val());
    } else if (c == static_cast<TMacroOperator>('*')) {
      Stack.emplace_back(params1.Val() * params2.Val());
    } else if (c == static_cast<TMacroOperator>('/')) {
      Stack.emplace_back(params1.Val() / params2.Val());
    } else if (c == static_cast<TMacroOperator>('%')) {
      Stack.emplace_back(
          static_cast<int>(params1.Val()) % static_cast<int>(params2.Val()));
    } else if (c == TMacroOperator::Eq) {
      Stack.emplace_back(((params1.Str() == params2.Str()) ? 1 : 0));
    } else if (c == TMacroOperator::NEq) {
      Stack.emplace_back(((params1.Str() != params2.Str()) ? 1 : 0));
    } else if (c == static_cast<TMacroOperator>('<')) {
      Stack.emplace_back(((params1.Val() < params2.Val()) ? 1 : 0));
    } else if (c == static_cast<TMacroOperator>('>')) {
      Stack.emplace_back(((params1.Val() > params2.Val()) ? 1 : 0));
    } else if (c == TMacroOperator::LEq) {
      Stack.emplace_back(((params1.Val() <= params2.Val()) ? 1 : 0));
    } else if (c == TMacroOperator::GEq) {
      Stack.emplace_back(((params1.Val() >= params2.Val()) ? 1 : 0));
    } else if (c == static_cast<TMacroOperator>('&')) {
      if (params1.Val() == 0) {
        Stack.push_back(params1);
        p = params2.Val();
        return;
      }
    } else if (c == static_cast<TMacroOperator>('|')) {
      if (params1.Val() != 0) {
        Stack.push_back(params1);
        p = params2.Val();
        return;
      }
    } else if (c == static_cast<TMacroOperator>('.')) {
      if (params1.Value().Type != etObject) {
        if (params2.Str() == "length") {
          Stack.emplace_back(params1.Str().Length());
          return;
        } else if (params2.isNum()) {
          String target = params1.Str();
          int index = params2.Val();
          if (index >= 0 && index < target.Length()) {
            Stack.emplace_back(target.SubString(params2.Val() + 1, 1));
          } else {
            Stack.emplace_back("");
          }
          return;
        }
        throw MacroException(L"「.」の左がオブジェクトではありません："
            + (params1.Type == etVar ? params1.Name() : params1.Str()));
      }
      Stack.push_back(params1.GetMember(params2.Str()));
    } else if (c == TMacroOperator::In) {
      if (params2.Value().Type != etObject) {
        throw MacroException(L"in の右がオブジェクトではありません："
            + (params2.Type == etVar ? params2.Name() : params2.Str()));
      }
      std::map<String, Element>& vars = env.GetObject(params2.Val())->Vars;
      Stack.emplace_back((vars.find(params1.Str()) != vars.end()) ? 1 : 0);
    } else if (c == TMacroOperator::IfThen) {
      if (params1.Val() == 0) {
        p = params2.Val();
      }
    } else if (c == TMacroOperator::Cell) {
      Stack.emplace_back(params1.Val(), params2.Val(), &env);
    } else if (c == TMacroOperator::DefParam) {
      if (Stack.size() >= params1.Val()) {
        p = params2.Val();
      }
    } else {
      throw MacroException(
          String(L"Invalid operator: ") + static_cast<char>(c));
    }
  }
}
//---------------------------------------------------------------------------
const TByteVector& TMacro::GetByteVector(const String& funcName) {
  try {
    return Context.Modules.at(funcName);
  } catch (...) {
    throw MacroException(funcName + L"\nユーザー関数が見つかりません。");
  }
}
//---------------------------------------------------------------------------
Element TMacro::Do(const String& FileName, const std::vector<Element> &AStack,
                   int x, int y, Element *thisPtr) {
  Element ReturnValue;
  Stack = AStack;

  const TByteVector& bytes = GetByteVector(FileName);
  size_t p = 0;

  env.Vars.clear();
  env.Vars["x"] = Element(((x >= 0) ? x : env.Grid->GetCol()));
  env.Vars["y"] = Element(((y >= 0) ? y : env.Grid->GetRow()));
  env.Vars["Left"] = Element(env.Grid->GetLeft());
  env.Vars["Top"] = Element(env.Grid->GetTop());
  if (thisPtr) {
    env.Vars["this"] = *thisPtr;
  }
  LoopCount = 0;
  while (p < bytes.Size()) {
    char Type = bytes.ReadChar(p);
    if (Type == '$') {
      Stack.emplace_back(bytes.ReadString(p));
    } else if (Type == '*') {
      ExecFnc(bytes.ReadString(p));
    } else if (Type == '~') {
      String name = bytes.ReadString(p);
      Stack.emplace_back(name, etVar, &env);
    } else if (Type == '!') {
      Stack.emplace_back(bytes.ReadString(p), etSystem, &env);
    } else if (Type == '-') {
      TMacroOperator c = bytes.ReadOperator(p);
      if (c == TMacroOperator::Return) {
        if (Stack.size() > 0) {
          ReturnValue = Stack.back().Value();
        }
        break;
      } else {
        ExecOpe(c, p);
      }
    } else if (Type == 'i') {
      Stack.emplace_back(bytes.ReadInteger(p));
    } else if (Type == 'd') {
      Stack.emplace_back(bytes.ReadDouble(p));
    }
  }
  return ReturnValue;
}
//---------------------------------------------------------------------------
TMacroValue RunMacro(const String& FileName, int MaxLoop,
    const TMacroContext &Context, int x, int y, bool ReadOnly,
    EncodedWriter *IO, TStringList *StringArguments)
{
  if (!RunningOk) { return TMacroValue(); }
  if (!ReadOnly) { RunningCount++; }
  Randomize();
  GridProxy grid(fmMain->MainGrid, ReadOnly);
  TMacro mcr(IO, MaxLoop, Context, TEnvironment(ReadOnly, &grid, nullptr));
  Element r;
  try {
    std::vector<Element> stack;
    if (StringArguments) {
      for (int i = 0; i < StringArguments->Count; i++) {
        stack.emplace_back(StringArguments->Strings[i]);
      }
    }
    r = mcr.Do(FileName, stack, x, y);
  } catch (MacroException e) {
    if (ReadOnly) {
      RunningCount--;
      throw e;
    } else if (e.Type != ME_CANCELED) {
      if (e.Type == ME_HIKISU) {
        e.Message = L"引数の数が足りません:" + e.Message;
      }
      Application->MessageBox((FileName + "\n" + e.Message).c_str(),
                              L"Cassava Macro Interpreter", 0);
    }
  } catch (Exception *e) {
    if (ReadOnly) {
      RunningCount--;
      throw e;
    } else {
      Application->MessageBox((FileName + "\n" + e->Message).c_str(),
                              L"Cassava Macro Interpreter", 0);
    }
  }
  if (!ReadOnly) {
    RunningCount--;
    if (RunningCount == 0) {
      RunningOk = true;
    }
  }

  if (r.Type == etErr) {
    return TMacroValue();
  }
  if (r.Type == etObject) {
    std::map<String, String> members;
    for (const auto& [key, value] : r.GetMembers()) {
      members[key] = value.Str();
    }
    return TMacroValue(r.Str(), members);
  }
  return TMacroValue(r.Str(), {});
}
//---------------------------------------------------------------------------
void StopAllMacros()
{
  if (RunningCount > 0) {
    RunningOk = false;
  }
}
//---------------------------------------------------------------------------
int GetRunningMacroCount()
{
  return RunningCount;
}
//---------------------------------------------------------------------------
