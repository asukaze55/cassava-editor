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
  void __fastcall tvCategoryChange(TObject *Sender, TTreeNode *Node);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall btnOKClick(TObject *Sender);
private:  // ユーザー宣言
public:   // ユーザー宣言
  __fastcall TfmOption(TComponent* Owner);
  TTreeNode *tnDataFormat;
  TTreeNode *tnFile;
  TTreeNode *tnBackUp;
  TTreeNode *tnLaunch;
  TTreeNode *tnBehavior;
  TTreeNode *tnView;
  TTreeNode *tnColor;
};
//---------------------------------------------------------------------------
extern PACKAGE TfmOption *fmOption;
//---------------------------------------------------------------------------
#endif
