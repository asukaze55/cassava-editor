//---------------------------------------------------------------------------

#ifndef SizeH
#define SizeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CSPIN.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfmSize : public TForm
{
__published:	// IDE 管理のコンポーネント
		TLabel *Label1;
		TLabel *Label2;
        TBitBtn *btnCancel;
        TBitBtn *btnOK;
	TEdit *seHeight;
	TUpDown *udHeight;
	TEdit *seWidth;
	TUpDown *udWidth;
private:	// ユーザー宣言
public:		// ユーザー宣言
        __fastcall TfmSize(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSize *fmSize;
//---------------------------------------------------------------------------
#endif
