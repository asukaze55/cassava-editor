//---------------------------------------------------------------------------
#ifndef ReloadH
#define ReloadH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Buttons.hpp>
//---------------------------------------------------------------------------
class TfmReload : public TForm
{
__published:	// IDE 管理のコンポーネント
	TCheckBox *cbDblQuote;
	TCheckBox *cbCamma;
	TCheckBox *cbTab;
	TCheckBox *cbSpace;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TfmReload(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfmReload *fmReload;
//---------------------------------------------------------------------------
#endif
