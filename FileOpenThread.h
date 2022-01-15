//---------------------------------------------------------------------------
#ifndef FileOpenThreadH
#define FileOpenThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "MainGrid.h"
//---------------------------------------------------------------------------
TThread *ThreadFileOpen(TMainGrid *AGrid, String AFileName, String AData);
//---------------------------------------------------------------------------
#endif
