//---------------------------------------------------------------------------

#ifndef OptionBackUpH
#define OptionBackUpH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrOptionBackUp : public TFrame
{
__published:	// IDE 管理のコンポーネント
	TCheckBox *cbBackupOnSave;
	TLabel *Label1;
	TEdit *edBuFileNameS;
  TCheckBox *cbDelBuSExit;
	TCheckBox *cbBackupOnTime;
	TLabel *Label2;
	TEdit *seBuInterval;
	TUpDown *udBuInterval;
	TLabel *Label4;
	TEdit *edBuFileNameT;
	TLabel *Label3;
	TCheckBox *cbBackupOnOpen;
	TCheckBox *cbDelBuT;
	TLabel *Label5;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label6;
  TCheckBox *cbDelBuSSaved;
	void __fastcall cbBackupOnSaveClick(TObject *Sender);
	void __fastcall cbBackupOnTimeClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TfrOptionBackUp(TComponent* Owner);
	void RestoreFromMainForm();
	void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionBackUp *frOptionBackUp;
//---------------------------------------------------------------------------
#endif
