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
  bool DummyEol;

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
  TStreamReader *reader;
  String data;
  int last;
  int pos;
  int delimiterType;
  void __fastcall IncrementPos(bool Quoted);
public:
  CsvReader(TTypeOption* TypeOption, String FileName, TEncoding *Encoding);
  ~CsvReader();
  CsvReaderState GetNextType();
  String Next();
  bool ReadLine(TStringList *);
};
//---------------------------------------------------------------------------
#endif
