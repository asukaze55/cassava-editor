//---------------------------------------------------------------------------
#ifndef FileOpenThreadH
#define FileOpenThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "MainGrid.h"
//---------------------------------------------------------------------------
TThread *ThreadFileOpen(TMainGrid *Grid, const String& FileName,
    TEncoding *Encoding, bool IsDetectedEncoding);
//---------------------------------------------------------------------------
#endif
