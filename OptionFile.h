//---------------------------------------------------------------------------

#ifndef OptionFileH
#define OptionFileH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrOptionFile : public TFrame
{
__published:  // IDE 管理のコンポーネント
  TGroupBox *gbLock;
  TCheckBox *cbCheckTimeStamp;
  TGroupBox *gbKanren;
  TCheckBox *cbKanrenCSV;
  TCheckBox *cbKanrenCSVNew;
  TCheckBox *cbKanrenTSVNew;
  TCheckBox *cbKanrenTSV;
  TGroupBox *gbCharCode;
  TCheckBox *cbCheckKanji;
  TComboBox *cbLockFile;
  TLabel *Label1;
  TGroupBox *gbOpen;
  TCheckBox *cbNewWindow;
  TCheckBox *cbTitleFullPath;
private:  // ユーザー宣言
public:   // ユーザー宣言
  __fastcall TfrOptionFile(TComponent* Owner);
  void SetKanren(TCHAR *CassavaType, TCHAR *CassavaTypeName, TCHAR *Ext,
          String OldFileType, bool OldKanren, bool OldKanrenNew,
          bool NewKanren, bool NewKanrenNew);
  bool KanrenCSV;
  bool KanrenCSVNew;
  AnsiString CSVFileType;
  bool KanrenTSV;
  bool KanrenTSVNew;
  AnsiString TSVFileType;
  void RestoreFromMainForm();
  void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionFile *frOptionFile;
//---------------------------------------------------------------------------
#endif
