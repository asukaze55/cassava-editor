//---------------------------------------------------------------------------
#ifndef PasteDlgH
#define PasteDlgH
//---------------------------------------------------------------------------
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
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

