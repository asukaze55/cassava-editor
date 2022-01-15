//---------------------------------------------------------------------------
#include <deque>
#include <map>
#include <vcl.h>
#include <vector>
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
#define tpOpe       '-'
#define tpStructure ';'

#define prElement 100
#define prEOF -1

#define CMCEOF CMCElement("", prEOF, tpStructure, 0)

static const int INT_SIZE = sizeof(int);
//---------------------------------------------------------------------------
class CMCElement{
public:
  String str;
  int pri;
  char type;
  int jump;

  CMCElement() : str(""), pri(prEOF), type(tpStructure), jump(0) {}
  CMCElement(String s);
  CMCElement(String s, int p, char t, int j = 0) :
      str(s), pri(p), type(t), jump(j) {}

  bool iseof() const { return(pri == prEOF); }
  bool isSystemVar() const {
    if (type != tpVar) { return false; }
    return AnsiPos("/" + str + "/",
        "/Col/Row/Right/Bottom/SelLeft/SelTop/SelRight/SelBottom/") > 0;
  }
};
//---------------------------------------------------------------------------
CMCElement::CMCElement(String s){
  str = s; pri = 100; type = tpVar; jump = 0;
  if (s == ";") {
    pri = 0; type = tpStructure;
  } else if (s == ",") {
    pri = 2; type = tpStructure;
  } else if (s == "(" || s == ")" || s == "{" || s == "}") {
    pri = 1; type = tpStructure;
  } else if (s == "[" || s == "]") {
    pri = 60; type = tpStructure;
  } else if (s == "." || s == "::") {
    pri = 60; type = tpOpe;
  } else if (s == "++" || s == "--" || s == "!") {
    pri = 50; type = tpOpe;
  } else if (s == "^") {
    pri = 40; type = tpOpe;
  } else if (s == "*" || s == "/" || s == "%") {
    pri = 30; type = tpOpe;
  } else if (s == "+" || s == "-") {
    pri = 20; type = tpOpe;
  } else if (s == "<" || s == ">" || s == "<=" || s == ">=" || s == "=>") {
    pri = 17; type = tpOpe;
  } else if (s == "==" || s == "!=") {
    pri = 15; type = tpOpe;
  } else if (s == "&&" || s == "&") {
    pri = 13; type = tpOpe;
  } else if (s == "||" || s == "|") {
    pri = 12; type = tpOpe;
  } else if (s == "=" || s == ":" || (s.Length() == 2 && s[2] == '=')) {
    pri = 10; type = tpOpe;
  } else if (s.Length() > 0 && s[1] == '.') {
    type = tpDouble;
  } else if (s.Length() > 0 && s[1] >= '0' && s[1] <= '9') {
    if (s.Pos(".") > 0) { type = tpDouble; } else { type = tpInteger; }
  }
}
//---------------------------------------------------------------------------
class CMCException{
public:
  String Message;
  CMCException(String Mes) : Message(Mes) {}
};
//---------------------------------------------------------------------------
class TTokenizer {
  String source;
  int pos;
  bool Read(wchar_t *c);
  wchar_t ReadHex();
  wchar_t Peek();
  CMCElement last;
  std::deque<CMCElement> elements;
  CMCElement GetR();
public:
  int x, y;
  TTokenizer(String src) :
      source(src), pos(0), last(CMCEOF), elements(), x(0), y(1) {}
  String GetString(wchar_t EOS);
  CMCElement Get();
  CMCElement GetNext(size_t index = 0);
};
//---------------------------------------------------------------------------
bool TTokenizer::Read(wchar_t *c)
{
  if (pos >= source.Length()) {
    *c = '\0';
    return false;
  }
  *c = source[++pos];
  if (*c == '\n') {
    y++;
    x = 0;
  } else {
    x++;
  }
  return true;
}
//---------------------------------------------------------------------------
wchar_t TTokenizer::ReadHex()
{
  wchar_t c;
  Read(&c);
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return (c - 'A') + 10;
  } else if (c >= 'a' && c <= 'f') {
    return (c - 'a') + 10;
  }
  throw CMCException((String)"エスケープシーケンス が不正です：" + c);
}
//---------------------------------------------------------------------------
wchar_t TTokenizer::Peek()
{
  if (pos >= source.Length()) {
    return '\0';
  }
  return source[pos + 1];
}
//---------------------------------------------------------------------------
String TTokenizer::GetString(wchar_t EOS)
{
  String s = "";
  AnsiString ansiStr = "";
  wchar_t c;
  while (Read(&c)) {
    if (c == EOS) {
      return s + ansiStr;
    } else if (c == '\n') {
      throw CMCException("文字列定数が終了していません。");
    } else if (c == '\\') {
      Read(&c);
      if (c == 'n') {
        c = '\n';
      } else if (c == 't') {
        c = '\t';
      } else if (c == 'u') {
        c = ReadHex() * 4096 + ReadHex() * 256 + ReadHex() * 16 + ReadHex();
      } else if (c == 'x') {
        ansiStr += (char)(ReadHex() * 16 + ReadHex());
        continue;
      }
    }
    if (ansiStr != "") {
      s += ansiStr;
      ansiStr = "";
    }
    s += c;
  }
  throw CMCException("文字列定数が終了していません。");
}
//---------------------------------------------------------------------------
inline bool IsNumChar(wchar_t c) { return (c >= '0' && c <= '9'); }
inline bool IsNumCharOrDot(wchar_t c) { return IsNumChar(c) || c == '.'; }
inline bool IsAlphaChar(wchar_t c) {
  return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_');
}
inline bool IsAlphaNumChar(wchar_t c) {
  return IsAlphaChar(c) || IsNumChar(c);
}
inline bool IsUnaryOpeExpected(const CMCElement& last) {
  return last.type == tpOpe
      || (last.type == tpStructure && last.str != ")" && last.str != "]")
      || (last.type == tpFunc && last.str == "return");
}
//---------------------------------------------------------------------------
CMCElement TTokenizer::GetR()
{
  wchar_t c;
  do {
    if (!Read(&c)) {
      return CMCEOF;
    }
  } while (c <= ' ' || c == L'　' || c == L'\ufeff');

  if (c == '$' || c == '#' || c == '@' || c >= '~') {
    throw CMCException((String)"不正な文字です：" + c);
  }

  if (c == DBLQUOTE || c == QUOTE) {
    return CMCElement(GetString(c), prElement, tpString);
  }
  bool isUnaryOpeExpected = IsUnaryOpeExpected(elements.size() == 0 ?
                                               last : elements.back());
  if (IsNumChar(c) || (c == '.' && isUnaryOpeExpected)) {
    bool IsDouble = (c == '.');
    String s = c;
    while (IsNumCharOrDot(Peek())) {
      Read(&c);
      s += c;
      if(c == '.') IsDouble = true;
    }
    if (s == "...") {
      return CMCElement(s, prElement, tpOpe);
    }
    return CMCElement(s, prElement, (IsDouble ? tpDouble : tpInteger));
  }
  if (IsAlphaChar(c)) {
    String s = c;
    while (IsAlphaNumChar(Peek())) {
      Read(&c);
      s += c;
    }
    while (Peek() <= ' ') {
      if (!Read(&c)) { break; }
    }
    if (s == "function" || s == "return") {
      return CMCElement(s, prElement, tpFunc);
    } else if (s == "in" && !isUnaryOpeExpected) {
      return CMCElement(CMO_In, 17, tpOpe);
    }
    return CMCElement(s, prElement, (Peek() == '(') ? tpFunc : tpVar);
  }

  String s1 = c;
  String s2 = s1 + Peek();
  if (s2 == "//") {
    while (Read(&c)) {
      if (c == '\n' || c == '\r') { return GetR(); }
    }
    return CMCEOF;
  }
  if(s2 == "/*"){
    wchar_t c1;
    Read(&c1); // *
    Read(&c1); // First char in the comment
    while (Read(&c)) {
      if (c1 == '*' && c == '/') { return GetR(); }
      c1 = c;
    }
    return CMCEOF;
  }

  CMCElement elm = CMCElement(s1);
  if (s2 == "==" || s2 == "!=" || s2 == "<=" || s2 == ">=" || s2 == "=>" ||
      s2 == "++" || s2 == "--" || s2 == "&&" || s2 == "||" || s2 == "::" ||
      s2 == "+=" || s2 == "-=" || s2 == "*=" || s2 == "/=") {
    Read(&c);
    elm = CMCElement(s2);
  }
  if (elm.type == tpStructure) {
    return elm;
  }
  if (isUnaryOpeExpected) {
    if (s1 == "!" || s2 == "++" || s2 == "--" || s2 == "::") {
      return elm;
    } else if (s1 == "-") {
      return CMCElement("m", 50, tpOpe);
    } else if (s1 == "+") {
      return GetR();
    } else {
      throw CMCException("演算子の位置が不正です：" + elm.str);
    }
  }
  return elm;
}
//---------------------------------------------------------------------------
CMCElement TTokenizer::Get(){
  if (elements.size() == 0) {
    last = GetR();
  } else {
    last = elements.front();
    elements.pop_front();
  }

  if (last.type != tpVar) {
    return last;
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
  }

  return last;
}
//---------------------------------------------------------------------------
CMCElement TTokenizer::GetNext(size_t index){
  while (elements.size() <= index) {
    elements.push_back(GetR());
  }
  return elements.at(index);
}
//---------------------------------------------------------------------------
inline TStringList *newTStringList() {
  TStringList *list = new TStringList();
  list->CaseSensitive = true;
  return list;
}
//---------------------------------------------------------------------------
enum FunctionType { FUNCTION, METHOD, LAMBDA };
//---------------------------------------------------------------------------
class TCompiler {
private:
  bool Fail;
  TTokenizer *lex;
  TStream *fout;
  String InName;
  std::map<String, String> ImportedFunctions;
  TStringList *Modules;
  TStringList *Variables;
  TStringList *Constants;
  TStringList *CapturableVariables;
  TStringList *CapturedVariables;
  std::vector<__int64> *Breaks;
  std::vector<__int64> *Continues;
  int DummyFunctionName;

  void Output(CMCElement e);
  int OutputPositionPlaceholder();
  void Push(CMCElement e, std::deque<CMCElement> *L);
  void PushAll(std::deque<CMCElement> *L);
  void GetIf();
  void GetWhile();
  void GetFor();
  void GetBasicFor();
  String GetFunction(FunctionType functionType, String paramName = "");
  void GetReturn(char EOS);
  void GetLambda(String paramName = "");
  void GetCell();
  String GetObject();
  void GetObjectKey();
  void GetClass();
  void GetImport();
  bool GetValues(char EOS, char *nHikisu = NULL);
  bool GetSentence(char EOS, bool allowBlock = true, char *nHikisu = NULL);
  void GetBlock();

  bool IsKnownVariable(const CMCElement& e) const {
    return e.type == tpVar
        && (e.isSystemVar() || Variables->IndexOf(e.str) >= 0 ||
            CapturableVariables->IndexOf(e.str) >= 0);
  }

public:
  TStringList *import;

  bool Compile(String source, String filePath, String libName,
               TStringList *modules, bool showMessage);
  TCompiler() : Breaks(NULL), Continues(NULL), DummyFunctionName(0) {
    import = newTStringList();
  }
  ~TCompiler() { delete import; }
};
//---------------------------------------------------------------------------
void TCompiler::GetIf()
{
  lex->Get(); // '('
  GetValues(')');
  int bp1fr = OutputPositionPlaceholder();
  fout->Write("-?",2);
  GetSentence(';');

  if(lex->GetNext().str == "else"){
    lex->Get(); // else
    int bp2fr = OutputPositionPlaceholder();
    fout->Write("-g",2);

    int bp1to = fout->Position;
    fout->Position = bp1fr;
    fout->Write(&bp1to, INT_SIZE);
    fout->Position = bp1to;

    GetSentence(';');

    int bp2to = fout->Position;
    fout->Position = bp2fr;
    fout->Write(&bp2to, INT_SIZE);
    fout->Position = bp2to;
  }else{
    int bp1to = fout->Position;
    fout->Position = bp1fr;
    fout->Write(&bp1to, INT_SIZE);
    fout->Position = bp1to;
  }
}
//---------------------------------------------------------------------------
void TCompiler::GetWhile()
{
  std::vector<__int64> *originalBreaks = Breaks;
  std::vector<__int64> breaks;
  Breaks = &breaks;

  std::vector<__int64> *originalContinues = Continues;
  std::vector<__int64> continues;
  Continues = &continues;

  lex->Get(); // '('
  int bp1to = fout->Position;
  GetValues(')');
  breaks.push_back(OutputPositionPlaceholder());
  fout->Write("-?",2);
  GetSentence(';');
  fout->Write("i",1);
  fout->Write(&bp1to, INT_SIZE);
  fout->Write("-g",2);
  int bp2to = fout->Position;

  for (std::vector<__int64>::iterator p = breaks.begin();
       p != breaks.end(); ++p) {
    fout->Position = *p;
    fout->Write(&bp2to, INT_SIZE);
  }
  Breaks = originalBreaks;

  for (std::vector<__int64>::iterator p = continues.begin();
       p != continues.end(); ++p) {
    fout->Position = *p;
    fout->Write(&bp1to, INT_SIZE);
  }
  Continues = originalContinues;

  fout->Position = bp2to;
}
//---------------------------------------------------------------------------
void TCompiler::GetFor()
{
  std::vector<__int64> *originalBreaks = Breaks;
  std::vector<__int64> breaks;
  Breaks = &breaks;

  std::vector<__int64> *originalContinues = Continues;
  std::vector<__int64> continues;
  Continues = &continues;

  lex->Get(); // '('
  GetValues(';'); // 初期設定部
  int bp1to = fout->Position; // ここに戻ってループ
  GetValues(';'); // 条件部
  breaks.push_back(OutputPositionPlaceholder());
  fout->Write("-?",2);

  TStream *fs = fout;
  TStream *ms = new TMemoryStream();
  fout = ms;
  GetValues(')'); // 再初期化
  fout = fs;

  GetSentence(';'); // ループ本体
  int bp3to = fout->Position; // Continue
  fout->CopyFrom(ms, 0);
  delete ms;

  fout->Write("i",1);
  fout->Write(&bp1to, INT_SIZE);
  fout->Write("-g",2);
  int bp2to = fout->Position;

  for (std::vector<__int64>::iterator p = breaks.begin();
       p != breaks.end(); ++p) {
    fout->Position = *p;
    fout->Write(&bp2to, INT_SIZE);
  }
  Breaks = originalBreaks;

  for (std::vector<__int64>::iterator p = continues.begin();
       p != continues.end(); ++p) {
    fout->Position = *p;
    fout->Write(&bp3to, INT_SIZE);
  }
  Continues = originalContinues;

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
  fout->Write(&istt, INT_SIZE);
  fout->Write("-=",2);
  int bp1to = fout->Position; // ここに戻ってループ

  Output(evar);
  GetValues(')');     // 最大値
  fout->Write("-L", 2); // "-L" は "<="
  int bp2fr = OutputPositionPlaceholder(); // ループから外へ
  fout->Write("-?",2);

  GetSentence(';');     // ループ本体

  Output(evar);
  fout->Write("-ii",3);  // "-i" は "++"
  fout->Write(&bp1to, INT_SIZE);
  fout->Write("-g",2);
  int bp2to = fout->Position;
  fout->Position = bp2fr;
  fout->Write(&bp2to, INT_SIZE);
  fout->Position = bp2to;
}
//---------------------------------------------------------------------------
#define LAMBDA_EOS 'L'
//---------------------------------------------------------------------------
String TCompiler::GetFunction(FunctionType functionType, String paramName)
{
  TStream *orgFOut = fout;
  TStringList* orgVariables = Variables;
  TStringList* orgConstants = Constants;

  String functionName;
  if (functionType == LAMBDA) {
    functionName = ++DummyFunctionName;
  } else {
    functionName = lex->Get().str;
    if (functionName == "(") {
      functionName = ++DummyFunctionName;
    } else if (lex->Get().str == "(") {
      ImportedFunctions[functionName] = InName + "\n" + functionName;
    } else {
      throw CMCException("function文が不正です：" + functionName);
    }
  }

  Variables = newTStringList();
  Constants = newTStringList();
  if (functionType == METHOD) {
    Variables->Add("this");
    Constants->Add("this");
  }

  bool varArg = false;
  fout = new TMemoryStream();
  wchar_t H = 0;
  if (paramName != "") {
    H = 1;
    Variables->Add(paramName);
    Output(CMCElement(paramName, prElement, tpVar));
  } else if (lex->GetNext().str == ")") {
    lex->Get();
  } else {
    while (true) {
      CMCElement e = lex->Get();
      if (e.str == "...") {
        varArg = true;
        e = lex->Get();
      }
      if (e.type != tpVar) {
        throw CMCException("引数名が不正です：" + e.str);
      }
      if (e.isSystemVar() || Variables->IndexOf(e.str) >= 0) {
        throw CMCException("引数名がすでに使用されています：" + e.str);
      }
      Variables->Add(e.str);
      Output(e);
      H++;
      e = lex->Get();
      if (e.str == ')') {
        break;
      } else if (varArg) {
        throw CMCException(") が必要です：" + e.str);
      } else if (e.str != ',') {
        throw CMCException(", もしくは ) が必要です：" + e.str);
      }
    }
  }
  int argCount = H;
  H *= 2;
  bool funcEqual = false;
  String outName = GetMacroModuleName(InName, functionName,
                                      varArg ? argCount - 1 : argCount, varArg);
  if (functionType == LAMBDA) {
    CMCElement arrow = lex->Get();
    if (arrow.str != "=>") {
      throw CMCException("ラムダ式が不正です：" + arrow.str);
    }
    if (lex->GetNext().str != "{") {
      funcEqual = true;
    }
  } else if (lex->GetNext().str == ";") {
    delete fout;
    delete Variables;
    delete Constants;
    fout = orgFOut;
    Variables = orgVariables;
    Constants = orgConstants;
    return outName;
  } else if (lex->GetNext().str == "=") {
    lex->Get();
    funcEqual = true;
  }

  Modules->AddObject(outName, fout);

  fout->Write("*\x06",2);
  fout->Write(&H, 2);
  fout->Write(L"func=", 10);

  if (functionType != LAMBDA) {
    // Ok to use these pre-defined variables.
    Variables->Add("x");
    Variables->Add("y");
    Variables->Add("Left");
    Variables->Add("Top");
  }

  if(funcEqual){
    GetReturn(LAMBDA_EOS);
  }else{
    GetSentence(';');     // 関数本体
  }

  delete Variables;
  delete Constants;
  fout = orgFOut;
  Variables = orgVariables;
  Constants = orgConstants;
  return outName;
}
//---------------------------------------------------------------------------
void TCompiler::GetReturn(char EOS)
{
  char H;
  GetValues(EOS, &H);
  if(H == 0){
    fout->Write("i\x7f\x7f\x7f\x7f-g",7);
  }else if(H == 1){
    fout->Write("-}",2);
  }else{
    throw CMCException((String)"return 文に引数が" + (int)H + "個あります。");
  }
}
//---------------------------------------------------------------------------
void TCompiler::GetLambda(String paramName)
{
  TStringList* orgCapturableVariables = CapturableVariables;
  TStringList* orgCapturedVariables = CapturedVariables;
  CapturableVariables = newTStringList();
  CapturableVariables->AddStrings(orgCapturableVariables);
  CapturableVariables->AddStrings(Variables);
  CapturedVariables = newTStringList();

  String functionName = GetFunction(LAMBDA, paramName);
  if (CapturedVariables->Count == 0) {
    Output(CMCElement(functionName, prElement, tpString));
  } else {
    Output(CMCElement((String)'\0' + "{}", prElement, tpFunc));
    Output(CMCElement("", prElement, tpString));
    Output(CMCElement(functionName, prElement, tpString));
    fout->Write("-:", 2);

    for (int i = 0; i < CapturedVariables->Count; i++) {
      String name = CapturedVariables->Strings[i];
      Output(CMCElement(name, prElement, tpString));
      if (Variables->IndexOf(name) < 0) {
        Output(CMCElement("this", prElement, tpVar));
        Output(CMCElement(name, prElement, tpString));
        Output(CMCElement("."));
        if (orgCapturedVariables->IndexOf(name) < 0) {
          orgCapturedVariables->Add(name);
        }
      } else {
        Output(CMCElement(name, prElement, tpVar));
      }
      fout->Write("-:", 2);
    }
  }

  delete CapturableVariables;
  delete CapturedVariables;
  CapturableVariables = orgCapturableVariables;
  CapturedVariables = orgCapturedVariables;
}
//---------------------------------------------------------------------------
void TCompiler::GetCell()
{
  char H;
  GetValues(']', &H);
  if(H == 2){
    fout->Write("-]",2);
  }else{
    throw CMCException((String)"[x,y] 形式に , が"
                     + (int)(H-1) + "個あります。");
  }
}
//---------------------------------------------------------------------------
void TCompiler::Output(CMCElement e)
{
  if (e.type == tpStructure) {
    if (e.jump != 0) {
      int current = fout->Position;
      fout->Position = e.jump;
      fout->Write(&current, INT_SIZE);
      fout->Position = current;
    }
    return;
  }
  if (e.isSystemVar()) {
    fout->Write("!", 1);
  } else {
    fout->Write(&(e.type), 1);
  }
  switch(e.type){
    case '$': case '*': case '~': {
      int iL = e.str.Length();
      if(iL > 255){
        throw CMCException("文字列定数は255文字以内にしてください：" + e.str);
      }
      unsigned char L = (unsigned char)iL;
      fout->Write(&L,1);
      fout->Write(e.str.c_str(), L * sizeof(wchar_t));
      break;
    }
    case '-': {
      AnsiString ansiStr = e.str;
      if(ansiStr.Length() == 1){
        fout->Write(ansiStr.c_str(), 1);
      }else{
        char c = CMOCode(e.str);
        if(c == '\0'){
          throw CMCException("サポートされていない演算子です：" + e.str);
        }else{
          fout->Write(&c, 1);
        }
      }
      break;
    }
    case 'i': {
      int i = e.str.ToInt();
      fout->Write(&i, INT_SIZE);
      break;
    }
    case 'd': {
      double d = e.str.ToDouble();
      fout->Write(&d, (int)sizeof(double));
      break;
    }
  }
}
//---------------------------------------------------------------------------
int TCompiler::OutputPositionPlaceholder()
{
  fout->Write("i", 1);
  int position = fout->Position;
  fout->Seek(INT_SIZE, soFromCurrent);
  return position;
}
//---------------------------------------------------------------------------
void TCompiler::Push(CMCElement e, std::deque<CMCElement> *L)
{
  if(e.iseof()) return;
  while (L->size() > 0) {
    const CMCElement& b = L->back();
    if (b.pri > e.pri || (b.pri == e.pri && b.jump == 0)) {
      Output(b);
      L->pop_back();
    } else {
      break;
    }
  }
  L->push_back(e);
}
//---------------------------------------------------------------------------
void TCompiler::PushAll(std::deque<CMCElement> *L)
{
  while (L->size() > 0) {
    Output(L->back());
    L->pop_back();
  }
}
//---------------------------------------------------------------------------
String TCompiler::GetObject()
{
  String constructor = "";
  Output(CMCElement((String)'\0' + "{}", prElement, tpFunc));
  while (true) {
    CMCElement key = lex->Get();
    if (key.type == tpVar || key.type == tpString || key.type == tpInteger ||
        key.type == tpDouble) {
      CMCElement colon = lex->Get();
      if (colon.str != ":") {
        throw CMCException(
            "オブジェクトリテラルの形式が正しくありません：" + key.str);
      }
      Output(key);
      bool inBlock = GetValues(',');
      fout->Write("-:", 2);
      if (!inBlock) {
        return constructor;
      }
    } else if (key.type == tpFunc) {
      Output(CMCElement(key.str, prElement, tpVar));
      String functionName = GetFunction(METHOD);
      Output(CMCElement(functionName, prElement, tpString));
      fout->Write("-:", 2);
      if (lex->GetNext().str == ",") { lex->Get(); }
      if (key.str == "constructor") { constructor = functionName; }
    } else if (key.str == "}") {
      return constructor;
    } else if (key.iseof()) {
      throw CMCException("オブジェクトリテラルが終了していません。");
    } else {
      throw CMCException(
          "オブジェクトリテラルの形式が正しくありません：" + key.str);
    }
  }
}
//---------------------------------------------------------------------------
void TCompiler::GetObjectKey()
{
  char H;
  GetValues(']', &H);
  if(H != 1){
    throw CMCException("obj[key] 形式が正しくありません。");
  }
  fout->Write("-.", 2);
}
//---------------------------------------------------------------------------
void TCompiler::GetClass()
{
  String name = lex->Get().str;
  String paren = lex->Get().str;
  if (paren != "{") {
    throw CMCException("クラス宣言には { が必要です：" + paren);
  }
  ImportedFunctions[name] = InName + "\n" + name;
  TStream *orgFOut = fout;
  fout = new TMemoryStream();
  String constructor = GetObject();

  String outName;
  int p = constructor.LastDelimiter("/");
  if (p > 0) {
    outName = InName + "\n" + name
        + constructor.SubString(p, constructor.Length());
    Output(CMCElement((String)'\0' + "(constructor)", prElement, tpFunc));
  } else {
    outName = GetMacroModuleName(InName, name, 0, false);
  }

  fout->Write("-}",2);
  Modules->AddObject(outName, fout);
  fout = orgFOut;
}
//---------------------------------------------------------------------------
void TCompiler::GetImport()
{
  CMCElement e = lex->Get();
  if (e.str != "{") {
    throw CMCException("import 文に { が必要です：" + e.str);
  }
  std::map<String, String> nameMap;
  while (true) {
    e = lex->Get();
    if (e.type != tpVar) {
      throw CMCException("import する関数名・クラス名が不正です：" + e.str);
    }
    String originalName = e.str;
    String aliasName = originalName;
    e = lex->Get();
    if (e.str == "as") {
      e = lex->Get();
      if (e.type != tpVar) {
        throw CMCException("import の別名が不正です：" + e.str);
      }
      aliasName = e.str;
      e = lex->Get();
    }
    nameMap[aliasName] = originalName;
    if (e.str == "}") { break; }
    if (e.str != ",") {
      throw CMCException(", もしくは } が必要です：" + e.str);
    }
  }
  e = lex->Get();
  if (e.str != "from") {
    throw CMCException("import 文に from が必要です：" + e.str);
  }
  e = lex->Get();
  if (e.type != tpString) {
    throw CMCException("import するファイル名が不正です：" + e.str);
  }
  String libName = e.str;
  if (import->IndexOf(libName) < 0) {
    import->Add(libName);
  }
  for (std::map<String, String>::iterator p = nameMap.begin();
       p != nameMap.end(); ++p) {
    ImportedFunctions[p->first] = libName + "\n" + p->second;
  }
  e = lex->Get();
  if (e.str != ";") {
    throw CMCException("import 文に ; が必要です：" + e.str);
  }
}
//---------------------------------------------------------------------------
bool TCompiler::GetValues(char EOS, char *nHikisu)
{
  return GetSentence(EOS, false, nHikisu);
}
//---------------------------------------------------------------------------
bool IsLambda(TTokenizer *lex)
{
  CMCElement next0 = lex->GetNext(0);
  CMCElement next1 = lex->GetNext(1);
  if (next0.type == tpStructure && next0.str == ")" &&
      next1.type == tpOpe && next1.str == "=>") {
    return true;
  }
  if (next0.type == tpVar) {
    if (next1.type == tpStructure && next1.str == ",") {
      return true;
    }
    CMCElement next2 = lex->GetNext(2);
    if (next1.type == tpStructure && next1.str == ")" &&
        next2.type == tpOpe && next2.str == "=>") {
      return true;
    }
  }
  if (next0.type == tpOpe && next0.str == "...") {
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
bool TCompiler::GetSentence(char EOS, bool allowBlock, char *nHikisu)
{
  CMCElement e;
  char hikisu = 1;

  bool firstloop = true;
  std::deque<CMCElement> ls;
  while (true) {
    if (EOS == LAMBDA_EOS) {
      e = lex->GetNext();
      if (e.type == tpStructure && (e.str == ',' || e.str == ';' ||
          e.str == ')' || e.str == ']' || e.str == '}')) {
        PushAll(&ls);
        if (nHikisu) { *nHikisu = firstloop ? 0 : hikisu; }
        return true;
      }
    }

    e = lex->Get();
    if(e.iseof()){ PushAll(&ls); return false; }

    if(e.type == tpStructure){
      char c = e.str[1];

      switch(c){
      case ';': case ')': case ']':
        if(c != EOS){
          throw CMCException("括弧の対応が正しくありません。");
        }
        PushAll(&ls);
        if(firstloop) hikisu = 0;
        if(nHikisu) *nHikisu = hikisu;
        return true;
      case '}':
        if (EOS == '}') {
          PushAll(&ls);
          return true;
        }
        if (EOS == ',') { // Used for object literals.
          PushAll(&ls);
          return false;
        }
        if (!firstloop) {
          throw CMCException("} の前に ; が必要です。");
        }
        return false; // ブロック終了
      case '(':
        if (IsLambda(lex)) {
          GetLambda();
        } else {
          GetValues(')');
        }
        break;
      case '{':
        if (firstloop && allowBlock) {
          GetBlock();
          return true; // 外側のブロックは終了しない
        }
        GetObject();
        break;
      case ',':
        if(c == EOS){
          PushAll(&ls);
          if(firstloop) hikisu = 0;
          if(nHikisu) *nHikisu = hikisu;
          return true;
        }
        hikisu++;
        Push(e, &ls);
        break;
      case '[':
        if (ls.size() > 0) {
          const CMCElement& prev = ls.back();
          if (prev.type == tpVar || prev.type == tpFunc ||
              prev.type == tpString || prev.str == '[') {
            Push(e, &ls);
            GetObjectKey();
            break;
          }
        }
        GetCell();
        break;
      }
    }else if(e.type == tpFunc){
      if (e.str == "function") {
        String funcName = GetFunction(FUNCTION);
        if (firstloop && allowBlock) {
          return true;
        }
        Push(CMCElement(funcName, prElement, tpString), &ls);
      } else if (AnsiPos("/" + e.str + "/",
                         "/if/while/for/For/return/") > 0) {
        if (!firstloop) {
          throw CMCException(e.str + " の前に ; が必要です。");
        }
        if (e.str == "if") {
          GetIf();
        } else if (e.str == "while") {
          GetWhile();
        } else if (e.str == "for") {
          GetFor();
        } else if (e.str == "For") {
          GetBasicFor();
        } else if (e.str == "return") {
          GetReturn(';');
        }
        return true;
      } else {
        lex->Get(); // "(" があるはず
        bool isLambdaCall = ((Variables->IndexOf(e.str) >= 0
                              || CapturableVariables->IndexOf(e.str) >= 0)
                             && (ls.size() == 0 || ls.back().str != "."));
        if (isLambdaCall) {
          if (Variables->IndexOf(e.str) < 0) {
            Output(CMCElement("this", prElement, tpVar));
            Output(CMCElement(e.str, prElement, tpString));
            Output(CMCElement("."));
            if (CapturedVariables->IndexOf(e.str) < 0) {
              CapturedVariables->Add(e.str);
            }
          } else {
            Output(CMCElement(e.str, prElement, tpVar));
          }
        }
        char H;
        GetValues(')', &H);
        if(e.str == "mid" && lex->GetNext().str == "=" && H == 3 && firstloop){
          lex->Get(); // "="
          GetValues(';');
          e.str = "\x04" "mid=";
          Push(e, &ls);
          PushAll(&ls);
          return true;
        }
        if (ls.size() > 0 && ls.back().str == ".") {
          e.str = (String)(++H)  + "." + e.str;
          ls.pop_back();
        } else {
          String prefix = isLambdaCall ? (String)(++H) + ">" : (String) H;
          String funcName = ImportedFunctions.count(e.str) > 0
                          ? "$" + ImportedFunctions[e.str] : e.str;
          e = CMCElement(prefix + funcName, prElement, tpFunc);
        }
        Push(e, &ls);
      }
    }else if(e.type == tpVar) {
      bool isUndefined = !IsKnownVariable(e);
      if (ls.size() > 0 && ls.back().str == ".") {
        Push(CMCElement(e.str, prElement, tpString), &ls);
      } else if (lex->GetNext().str == "." && isUndefined) {
        lex->Get(); // "."
        CMCElement f = lex->Get();
        if (lex->GetNext().str != "(") {
          throw CMCException("値が代入されていない変数です：" + e.str);
        }
        lex->Get(); // "("
        char H;
        GetValues(')', &H);
        String libName = e.str + ".cms";
        if (import->IndexOf(libName) < 0) {
          import->Add(libName);
        }
        String internalName = (String)H + "$" + libName + "\n" + f.str;
        Push(CMCElement(internalName, prElement, tpFunc), &ls);
      } else if ((e.str == "const" || e.str == "let" || e.str == "var") &&
                 lex->GetNext().type == tpVar) {
        bool isConst = (e.str == "const");
        e = lex->Get();
        if (IsKnownVariable(e)) {
          throw CMCException("変数名がすでに使用されています：" + e.str);
        } else if (isConst && lex->GetNext().str != "=") {
          throw CMCException(
              "定数宣言が不正です。「=」で初期値を代入してください：" + e.str);
        }
        Variables->Add(e.str);
        if (isConst) {
          Constants->Add(e.str);
        }
        Push(e, &ls);
      } else if (e.str == "break" && firstloop && lex->GetNext().str == ";") {
        if (!Breaks) {
          throw CMCException("break はループ内でのみ使用できます。");
        }
        Breaks->push_back(OutputPositionPlaceholder());
        fout->Write("-g", 2);
      } else if (e.str == "continue" && firstloop
                 && lex->GetNext().str == ";") {
        if (!Continues) {
          throw CMCException("continue はループ内でのみ使用できます。");
        }
        Continues->push_back(OutputPositionPlaceholder());
        fout->Write("-g", 2);
      } else if (e.str == "class" && lex->GetNext().type == tpVar) {
        if (!firstloop) {
          throw CMCException(e.str + " 宣言の前に ; が必要です。");
        }
        GetClass();
        return true;
      } else if (e.str == "import" && lex->GetNext().str == "{") {
        if (!firstloop) {
          throw CMCException(e.str + " 文の前に ; が必要です。");
        }
        GetImport();
        return true;
      } else if (e.str == "new"
          && (lex->GetNext().type == tpFunc || lex->GetNext().type == tpVar)) {
        // Ignore.
      } else if (lex->GetNext().str == "=>") {
        GetLambda(e.str);
      } else {
        if (isUndefined) {
          if (lex->GetNext().str != "=") {
            throw CMCException("値が代入されていない変数です：" + e.str);
          }
          Variables->Add(e.str);
        } else if (lex->GetNext().str == "=") {
          if (Constants->IndexOf(e.str) >= 0) {
            throw CMCException("定数への再代入はできません：" + e.str);
          } else if (CapturableVariables->IndexOf(e.str) >= 0) {
            throw CMCException("ラムダ式内では変数への再代入はできません：" +
                               e.str);
          }
        } else if (Variables->IndexOf(e.str) < 0 &&
                   CapturableVariables->IndexOf(e.str) >= 0) {
          Push(CMCElement("this", prElement, tpVar), &ls);
          Push(CMCElement("."), &ls);
          if (CapturedVariables->IndexOf(e.str) < 0) {
            CapturedVariables->Add(e.str);
          }
          e.type = tpString;
        }
        Push(e, &ls);
      }
    } else if (e.type == tpOpe) {
      if (e.str == "::") {
        CMCElement f = lex->Get();
        if (lex->GetNext().str != "(") {
          throw CMCException("関数呼び出しには () が必要です：" + f.str);
        }
        lex->Get(); // "("
        char H;
        GetValues(')', &H);
        Push(CMCElement((String)H + f.str, prElement, tpFunc), &ls);
      } else if (e.str == "&&" || e.str == "||" || e.str == "&" ||
                 e.str == "|") {
        Push(CMCElement("", e.pri, tpStructure), &ls);
        int jump = OutputPositionPlaceholder();
        Output(e);
        Push(CMCElement("", e.pri, tpStructure, jump), &ls);
      } else {
        Push(e, &ls);
      }
    } else{
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
bool TCompiler::Compile(String string, String filePath, String libName,
                        TStringList *modules, bool showMessage)
{
  InName = libName;
  Fail = false;
  lex = new TTokenizer(string);
  fout = new TMemoryStream();
  Modules = modules;
  Modules->AddObject(libName, fout);
  Variables = newTStringList();
  Constants = newTStringList();
  CapturableVariables = newTStringList();
  CapturedVariables = newTStringList();
  // Ok to use these pre-defined variables.
  Variables->Add("x");
  Variables->Add("y");
  Variables->Add("Left");
  Variables->Add("Top");

  try {
    GetBlock();
  } catch (CMCException e) {
    if (showMessage) {
      Application->MessageBox(
          (filePath + "\n" + lex->y + "行目\t" + lex->x + "文字目\n"
              + e.Message).c_str(),
          TEXT("Cassava Macro Compiler"), 0);
    }
    Fail = true;
  } catch (Exception *e) {
    if (showMessage) {
      Application->MessageBox(
          (filePath + "\n" + lex->y + "行目\t" + lex->x + "文字目\n"
              + e->Message).c_str(),
          TEXT("Cassava Macro Compiler"), 0);
    }
    Fail = true;
  }
  // Do not delete fout. It is stored in Modules.
  delete lex;
  delete Variables;
  delete Constants;
  delete CapturableVariables;
  delete CapturedVariables;
  return !Fail;
}
//---------------------------------------------------------------------------
bool MacroCompile(String *source, String name, TStringList *macroDirs,
                  TStringList *modules, bool showMessage)
{
  if (macroDirs == NULL || modules == NULL) { return false; }

  TCompiler compiler;
  compiler.import->Add(name);
  int processed = 0;

  if (source != NULL) {
    try {
      bool ok = compiler.Compile(*source, name, name, modules, showMessage);
      if (!ok) { return false; }
      processed = 1;
    } catch (Exception *e) {
      if(showMessage) {
        Application->MessageBox((name + "\n" + e->Message).c_str(),
                                TEXT("Cassava Macro Compiler"), 0);
      }
      return false;
    }
  }

  for (int i = processed; i < compiler.import->Count; i++) {
    String libName = compiler.import->Strings[i];
    String libFileName = "";
    TStreamReader *libReader = NULL;
    try{
      if (libName.Pos(":") > 0 || libName.SubString(1, 1) == "\\") {
        libFileName = libName;
      } else {
        for (int j = 0; j < macroDirs->Count; j++) {
          String pathName = macroDirs->Strings[j] + libName;
          if (FileExists(pathName)) {
            libFileName = pathName;
            break;
          }
        }
      }
      if (libFileName == "" || !FileExists(libFileName)) {
        if (showMessage) {
          Application->MessageBox(
              (libName + "\nファイルが見つかりません。").c_str(),
              TEXT("Cassava Macro Compiler"), 0);
        }
        return false;
      }
      TFileStream *file =
          new TFileStream(libFileName, fmOpenRead|fmShareDenyNone);
      int filelength = file->Size;
      DynamicArray<Byte> buf;
      buf.Length = filelength;
      filelength = file->Read(&(buf[0]), filelength);
      buf.Length = filelength;
      delete file;
      String string;
      try {
        string = TEncoding::UTF8->GetString(buf);
      } catch (...) {
        string = TEncoding::GetEncoding(932)->GetString(buf);
      }
      bool ok = compiler.Compile(
          string, libFileName, libName, modules, showMessage);
      if (!ok) { return false; }
    } catch (Exception *e) {
      if (libReader != NULL) { delete libReader; }
      if(showMessage) {
        Application->MessageBox((libFileName + "\n" + e->Message).c_str(),
                                TEXT("Cassava Macro Compiler"), 0);
      }
      return false;
    }
  }
  return true;
}
//---------------------------------------------------------------------------
bool MacroCompile(String fileName, TStringList *macroDirs, TStringList *modules,
                  bool showMessage)
{
  return MacroCompile(NULL, fileName, macroDirs, modules, showMessage);
}
//---------------------------------------------------------------------------
String GetMacroModuleName(String fileName, String funcName, String argCount,
                          bool varArg)
{
  return fileName + "\n" + funcName + (varArg ? "/+" : "/") + argCount;
}
//---------------------------------------------------------------------------
