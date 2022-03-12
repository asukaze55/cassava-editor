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
__published:    // IDE �Ǘ��̃R���|�[�l���g
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
private:    // ���[�U�[�錾
public:     // ���[�U�[�錾
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
    int TypeIndex;       // ���ݕ\����
    int ActiveTypeIndex; // MainForm�̃A�N�e�B�u
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionDataFormat *frOptionDataFormat;
//---------------------------------------------------------------------------
#endif
