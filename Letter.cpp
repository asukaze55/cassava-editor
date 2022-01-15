//---------------------------------------------------------------------------
#include <vcl.h>
#include <Vcl.printers.hpp>
#include <tchar.h>
#pragma hdrstop

#include "Letter.h"
#include "MainForm.h"
#include "AutoOpen.h"
//---------------------------------------------------------------------------
#pragma link "CSPIN"
#pragma resource "*.dfm"
TfmLetter *fmLetter;
//---------------------------------------------------------------------------
bool IsNumericChar(TCHAR c){
  return (c >= '0' && c <= '9');
}
//---------------------------------------------------------------------------
double min(double a, double b){
  return (a < b) ? a : b;
}
//---------------------------------------------------------------------------
__fastcall TfmLetter::TfmLetter(TComponent* Owner)
    : TForm(Owner)
{
  cbxHowUseNote->ItemIndex = 0;

  try{
    cbxFont->Items->Assign(Printer()->Fonts);
  }catch(...){
    cbxFont->Items->Assign(Screen->Fonts);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::FormShow(TObject *Sender)
{
  int max = fmMain->MainGrid->ColCount-1;
  udToNumber->Max = max;
  udToAddress1->Max = max;
  udToAddress2->Max = max;
  udToName->Max = max;
  udToName2->Max = max;
  udToNote->Max = max;
  max = fmMain->MainGrid->RowCount-1;
  udTop->Max = max;
  edTop->Text = fmMain->MainGrid->Selection.Top;
  udBottom->Max = max;
  edBottom->Text = fmMain->MainGrid->Selection.Bottom;
  udMyDataInCsv->Max = max;

  DataSetDefault();
  if(FileExists(fmMain->Pref->Path + "Auto.dat")) {
    DataRead(fmMain->Pref->Path + "Auto.dat");
  }

  UpdatePreviewImage(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::FormClose(TObject *Sender, TCloseAction &Action)
{
  DataSave(fmMain->Pref->Path + "Auto.dat");
}
//---------------------------------------------------------------------------
void TfmLetter::PrintOut()
{
  if(udTop->Position > udBottom->Position) {
    udBottom->Position = udTop->Position;
    return;
  }

  btnPrint->Enabled = false;
  btnClose->Enabled = false;

  try {
    TPrinter *printer = Printer();
    printer->Title = CASSAVA_TITLE;
    printer->Copies = udCopies->Position;
    printer->BeginDoc();
    bool newPage = false;
    for(int i = udTop->Position; i<=udBottom->Position; i++) {
      if(newPage) Printer()->NewPage();
      newPage = Print(Printer()->Canvas,i);
    }
    printer->EndDoc();
  } catch (...) {
    btnPrint->Enabled = true;
    btnClose->Enabled = true;
    throw;
  }

  btnPrint->Enabled = true;
  btnClose->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::UpdatePreviewImage(TObject *Sender)
{
  if (cbxFont->Text == "") { return; }

  Graphics::TBitmap *Bmp = new Graphics::TBitmap;
  const float mmPt = Bmp->Canvas->Font->PixelsPerInch / 25.4;
  Bmp->Width = 100 * mmPt;
  Bmp->Height = 148 * mmPt;
  Print(Bmp->Canvas, udTop->Position);

  TRect Rect;
  Rect.Left = 0; Rect.Top = 0;
  Rect.Right = imPreview->Width; Rect.Bottom = imPreview->Height;
  imPreview->Canvas->StretchDraw(Rect, Bmp);
}
//---------------------------------------------------------------------------
int TfmLetter::TateBytes(String Str)
{
  int Count = 0;
  bool num = false;
  for (int i = 1; i <= Str.Length(); i++) {
    TCHAR c = Str[i];
    if (c == ' ' || c == '_') {
      Count++;
      num = false;
    } else if (cbHorzNum->Checked && IsNumericChar(c)) {
      if (!num) {
        Count += 2;
        num = true;
      }
    } else {
      Count += 2;
      num = false;
      if (Str.IsLeadSurrogate(i)) {
        i++;
      }
    }
  }
  return Count;
}
//---------------------------------------------------------------------------
int TfmLetter::Name2Indent(String Name1)
{
  int pos = 1;
  bool flag = false;
  for (int i = 1; i <= Name1.Length(); i++) {
    TCHAR c = Name1[i];
    if (c == L'　' || c == ' ' || c == '_') {
      flag = true;
    } else if (flag) {
      pos = i;
      break;
    }
  }
  return pos;
}
//---------------------------------------------------------------------------
bool TfmLetter::Print(TCanvas *Canvas, int Index)
{
if(cbMyDataInCsv->Checked)
{
  int MyDat = udMyDataInCsv->Position;
  edMyName->Text = fmMain->MainGrid->ACells[udToName->Position][MyDat];
  edMyName2->Text = fmMain->MainGrid->ACells[udToName2->Position][MyDat];
  edMyNumber->Text = fmMain->MainGrid->ACells[udToNumber->Position][MyDat];
  edMyAddress1->Text = fmMain->MainGrid->ACells[udToAddress1->Position][MyDat];
  edMyAddress2->Text = fmMain->MainGrid->ACells[udToAddress2->Position][MyDat];
}

String Number[2];        //宛先or自分
Number[0] = fmMain->MainGrid->ACells[udToNumber->Position][Index];
Number[1] = edMyNumber->Text;
String Box[2][2][2];    //[住所名前][宛先自分][右左]
int Name2Pos[2];
Box[0][0][0] = Tate(fmMain->MainGrid->ACells[udToAddress1->Position][Index]);
Box[0][0][1] = Tate(fmMain->MainGrid->ACells[udToAddress2->Position][Index]);
Box[0][1][0] = Tate(edMyAddress1->Text);
Box[0][1][1] = Tate(edMyAddress2->Text);
String NameTmp = fmMain->MainGrid->ACells[udToName->Position][Index];
Box[1][0][0] = Tate(NameTmp + edPrefix->Text);
Name2Pos[0] = Name2Indent(NameTmp);
NameTmp = fmMain->MainGrid->ACells[udToName2->Position][Index];
if(NameTmp == ""){
  Box[1][0][1] = "";
}else{
  Box[1][0][1] = Tate(fmMain->MainGrid->ACells[udToName2->Position][Index]
               + edPrefix->Text);
}
Box[1][1][0] = Tate(edMyName->Text);
Box[1][1][1] = Tate(edMyName2->Text);
Name2Pos[1] = Name2Indent(edMyName->Text);
String Note = fmMain->MainGrid->ACells[udToNote->Position][Index];

const int PX = udHorz->Position;
const int PY = udVert->Position;

const double mmPt = ((double) Canvas->Font->PixelsPerInch) / 25.4;

if(cbUseNote->Checked)
{
  switch(cbxHowUseNote->ItemIndex)
  {
  case 0:
    if(Note != edUseNote->Text) return(false);
    break;
  case 1:
    if(Note == edUseNote->Text) return(false);
    break;
  }
}

for(int Who=0; Who<=1; Who++)
{
if(Who == 1 && Box[1][Who][0] == "") break;
else if(Box[1][Who][0] == "") return(false);

int NengaDY = (Who==1 && cbNenga->Checked) ? -14 : 0;

//郵便番号
Canvas->Font->Name = "ＭＳ Ｐ明朝";
Canvas->Font->Height = NumberSize[Who] * mmPt;
if (Number[Who].Pos("-")) {
  Number[Who].Delete(Number[Who].Pos("-"), 1);
}
for(int i=0; i<=6 && i<Number[Who].Length(); i++)
{
  Canvas->TextOut(((double)(NumberLeft[Who][i]) / 10 + PX) * mmPt,
		  (NumberTop[Who] + NengaDY + PY) * mmPt,
		   Number[Who][i+1]);
}
//住所、宛名
Canvas->Font->Name = cbxFont->Text;
bool verticalFont = (cbxFont->Text[1] == '@');
for(int ibox=0; ibox<2; ibox++){
  int Lng = TateBytes(Box[ibox][Who][0]);
  int Lng2 = TateBytes(Box[ibox][Who][1]);
  if(ibox == 1) Lng2 += Name2Pos[Who] - 1;
  if(Lng2 > Lng) Lng = Lng2;

  double PCY;
  double YItv;
  double BoxTop, BoxBottom, BoxMiddle, Name2Top;
  switch(ibox){
    case 0:
      YItv = (double)AddressSize[Who] / 2;
      BoxTop = AddressTop[Who];
      BoxBottom = AddressBottom[Who];
      break;
    case 1:
      YItv = (double)NameSize[Who] / 2;
      BoxTop = NameTop[Who];
      BoxBottom = NameBottom[Who];
      break;
  }
  Name2Top = BoxTop;

  int FE;
  if(Lng > 1) {
    YItv = min(YItv, double(BoxBottom - BoxTop) / Lng);
  }
  Canvas->Font->Height = ((YItv * 2) - 0.25) * mmPt;

  LOGFONT VerticalLogFont;
  LOGFONT NonVerticalLogFont;
  if (verticalFont) {
    ZeroMemory(&VerticalLogFont, sizeof(LOGFONT));
    VerticalLogFont.lfHeight = YItv * 2 * mmPt;
    VerticalLogFont.lfEscapement = -90 * 10;
    VerticalLogFont.lfOrientation = -90 * 10;
    VerticalLogFont.lfCharSet = DEFAULT_CHARSET;
    _tcscpy(VerticalLogFont.lfFaceName, Canvas->Font->Name.c_str());

    ZeroMemory(&NonVerticalLogFont, sizeof(LOGFONT));
    NonVerticalLogFont.lfHeight = Canvas->Font->Height;
    NonVerticalLogFont.lfCharSet = DEFAULT_CHARSET;
    _tcscpy(NonVerticalLogFont.lfFaceName, Canvas->Font->Name.c_str());
  }

  for(int i=0; i<=1; i++)
  {
    FE = Box[ibox][Who][i].Length();
    switch(ibox){
      case 0:
        BoxMiddle = AddressMiddle[Who][i]; break;
      case 1:
        if(Box[ibox][Who][1] != ""){
          int v = (i==0) ? 1 : -1;
          BoxMiddle = NameMiddle[Who] + (YItv * v);
        }else{
          BoxMiddle = NameMiddle[Who];
        }
        break;
    }

    if(i == 1 && ibox == 1){
      PCY = Name2Top;
    }else if(i == 0){
      PCY = BoxTop + NengaDY + PY;
    }else{
      PCY = BoxBottom + NengaDY + PY - (YItv * Lng2);
    }

    for(int j=1; j <= FE; j++)
    {
      String Msg;
      TCHAR c = Box[ibox][Who][i][j];
      bool num = false;
      if(j == Name2Pos[Who]) Name2Top = PCY;
      if(Box[ibox][Who][i].IsLeadSurrogate(j)){
        Msg = Box[ibox][Who][i].SubString(j,2);
        j++;
      }else if(cbHorzNum->Checked && IsNumericChar(c)){
        Msg = c;
        while(j<FE && (c = Box[ibox][Who][i][j+1], IsNumericChar(c))){
          j++;
          Msg += c;
        }
        num = true;
      }else{
        Msg = c;
      }

      int HWidth = Canvas->TextWidth(Msg) / 2;
      if(Msg == " " || Msg == "_"){
        PCY += YItv;
      }else if(verticalFont){
        if(num || Msg == "｜"){
          Canvas->TextOut((BoxMiddle + PX)*mmPt - (Canvas->TextWidth(Msg) / 2),
                          PCY*mmPt, Msg);
          PCY += (Canvas->TextHeight(Msg) / mmPt);
        }else{
          Canvas->Font->Handle = CreateFontIndirect(&VerticalLogFont);
          int HWidth = Canvas->TextHeight(Box[ibox][Who][i]) / 2;
          Canvas->TextOut((BoxMiddle + PX)*mmPt + HWidth, PCY*mmPt, Msg);
          PCY += (Canvas->TextWidth(Msg) / mmPt);
          Canvas->Font->Handle = CreateFontIndirect(&NonVerticalLogFont);
        }
      }else{
        Canvas->TextOut((BoxMiddle + PX)*mmPt - HWidth, PCY*mmPt, Msg);
        PCY += YItv * 2;
      }
    }
  }
}

} //Whoループの終わり
return(true);
}
//---------------------------------------------------------------------------
String TfmLetter::Tate(String Str)
{
  bool verticalFont = (cbxFont->Text[1] == '@');
  for (int i = 1; i <= Str.Length(); i++) {
    TCHAR c = Str[i];
    if (c == '_') {
      Str[i] = ' ';
    } else if (!verticalFont && (c == '-' || c == L'ー' || c == L'－')) {
      Str[i] = L'｜';
    } else if (!verticalFont && c == L'、') {
      Str[i] = L'　';
    } else if (IsNumericChar(c)) {
      int length = 1;
      while (i + length <= Str.Length() && IsNumericChar(Str[i + length])) {
        length++;
      }
      if (!cbHorzNum->Checked || length == 1
          || length > udHorzNumMax->Position) {
        for (int j = 0; j < length; j++) {
          Str[i + j] = L"０１２３４５６７８９"[Str[i + j] - '0'];
        }
      }
      i += length - 1;
    }
  }
  return Str;
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::btnPrintClick(TObject *Sender)
{
  if (dlgPrinter->Execute()) {
    PrintOut();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::btnSaveClick(TObject *Sender)
{
  dlgSave->FileName = fmMain->Pref->Path + "Auto.dat";
  if(dlgSave->Execute())
  {
    DataSave(dlgSave->FileName);
  }
}
//---------------------------------------------------------------------------
void TfmLetter::DataSave(String FileName)
{
  IniFile *Ini = new IniFile(FileName);
  Ini->WriteString("差出人","Name",edMyName->Text);
  Ini->WriteString("差出人","Name2",edMyName2->Text);
  Ini->WriteString("差出人","Number",edMyNumber->Text);
  Ini->WriteString("差出人","Address1",edMyAddress1->Text);
  Ini->WriteString("差出人","Address2",edMyAddress2->Text);
  Ini->WriteBool("差出人","UseDataInCsv",cbMyDataInCsv->Checked);
  Ini->WriteString("差出人","DataInCsv",edMyDataInCsv->Text);
  Ini->WriteBool("差出人","Nenga",cbNenga->Checked);
  Ini->WriteString("宛先","Name",edToName->Text);
  Ini->WriteString("宛先","Name2",edToName2->Text);
  if (edPrefix->Text != "" && edPrefix->Text[1] == ' ') {
    String Str = edPrefix->Text;
    Str[1] = '_';
    edPrefix->Text = Str;
  }
  Ini->WriteString("宛先","Prefix",edPrefix->Text);
  Ini->WriteString("宛先","Number",edToNumber->Text);
  Ini->WriteString("宛先","Address1",edToAddress1->Text);
  Ini->WriteString("宛先","Address2",edToAddress2->Text);
  Ini->WriteString("宛先","Note",edToNote->Text);
  Ini->WriteBool("備考","Use",cbUseNote->Checked);
  Ini->WriteString("備考","KeyWord",edUseNote->Text);
  Ini->WriteInteger("備考","How",cbxHowUseNote->ItemIndex);
  Ini->WriteBool("フォント","HorzNum",cbHorzNum->Checked);
  Ini->WriteString("フォント","HorzNumMax",seHorzNumMax->Text);
  Ini->WriteInteger("フォント","Font",cbxFont->ItemIndex);
  Ini->WriteString("微調整","Horz",edHorz->Text);
  Ini->WriteString("微調整","Vert",edVert->Text);

  for(int i=0; i<2; i++){
    String Section = (i==0 ? "印刷位置：宛先" : "印刷位置：差出人");
    Ini->WriteInteger(Section,"NumberTop",NumberTop[i]);
    for(int j=0; j<7; j++){
      Ini->WriteFloat(Section, (String)"NumberLeft" + j, NumberLeft[i][j]);
    }
    Ini->WriteInteger(Section,"NumberSize", NumberSize[i]);
    Ini->WriteInteger(Section,"AddressTop", AddressTop[i]);
    Ini->WriteInteger(Section,"AddressBottom",AddressBottom[i]);
    Ini->WriteInteger(Section,"AddressX0",AddressMiddle[i][0]);
    Ini->WriteInteger(Section,"AddressX1",AddressMiddle[i][1]);
    Ini->WriteInteger(Section,"AddressSize",AddressSize[i]);
    Ini->WriteInteger(Section,"NameTop",NameTop[i]);
    Ini->WriteInteger(Section,"NameBottom",NameBottom[i]);
    Ini->WriteInteger(Section,"NameX",NameMiddle[i]);
    Ini->WriteInteger(Section,"NameSize",NameSize[i]);
  }

  delete Ini;
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::btnReadClick(TObject *Sender)
{
  dlgOpen->FileName = fmMain->Pref->Path + "Auto.dat";
  if(dlgOpen->Execute())
  {
    DataRead(dlgOpen->FileName);
    UpdatePreviewImage(Sender);
  }
}
//---------------------------------------------------------------------------
void TfmLetter::DataRead(String FileName)
{
  IniFile *Ini = new IniFile(FileName);
  edMyName->Text = Ini->ReadString("差出人","Name",edMyName->Text);
  edMyName2->Text = Ini->ReadString("差出人","Name2",edMyName->Text);
  edMyNumber->Text = Ini->ReadString("差出人","Number",edMyNumber->Text);
  edMyAddress1->Text = Ini->ReadString("差出人","Address1",edMyAddress1->Text);
  edMyAddress2->Text = Ini->ReadString("差出人","Address2",edMyAddress2->Text);
  cbMyDataInCsv->Checked = Ini->ReadBool("差出人","UseDataInCsv",cbMyDataInCsv->Checked);
  edMyDataInCsv->Text = Ini->ReadString("差出人","DataInCsv",edMyDataInCsv->Text);
  cbNenga->Checked = Ini->ReadBool("差出人","Nenga",cbNenga->Checked);
  edToName->Text = Ini->ReadString("宛先","Name",edToName->Text);
  edToName2->Text = Ini->ReadString("宛先","Name2",edToName2->Text);
  edPrefix->Text = Ini->ReadString("宛先","Prefix",edPrefix->Text);
  edToNumber->Text = Ini->ReadString("宛先","Number",edToNumber->Text);
  edToAddress1->Text = Ini->ReadString("宛先","Address1",edToAddress1->Text);
  edToAddress2->Text = Ini->ReadString("宛先","Address2",edToAddress2->Text);
  edToNote->Text = Ini->ReadString("宛先","Note",edToNote->Text);
  cbUseNote->Checked = Ini->ReadBool("備考","Use",cbUseNote->Checked);
  edUseNote->Text = Ini->ReadString("備考","KeyWord",edUseNote->Text);
  cbxHowUseNote->ItemIndex = Ini->ReadInteger("備考","How",cbxHowUseNote->ItemIndex);
  cbHorzNum->Checked = Ini->ReadBool("フォント","HorzNum",cbHorzNum->Checked);
  seHorzNumMax->Text = Ini->ReadString("フォント","HorzNumMax",seHorzNumMax->Text);
  cbxFont->ItemIndex = Ini->ReadInteger("フォント","Font",cbxFont->ItemIndex);
  edHorz->Text = Ini->ReadString("微調整","Horz",edHorz->Text);
  edVert->Text = Ini->ReadString("微調整","Vert",edVert->Text);

  for(int i=0; i<2; i++){
    String Section = (i==0 ? "印刷位置：宛先" : "印刷位置：差出人");
    NumberTop[i] = Ini->ReadInteger(Section,"NumberTop",NumberTop[i]);
    for(int j=0; j<7; j++){
      NumberLeft[i][j] =
          Ini->ReadFloat(Section, (String)"NumberLeft" + j, NumberLeft[i][j]);
    }
    NumberSize[i] = Ini->ReadInteger(Section,"NumberSize", NumberSize[i]);
    AddressTop[i] = Ini->ReadInteger(Section,"AddressTop", AddressTop[i]);
    AddressBottom[i]
      = Ini->ReadInteger(Section,"AddressBottom",AddressBottom[i]);
    AddressMiddle[i][0]
      = Ini->ReadInteger(Section,"AddressX0",AddressMiddle[i][0]);
    AddressMiddle[i][1]
      = Ini->ReadInteger(Section,"AddressX1",AddressMiddle[i][1]);
    AddressSize[i] = Ini->ReadInteger(Section,"AddressSize",AddressSize[i]);
    NameTop[i] = Ini->ReadInteger(Section,"NameTop",NameTop[i]);
    NameBottom[i] = Ini->ReadInteger(Section,"NameBottom",NameBottom[i]);
    NameMiddle[i] = Ini->ReadInteger(Section,"NameX",NameMiddle[i]);
    NameSize[i] = Ini->ReadInteger(Section,"NameSize",NameSize[i]);
  }

  delete Ini;
}
//---------------------------------------------------------------------------
void TfmLetter::DataSetDefault()
{
  NumberTop[0] =  12;       NumberTop[1] = 137;
  NumberLeft[0][0] = 443;
  NumberLeft[0][1] = 513;
  NumberLeft[0][2] = 583;
  NumberLeft[0][3] = 659;
  NumberLeft[0][4] = 727;
  NumberLeft[0][5] = 795;
  NumberLeft[0][6] = 863;
  NumberLeft[1][0] =  50;
  NumberLeft[1][1] =  90;
  NumberLeft[1][2] = 130;
  NumberLeft[1][3] = 180;
  NumberLeft[1][4] = 220;
  NumberLeft[1][5] = 260;
  NumberLeft[1][6] = 300;
  NumberSize[0] = 8;        NumberSize[1] = 6;
  AddressTop[0] = 25;       AddressTop[1] = 74;
  AddressBottom[0] = 120;   AddressBottom[1] = 129;
  AddressMiddle[0][0] = 87;
  AddressMiddle[0][1] = 75;
  AddressMiddle[1][0] = 26;
  AddressMiddle[1][1] = 19;
  AddressSize[0] = 7;       AddressSize[1] = 6;
  NameTop[0] = 40;          NameTop[1] = 90;
  NameBottom[0] = 120;      NameBottom[1] = 129;
  NameMiddle[0] = 55;       NameMiddle[1] = 11;
  NameSize[0] = 10;         NameSize[1] = 6;
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::btnHelpClick(TObject *Sender)
{
  AutoOpen("Help\\Letter.html", fmMain->FullPath);
}
//---------------------------------------------------------------------------



