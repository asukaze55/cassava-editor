//---------------------------------------------------------------------------
#pragma hdrstop
#include "CsvReader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#define DELIMITER_TYPE_NONE      0x00
#define DELIMITER_TYPE_WEAK_PRE  0x01
#define DELIMITER_TYPE_WEAK_POST 0x02
#define DELIMITER_TYPE_STRONG    0x03
//---------------------------------------------------------------------------
TTypeOption::TTypeOption()
{
  init();
  Name = TEXT("Default");
  Exts->Add(TEXT("csv"));
  Exts->Add(TEXT("txt"));
}
//---------------------------------------------------------------------------
TTypeOption::TTypeOption(String str)
{
  init();
  Name = str;
  Exts->Add(str.LowerCase());
  if(str == TEXT("CSV")){
    SepChars = TEXT(",\t");  WeakSepChars = TEXT("");
  }else if(str == "TSV"){
    SepChars = TEXT("\t"); WeakSepChars = TEXT("");
    QuoteOption  = 0;
  }
}
//---------------------------------------------------------------------------
TTypeOption::TTypeOption(TTypeOption *p) :
  Name(p->Name), ForceExt(p->ForceExt), OmitComma(p->OmitComma),
  SepChars(p->SepChars), WeakSepChars(p->WeakSepChars),
  QuoteChars(p->QuoteChars), QuoteOption(p->QuoteOption),
  DummyEof(p->DummyEof)
{
  Exts = new TStringList();
  Exts->Assign(p->Exts);
}
//---------------------------------------------------------------------------
TTypeOption::~TTypeOption()
{
  delete Exts;
}
//---------------------------------------------------------------------------
void TTypeOption::init()
{
  Exts = new TStringList();
  Exts->Clear();
  ForceExt = false;
  SepChars = TEXT(",\t");
  WeakSepChars = TEXT(" ");
  QuoteChars = TEXT("\"");
  QuoteOption = 1;
  OmitComma = false;
  DummyEof = true;
}
//---------------------------------------------------------------------------
String TTypeOption::DefExt()
{
  return ((Exts->Count > 0) ? Exts->Strings[0] : (String)TEXT(""));
}
//---------------------------------------------------------------------------
wchar_t TTypeOption::DefSepChar()
{
  return ((SepChars.Length() > 0 ) ? SepChars[1] : TEXT('\0'));
}
//---------------------------------------------------------------------------
bool TTypeOption::UseQuote()
{
  return (QuoteOption > 0);
}
//---------------------------------------------------------------------------
void TTypeOption::SetExts(String ExtString)
{
  Exts->Clear();
  int pos;
  while((pos = ExtString.Pos(TEXT(";"))) > 0){
    String ext = ExtString.SubString(1,pos-1);
    if(ext != TEXT("")) { Exts->Add(ext); }
    ExtString.Delete(1,pos);
  }
  if(ExtString != TEXT("")) Exts->Add(ExtString);
}
//---------------------------------------------------------------------------
String TTypeOption::GetExtsStr(int From)
{
  String Temp = TEXT("");
  for(int i=From; i<Exts->Count; i++){
    if(i > From) Temp += TEXT(";");
    Temp += Exts->Strings[i];
  }
  return Temp;
}
//---------------------------------------------------------------------------
CsvReader::CsvReader(TTypeOption *_typeOption, String _data)
  : typeOption(_typeOption), data(_data), pos(0),
    delimiterType(DELIMITER_TYPE_STRONG)
{
  pos = _data.c_str();
  last = _data.LastChar() + 1;
}
//---------------------------------------------------------------------------
CsvReaderState CsvReader::GetNextType()
{
  if (delimiterType == DELIMITER_TYPE_STRONG) {
    // Always create a cell after a strong delimiter.
    return NEXT_TYPE_HAS_MORE_CELL;
  }
  if (delimiterType == DELIMITER_TYPE_WEAK_PRE
      || delimiterType == DELIMITER_TYPE_WEAK_POST) {
    // Ignore consecutive weak delimiters.
    while (pos < last && typeOption->WeakSepChars.Pos(*pos) > 0) {
      pos++;
    }
  }
  if (pos >= last) {
    return NEXT_TYPE_END_OF_FILE;
  } else if (*pos == TEXT('\r') || *pos == TEXT('\n')) {
    return NEXT_TYPE_HAS_MORE_ROW;
  } else {
    return NEXT_TYPE_HAS_MORE_CELL;
  }
}
//---------------------------------------------------------------------------
String CsvReader::Next()
{
  if (delimiterType == DELIMITER_TYPE_WEAK_PRE
      || delimiterType == DELIMITER_TYPE_WEAK_POST) {
    // 改行の次へ進める。改行の情報は GetNextType で取る。
    if (*pos == TEXT('\r')) { pos++; delimiterType = DELIMITER_TYPE_STRONG; }
    if (*pos == TEXT('\n')) { pos++; delimiterType = DELIMITER_TYPE_STRONG; }
  }

  bool quoted = false;
  wchar_t *cellstart = pos;
  wchar_t *cellend = pos;

  for (;pos < last; pos++, cellend++) {
    wchar_t ch = *pos;
    if (!quoted) {
      if (ch == TEXT('\r') || ch == TEXT('\n')) {
        // 改行
        delimiterType = DELIMITER_TYPE_WEAK_POST;
        return String(cellstart, cellend - cellstart);
      } else if (typeOption->SepChars.Pos(ch) > 0) {
        // 強区切り
        if (delimiterType != DELIMITER_TYPE_WEAK_PRE) {
          pos++;
          delimiterType = DELIMITER_TYPE_STRONG;
          return String(cellstart, cellend - cellstart);
        }
        delimiterType = DELIMITER_TYPE_STRONG;
        cellstart = pos + 1;
      } else if (typeOption->WeakSepChars.Pos(ch) > 0) {
        // 弱区切り
        if (delimiterType == DELIMITER_TYPE_NONE) {
          pos++;
          delimiterType = DELIMITER_TYPE_WEAK_PRE;
          return String(cellstart, cellend - cellstart);
        }
        if (delimiterType == DELIMITER_TYPE_STRONG) {
          delimiterType = DELIMITER_TYPE_WEAK_POST;
        }
        cellstart = pos + 1;
      } else if (typeOption->UseQuote()
                 && typeOption->QuoteChars.Pos(ch) > 0
                 && delimiterType != DELIMITER_TYPE_NONE) {
        // クオート
        quoted = true;
        delimiterType = DELIMITER_TYPE_NONE;
        cellstart = pos + 1;
      } else {
        // コンテンツ
        delimiterType = DELIMITER_TYPE_NONE;
      }
    } else { // Quoted
      if (typeOption->UseQuote() && typeOption->QuoteChars.Pos(ch) > 0) {
        if (pos + 1 < last && *(pos + 1) == ch) {
          pos++;
          *cellend = *pos;
        } else {
          pos++;
          delimiterType = DELIMITER_TYPE_WEAK_PRE;
          return String(cellstart, cellend - cellstart);
        }
      } else {
        // コンテンツ
        delimiterType = DELIMITER_TYPE_NONE;
        if(cellend < pos){
          *cellend = *pos;
        }
      }
    }
  }

  delimiterType = DELIMITER_TYPE_NONE;
  if (cellstart < cellend) {
    return String(cellstart, cellend - cellstart);
  }
  return TEXT("");
}
//---------------------------------------------------------------------------
bool CsvReader::ReadLine(TStringList *List)
{
  List->Clear();
  if(GetNextType() == NEXT_TYPE_END_OF_FILE){
    return false;
  }
  do{
    List->Add(Next());
  }while(GetNextType() == NEXT_TYPE_HAS_MORE_CELL);
  return true;
}
//---------------------------------------------------------------------------
