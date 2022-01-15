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
__published:	// IDE 管理のコンポーネント
	TImage *imPreview;
	void __fastcall imPreviewClick(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TfmPreview(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TfmPreview *fmPreview;
//---------------------------------------------------------------------------
#endif
