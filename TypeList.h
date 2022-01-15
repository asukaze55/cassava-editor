//---------------------------------------------------------------------------
#ifndef TypeListH
#define TypeListH
//---------------------------------------------------------------------------
#include <vcl.h>
#include "CsvReader.h"
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
  int IndexOf(String Ext){
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
String Ascii2Ctrl(String str);
String Ctrl2Ascii(String str);
//---------------------------------------------------------------------------
#endif
