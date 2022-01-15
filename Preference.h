//---------------------------------------------------------------------------
#ifndef PreferenceH
#define PreferenceH
//---------------------------------------------------------------------------
#include <vcl\inifiles.hpp>
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
