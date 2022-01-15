//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SaveFormat.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmSaveFormat *fmSaveFormat;
//---------------------------------------------------------------------------
__fastcall TfmSaveFormat::TfmSaveFormat(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
