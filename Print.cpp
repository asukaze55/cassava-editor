//---------------------------------------------------------------------------
#include <vcl.h>
#include <Vcl.printers.hpp>
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
  Printer()->Canvas->Font->Name = fmMain->PrintFontName;
  Printer()->Canvas->Font->Size = fmMain->PrintFontSize;
  csYohaku0->Text = fmMain->PrintMargin[0];
  csYohaku1->Text = fmMain->PrintMargin[1];
  csYohaku2->Text = fmMain->PrintMargin[2];
  csYohaku3->Text = fmMain->PrintMargin[3];

  edHeader->Text = fmMain->PrintHeader;
  cbHeaderPosition->ItemIndex = fmMain->PrintHeaderPosition;
  edFooter->Text = fmMain->PrintFooter;
  cbFooterPosition->ItemIndex = fmMain->PrintFooterPosition;
  lblFont->Caption = Printer()->Canvas->Font->Name + "    " +
                     Printer()->Canvas->Font->Size + " pt";
}
//---------------------------------------------------------------------------
void __fastcall TfmPrint::FormClose(TObject *Sender, TCloseAction &Action)
{
  fmMain->PrintFontName = Printer()->Canvas->Font->Name;
  fmMain->PrintFontSize = Printer()->Canvas->Font->Size;
  fmMain->PrintMargin[0] = udYohaku0->Position;
  fmMain->PrintMargin[1] = udYohaku1->Position;
  fmMain->PrintMargin[2] = udYohaku2->Position;
  fmMain->PrintMargin[3] = udYohaku3->Position;
  fmMain->PrintHeaderPosition = cbHeaderPosition->ItemIndex;
  fmMain->PrintHeader = edHeader->Text;
  fmMain->PrintFooterPosition = cbFooterPosition->ItemIndex;
  fmMain->PrintFooter = edFooter->Text;
}
//---------------------------------------------------------------------------
void TfmPrint::PrintOut()
{
  TPrinter *printer = Printer();
  String fileName = ExtractFileName(fmMain->FileName);
  printer->Title = fileName == "" ? (String)CASSAVA_TITLE : fileName;
  printer->Copies = 1;
  printer->BeginDoc();
  const double mmPt = printer->Canvas->Font->PixelsPerInch / 25.4;
  int leftMargin = udYohaku0->Position * mmPt;
  int rightMargin = udYohaku1->Position * mmPt;

  int *widths = new int[fmMain->MainGrid->ColCount];
  int pageWidth = printer->PageWidth;
  int pageHeight = printer->PageHeight;
  int minWidth =
      printer->Canvas->TextWidth("‚ ") + (2 * fmMain->MainGrid->LRMargin);
  fmMain->MainGrid->CompactWidth(
      widths, pageWidth - leftMargin - rightMargin, minWidth, printer->Canvas);

  int row = fmMain->MainGrid->DataTop;
  bool newPage = false;
  for (int page = 1; row <= fmMain->MainGrid->DataBottom; page++) {
    if (newPage) {
      Application->ProcessMessages();
      if (ModalResult == mrCancel) { break; }
      printer->NewPage();
    }
    row += PrintPage(printer->Canvas, pageWidth, pageHeight, row, widths, page);
    newPage = true;
  }
  printer->EndDoc();
  delete[] widths;
}
//---------------------------------------------------------------------------
static TRect rectToDraw(int position, const TRect& page, const TRect& size,
                        bool isFooter)
{
  TRect rect = page;
  if (position == 2) {
    rect.Left = (page.Left + page.Right - size.Width()) / 2;
  } else if (position == 3) {
    rect.Left = page.Right - size.Width();
  }
  if (isFooter) {
    rect.Top = page.Bottom - size.Height();
  }
  return rect;
}
//---------------------------------------------------------------------------
static String formatHeaderFooter(String format, String fileName, int page)
{
  String result = "";
  for (int i = 1; i <= format.Length(); i++) {
    if (format[i] == '%' && i < format.Length()) {
      switch (format[i + 1]) {
        case 'f': result += fileName; break;
        case 'F': result += ExtractFileName(fileName); break;
        case 'p': result += page; break;
        case '%': result += '%'; break;
        default: result += '%'; result += format[i + 1]; break;
      }
      i++;
    } else {
      result += format[i];
    }
  }
  return result;
}
//---------------------------------------------------------------------------
int TfmPrint::PrintPage(TCanvas *Canvas, int Width, int Height, int Top,
                        int Widths[], int page)
{
  TMainGrid *mg = fmMain->MainGrid;
  const double mmPt = Canvas->Font->PixelsPerInch / 25.4;
  int leftMargin = udYohaku0->Position * mmPt;
  int topMargin = udYohaku2->Position * mmPt;
  int right = Width - udYohaku1->Position * mmPt;
  int bottom = Height - udYohaku3->Position * mmPt;
  int cellLRMargin = mg->LRMargin;
  int cellTBMargin = mg->TBMargin;

  TRect pageRect(leftMargin, topMargin, right, bottom);
  if (edHeader->Text != "" && cbHeaderPosition->ItemIndex > 0) {
    String header = formatHeaderFooter(edHeader->Text, fmMain->FileName, page);
    TRect size = mg->DrawTextRect(Canvas, pageRect, header, true, true);
    mg->DrawTextRect(Canvas, rectToDraw(cbHeaderPosition->ItemIndex, pageRect,
                                        size, false),
                     header, true, false);
    topMargin += 2 * size.Height();
  }
  if (edFooter->Text != "" && cbFooterPosition->ItemIndex > 0) {
    String footer = formatHeaderFooter(edFooter->Text, fmMain->FileName, page);
    TRect size = mg->DrawTextRect(Canvas, pageRect, footer, true, true);
    mg->DrawTextRect(Canvas, rectToDraw(cbFooterPosition->ItemIndex, pageRect,
                                        size, true),
                     footer, true, false);
    bottom -= 2 * size.Height();
  }

  int widthSum = 0;
  for (int col = mg->DataLeft; col <= mg->DataRight; col++) {
    widthSum += Widths[col];
  }
  int y = topMargin;
  Canvas->MoveTo(leftMargin, y);
  Canvas->LineTo(leftMargin + widthSum, y);
  int row = Top;
  for (; row <= mg->DataBottom; row++) {
    int maxHeight = 0;
    for (int col = mg->DataLeft; col <= mg->DataRight; col++) {
      String str = mg->GetCellToDraw(col, row, NULL, NULL);
      TRect rect(0, 0, Widths[col] - 2 * cellLRMargin, Height);
      int cellHeight = mg->DrawTextRect(Canvas, rect, str, true, true).Height();
      if (cellHeight > maxHeight) {
        maxHeight = cellHeight;
        if (y + maxHeight >= bottom && row > Top) { return row - Top; }
      }
    }

    int x = leftMargin;
    y += cellTBMargin;
    for (int col = mg->DataLeft; col <= mg->DataRight; col++) {
      int width = Widths[col];
      bool isNum;
      String str = mg->GetCellToDraw(col, row, NULL, &isNum);
      TRect rect(x + cellLRMargin, y, x + width - cellLRMargin, y + maxHeight);
      if (isNum && mg->TextAlignment == cssv_taNumRight) {
        int left = rect.Right - Canvas->TextWidth(str) - cellLRMargin;
        if (rect.Left < left) rect.Left = left;
      }
      mg->DrawTextRect(Canvas, rect, str, true, false);
      Canvas->MoveTo(x, y);
      Canvas->LineTo(x, y + maxHeight);
      x += width;
    }
    Canvas->MoveTo(x, y);
    y += maxHeight + cellTBMargin;
    Canvas->LineTo(x, y);
    Canvas->LineTo(leftMargin, y);
  }
  return row - Top;
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
  if (dlgPrinter->Execute()) {
    PrintOut();
    ModalResult = mrOk;
  }
}
//---------------------------------------------------------------------------
