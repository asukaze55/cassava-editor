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
  TGroupBox *gbCharCode;
  TCheckBox *cbCheckKanji;
  TComboBox *cbLockFile;
  TLabel *Label1;
  TGroupBox *gbOpen;
  TCheckBox *cbNewWindow;
  TCheckBox *cbTitleFullPath;
  TCheckBox *cbUseUtf8AsDefault;
private:  // ユーザー宣言
public:   // ユーザー宣言
  __fastcall TfrOptionFile(TComponent* Owner);
  void RestoreFromMainForm();
  void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionFile *frOptionFile;
//---------------------------------------------------------------------------
#endif
