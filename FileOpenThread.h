//---------------------------------------------------------------------------
#ifndef FileOpenThreadH
#define FileOpenThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "MainGrid.h"
//---------------------------------------------------------------------------
TThread *ThreadFileOpen(TMainGrid *Grid, String FileName, TEncoding *Encoding);
//---------------------------------------------------------------------------
#endif
