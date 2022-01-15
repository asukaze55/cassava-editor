//---------------------------------------------------------------------------
#ifndef TypeListH
#define TypeListH
//---------------------------------------------------------------------------
#include <algorithm>
#include <vcl.h>
#include <vector>
#include "CsvReader.h"
//---------------------------------------------------------------------------
class TTypeList {
  std::vector<TTypeOption> v;
  int GetCount() const {
    return v.size();
  }
public:
  TTypeList() {
    v.push_back(TTypeOption());
    v.push_back(TTypeOption("CSV"));
    v.push_back(TTypeOption("TSV"));
  }
  void Clear() {
    v.clear();
  }
  __property int Count = { read=GetCount };
  void Add(const TTypeOption &op) {
    v.push_back(op);
  }
  void Delete(int i) {
    v.erase(v.begin() + i);
  }
  TTypeOption *Items(size_t i) {
    if (i >= v.size()) {
      return &v[0];
    }
    return &v[i];
  }
  int IndexOf(String Ext) const {
    Ext = Ext.LowerCase();
    for (size_t i = 1; i < v.size(); i++) {
      const std::vector<String> &exts = v[i].Exts;
      if (std::find(exts.begin(), exts.end(), Ext) != exts.end()) {
        return i;
      }
    }
    return 0;
  }
  TTypeOption *DefItem() {
    return &v[0];
  }
  TTypeList *operator =(const TTypeList &src) {
    v = src.v;
    return this;
  }
};
//---------------------------------------------------------------------------
String Ascii2Ctrl(String str);
String Ctrl2Ascii(String str);
//---------------------------------------------------------------------------
#endif
