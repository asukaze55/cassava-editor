//---------------------------------------------------------------------------
#ifndef OptionLaunchH
#define OptionLaunchH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <registry.hpp>
//---------------------------------------------------------------------------
class TfrOptionLaunch : public TFrame
{
__published:	// IDE 管理のコンポーネント
	TGroupBox *gbAppli;
	TSpeedButton *sbRefer0;
	TSpeedButton *sbRefer1;
	TSpeedButton *sbRefer2;
	TEdit *edLaunchName1;
	TEdit *edLaunchName0;
	TEdit *edLaunch0;
	TEdit *edLaunch1;
	TEdit *edLaunch2;
	TEdit *edLaunchName2;
	TBitBtn *btnKanrenR;
	TGroupBox *gbBrowser;
	TSpeedButton *sbRefer3;
	TLabel *Label16;
	TEdit *edBrowser;
	TRadioGroup *rgUseURL;
	TOpenDialog *dlgOpen;
	void __fastcall edLaunchChange(TObject *Sender);
	void __fastcall sbReferClick(TObject *Sender);
	void __fastcall btnKanrenRClick(TObject *Sender);
private:	// ユーザー宣言
  void SetKanrenLaunch(TRegistry *Reg, AnsiString CassavaTypePath);
public:		// ユーザー宣言
	__fastcall TfrOptionLaunch(TComponent* Owner);
	void RestoreFromMainForm();
	void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionLaunch *frOptionLaunch;
//---------------------------------------------------------------------------
#endif
