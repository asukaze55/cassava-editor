//----------------------------------------------------------------------------
#ifndef MultiLineInputBoxH
#define MultiLineInputBoxH
//----------------------------------------------------------------------------
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Graphics.hpp>
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
bool InputBoxMultiLine(
    const String ACaption, const String APrompt, String& AValue);
//----------------------------------------------------------------------------
#endif
