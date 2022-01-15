//---------------------------------------------------------------------------
#ifdef CssvMacro
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Compiler.h"
#include "MacroOpeCode.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#define QUOTE '\''
#define DBLQUOTE '\"'

#define tpInteger   'i'
#define tpDouble    'd'
#define tpString    '$'
#define tpFunc      '*'
#define tpVar       '~'
#define tpSystemVar '!'
#define tpOpe       '-'
#define tpComment   '#'
#define tpStructure ';'

#define prElement 100
#define prEOF -1

#define CMCEOF CMCElement("",prEOF,tpStructure)
//---------------------------------------------------------------------------
class CMCElement{
public:
  AnsiString str;
  int pri;
  char type;
  CMCElement() : str(""), pri(prEOF), type(tpStructure) {};
  CMCElement(AnsiString s);
  CMCElement(char *s);
  CMCElement::CMCElement(AnsiString s, int p, char t) : str(s), pri(p), type(t) {};
  CMCElement::CMCElement(const CMCElement &e) : str(e.str), pri(e.pri), type(e.type) {};
  bool operator >(CMCElement e);
  bool operator ==(CMCElement e);
  bool iseof(){ return(pri == prEOF); }
};
//---------------------------------------------------------------------------
CMCElement::CMCElement(AnsiString s){
  str = s; pri = 100; type=tpVar;
  if(s == ";"){ pri = 0; type=tpStructure; }
  else if(s == ","){ pri = 2; type=tpStructure; }
  else if(s == "(" || s == ")" || s == "{" || s == "}" || s == "[" || s == "]"){
    pri = 1; type=tpStructure;
  }
  else if(s == "++" || s == "--" || s == "!"){ pri = 50; type='-'; }
  //           "P"        50 ;     '-'
  else if(s == "^"){ pri = 40; type='-'; }
  else if(s == "*" || s == "/" || s == "%"){ pri = 30; type='-'; }
  else if(s == "+" || s == "-"){ pri = 20; type='-'; }
  else if(s == "<" || s == ">" || s == "<=" || s == ">="){ pri = 17; type='-'; }
  else if(s == "==" || s == "!="){ pri = 15; type='-'; }
  else if(s == "&&" || s == "&"){ pri = 13; type='-'; }
  else if(s == "||" || s == "|"){ pri = 12; type='-'; }
  else if(s == "=" || s == "=&"){ pri = 10; type='-'; }
  else if(s.Length()>0 && s[1]=='.'){type='d';}
  else if(s.Length()>0 && s[1]>='0' && s[1]<='9'){
    if(s.AnsiPos(".") > 0) type='d'; else  type='i';
  }
}
//---------------------------------------------------------------------------
CMCElement::CMCElement(char *s){
  CMCElement((AnsiString)s);
}
//---------------------------------------------------------------------------
bool CMCElement::operator >(CMCElement e){ return pri > e.pri; }
bool CMCElement::operator ==(CMCElement e){ return pri == e.pri; }
//---------------------------------------------------------------------------
class TEList {
  TList *L;
public:
  TEList(){
    L = new TList();
    L->Clear();
  }
  int size(){ return L->Count; }
  CMCElement back(){ return *(static_cast<CMCElement *>(L->Last())); }
  void pop_back(){
    CMCElement *e = static_cast<CMCElement *>(L->Last());
    if(e) delete e;
    L->Delete(L->Count - 1);
  }
  void push_back(CMCElement e){ L->Add(new CMCElement(e)); }
  void clear(){
    for(int i=L->Count-1; i>=0; i--){
      CMCElement *e = static_cast<CMCElement *>(L->Items[i]);
      if(e) delete e;
    }
    L->Clear();
  }
  ~TEList(){
    clear();
    delete L;
  }
};
//---------------------------------------------------------------------------
class CMCException{
public:
  AnsiString Message;
  CMCException(AnsiString Mes) : Message(Mes) {}
};
//---------------------------------------------------------------------------
class TTokenizer {
  TStream *fin;
  char buf;
  bool Read(char *c);
  CMCElement last;
  CMCElement next;
  CMCElement GetR();
public:
  int x, y;
  TTokenizer(TStream *stream);
  AnsiString GetString(char EOS);
  CMCElement Get();
  CMCElement GetNext();
};
//---------------------------------------------------------------------------
TTokenizer::TTokenizer(TStream *stream) {
  fin = stream;
  buf = '\0';
  last = CMCEOF;
  next = CMCEOF;
  x = -1; y = 1;
}
//---------------------------------------------------------------------------
bool TTokenizer::Read(char *c) {
  bool ok = (fin->Read(c, 1) == 1);
  if(!ok){
    *c = '\0';
    return false;
  }

  if(*c == '\n'){
    y++; x = 0;
  }else{
    x++;
  }
  return true;
}
//---------------------------------------------------------------------------
AnsiString TTokenizer::GetString(char EOS)
{
	AnsiString s = "";
	char c;
	while(Read(&c)){
		if(c == EOS){
			return s;
		}else if(c == '\n'){
			throw CMCException("文字列定数が終了していません。");
		}else if(c == '\\'){
			Read(&c);
			if(c == 'n') c = '\n';
			else if(c == 't') c = '\t';
			else if(c == 'x'){
       char x, ci, xi;
       Read(&c); Read(&x);
       if(c >= '0' && c <= '9') { ci = (c - '0'); }
       else if(c >= 'A' && c <= 'F') { ci = (c - 'A') + 10; }
       else if(c >= 'a' && c <= 'f') { ci = (c - 'a') + 10; }
       else { throw CMCException("\\x?? エスケープシーケンス が不正です。"); }
       if(x >= '0' && x <= '9') { xi = (x - '0'); }
       else if(x >= 'A' && x <= 'F') { xi = (x - 'A') + 10; }
       else if(x >= 'a' && x <= 'f') { xi = (x - 'a') + 10; }
       else { throw CMCException("\\x?? エスケープシーケンス が不正です。"); }
       c = ci * 16 + xi;
      }
		}else if(c < 0){
      s += c;
      Read(&c);
    }
		s += c;
	}
	throw CMCException("文字列定数が終了していません。");
}
//---------------------------------------------------------------------------
bool IsNumChar(char c) { return ((c >= '0' && c <= '9') || c=='.'); }
bool IsAlphaChar(char c) {
  return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
        || c == '_' || c == ':');
}
//---------------------------------------------------------------------------
CMCElement TTokenizer::GetR()
{
  while(buf <= ' '){
    if(buf < 0){
      AnsiString s = buf;
      Read(&buf);
      s += buf;
      if(s != "　"){
        throw CMCException("不正な文字です：" + s);
      }
    }
    if(!Read(&buf)) {
      return CMCEOF;
    }
  }
  if(buf == '$' || buf == '#' || buf == '~' || buf == '@'){
    throw CMCException((AnsiString)"不正な文字です：" + buf);
  }
  AnsiString s = "";

  if(buf == DBLQUOTE || buf == QUOTE){
    char eos = buf; buf = '\0';
    return CMCElement(GetString(eos), prElement, tpString);
  }else if(IsNumChar(buf)){
    bool IsDouble = false;
    do{
      s += buf;
      if(buf == '.') IsDouble = true;
    }while(Read(&buf) && IsNumChar(buf));
    return CMCElement(s, prElement, (IsDouble ? tpDouble : tpInteger));
  }else if(IsAlphaChar(buf)){
    do{
      s += buf;
      if(buf == ':'){
        if(s.Length() > 2 || s[1] != ':'){
          throw CMCException("記号の位置が不正です：:");
        }
      }
    }while(Read(&buf) && (IsAlphaChar(buf) || IsNumChar(buf)));

    while(buf <= ' ' && Read(&buf));
    if(s == "function" || s == "return"){
      return CMCElement(s, prElement, tpFunc);
    }
    return CMCElement(s, prElement, ((buf == '(') ? tpFunc : tpVar));
  }else{
    char c1 = buf;
    if(!Read(&buf)) buf = '\0';
    s = (AnsiString)c1 + (AnsiString)buf;
    if(s == "//"){
      while(Read(&buf)) {
        if(buf=='\n' || buf=='\r'){ buf='\0'; break; }
      }
      return GetR();
    }else if(s == "/*"){
    	while(Read(&buf)){
			  if(c1 == '*' && buf == '/'){ buf='\0'; break; }
				c1 = buf;
      }
      return GetR();
    }else{
      CMCElement elm = CMCElement((AnsiString)c1);
      if(s == "==" || s == "=&" || s == "!=" || s == "<=" || s == ">=" ||
         s == "++" || s == "--" || s == "&&" || s == "||"){
         buf = '\0';
        elm = CMCElement(s);
      }
      if(elm.type == tpStructure) return elm;
      if(last.type == tpOpe ||
        (last.type == tpStructure && last.str != ")" && last.str != "]") ||
        (last.type == tpFunc && last.str == "return")){
        if(c1 == '!' || s == "++" || s == "--"){
          return elm;
        }else if(c1 == '*'){
          return CMCElement("P", 50, tpOpe);
        }else if(c1 == '-'){
          return CMCElement("m", 50, tpOpe);
        }else if(c1 == '+'){
          return GetR();
        }else{
          throw CMCException("演算子の位置が不正です：" + elm.str);
        }
      }
      return elm;
    }
  }
}
//---------------------------------------------------------------------------
CMCElement TTokenizer::Get(){
  if(next.iseof()){
    last = GetR();
  }else{
    last = next;
    next = CMCEOF;
  }

  if(last.str.LowerCase() == "true"){
    last = CMCElement("1", prElement, tpInteger);
  }else if(last.str.LowerCase() == "false"
        || last.str.LowerCase() == "null"){
    last = CMCElement("0", prElement, tpInteger);

  }else if(last.str == "MB_ABORTRETRYIGNORE"){
    last = CMCElement(MB_ABORTRETRYIGNORE, prElement, tpInteger);
  }else if(last.str == "MB_OK"){
    last = CMCElement(MB_OK, prElement, tpInteger);
  }else if(last.str == "MB_OKCANCEL"){
    last = CMCElement(MB_OKCANCEL, prElement, tpInteger);
  }else if(last.str == "MB_RETRYCANCEL"){
    last = CMCElement(MB_RETRYCANCEL, prElement, tpInteger);
  }else if(last.str == "MB_YESNO"){
    last = CMCElement(MB_YESNO, prElement, tpInteger);
  }else if(last.str == "MB_YESNOCANCEL"){
    last = CMCElement(MB_YESNOCANCEL, prElement, tpInteger);

  }else if(last.str == "IDABORT"){
    last = CMCElement(IDABORT, prElement, tpInteger);
  }else if(last.str == "IDCANCEL"){
    last = CMCElement(IDCANCEL, prElement, tpInteger);
  }else if(last.str == "IDIGNORE"){
    last = CMCElement(IDIGNORE, prElement, tpInteger);
  }else if(last.str == "IDNO"){
    last = CMCElement(IDNO, prElement, tpInteger);
  }else if(last.str == "IDOK"){
    last = CMCElement(IDOK, prElement, tpInteger);
  }else if(last.str == "IDRETRY"){
    last = CMCElement(IDRETRY, prElement, tpInteger);
  }else if(last.str == "IDYES"){
    last = CMCElement(IDYES, prElement, tpInteger);

  }else if(last.type == tpVar && last.str.Length()>0 && last.str[1] == ':'){
    throw CMCException("変数名が不正です：" + last.str);
  }

  return last;
}
//---------------------------------------------------------------------------
CMCElement TTokenizer::GetNext(){
  if(next.iseof()){
    next = GetR();
  }
  return next;
}
//---------------------------------------------------------------------------
class TCompiler {
private:
  enum chartype { ctSpace, ctAlpha, ctNum, ctOpe };
  bool Fail;
  TTokenizer *lex;
  TStream *fout;
  AnsiString InName;
  TStrings *PrivateFunctions;
  TStringList *Modules;

  chartype GetType(char c);
  void Output(CMCElement e);
  void Push(CMCElement e, TEList *L);
  void GetIf();
  void GetWhile();
  void GetFor();
  void GetBasicFor();
  void GetFunction();
  void GetReturn();
  void GetCell();
  bool GetSentence(char EOS, char *nHikisu = NULL);
  void GetBlock();

public:
  TStrings *import;

  bool Compile(TStream *stream, AnsiString inpath, AnsiString inname,
               AnsiString ext, TStringList *modules, bool showMessage);
  TCompiler() { import = new TStringList; }
  ~TCompiler() { delete import; }
};
//---------------------------------------------------------------------------
TCompiler::chartype TCompiler::GetType(char c)
{
	if(c <= ' ') return ctSpace;
	else if(IsAlphaChar(c)) return ctAlpha;
	else if(IsNumChar(c)) return ctNum;
	return ctOpe;
}
//---------------------------------------------------------------------------
void TCompiler::GetIf()
{
  lex->Get(); // '('
  GetSentence(')');
  fout->Write("i",1);
  int bp1fr = fout->Position;
  fout->Seek(sizeof(int), soFromCurrent);
  fout->Write("-?",2);
  GetSentence(';');

  if(lex->GetNext().str == "else"){
    lex->Get(); // else
    fout->Write("i",1);
    int bp2fr = fout->Position;
    fout->Seek(sizeof(int), soFromCurrent);
    fout->Write("-g",2);

    int bp1to = fout->Position;
    fout->Position = bp1fr;
    fout->Write(&bp1to, sizeof(int));
    fout->Position = bp1to;

    GetSentence(';');

    int bp2to = fout->Position;
    fout->Position = bp2fr;
    fout->Write(&bp2to, sizeof(int));
    fout->Position = bp2to;
  }else{
    int bp1to = fout->Position;
    fout->Position = bp1fr;
    fout->Write(&bp1to, sizeof(int));
    fout->Position = bp1to;
  }
}
//---------------------------------------------------------------------------
void TCompiler::GetWhile()
{
  lex->Get(); // '('
  int bp1to = fout->Position;
  GetSentence(')');
  fout->Write("i",1);
  int bp2fr = fout->Position;
  fout->Seek(sizeof(int), soFromCurrent);
  fout->Write("-?",2);
  GetSentence(';');
  fout->Write("i",1);
  fout->Write(&bp1to, sizeof(int));
  fout->Write("-g",2);
  int bp2to = fout->Position;
  fout->Position = bp2fr;
  fout->Write(&bp2to, sizeof(int));
  fout->Position = bp2to;
}
//---------------------------------------------------------------------------
void TCompiler::GetFor()
{
  lex->Get(); // '('
  GetSentence(';'); // 初期設定部
  int bp1to = fout->Position; // ここに戻ってループ
  GetSentence(';'); // 条件部
  fout->Write("i",1);
  int bp2fr = fout->Position; // ループから外へ
  fout->Seek(sizeof(int), soFromCurrent);
  fout->Write("-?",2);

  TStream *fs = fout;
  TStream *ms = new TMemoryStream();
  fout = ms;
  GetSentence(')'); // 再初期化
  fout = fs;

  GetSentence(';'); // ループ本体
  fout->CopyFrom(ms, 0);
  delete ms;

  fout->Write("i",1);
  fout->Write(&bp1to, sizeof(int));
  fout->Write("-g",2);
  int bp2to = fout->Position;
  fout->Position = bp2fr;
  fout->Write(&bp2to, sizeof(int));
  fout->Position = bp2to;
}
//---------------------------------------------------------------------------
void TCompiler::GetBasicFor()
{
  lex->Get();                    // '('
  CMCElement evar  = lex->Get(); // 変数
  CMCElement eto = lex->Get();   // "To"
  if(eto.str.LowerCase() != "to"){
    throw CMCException("For([変数] to [最大値]){} 構文が間違っています");
  }

  int istt = 1;
  Output(evar);
  fout->Write("i",1);
  fout->Write(&istt, sizeof(int));
  fout->Write("-=",2);
  int bp1to = fout->Position; // ここに戻ってループ

  Output(evar);
  GetSentence(')');     // 最大値
  fout->Write("-Li",3); // "-L" は "<="
  int bp2fr = fout->Position; // ループから外へ
  fout->Seek(sizeof(int), soFromCurrent);
  fout->Write("-?",2);

  GetSentence(';');     // ループ本体

  Output(evar);
  fout->Write("-ii",3);  // "-i" は "++"
  fout->Write(&bp1to, sizeof(int));
  fout->Write("-g",2);
  int bp2to = fout->Position;
  fout->Position = bp2fr;
  fout->Write(&bp2to, sizeof(int));
  fout->Position = bp2to;
}
//---------------------------------------------------------------------------
void TCompiler::GetFunction()
{
  TStream *orgFOut = fout;

  AnsiString FunctionName = lex->Get().str;
  if(lex->Get().str != "("){
    throw CMCException("function文が不正です。");
  }else if(FunctionName.AnsiPos(':')>0){
    throw CMCException("function名が不正です：" + FunctionName);
  }
  if(PrivateFunctions->IndexOf(FunctionName) < 0){
    PrivateFunctions->Add(FunctionName);
  }

  fout = new TMemoryStream();
  char H;
  GetSentence(')', &H);     // 引数
  int argcount = H;
  H *= 2;
  bool func_equal = false;

  if(lex->GetNext().str == ";"){
    delete fout;
    fout = orgFOut;
    return;
  }else if(lex->GetNext().str == "="){
    lex->Get();
    func_equal = true;
  }


  AnsiString OutName = InName + "$" + FunctionName + "$" + argcount;
  Modules->AddObject(OutName, fout);

  fout->Write("*\x06",2);
  fout->Write(&H, 1);
  fout->Write("func=",5);

  if(func_equal){
    GetReturn();
  }else{
    GetSentence(';');     // 関数本体
  }

  fout = orgFOut;
}
//---------------------------------------------------------------------------
void TCompiler::GetReturn()
{
  char H;
  GetSentence(';', &H);
  if(H == 0){
    fout->Write("i\x7f\x7f\x7f\x7f-g",7);
  }else if(H == 1){
    fout->Write("-}",2);
  }else{
    throw CMCException((AnsiString)"return 文に引数が"
                     + (int)H + "個あります。");
  }
}
//---------------------------------------------------------------------------
void TCompiler::GetCell()
{
  char H;
  GetSentence(']', &H);
  if(H == 2){
    fout->Write("-]",2);
  }else{
    throw CMCException((AnsiString)"[x,y] 形式に , が"
                     + (int)(H-1) + "個あります。");
  }
}
//---------------------------------------------------------------------------
void TCompiler::Output(CMCElement e)
{
  if(e.type == tpComment || e.type == tpStructure) return;
    if(e.type == tpVar){
      if(AnsiPos("/" + e.str + "/",
         "/Col/Row/Right/Bottom/SelLeft/SelTop/SelRight/SelBottom/") > 0) {
        e.type = tpSystemVar;
      }
    }

    fout->Write(&(e.type), 1);
    switch(e.type){
    case '$': case '*': case '~': case '!': {
      int iL = e.str.Length();
      if(iL > 255){
        throw CMCException("文字列定数は255文字以内にしてください：" + e.str);
      }
      unsigned char L = (unsigned char)iL;
      fout->Write(&L,1);
      fout->Write(e.str.c_str(), L);
      break;
    }
    case '-':
      if(e.str.Length() == 1){
        fout->Write(e.str.c_str(), 1);
      }else{
        char c = CMOCode(e.str);
        if(c == '\0'){
          throw CMCException("サポートされていない演算子です：" + e.str);
        }else{
          fout->Write(&c, 1);
        }
      }
      break;
    case 'i': {
      int i = e.str.ToInt();
      fout->Write(&i, sizeof(int));
      break;
    }
    case 'd': {
      double d = e.str.ToDouble();
      fout->Write(&d, sizeof(double));
      break;
    }
  }
}
//---------------------------------------------------------------------------
void TCompiler::Push(CMCElement e, TEList *L)
{
  if(e.iseof()) return;
	while(L->size() > 0){
		if(L->back() > e || (L->back() == e && e.str != "P")) {
			Output(L->back());
			L->pop_back();
		}else{
			break;
		}
	}
	L->push_back(e);
}
//---------------------------------------------------------------------------
bool TCompiler::GetSentence(char EOS, char *nHikisu)
{
  CMCElement e;
  char hikisu = 1;

  bool firstloop = true;
	TEList ls;
	while(true){
    e = lex->Get();
    if(e.iseof()){ return false; }

    if(e.type == tpStructure){
      char c = e.str[1];

      switch(c){
      case ';': case ')': case ']':
        if(c != EOS){
          throw CMCException("括弧の対応が正しくありません。");
        }
        Push(e, &ls);
        if(firstloop) hikisu = 0;
        if(nHikisu) *nHikisu = hikisu;
        return true;
      case '}':
        if(!firstloop){
          throw CMCException("} の前に ; が必要です。");
        }
        return false; // ブロック終了
      case '(':
        GetSentence(')');
        break;
      case '{':
        if(!firstloop){
          throw CMCException("{ の前に ; が必要です。");
        }
        GetBlock();
        return true; // 外側のブロックは終了しない
      case ',':
        if(c == EOS){
          Push(e, &ls);
          if(firstloop) hikisu = 0;
          if(nHikisu) *nHikisu = hikisu;
          return true;
        }
        hikisu++;
        Push(e, &ls);
        break;
      case '[':
        GetCell();
        break;
      }
    }else if(e.type == tpFunc){
      if(AnsiPos("/" + e.str + "/",
                 "/if/while/for/For/function/return/") > 0) {
        if(!firstloop){
          throw CMCException(e.str + " の前に ; が必要です。");
        }
        if     (e.str == "if")       { GetIf();       }
        else if(e.str == "while")    { GetWhile();    }
        else if(e.str == "for")      { GetFor();      }
        else if(e.str == "For")      { GetBasicFor(); }
        else if(e.str == "function") { GetFunction(); }
        else if(e.str == "return")   { GetReturn();   }
        return true;
      }else{
        lex->Get(); // "(" があるはず
        char H;
        GetSentence(')', &H);
        if(e.str == "mid" && lex->GetNext().str == "=" && H == 3 && firstloop){
          lex->Get(); // "="
          GetSentence(';');
          e.str = "\x04" "mid=";
          Push(e, &ls);
          Push(CMCElement(";"), &ls);
          return true;
        }else{
          int pp;
          if((pp = AnsiPos(".", e.str)) > 0){
            AnsiString imp = e.str.SubString(1, pp-1);
            if(import->IndexOf(imp) < 0){
              import->Add(imp);
            }
            e.str = (AnsiString)H + "$" + imp + "$"
                  + e.str.SubString(pp + 1, e.str.Length() - pp);
          }else if(PrivateFunctions->IndexOf(e.str) >= 0){
            AnsiString imp = InName;
            e.str = (AnsiString)H + "$" + imp + "$" + e.str;
          }else if(e.str.Length() > 2 && e.str[1] == ':' && e.str[2] == ':'){
            e.str = (AnsiString)H + e.str.SubString(3, e.str.Length()-2);
          }else{
            e.str = (AnsiString)H + e.str;
          }
          Push(e, &ls);
        }
      }
    }else{
      Push(e, &ls);
    }
    firstloop = false;
  }
}
//---------------------------------------------------------------------------
void TCompiler::GetBlock()
{
  while(GetSentence(';'));
}
//---------------------------------------------------------------------------
bool TCompiler::Compile(TStream *stream,
                        AnsiString inpath, AnsiString inname, AnsiString ext,
                        TStringList *modules, bool showMessage)
{
  InName = inname;
  Fail = false;
  lex = new TTokenizer(stream);
  fout = new TMemoryStream();
  Modules = modules;
  Modules->AddObject(inname, fout);
  PrivateFunctions = new TStringList();

  try {
    GetBlock();
  }catch(CMCException e){
    if(showMessage){
	  String str = inpath + inname + ext
		+ (AnsiString)"\nL" + lex->y + "\tC" + lex->x + "\n" + e.Message;
	  Application->MessageBox(str.c_str(), TEXT("Cassava Macro Compiler"), 0);
	}
	Fail = true;
  }catch(Exception *e){
	if(showMessage){
      String str = inpath + inname + ext
        + (AnsiString)"\nL" + lex->y + "\tC" + lex->x + "\n" + e->Message;
      Application->MessageBox(str.c_str(), TEXT("Cassava Macro Compiler"), 0);
    }
    Fail = true;
  }
  delete lex;
  delete PrivateFunctions;
  return !Fail;
}
//---------------------------------------------------------------------------
bool MacroCompile(TStream *stream, TStringList *inpaths,
                  AnsiString inpath, AnsiString inname, AnsiString ext,
                  TStringList *modules, bool showMessage)
{
  if(modules == NULL){ return false; }

  TCompiler Cmp;
  Cmp.import->Add(inname);

  bool OK = Cmp.Compile(stream, inpath, inname, ext, modules, showMessage);
  for(int i=1; OK && (i < Cmp.import->Count); i++){
    TStream *libstream = NULL;
    AnsiString libname = Cmp.import->Strings[i];
    AnsiString libFileName = "";
    try{
      for(int j=0; j<inpaths->Count; j++){
        AnsiString str = inpaths->Strings[j] + libname + ext;
        if(FileExists(str)){
          libFileName = str;
          break;
        }
      }
      if(libFileName != ""){
        libstream = new TFileStream(libFileName, fmOpenRead | fmShareDenyWrite);
        OK = Cmp.Compile(libstream, inpath, libname, ext, modules, showMessage);
      }else{
        if(showMessage){
          String str = libname + ext + "\nファイルが見つかりません。";
          Application->MessageBox(str.c_str(), TEXT("Cassava Macro Compiler"), 0);
        }
        OK = false;
      }
    }catch(Exception *e){
      if(showMessage){
        String str = libFileName + "\n" + e->Message;
        Application->MessageBox(str.c_str(), TEXT("Cassava Macro Compiler"), 0);
      }
      OK = false;
    }
    if(libstream) { delete libstream; }
  }
  return OK;
}
//---------------------------------------------------------------------------
bool MacroCompile(AnsiString infile, TStringList *inpaths,
                  TStringList *modules, bool showMessage)
{
  TStream *stream = NULL;
  bool OK;
  try{
    stream = new TFileStream(infile, fmOpenRead | fmShareDenyWrite);
    String inpath = ExtractFilePath(infile);
	if(*(inpath.LastChar()) != '\\') inpath += "\\";
	String inname = ChangeFileExt(ExtractFileName(infile),"");
	String ext  = ExtractFileExt(infile);
	OK = MacroCompile(stream, inpaths, inpath, inname, ext, modules, showMessage);
  }catch(Exception *e){
	if(showMessage){
	  String str = infile + "\n" + e->Message;
      Application->MessageBox(str.c_str(), TEXT("Cassava Macro Compiler"), 0);
    }
    OK = false;
  }
  if(stream) { delete stream; }
  return OK;
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

