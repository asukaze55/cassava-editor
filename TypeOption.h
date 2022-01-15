//---------------------------------------------------------------------------
#ifndef TypeOptionH
#define TypeOptionH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
class TTypeOption
{
public:
  AnsiString Name;
  TStringList *Exts;
  bool ForceExt;
  AnsiString SepChars;
  AnsiString WeakSepChars;
  AnsiString QuoteChars;
  int QuoteOption;
  bool OmitComma;
  bool DummyEof;

  AnsiString DefExt() {
    return ((Exts->Count > 0) ? Exts->Strings[0] : (AnsiString)"");
  }
  char DefSepChar() { return ((SepChars.Length() > 0 ) ? SepChars[1] : '\0'); }
  bool UseQuote()   { return (QuoteOption > 0); }

  void init(){
    Exts = new TStringList();
    Exts->Clear();
    ForceExt = false;
    SepChars = ",\t";
    WeakSepChars = " ";
    QuoteChars = "\"";
    QuoteOption = 1;
    OmitComma = true;
    DummyEof = false;
  }
  TTypeOption() {
    init();
    Name = "Default";
    Exts->Add("csv");
    Exts->Add("txt");
  }
  TTypeOption(AnsiString str) {
    init();
    Name = str;
    Exts->Add(str.LowerCase());
    if(str == "CSV"){
      SepChars = ",\t";  WeakSepChars = "";
    }else if(str == "TSV"){
      SepChars = "\t"; WeakSepChars = "";
      QuoteOption  = 0;
    }
  }
  TTypeOption(TTypeOption *p) :
    Name(p->Name), ForceExt(p->ForceExt), OmitComma(p->OmitComma),
    SepChars(p->SepChars), WeakSepChars(p->WeakSepChars),
    QuoteChars(p->QuoteChars), QuoteOption(p->QuoteOption),
    DummyEof(p->DummyEof)
  {
    Exts = new TStringList();
    Exts->Assign(p->Exts);
  }
  ~TTypeOption(){
    delete Exts;
  }
  void SetExts(AnsiString ExtString);
  AnsiString GetExtsStr(int From);

};
//---------------------------------------------------------------------------
class TTypeList {
  TList *L;
  int GetCount() { return L->Count; }
public:
  TTypeList() {
    L = new TList();
    L->Add(new TTypeOption());
    L->Add(new TTypeOption("CSV"));
    L->Add(new TTypeOption("TSV"));
  }
  void Clear(){
    for(int i=L->Count-1; i>=0; i--){
      if(L->Items[i]) delete L->Items[i];
    }
    L->Clear();
  }
  ~TTypeList(){ Clear(); delete L; }
  __property int Count = { read=GetCount };
  void Add(TTypeOption *op){
    L->Add(op);
  }
  void Delete(int i){
    if(L->Items[i]) delete L->Items[i];
    L->Delete(i);
  }
  TTypeOption *Items(int i){
    if(i < 0 || i >= L->Count) i = 0;
    return static_cast<TTypeOption *>(L->Items[i]);
  }
  int IndexOf(AnsiString Ext){
    TTypeOption *p;
    for(int i=1; i<L->Count; i++) {
      p = static_cast<TTypeOption *>(L->Items[i]);
      if(p->Exts->IndexOf(Ext) >= 0) { return i; }
    }
    return 0;
  }
  TTypeOption *DefItem(){ return static_cast<TTypeOption *>(L->Items[0]); }
  TTypeList *operator =(TTypeList &src) {
    Clear();
    for(int i=0; i<src.Count; i++){
      L->Add(new TTypeOption(src.Items(i)));
    }
    return this;
  }
};
//---------------------------------------------------------------------------
AnsiString Ascii2Ctrl(AnsiString str);
AnsiString Ctrl2Ascii(AnsiString str);
//---------------------------------------------------------------------------
#endif
