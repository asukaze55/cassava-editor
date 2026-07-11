//---------------------------------------------------------------------------

#ifndef OptionH
#define OptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Vcl.Menus.hpp>

#include "OptionBackUp.h"
#include "OptionBehavior.h"
#include "OptionColor.h"
#include "OptionDataFormat.h"
#include "OptionFile.h"
#include "OptionLaunch.h"
#include "OptionView.h"
//---------------------------------------------------------------------------
class TfmOption : public TForm
{
__published:  // IDE 管理のコンポーネント
  TTreeView *tvCategory;
  TPanel *pnlClient;
  TPanel *pnlBottom;
  TBitBtn *btnOK;
  TBitBtn *btnCancel;
  TPanel *pnlOption;
  TPanel *pnlHeader;
  TShape *shapeHeader;
  TLabel *lblHeader;
  TPopupMenu *popupMenu;
  TMenuItem *mnApplyToCurrentFile;
  TMenuItem *mnDelete;
  void __fastcall tvCategoryChange(TObject *Sender, TTreeNode *Node);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall btnOKClick(TObject *Sender);
  void __fastcall tvCategoryMouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y);
  void __fastcall mnApplyToCurrentFileClick(TObject *Sender);
  void __fastcall mnDeleteClick(TObject *Sender);
private:  // ユーザー宣言
public:   // ユーザー宣言
  __fastcall TfmOption(TComponent* Owner);
  TfrOptionDataFormat *frOptionDataFormat;
  TfrOptionFile *frOptionFile;
  TfrOptionBackUp *frOptionBackUp;
  TfrOptionLaunch *frOptionLaunch;
  TfrOptionBehavior *frOptionBehavior;
  TfrOptionView *frOptionView;
  TfrOptionColor *frOptionColor;
  TTreeNode *tnDataFormat;
  TTreeNode *tnFile;
  TTreeNode *tnBackUp;
  TTreeNode *tnLaunch;
  TTreeNode *tnBehavior;
  TTreeNode *tnView;
  TTreeNode *tnColor;
};
//---------------------------------------------------------------------------
#endif
