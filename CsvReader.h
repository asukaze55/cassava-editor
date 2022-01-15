//---------------------------------------------------------------------------
#ifndef CsvReaderH
#define CsvReaderH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
//---------------------------------------------------------------------------
class TTypeOption
{
public:
  String Name;
  std::vector<String> Exts;
  bool ForceExt;
  String SepChars;
  String WeakSepChars;
  String QuoteChars;
  int QuoteOption;
  bool OmitComma;
  bool DummyEof;

  TTypeOption();
  TTypeOption(String str);

  void init();
  String DefExt();
  wchar_t DefSepChar();
  bool UseQuote();
  void SetExts(String ExtString);
  String GetExtsStr(int From);
};
//---------------------------------------------------------------------------
enum CsvReaderState
{
  NEXT_TYPE_HAS_MORE_CELL,
  NEXT_TYPE_HAS_MORE_ROW,
  NEXT_TYPE_END_OF_FILE
};
//---------------------------------------------------------------------------
class CsvReader
{
private:
  TTypeOption *typeOption;
  String data;
  wchar_t *last;
  wchar_t *pos;
  int delimiterType;
public:
  CsvReader(TTypeOption*, String);
  CsvReaderState GetNextType();
  String Next();
  bool ReadLine(TStringList *);
};
//---------------------------------------------------------------------------
#endif
