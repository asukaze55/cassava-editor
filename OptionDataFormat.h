//---------------------------------------------------------------------------

#ifndef OptionDataFormatH
#define OptionDataFormatH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "MainForm.h"
//---------------------------------------------------------------------------
class TfrOptionDataFormat : public TFrame
{
__published:    // IDE 管理のコンポーネント
    TLabel *lblName;
    TEdit *edName;
    TButton *btnDeleteType;
    TGroupBox *gbExt;
    TLabel *Label9;
    TLabel *Label13;
    TEdit *edExts;
    TEdit *edDefExt;
    TCheckBox *cbForceExt;
    TGroupBox *gbKugiri;
    TLabel *Label10;
    TLabel *Label11;
    TLabel *Label12;
    TEdit *edDefSepChar;
    TEdit *edSepChars;
    TEdit *edWeakSepChars;
    TRadioGroup *rgSaveQuote;
    TCheckBox *cbCommaRect;
    TCheckBox *cbDummyEOF;
    void __fastcall edNameChange(TObject *Sender);
    void __fastcall btnDeleteTypeClick(TObject *Sender);
private:    // ユーザー宣言
public:     // ユーザー宣言
    __fastcall TfrOptionDataFormat(TComponent* Owner);
    void RestoreFromMainForm();
    void StoreToMainForm();
    void RestoreDataPage(int id);
    void StoreDataPage();
    void UpdateOptionTree();
    void Select(int index);
    void ApplyToCurrentFile(int index);
    void Delete(int index);
    TTypeList TypeList;
    int TypeIndex;       // 現在表示中
    int ActiveTypeIndex; // MainFormのアクティブ
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionDataFormat *frOptionDataFormat;
//---------------------------------------------------------------------------
#endif
