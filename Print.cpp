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
}
//---------------------------------------------------------------------------
void __fastcall TfmPrint::FormShow(TObject *Sender)
{
  DataWidth = fmMain->MainGrid->DataRight - fmMain->MainGrid->DataLeft + 1;
  DataHeight = fmMain->MainGrid->DataBottom - fmMain->MainGrid->DataTop + 1;
  Widths = new int[DataWidth+2];
  Printer()->Canvas->Font->Name = fmMain->PrintFontName;
  Printer()->Canvas->Font->Size = fmMain->PrintFontSize;
  csYohaku0->Text = AnsiString(fmMain->PrintMargin[0]);
  csYohaku1->Text = AnsiString(fmMain->PrintMargin[1]);
  csYohaku2->Text = AnsiString(fmMain->PrintMargin[2]);
  csYohaku3->Text = AnsiString(fmMain->PrintMargin[3]);
  lblFont->Caption = Printer()->Canvas->Font->Name + "    " +
					 Printer()->Canvas->Font->Size + " pt";
}
//---------------------------------------------------------------------------
void __fastcall TfmPrint::FormClose(TObject *Sender, TCloseAction &Action)
{
  delete Widths;
  fmMain->PrintFontName = Printer()->Canvas->Font->Name;
  fmMain->PrintFontSize = Printer()->Canvas->Font->Size;
  fmMain->PrintMargin[0] = udYohaku0->Position;
  fmMain->PrintMargin[1] = udYohaku1->Position;
  fmMain->PrintMargin[2] = udYohaku2->Position;
  fmMain->PrintMargin[3] = udYohaku3->Position;
}
//---------------------------------------------------------------------------
void TfmPrint::PrintOut()
{
  TPrinter *printer = Printer();
  printer->Title = "Cassava";
  printer->Copies = 1;
  printer->BeginDoc();
  int Row = 1;
  bool NewPage = false;
  const double mmPt = printer->Canvas->Font->PixelsPerInch / 25.4;
  Yohaku[0] = udYohaku0->Position * mmPt;
  Yohaku[1] = udYohaku1->Position * mmPt;
  Yohaku[2] = udYohaku2->Position * mmPt;
  Yohaku[3] = udYohaku3->Position * mmPt;

  fmMain->MainGrid->CompactWidth(Widths,
	printer->PageWidth-Yohaku[0]-Yohaku[1], 16, printer->Canvas);


  while(Row <= fmMain->MainGrid->DataBottom)
  {
	if(NewPage) printer->NewPage();
	Print(printer->Canvas, printer->PageWidth,
						   printer->PageHeight-Yohaku[3], &Row);
    NewPage = true;
  }

  printer->EndDoc();

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

  int y=Yohaku[2];
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

