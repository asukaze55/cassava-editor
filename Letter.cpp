//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <vcl\printers.hpp>
#include <vcl\inifiles.hpp>
#pragma hdrstop

#include "Letter.h"
#include "MainForm.h"
#include "Preview.h"
#include "AutoOpen.h"
//---------------------------------------------------------------------------
#pragma link "CSPIN"
#pragma resource "*.dfm"
TfmLetter *fmLetter;
//---------------------------------------------------------------------------
bool IsNumericChar(char c){
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
  
  fmPreview = new TfmPreview(Application);
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
  if(FileExists(fmMain->FullPath + "Auto.dat")) {
    DataRead(fmMain->FullPath + "Auto.dat");
  }

  PreView();
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::FormClose(TObject *Sender, TCloseAction &Action)
{
  delete fmPreview;
  DataSave(fmMain->FullPath + "Auto.dat");
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::imPreviewClick(TObject *Sender)
{
  PreView();
  fmPreview->Show();
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
  Printer()->Title = "Cassava";
  Printer()->Copies = udCopies->Position;
  Printer()->BeginDoc();
  bool NewPage = false;
  for(int i = udTop->Position; i<=udBottom->Position; i++)
  {
    if(NewPage) Printer()->NewPage();
    // fmMain->MainGrid->TopRow = i;
    NewPage = Print(Printer()->Canvas,i);
  }
  Printer()->EndDoc();

  btnPrint->Enabled = true;
  btnClose->Enabled = true;
  // edTop->Value = edBottom->Value+1;
}
//---------------------------------------------------------------------------
void TfmLetter::PreView()
{
  Graphics::TBitmap *Bmp = new Graphics::TBitmap;
  const float mmPt = Bmp->Canvas->Font->PixelsPerInch / 25.4;
  fmPreview->imPreview->Width  = 100 * mmPt;
       fmPreview->ClientWidth  = 100 * mmPt;
		   Bmp->Width  = 100 * mmPt;
  fmPreview->imPreview->Height = 148 * mmPt;
       fmPreview->ClientHeight = 148 * mmPt;
		   Bmp->Height = 148 * mmPt;
  Print(Bmp->Canvas, udTop->Position);

  fmPreview->imPreview->Picture->Assign(Bmp);

  TRect Rect;
  Rect.Left = 0; Rect.Top = 0;
  Rect.Right = imPreview->Width; Rect.Bottom = imPreview->Height;
  imPreview->Canvas->StretchDraw(Rect, Bmp);
}
//---------------------------------------------------------------------------
int TfmLetter::TateBytes(AnsiString Str)
{
  int Count = 0;
  bool num = false;
  int L = Str.Length();
  for(int i=1; i<=L; i++){
    if(Str.IsLeadByte(i)){
      Count += 2;
      num = false;
      i++;
    }else if(Str[i] == ' ' || Str[i] == '_'){
      Count++;
      num = false;
    }else if(cbHorzNum->Checked && IsNumericChar(Str[i])){
      if(!num){
        Count += 2;
        num = true;
      }
    }else{
      Count += 2;
      num = false;
    }
  }
  return Count;
}
//---------------------------------------------------------------------------
int TfmLetter::Name2Indent(AnsiString Name1)
{
  int pos = 1;
  bool flag = false;
  for(int i=1; i<Name1.Length(); i++){
    if(Name1.IsLeadByte(i)){
      if(Name1.SubString(i,2)=="　"){
        flag = true;
      }else if(flag){
        pos = i; break;
      }
      i++;
    }else{
      if(Name1[i]==' ' || Name1[i]=='_'){
        flag = true;
      }else if(flag){
        pos = i; break;
      }
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

AnsiString Number[2];        //宛先or自分
Number[0] = fmMain->MainGrid->ACells[udToNumber->Position][Index];
Number[1] = edMyNumber->Text;
AnsiString Box[2][2][2];    //[住所名前][宛先自分][右左]
int Name2Pos[2];
Box[0][0][0] = Tate(fmMain->MainGrid->ACells[udToAddress1->Position][Index]);
Box[0][0][1] = Tate(fmMain->MainGrid->ACells[udToAddress2->Position][Index]);
Box[0][1][0] = Tate(edMyAddress1->Text);
Box[0][1][1] = Tate(edMyAddress2->Text);
AnsiString NameTmp = fmMain->MainGrid->ACells[udToName->Position][Index];
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
AnsiString Note = fmMain->MainGrid->ACells[udToNote->Position][Index];

const int PX = udHorz->Position;
const int PY = udVert->Position;

const float mmPt = Canvas->Font->PixelsPerInch / 25.4;

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
if(Number[Who].AnsiPos("-")) Number[Who].Delete(Number[Who].AnsiPos("-"),1);
for(int i=0; i<=6 && i<Number[Who].Length(); i++)
{
  Canvas->TextOut(((double)(NumberLeft[Who][i]) / 10 + PX) * mmPt,
		  (NumberTop[Who] + NengaDY + PY) * mmPt,
		   Number[Who][i+1]);
}
//住所、宛名
Canvas->Font = lbFont->Font;
for(int ibox=0; ibox<2; ibox++){
  int Lng = TateBytes(Box[ibox][Who][0]);
  int Lng2 = TateBytes(Box[ibox][Who][1]);
  if(ibox == 1) Lng2 += Name2Pos[Who] - 1;
  if(Lng2 > Lng) Lng = Lng2;

  double PCY;
  double YItv;
  int BoxTop, BoxBottom, BoxMiddle, Name2Top;
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

    if(Canvas->Font->Name[1] ==  '@'){
      if(i == 1 && ibox == 1){
        PCY = (BoxTop + NengaDY + PY) * mmPt
            + Canvas->TextWidth(Box[1][Who][0].SubString(1, Name2Pos[Who]-1));
      }else if(i == 0){
        PCY = (BoxTop + NengaDY + PY) * mmPt;
      }else{
        PCY = (BoxBottom + NengaDY + PY) * mmPt
            - Canvas->TextWidth(Box[ibox][Who][i]);
      }
      LOGFONT lf;
      Canvas->Brush->Style = bsClear;
      ZeroMemory(&lf, sizeof(LOGFONT));
      lf.lfHeight = YItv * 2 * mmPt;
      lf.lfEscapement = -90 * 10;
      lf.lfOrientation = -90 * 10;
      lf.lfCharSet = DEFAULT_CHARSET;
      strcpy(lf.lfFaceName, Canvas->Font->Name.c_str());
      Canvas->Font->Handle = CreateFontIndirect(&lf);
      int HWidth = Canvas->TextHeight(Box[ibox][Who][i]) / 2;
      Canvas->TextOut((BoxMiddle + PX)*mmPt + HWidth, PCY, Box[ibox][Who][i]);
      continue;
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
      AnsiString Msg;
      char c = Box[ibox][Who][i][j];
      if(j == Name2Pos[Who]) Name2Top = PCY; 
      if(Box[ibox][Who][i].IsLeadByte(j)){
        Msg = Box[ibox][Who][i].SubString(j,2);
        j++;
      }else if(cbHorzNum->Checked && IsNumericChar(c)){
        Msg = c;
        while(j<FE && (c = Box[ibox][Who][i][j+1], IsNumericChar(c))){
          j++;
          Msg += c;
        }
      }else{
        Msg = c;
      }

      int HWidth = Canvas->TextWidth(Msg) / 2;
      if(Msg == " " || Msg == "_"){
        PCY += YItv;
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
AnsiString TfmLetter::Tate(AnsiString Source)
{
  if(lbFont->Font->Name[1] == '@'){
    int p;
    while((p = Source.AnsiPos("_")) > 0){
      Source[p] = ' ';
    }
    return Source;
  }

  AnsiString St = "";
  bool longnumber = false;
  int L = Source.Length();
  for(int i=1; i<=L; i++){
    char c = Source[i];
    if(Source.IsLeadByte(i)){
      longnumber = false;
      AnsiString S = Source.SubString(i,2);
      i++;
      if(S == "ー" || S == "－") St += "｜";
      else if(S == "、")         St += "　";
      else                       St += S;
    }else if(cbHorzNum->Checked && IsNumericChar(c)){
      int r = i + udHorzNumMax->Position;
      if(!longnumber && r <= L){
        longnumber = true;
        for(int j=r; j>i; j--){
          if(!IsNumericChar(Source[j])){
            longnumber = false;
            break;
          }
        }
      }
      bool shortnumber = true;
      if(i+1 <= L && IsNumericChar(Source[i+1])) shortnumber = false;
      else if(i-1 >= 1 && IsNumericChar(Source[i-1])) shortnumber = false;

      if(longnumber || shortnumber){
        St += ((AnsiString)"０１２３４５６７８９").SubString((c-'0')*2+1,2);
      }else{
        St += c;
      }
    }else if(!cbHorzNum->Checked && IsNumericChar(c)){
      St += ((AnsiString)"０１２３４５６７８９").SubString((c-'0')*2+1,2);
    }else{
      longnumber = false;
      if(c == '-'){
        St += "｜";
      }else{
        St += c;
      }
    }
  }


  return(St);
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::btnPrintClick(TObject *Sender)
{
  PrintOut();
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::cbxFontChange(TObject *Sender)
{
  if(cbxFont->Text != "")
  {
    lbFont->Font->Name = cbxFont->Text;
    PreView();
  }
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::btnSaveClick(TObject *Sender)
{
  dlgSave->FileName = fmMain->FullPath + "Auto.dat";
  if(dlgSave->Execute())
  {
    DataSave(dlgSave->FileName);
  }
}
//---------------------------------------------------------------------------
void TfmLetter::DataSave(AnsiString FileName)
{
  TIniFile *Ini = new TIniFile(FileName);
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
  if(edPrefix->Text != "" && edPrefix->Text[1] == ' ')
  { AnsiString Str = edPrefix->Text; Str[1] = '_'; edPrefix->Text = Str; }
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
    AnsiString Section = (i==0 ? "印刷位置：宛先" : "印刷位置：差出人");
    Ini->WriteInteger(Section,"NumberTop",NumberTop[i]);
    for(int j=0; j<7; j++){
      Ini->WriteFloat(Section,
        (AnsiString)"NumberLeft" + (AnsiString)j, NumberLeft[i][j]);
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
  dlgOpen->FileName = fmMain->FullPath + "Auto.dat";
  if(dlgOpen->Execute())
  {
    DataRead(dlgOpen->FileName);
    PreView();
  }
}
//---------------------------------------------------------------------------
void TfmLetter::DataRead(AnsiString FileName)
{
  TIniFile *Ini = new TIniFile(FileName);
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
  lbFont->Font->Name = cbxFont->Text;
  edHorz->Text = Ini->ReadString("微調整","Horz",edHorz->Text);
  edVert->Text = Ini->ReadString("微調整","Vert",edVert->Text);

  for(int i=0; i<2; i++){
    AnsiString Section = (i==0 ? "印刷位置：宛先" : "印刷位置：差出人");
    NumberTop[i] = Ini->ReadInteger(Section,"NumberTop",NumberTop[i]);
    for(int j=0; j<7; j++){
      NumberLeft[i][j] = Ini->ReadFloat(Section,
        (AnsiString)"NumberLeft" + (AnsiString)j, NumberLeft[i][j]);
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
void __fastcall TfmLetter::btnPrinterClick(TObject *Sender)
{
  dlgPrinter->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::edTopChange(TObject *Sender)
{
  PreView();
}
//---------------------------------------------------------------------------
void __fastcall TfmLetter::btnHelpClick(TObject *Sender)
{
  AutoOpen(fmMain->FullPath + "Help\\Letter.html");
}
//---------------------------------------------------------------------------



