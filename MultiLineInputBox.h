//----------------------------------------------------------------------------
#ifndef MultiLineInputBoxH
#define MultiLineInputBoxH
//----------------------------------------------------------------------------
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <System.Classes.hpp>
//----------------------------------------------------------------------------
class TfmMultiLineInputBox : public TForm
{
__published:
  TButton *OKBtn;
  TButton *CancelBtn;
  TMemo *Memo1;
  TLabel *Label1;
private:
public:
  virtual __fastcall TfmMultiLineInputBox(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TfmMultiLineInputBox *fmMultiLineInputBox;
//----------------------------------------------------------------------------
AnsiString InputBoxMultiLine(const AnsiString ACaption,
  const AnsiString APrompt, const AnsiString ACancel,
  const AnsiString ADefault);
//----------------------------------------------------------------------------
#endif
