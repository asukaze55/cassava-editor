//---------------------------------------------------------------------------
#ifndef PreviewH
#define PreviewH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfmPreview : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TImage *imPreview;
	void __fastcall imPreviewClick(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
	__fastcall TfmPreview(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfmPreview *fmPreview;
//---------------------------------------------------------------------------
#endif
