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
  const TTypeOption *FindForFileName(String FileName) const {
    String Ext = ExtractFileExt(FileName).LowerCase();
    if (Ext.Length() > 1 && Ext[1]=='.') { Ext.Delete(1,1); }
    for (size_t i = 1; i < v.size(); i++) {
      const std::vector<String> &exts = v[i].Exts;
      if (std::find(exts.begin(), exts.end(), Ext) != exts.end()) {
        return &v[i];
      }
    }
    return &v[0];
  }
  int IndexOf(const TTypeOption *Format) const {
    return IndexOf(Format->Name);
  }
  int IndexOf(String Name, int notFoundIndex = 0) const {
    for (size_t i = 0; i < v.size(); i++) {
      if (v[i].Name == Name) {
        return i;
      }
    }
    return notFoundIndex;
  }
  TTypeOption *DefItem() {
    return &v[0];
  }
};
//---------------------------------------------------------------------------
String Ascii2Ctrl(String str);
String Ctrl2Ascii(String str);
//---------------------------------------------------------------------------
#endif
