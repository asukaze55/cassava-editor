//---------------------------------------------------------------------------
#ifndef AutoOpenH
#define AutoOpenH
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
bool isUrl(const String& FileName);
bool AutoOpen(const String& FileName, const String& BasePath);
int ShellOpen(const std::vector<String>& Params);
int SpawnProcess(const std::vector<String>& Params, bool LookupPath = false);
//---------------------------------------------------------------------------
#endif
