//---------------------------------------------------------------------------
#ifndef VersionH
#define VersionH
//---------------------------------------------------------------------------
#include <vcl\vcl.h>
//---------------------------------------------------------------------------
namespace Version
{
  String Current();
  String CurrentDate();
  String CurrentText();
  bool CurrentIsBeta();
  int Compare(String ver1, String ver2);
  void UpdateCheck();
}
//---------------------------------------------------------------------------
#endif
