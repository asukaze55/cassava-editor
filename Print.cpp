//---------------------------------------------------------------------------
#include <vcl.h>
#include <vcl\printers.hpp>
#pragma hdrstop

#include "Print.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TfmPrint *fmPrint;
//---------------------------------------------------------------------------
__fastcall TfmPrint::TfmPrint(TComponent* Owner)
        : TForm(Owner)
{
  DataWidth = fmMain->MainGrid->DataRight - fmMain->MainGrid->DataLeft + 1;
  DataHeight = fmMain->MainGrid->DataBottom - fmMain->MainGrid->DataTop + 1;
  Widths = new int[DataWidth+2];
  Printer()->Canvas->Font->Name = fmMain->PrintFontName;
  Printer()->Canvas->Font->Size = fmMain->PrintFontSize;
  csYohaku0->Value = fmMain->PrintMargin[0];
  csYohaku1->Value = fmMain->PrintMargin[1];
  csYohaku2->Value = fmMain->PrintMargin[2];
  csYohaku3->Value = fmMain->PrintMargin[3];
  lblFont->Caption = Printer()->Canvas->Font->Name + "    " +
                     Printer()->Canvas->Font->Size + " pt";
}
//---------------------------------------------------------------------------
void __fastcall TfmPrint::FormClose(TObject *Sender, TCloseAction &Action)
{
  delete Widths;
  fmMain->PrintFontName = Printer()->Canvas->Font->Name;
  fmMain->PrintFontSize = Printer()->Canvas->Font->Size;
  fmMain->PrintMargin[0] = csYohaku0->Value;
  fmMain->PrintMargin[1] = csYohaku1->Value;
  fmMain->PrintMargin[2] = csYohaku2->Value;
  fmMain->PrintMargin[3] = csYohaku3->Value;
}
//---------------------------------------------------------------------------
void TfmPrint::PrintOut()
{
  Printer()->Title = "Cassava";
  Printer()->Copies = 1;
  Printer()->BeginDoc();
  int Row = 1;
  bool NewPage = false;
  const float mmPt = Printer()->Canvas->Font->PixelsPerInch / 25.4;
  Yohaku[0] = csYohaku0->Value * mmPt;
  Yohaku[1] = csYohaku1->Value * mmPt;
  Yohaku[2] = csYohaku2->Value * mmPt;
  Yohaku[3] = csYohaku3->Value * mmPt;

  fmMain->MainGrid->CompactWidth(Widths,
    Printer()->PageWidth-Yohaku[0]-Yohaku[1], 16, Printer()->Canvas);


  while(Row <= fmMain->MainGrid->DataBottom)
  {
    if(NewPage) Printer()->NewPage();
    Print(Printer()->Canvas, Printer()->PageWidth,
                             Printer()->PageHeight-Yohaku[3], &Row);
    NewPage = true;
  }

  Printer()->EndDoc();

}
//---------------------------------------------------------------------------
void TfmPrint::Print(TCanvas *Canvas, int Width, int Height, int *Top)
{
  int strHeight = Canvas->TextHeight("A")+4;
  int Bottom = Height-strHeight;
  int WdSum = 0;
  for(int i=1; i<=DataWidth; i++){
    WdSum += Widths[i];
  }

  int y=Yohaku[0];
  while(*Top <= DataHeight && y < Bottom){
    Canvas->MoveTo(Yohaku[0],y);
    Canvas->LineTo(Yohaku[0]+WdSum,y);
    int x=Yohaku[0];
    for(int i=1; i<=DataWidth; i++){
        Canvas->MoveTo(x,y);
        Canvas->LineTo(x,y+strHeight);
        Canvas->TextOut(x+2, y+2, fmMain->MainGrid->ACells[i][*Top]);
        x += Widths[i];
    }
    Canvas->MoveTo(x,y);
    Canvas->LineTo(x,y+strHeight);
    y += strHeight;
    (*Top)++;
  }
  Canvas->MoveTo(Yohaku[0],y);
  Canvas->LineTo(Yohaku[0]+WdSum,y);
}
//---------------------------------------------------------------------------
void __fastcall TfmPrint::btnPrinterClick(TObject *Sender)
{
  dlgPrinter->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TfmPrint::btnFontClick(TObject *Sender)
{
  dlgFont->Font = Printer()->Canvas->Font;
  if(dlgFont->Execute())
  {
    Printer()->Canvas->Font = dlgFont->Font;
    lblFont->Caption = Printer()->Canvas->Font->Name + "    " +
                       Printer()->Canvas->Font->Size + " pt";
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmPrint::btnPrintClick(TObject *Sender)
{
  PrintOut();
}
//---------------------------------------------------------------------------

