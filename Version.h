//---------------------------------------------------------------------------
#ifndef VersionH
#define VersionH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
namespace Version
{
  String Current();
  String CurrentDate();
  String CurrentText();
  bool CurrentIsBeta();
  int Compare(const String& ver1, const String& ver2);
  void UpdateCheck();
}
//---------------------------------------------------------------------------
#endif
