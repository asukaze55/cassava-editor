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
  void RestoreFromMainForm();
  void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionFile *frOptionFile;
//---------------------------------------------------------------------------
#endif
