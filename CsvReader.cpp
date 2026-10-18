//---------------------------------------------------------------------------
#pragma hdrstop
#include "CsvReader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TTypeOption::TTypeOption()
{
  init();
  Name = L"Default";
  Exts.push_back(L"csv");
  Exts.push_back(L"txt");
}
//---------------------------------------------------------------------------
TTypeOption::TTypeOption(const String& str)
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
    QuoteOption = TQuoteOption::None;
  }
}
//---------------------------------------------------------------------------
void TTypeOption::init()
{
  ForceExt = false;
  SepChars = L",\t";
  WeakSepChars = L" ";
  QuoteChars = L"\"";
  QuoteOption = TQuoteOption::Normal;
  OmitComma = false;
  DummyEof = true;
  DummyEol = false;
}
//---------------------------------------------------------------------------
String TTypeOption::DefExt() const
{
  return Exts.size() > 0 ? Exts[0] : L"";
}
//---------------------------------------------------------------------------
wchar_t TTypeOption::DefSepChar() const
{
  return ((SepChars.Length() > 0 ) ? SepChars[1] : L'\0');
}
//---------------------------------------------------------------------------
bool TTypeOption::UseQuote() const
{
  return QuoteOption != TQuoteOption::None;
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
CsvReader::CsvReader(const TTypeOption* TypeOption, const String& FileName,
                     TEncoding *Encoding)
  : typeOption(TypeOption),
    reader(std::make_unique<TStreamReader>(
        FileName, Encoding, /* DetectBOM= */ true, /* BufferSize= */ 4096)),
    data(""), last(1), pos(0), delimiterType(TDelimiterType::Strong)
{
  IncrementPos(false);
}
//---------------------------------------------------------------------------
CsvReader::CsvReader(const TTypeOption* TypeOption, const String& CsvText)
  : typeOption(TypeOption),
    reader(std::make_unique<TStringReader>(CsvText)),
    data(""), last(1), pos(0), delimiterType(TDelimiterType::Strong)
{
  IncrementPos(false);
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
TCsvReaderState CsvReader::GetState()
{
  if (delimiterType == TDelimiterType::Strong) {
    // Always create a cell after a strong delimiter.
    return TCsvReaderState::HasMoreCell;
  }
  if (delimiterType == TDelimiterType::WeakPre ||
      delimiterType == TDelimiterType::WeakPost) {
    // Ignore consecutive weak delimiters.
    while (pos < last && typeOption->WeakSepChars.Pos(data[pos]) > 0) {
      IncrementPos(false);
    }
  }
  if (pos >= last) {
    return TCsvReaderState::EndOfFile;
  } else if (data[pos] == L'\r' || data[pos] == L'\n') {
    return TCsvReaderState::HasMoreRow;
  } else {
    return TCsvReaderState::HasMoreCell;
  }
}
//---------------------------------------------------------------------------
String CsvReader::Next()
{
  if (delimiterType == TDelimiterType::WeakPre ||
      delimiterType == TDelimiterType::WeakPost) {
    // 改行の次へ進める。改行の情報は GetState で取る。
    if (data[pos] == L'\r') {
      IncrementPos(false);
      delimiterType = TDelimiterType::Strong;
    }
    if (data[pos] == L'\n') {
      IncrementPos(false);
      delimiterType = TDelimiterType::Strong;
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
        delimiterType = TDelimiterType::WeakPost;
        return data.SubString(cellstart, cellend - cellstart);
      } else if (typeOption->SepChars.Pos(ch) > 0) {
        // 強区切り
        if (delimiterType != TDelimiterType::WeakPre) {
          IncrementPos(false);
          delimiterType = TDelimiterType::Strong;
          return data.SubString(cellstart, cellend - cellstart);
        }
        delimiterType = TDelimiterType::Strong;
        cellstart = pos + 1;
      } else if (typeOption->WeakSepChars.Pos(ch) > 0) {
        // 弱区切り
        if (delimiterType == TDelimiterType::None) {
          IncrementPos(false);
          delimiterType = TDelimiterType::WeakPre;
          return data.SubString(cellstart, cellend - cellstart);
        }
        if (delimiterType == TDelimiterType::Strong) {
          delimiterType = TDelimiterType::WeakPost;
        }
        cellstart = pos + 1;
      } else if (typeOption->UseQuote()
                 && typeOption->QuoteChars.Pos(ch) > 0
                 && delimiterType != TDelimiterType::None) {
        // クオート
        quoted = true;
        delimiterType = TDelimiterType::None;
        cellstart = pos + 1;
      } else {
        // コンテンツ
        delimiterType = TDelimiterType::None;
      }
    } else { // Quoted
      if (typeOption->UseQuote() && typeOption->QuoteChars.Pos(ch) > 0) {
        if (pos + 1 < last && data[pos + 1] == ch) {
          IncrementPos(true);
          data[cellend] = data[pos];
        } else {
          IncrementPos(true);
          delimiterType = TDelimiterType::WeakPre;
          return data.SubString(cellstart, cellend - cellstart);
        }
      } else {
        // コンテンツ
        delimiterType = TDelimiterType::None;
        if(cellend < pos){
          data[cellend] = data[pos];
        }
      }
    }
  }

  delimiterType = TDelimiterType::None;
  if (cellstart < cellend) {
    return data.SubString(cellstart, cellend - cellstart);
  }
  return L"";
}
//---------------------------------------------------------------------------
bool CsvReader::ReadLine(std::vector<String>& row)
{
  row.clear();
  if (GetState() == TCsvReaderState::EndOfFile) {
    return false;
  }
  do {
    row.push_back(Next());
  } while (GetState() == TCsvReaderState::HasMoreCell);
  return true;
}
//---------------------------------------------------------------------------
