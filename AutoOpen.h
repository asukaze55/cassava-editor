//---------------------------------------------------------------------------
#ifndef AutoOpenH
#define AutoOpenH
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
bool isUrl(String FileName);
bool AutoOpen(String FileName, String BasePath);
int ShellOpen(const std::vector<String>& Params);
int SpawnProcess(const std::vector<String>& Params, bool LookupPath = false);
//---------------------------------------------------------------------------
#endif
