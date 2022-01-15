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
__published:  // IDE �Ǘ��̃R���|�[�l���g
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
  TCheckBox *cbUseUtf8AsDefault;
private:  // ���[�U�[�錾
public:   // ���[�U�[�錾
  __fastcall TfrOptionFile(TComponent* Owner);
  void SetKanren(String CassavaType, String CassavaTypeName, String Ext,
                 String OldFileType, bool OldKanren, bool OldKanrenNew,
                 bool NewKanren, bool NewKanrenNew);
  bool KanrenCSV;
  bool KanrenCSVNew;
  String CSVFileType;
  bool KanrenTSV;
  bool KanrenTSVNew;
  String TSVFileType;
  void RestoreFromMainForm();
  void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionFile *frOptionFile;
//---------------------------------------------------------------------------
#endif
