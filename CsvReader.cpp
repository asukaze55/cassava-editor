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
  Name = L"Default";
  Exts.push_back(L"csv");
  Exts.push_back(L"txt");
}
//---------------------------------------------------------------------------
TTypeOption::TTypeOption(String str)
{
  init();
  Name = str;
  Exts.push_back(str.LowerCase());
  if (str == L"CSV") {
    SepChars = L",\t";
    WeakSepChars = L"";
  } else if (str == L"TSV") {
    SepChars = L"\t";
    WeakSepChars = L"";
    QuoteOption  = 0;
  }
}
//---------------------------------------------------------------------------
void TTypeOption::init()
{
  ForceExt = false;
  SepChars = L",\t";
  WeakSepChars = L" ";
  QuoteChars = L"\"";
  QuoteOption = 1;
  OmitComma = false;
  DummyEof = true;
  DummyEol = false;
}
//---------------------------------------------------------------------------
String TTypeOption::DefExt() const
{
  return Exts.size() > 0 ? Exts[0] : (String)"";
}
//---------------------------------------------------------------------------
wchar_t TTypeOption::DefSepChar() const
{
  return ((SepChars.Length() > 0 ) ? SepChars[1] : L'\0');
}
//---------------------------------------------------------------------------
bool TTypeOption::UseQuote() const
{
  return (QuoteOption > 0);
}
//---------------------------------------------------------------------------
void TTypeOption::SetExts(String ExtString)
{
  ExtString = ExtString.LowerCase();
  Exts.clear();
  int pos;
  while ((pos = ExtString.Pos(L";")) > 0) {
    String ext = ExtString.SubString(1, pos - 1);
    if (ext != "") {
      Exts.push_back(ext);
    }
    ExtString.Delete(1, pos);
  }
  if (ExtString != "") {
    Exts.push_back(ExtString);
  }
}
//---------------------------------------------------------------------------
String TTypeOption::GetExtsStr(int From) const
{
  String Temp = "";
  for (int i = From; i < Exts.size(); i++) {
    if (i > From) Temp += ";";
    Temp += Exts[i];
  }
  return Temp;
}
//---------------------------------------------------------------------------
CsvReader::CsvReader(const TTypeOption* TypeOption, String FileName,
                     TEncoding *Encoding)
  : typeOption(TypeOption),
    reader(new TStreamReader(FileName, Encoding, /* DetectBOM= */ true,
                             /* BufferSize= */ 4096)),
    data(""), last(1), pos(0), delimiterType(DELIMITER_TYPE_STRONG)
{
  IncrementPos(false);
}
//---------------------------------------------------------------------------
CsvReader::~CsvReader()
{
  delete reader;
}
//---------------------------------------------------------------------------
void __fastcall CsvReader::IncrementPos(bool Quoted)
{
  pos++;
  if (pos < last || reader->Peek() < 0) {
    return;
  }
  if (Quoted) {
    data += reader->ReadLine() + "\n";
    last = data.Length() + 1;
  } else {
    data = reader->ReadLine() + "\n";
    last = data.Length() + 1;
    pos = 1;
  }
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
    while (pos < last && typeOption->WeakSepChars.Pos(data[pos]) > 0) {
      IncrementPos(false);
    }
  }
  if (pos >= last) {
    return NEXT_TYPE_END_OF_FILE;
  } else if (data[pos] == L'\r' || data[pos] == L'\n') {
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
    if (data[pos] == L'\r') {
      IncrementPos(false);
      delimiterType = DELIMITER_TYPE_STRONG;
    }
    if (data[pos] == L'\n') {
      IncrementPos(false);
      delimiterType = DELIMITER_TYPE_STRONG;
    }
  }

  bool quoted = false;
  int cellstart = pos;
  int cellend = pos;

  for (;pos < last; IncrementPos(quoted), cellend++) {
    if (data[pos] == '\0') {
      data[pos] = ' ';
    }
    wchar_t ch = data[pos];
    if (!quoted) {
      if (ch == L'\r' || ch == L'\n') {
        // 改行
        delimiterType = DELIMITER_TYPE_WEAK_POST;
        return data.SubString(cellstart, cellend - cellstart);
      } else if (typeOption->SepChars.Pos(ch) > 0) {
        // 強区切り
        if (delimiterType != DELIMITER_TYPE_WEAK_PRE) {
          IncrementPos(false);
          delimiterType = DELIMITER_TYPE_STRONG;
          return data.SubString(cellstart, cellend - cellstart);
        }
        delimiterType = DELIMITER_TYPE_STRONG;
        cellstart = pos + 1;
      } else if (typeOption->WeakSepChars.Pos(ch) > 0) {
        // 弱区切り
        if (delimiterType == DELIMITER_TYPE_NONE) {
          IncrementPos(false);
          delimiterType = DELIMITER_TYPE_WEAK_PRE;
          return data.SubString(cellstart, cellend - cellstart);
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
        if (pos + 1 < last && data[pos + 1] == ch) {
          IncrementPos(true);
          data[cellend] = data[pos];
        } else {
          IncrementPos(true);
          delimiterType = DELIMITER_TYPE_WEAK_PRE;
          return data.SubString(cellstart, cellend - cellstart);
        }
      } else {
        // コンテンツ
        delimiterType = DELIMITER_TYPE_NONE;
        if(cellend < pos){
          data[cellend] = data[pos];
        }
      }
    }
  }

  delimiterType = DELIMITER_TYPE_NONE;
  if (cellstart < cellend) {
    return data.SubString(cellstart, cellend - cellstart);
  }
  return L"";
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
