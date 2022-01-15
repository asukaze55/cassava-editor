//---------------------------------------------------------------------------
#ifndef FileOpenThreadH
#define FileOpenThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "CsvGrid.h"
//---------------------------------------------------------------------------
TThread *ThreadFileOpen(TCsvGrid *AGrid, AnsiString AFileName,
                    char *buffer, unsigned long length);
//---------------------------------------------------------------------------
#endif
