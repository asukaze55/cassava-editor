//---------------------------------------------------------------------------
#ifndef PreferenceH
#define PreferenceH
//---------------------------------------------------------------------------
#include <System.IniFiles.hpp>
//---------------------------------------------------------------------------
class Preference
{
public:
  AnsiString Path;
  AnsiString SharedPath;
  AnsiString UserPath;

  Preference(AnsiString cassavaPath);
  TIniFile *GetInifile();
};
//---------------------------------------------------------------------------
#endif
