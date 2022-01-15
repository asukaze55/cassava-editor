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
	TEdit *edLaunchName0;
	TEdit *edLaunchName1;
	TEdit *edLaunchName2;
	TEdit *edLaunch0;
	TEdit *edLaunch1;
	TEdit *edLaunch2;
	TGroupBox *gbBrowser;
	TSpeedButton *sbRefer3;
	TLabel *Label16;
	TEdit *edBrowser;
	TRadioGroup *rgUseURL;
	TOpenDialog *dlgOpen;
	TCheckBox *cbClose0;
	TCheckBox *cbClose1;
	TCheckBox *cbClose2;
	TLabel *Label2;
	void __fastcall edLaunchChange(TObject *Sender);
	void __fastcall sbReferClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TfrOptionLaunch(TComponent* Owner);
	void RestoreFromMainForm();
	void StoreToMainForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionLaunch *frOptionLaunch;
//---------------------------------------------------------------------------
#endif
