//---------------------------------------------------------------------------
#ifndef CsvReaderH
#define CsvReaderH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
//---------------------------------------------------------------------------
enum class TQuoteOption : int
{
  None = 0,
  Normal = 1,
  String = 2,
  All = 3,
  Expression = 4
};
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
  TQuoteOption QuoteOption;
  String QuoteExpression;
  bool OmitComma;
  bool DummyEof;
  bool DummyEol;

  TTypeOption();
  TTypeOption(const String& str);

  void init();
  String DefExt() const;
  wchar_t DefSepChar() const;
  bool UseQuote() const;
  void SetExts(String ExtString);
  String GetExtsStr(int From) const;
};
//---------------------------------------------------------------------------
enum class TCsvReaderState { HasMoreCell, HasMoreRow, EndOfFile };
//---------------------------------------------------------------------------
enum class TDelimiterType { None, WeakPre, WeakPost, Strong };
//---------------------------------------------------------------------------
class CsvReader
{
private:
  const TTypeOption *typeOption;
  std::unique_ptr<TTextReader> reader;
  String data;
  int last;
  int pos;
  TDelimiterType delimiterType;
  void __fastcall IncrementPos(bool Quoted);
public:
  CsvReader(const TTypeOption* TypeOption, const String& FileName,
      TEncoding *Encoding);
  CsvReader(const TTypeOption* TypeOption, const String& CsvText);
  TCsvReaderState GetState();
  String Next();
  bool ReadLine(std::vector<String>& row);
};
//---------------------------------------------------------------------------
#endif
