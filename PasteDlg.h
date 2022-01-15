//---------------------------------------------------------------------------
#ifndef PasteDlgH
#define PasteDlgH
//---------------------------------------------------------------------------
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TfmPasteDialog : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
    TRadioGroup *Way;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *lbMessage;
    TBitBtn *btnOK;
    TBitBtn *btnCancel;
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
    __fastcall TfmPasteDialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfmPasteDialog *fmPasteDialog;
//---------------------------------------------------------------------------
#endif

