//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "Preview.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TfmPreview *fmPreview;
//---------------------------------------------------------------------------
__fastcall TfmPreview::TfmPreview(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmPreview::imPreviewClick(TObject *Sender)
{
  Close();	
}
//---------------------------------------------------------------------------