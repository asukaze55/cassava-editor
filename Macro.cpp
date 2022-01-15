//---------------------------------------------------------------------------
#ifdef CssvMacro
//---------------------------------------------------------------------------
#include <map>
#include <vcl\vcl.h>
#pragma hdrstop
#include "Macro.h"
#include "MacroOpeCode.h"
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
//---------------------------------------------------------------------------
#define TVar  map<AnsiString, Element>
#define MGCol (fmMain->MainGrid->Col - fmMain->MainGrid->DataLeft + 1)
#define MGRow (fmMain->MainGrid->Row - fmMain->MainGrid->DataTop  + 1)
#define RXtoAX(x)                 (x - fmMain->MainGrid->DataLeft + 1)
#define RYtoAY(y)                 (y - fmMain->MainGrid->DataTop  + 1)
//---------------------------------------------------------------------------
bool canReadFile;
bool canWriteFile;
TFileStream *fs_io;
//---------------------------------------------------------------------------
class Element {
private:
  AnsiString st;
  double vl;
  bool Num;
public:
  int Type;
  bool isNum();
  int X, Y;
  TVar *Var;
  Element(){ Type=etErr; Num=false; st=""; Var=NULL; }
  Element(double d, TVar *v){ Type=etAtom; Num=true; vl=d; Var=v; };
  Element(WideString s, TVar *v){ Type=etAtom; Num=false; st=(AnsiString)s; Var=v; };
  Element(AnsiString s, TVar *v){ Type=etAtom; Num=false; st=s; Var=v; };
  Element(AnsiString s, int t, TVar *v){ Type=t; Num=false; st=s; Var=v; };
  Element(int cl, int rw, TVar *v);
  Element(int cl, int rw, bool nm, TVar *v)
  { Type = etCell; Num = nm; X = cl; Y = rw; Var = v;};
  Element(const Element &e)
  { st=e.st; vl=e.vl; Type=e.Type; Num=e.Num; X=e.X; Y=e.Y; Var=e.Var;};
  double Val();
  AnsiString Str();
  void Sbst(Element e);
  void Pntr(Element e);
  Element Points();
};
//---------------------------------------------------------------------------
class TMacro{
private:
  TFileStream *fs;
  AnsiString ReadString(TFileStream *fs);
  void ExecOpe(char c);
  void ExecFnc(AnsiString s);
  TList *Stack;
  TVar Var;
  int LoopCount;
  AnsiString FileName;
public:
  int MaxLoop;
  Element *Do(AnsiString AFileName, TList *AStack);
};
//---------------------------------------------------------------------------
class MacroException{
public:
  AnsiString Message;
  int Type;
  MacroException(AnsiString Mes, int t = 0) : Message(Mes), Type(t) {}
};
//---------------------------------------------------------------------------
void CsvGridGoTo(TCsvGrid *g, int x, int y){
  x += g->DataLeft - 1;
  y += g->DataTop  - 1;
  if(x < g->FixedCols){
    x = g->FixedCols;
  }else if(x >= g->ColCount){
    g->ChangeColCount(x+1); g->ReNum();
  }
  if(y < g->FixedRows){
    y = g->FixedRows;
  }else if(y >= g->RowCount){
    g->ChangeRowCount(y+1); g->ReNum();
  }
  g->Col = x;
  g->Row = y;
}
//---------------------------------------------------------------------------
void Write(int x, int y, AnsiString str)
{
  if(x < 1 || y < 1) return;
  TCsvGrid *g = fmMain->MainGrid;
  x += g->DataLeft - 1;
  y += g->DataTop  - 1;
  if(x >= g->ColCount - 1){
    g->ChangeColCount(x+2); g->ReNum();
  }
  if(y >= g->RowCount - 1){
    g->ChangeRowCount(y+2); g->ReNum();
  }
  g->Cells[x][y] = str;
  g->Modified = true;
}
//---------------------------------------------------------------------------
Element::Element(int cl, int rw, TVar *v)
{
  Type = etCell; X = cl; Y = rw;
  Num = fmMain->MainGrid->IsNumberAt(cl,rw);
  Var = v;
}
//---------------------------------------------------------------------------
double Element::Val()
{
  try{
    if(Type == etCell) return (fmMain->MainGrid->ACells[X][Y].ToDouble());
    else if(Type == etVar) return (*Var)[st].Val();
    else if(Type == etSystem){
      if     (st == "Col")       { return MGCol; }
      else if(st == "Row")       { return MGRow; }
      else if(st == "Right")     { return RXtoAX(fmMain->MainGrid->DataRight); }
      else if(st == "Bottom")    { return RYtoAY(fmMain->MainGrid->DataBottom); }
      else if(st == "SelLeft")   { return RXtoAX(fmMain->MainGrid->SelLeft); }
      else if(st == "SelTop")    { return RYtoAY(fmMain->MainGrid->SelTop); }
      else if(st == "SelRight")  { return RXtoAX(fmMain->MainGrid->Selection.Right); }
      else if(st == "SelBottom") { return RYtoAY(fmMain->MainGrid->Selection.Bottom); }
    }else if(Num) return vl;
    return st.ToDouble();
  }catch(...){
    return 0;
  }
}
//---------------------------------------------------------------------------
AnsiString Element::Str()
{
  if(Type == etCell) return fmMain->MainGrid->ACells[X][Y];
  else if(Type == etVar) return (*Var)[st].Str();
  else if(Type == etSystem) return AnsiString(Val());
  else if(Num) return AnsiString(vl);
  else return st;
}
//---------------------------------------------------------------------------
bool Element::isNum()
{
  if(Type == etVar){
    return (*Var)[st].isNum();
  }else if(Type == etCell){
    return fmMain->MainGrid->IsNumberAt(X,Y);
  }
  return Num;
}
//---------------------------------------------------------------------------
void Element::Sbst(Element e)
{
  if(e.Type == etVar){
    (*Var)[st] = (*Var)[e.st];
  }else if(e.Type == etCell){
    (*Var)[st] = (e.Num ? Element(e.Val(), Var) : Element(e.Str(), Var));
  }else if(e.Type == etSystem){
    (*Var)[st] = Element(e.Val(), Var);
  }else{
    (*Var)[st] = e;
  }

  if(Type == etSystem){
    int v = (*Var)[st].Val();
    TCsvGrid *g = fmMain->MainGrid;
    if     (st == "Col")       { CsvGridGoTo(g, v, MGRow); }
    else if(st == "Row")       { CsvGridGoTo(g, MGCol, v); }
    else if(st == "Right")     { g->ChangeColCount(v + g->DataLeft); }
    else if(st == "Bottom")    { g->ChangeRowCount(v + g->DataTop);  }
    else {
      TGridRect Sel = g->Selection;
      if     (st == "SelLeft")   { Sel.Left   = v - 1 + g->DataLeft; }
      else if(st == "SelTop")    { Sel.Top    = v - 1 + g->DataTop; }
      else if(st == "SelRight")  { Sel.Right  = v - 1 + g->DataLeft; }
      else if(st == "SelBottom") { Sel.Bottom = v - 1 + g->DataTop; }
      g->Selection = Sel;
    }
  }else if(Type == etCell){
    Write(X, Y, (*Var)[st].Str());
  }
}
//---------------------------------------------------------------------------
void Element::Pntr(Element e)
{
  (*Var)[st] = e;
}
//---------------------------------------------------------------------------
Element Element::Points()
{
  if(Type != etVar) return *this;
  return (*Var)[st];
}
//---------------------------------------------------------------------------
void Clear(TList *L){
  for(int i=L->Count-1; i>=0; i--){
    if(L->Items[i]) delete L->Items[i];
  }
  L->Clear();
}
//---------------------------------------------------------------------------
AnsiString TMacro::ReadString(TFileStream *fs)
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
TMenuItem *MenuSearch(TMenuItem *m, AnsiString s)
{
  for(int i=0; i<m->Count; i++){
    if(m->Items[i]->Name == (AnsiString)"mn" + s) return (m->Items[i]);
    else if(m->Items[i]->Count > 0){
      TMenuItem *sub = MenuSearch(m->Items[i], s);
      if(sub) return sub;
    }
  }
  return NULL;
}
//---------------------------------------------------------------------------
#define STR0 ope[0]->Str()
#define VAL0 ope[0]->Val()
#define STR1 ope[1]->Str()
#define VAL1 ope[1]->Val()
#define STR2 ope[2]->Str()
#define VAL2 ope[2]->Val()
#define STR3 ope[3]->Str()
//---------------------------------------------------------------------------
void TMacro::ExecFnc(AnsiString s){
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
      TMacro mcr;
      mcr.MaxLoop = ((MaxLoop > 0) ? MaxLoop-LoopCount : 0);
      Element *r = mcr.Do(
        ExtractFilePath(FileName) + s + "$" + (AnsiString)H +
        ExtractFileExt(FileName), ArgStack);
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
    }else if(s == "MessageBox"){
      char *Text = (H >= 1) ? STR0.c_str() : "ブレークポイントです";
      int Flag = (H >= 2) ? (ope[H-1]->Val()) : MB_OK;
      char *Caption = (H >= 3) ? STR1.c_str() : "Cassava Macro";
      Stack->Add(new Element(Application->MessageBox(Text, Caption, Flag),&Var));
    }else if(s == "InputBox"){
      AnsiString Text = (H >= 1) ? STR0 : (AnsiString)"";
      AnsiString Def = (H >= 2) ? (ope[H-1]->Str()) : (AnsiString)"";
      AnsiString Caption = (H >= 3) ? STR1 : (AnsiString)"Cassava Macro";
      Stack->Add(new Element(InputBox(Caption, Text, Def),&Var));
    }else if(s == "GetRowHeight" && H == 0){
      Stack->Add(new Element(fmMain->MainGrid->DefaultRowHeight,&Var));
    }else if(s == "GetRowHeight" && H == 1){
      Stack->Add(new Element(fmMain->MainGrid->RowHeights[VAL0],&Var));
    }else if(s == "SetRowHeight" && H == 1){
      fmMain->MainGrid->DefaultRowHeight = VAL0;
    }else if(s == "SetRowHeight" && H == 2){
      fmMain->MainGrid->RowHeights[VAL0] = VAL1;
    }else if(s == "GetColWidth" && H == 0){
      Stack->Add(new Element(fmMain->MainGrid->DefaultColWidth,&Var));
    }else if(s == "GetColWidth" && H == 1){
      Stack->Add(new Element(fmMain->MainGrid->ColWidths[VAL0],&Var));
    }else if(s == "SetColWidth" && H == 1){
      fmMain->MainGrid->DefaultColWidth = VAL0;
    }else if(s == "SetColWidth" && H == 2){
      fmMain->MainGrid->ColWidths[VAL0] = VAL1;
    }else if(s == "GetYear" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack->Add(new Element((int)year ,&Var));
    }else if(s == "GetMonth" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack->Add(new Element((int)month ,&Var));
    }else if(s == "GetDate" && H == 0){
      unsigned short year, month, day;
      Date().DecodeDate(&year, &month, &day);
      Stack->Add(new Element((int)day ,&Var));
    }else if(s == "GetHours" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack->Add(new Element((int)hours ,&Var));
    }else if(s == "GetMinutes" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack->Add(new Element((int)minutes ,&Var));
    }else if(s == "GetSeconds" && H == 0){
      unsigned short hours, minutes, seconds, msec;
      Time().DecodeTime(&hours, &minutes, &seconds, &msec);
      Stack->Add(new Element((int)seconds ,&Var));
    }else if(s == "GetFilePath" && H == 0){
      AnsiString Path = ExtractFilePath(fmMain->FileName);
      if(Path != "" && (*(Path.AnsiLastChar()) != '\\')){ Path += '\\'; }
      Stack->Add(new Element(Path ,&Var));
    }else if(s == "GetFileName" && H == 0){
      Stack->Add(new Element(ExtractFileName(fmMain->FileName) ,&Var));
    }else if(s == "write" || s == "writeln"){
      if(canWriteFile){
        for(int i=0; i<H; i++){
          AnsiString str = ope[i]->Str();
          fs_io->Write(str.c_str(), str.Length());
        }
        if(s == "writeln"){ fs_io->Write("\r\n", 2); }
      }else{
        int wx = MGCol, wy = MGRow;
        for(int i=0; i<H; i++){
          Write(wx, wy, ope[i]->Str());
          wx++;
        }
        if(s == "writeln"){ CsvGridGoTo(fmMain->MainGrid, 1, wy+1); }
        else{ CsvGridGoTo(fmMain->MainGrid, wx, wy); }
      }
/*
    }else if(s == "read" || s == "readln"){
      if(canReadFile){
        if(s == "read"){
          int len = ((H == 0) ? 1 : (VAL0));
          if(len == 0) break;
          char *str = new char[len+2];
          int count = fs_io->Read(str, len);
          str[count] = '\0';
          Stack->Add(new Element((AnsiString)str, &Var));
        }else{
          char ch[2];
          ch[0] = '\0'; ch[1] = '\0';
          AnsiString str = "";
          while(fs_io->Read(ch, 1) == 1){
            if(ch[0] == '\r') continue;
            str += ch;
            if(ch[0] == '\n') break;
          }
          Stack->Add(new Element(str, &Var));
        }
      }else{
        Stack->Add(new Element(InputBox("Cassava Macro", s, ""),&Var));
      }
*/
    }else if(H == 0){
      TMenuItem *Menu = MenuSearch(fmMain->Menu->Items, s);
      if(Menu && Menu->OnClick){
        Menu->OnClick(Menu);
      }else{
        throw MacroException("定義されていない関数です:" + s + "/0");
      }
    }else if(s == "max"){
      int maxval = ope[0]->Val();
      for(int i=1; i<H; i++){
        maxval = max(maxval, ope[i]->Val());
      }
      Stack->Add(new Element(maxval,&Var));
    }else if(s == "min"){
      int minval = ope[0]->Val();
      for(int i=1; i<H; i++){
        minval = min(minval, ope[i]->Val());
      }
      Stack->Add(new Element(minval,&Var));
    }else if(s == "len" && H == 1){
      Stack->Add(new Element(STR0.Length(),&Var));
    }else if(s == "InsertRow" && H == 1){
      fmMain->MainGrid->InsertRow(VAL0 + fmMain->MainGrid->DataTop - 1);
    }else if(s == "DeleteRow" && H == 1){
      fmMain->MainGrid->DeleteRow(VAL0 + fmMain->MainGrid->DataTop - 1);
    }else if(s == "InsertCol" && H == 1){
      fmMain->MainGrid->InsertColumn(VAL0 + fmMain->MainGrid->DataLeft - 1);
    }else if(s == "DeleteCol" && H == 1){
      fmMain->MainGrid->DeleteColumn(VAL0 + fmMain->MainGrid->DataLeft - 1);
    }else if(s == "random" && H == 1){
      Stack->Add(new Element(random(VAL0), &Var));
    }else if(s == "cell" && H == 2){
      Stack->Add(new Element(VAL0, VAL1, &Var));
    }else if(s == "left" && H == 2){
      Stack->Add(new Element(((WideString)STR0).SubString(1, VAL1),&Var));
    }else if(s == "right" && H == 2){
      WideString st = (WideString)STR0;
      Stack->Add(new Element(st.SubString(st.Length()-VAL1+1, VAL1), &Var));
    }else if(s == "pos" && H == 2){
      Stack->Add(new Element(STR0.AnsiPos(STR1), &Var));
    }else if((s == "move" || s == "moveto") && H == 2){
      int c = VAL0;
      int r = VAL1;
      if(s == "move"){ c += MGCol; r += MGRow; }
      CsvGridGoTo(fmMain->MainGrid, c, r);
    }else if(s == "mid" && H == 2){
      WideString st = (WideString)STR0;
      Stack->Add(new Element(st.SubString(VAL1, st.Length()-VAL1+1), &Var));
    }else if(s == "mid" && H == 3){
      Stack->Add(new Element(((WideString)STR0).SubString(VAL1, VAL2), &Var));
    }else if(s == "mid=" && H == 4){
      WideString st = (WideString)STR0;
      int r = VAL1 + VAL2;
      st = st.SubString(1, VAL1-1) + (WideString)STR3 + st.SubString(r, st.Length() - r + 1);
      ope[0]->Sbst(Element(st, &Var));
    }else if(s == "replace" && H == 3){
      AnsiString fr = STR0;
      AnsiString to = "";
      int hitlen = STR1.Length();
      int p;
      while((p = fr.AnsiPos(STR1)) > 0){
        to += fr.SubString(1, p-1) + STR2;
        fr.Delete(1, hitlen + p - 1);
      }
      to += fr;
      Stack->Add(new Element(to, &Var));
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
      if(MaxLoop > 0 && LoopCount > MaxLoop){
        throw MacroException((AnsiString)"ループ回数が" + MaxLoop +
              "に達しました。処理を中断します。");
      }
    }
    else if(c == 'P') Stack->Add(new Element(ope->Points()));
    else if(c == 'm') Stack->Add(new Element(-(ope->Val()), &Var));
    else if(c == CMO_Inc) { ope->Sbst(Element(ope->Val() + 1, &Var)); }
    else if(c == CMO_Dec) { ope->Sbst(Element(ope->Val() - 1, &Var)); }
    else if(c == '!') Stack->Add(new Element(((ope->Val() == 0) ? 1 : 0), &Var));
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
        Write(ope1->X, ope1->Y, ope2->Str());
      }else{
        delete ope1; delete ope2;
        throw MacroException("「=」の左が左辺値ではありません");
      }
    }else if(c == '+'){
      if(ope1->isNum() && ope2->isNum()){
        Stack->Add(new Element(ope1->Val() + ope2->Val(), &Var));
      }else{
        Stack->Add(new Element(ope1->Str() + ope2->Str(), &Var));
      }
    }else if(c == '-'){
      Stack->Add(new Element(ope1->Val() - ope2->Val(), &Var));
    }else if(c == '*'){
      Stack->Add(new Element(ope1->Val() * ope2->Val(), &Var));
    }else if(c == '/'){
      Stack->Add(new Element(ope1->Val() / ope2->Val(), &Var));
    }else if(c == '%'){
      Stack->Add(new Element((int)(ope1->Val()) % (int)(ope2->Val()), &Var));
    }else if(c == CMO_Eq){
      Stack->Add(new Element(((ope1->Str() == ope2->Str()) ? 1 : 0), &Var));
    }else if(c == CMO_NEq){
      Stack->Add(new Element(((ope1->Str() != ope2->Str()) ? 1 : 0), &Var));
    }else if(c == '<'){
      Stack->Add(new Element(((ope1->Val() < ope2->Val()) ? 1 : 0), &Var));
    }else if(c == '>'){
      Stack->Add(new Element(((ope1->Val() > ope2->Val()) ? 1 : 0), &Var));
    }else if(c == CMO_LEq){
      Stack->Add(new Element(((ope1->Val() <= ope2->Val()) ? 1 : 0), &Var));
    }else if(c == CMO_GEq){
      Stack->Add(new Element(((ope1->Val() >= ope2->Val()) ? 1 : 0), &Var));
    }else if(c == '&'){
      Stack->Add(new Element((
        ((ope1->Val() != 0) && (ope2->Val() != 0)) ? 1 : 0), &Var));
    }else if(c == '|'){
      Stack->Add(new Element((
        ((ope1->Val() != 0) || (ope2->Val() != 0)) ? 1 : 0), &Var));
    }else if(c == '?'){
      if(ope1->Val() == 0){
        fs->Position = ope2->Val();
      }
      Clear(Stack);
    }else if(c == ']'){
      Stack->Add(new Element(ope1->Val(), ope2->Val(), &Var));
    }

    delete ope1;
    delete ope2;
  }
}
//---------------------------------------------------------------------------
Element *TMacro::Do(AnsiString AFileName, TList *AStack){
  Element *ReturnValue = NULL;
  try{
    FileName = AFileName;
    fs = new TFileStream(FileName, fmOpenRead | fmShareDenyWrite);
    if(AStack){
      Stack = AStack;
    }else{
      Stack = new TList();
    }

    Var.clear();
    Var["x"] = Element(MGCol, &Var);
    Var["y"] = Element(MGRow, &Var);
    char Type;
    LoopCount = 0;
    while(fs->Read(&Type, 1) > 0) {
      if(Type == '$'){
        Stack->Add(new Element(ReadString(fs), &Var));
      }else if(Type == '*'){
        ExecFnc(ReadString(fs));
      }else if(Type == '~'){
        Stack->Add(new Element(ReadString(fs), etVar, &Var));
      }else if(Type == '!'){
        Stack->Add(new Element(ReadString(fs), etSystem, &Var));
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
        Stack->Add(new Element((double)i, &Var));
      }else if(Type == 'd' || Type == 'I'){
        double d;
        fs->Read(&d, sizeof(double));
        Stack->Add(new Element(d, &Var));
      }
    }

  }catch(MacroException e){
    if(e.Type == ME_HIKISU){
      e.Message = "引数の数が足りません:" + e.Message;
    }
    Application->MessageBox(e.Message.c_str(), "Cassava Macro Interpreter", 0);
  }catch(Exception *e){
    Application->MessageBox(e->Message.c_str(), "Cassava Macro Interpreter", 0);
  }

  delete fs;
  //Application->MessageBox(((AnsiString)Stack->Count).c_str() ,"Stack",0);

  Clear(Stack); delete Stack;
  return ReturnValue;
}
//---------------------------------------------------------------------------
void ExecMacro(AnsiString FileName, int MaxLoop, AnsiString Read, AnsiString Write)
{
  if(Read != ""){
    canReadFile = true;
    canWriteFile = false;
    fs_io = new TFileStream(Read, fmOpenRead);
  }else if(Write != ""){
    canReadFile = false;
    canWriteFile = true;
    fs_io = new TFileStream(Write, fmCreate | fmShareDenyWrite);
  }else{
    canReadFile = false;
    canWriteFile = false;
    fs_io = NULL;
  }

  randomize();
  TMacro mcr;
  mcr.MaxLoop = MaxLoop;
  Element *r = mcr.Do(FileName, NULL);
  if(r) delete r;

  if(fs_io){ delete fs_io; }
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

