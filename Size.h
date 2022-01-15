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
//---------------------------------------------------------------------------
class TfmSize : public TForm
{
__published:	// IDE 管理のコンポーネント
        TCSpinEdit *seHeight;
        TLabel *Label1;
        TCSpinEdit *seWidth;
        TLabel *Label2;
        TBitBtn *btnCancel;
        TBitBtn *btnOK;
private:	// ユーザー宣言
public:		// ユーザー宣言
        __fastcall TfmSize(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmSize *fmSize;
//---------------------------------------------------------------------------
#endif
