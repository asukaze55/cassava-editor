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
    TComboBox *cbType;
    TButton *btnRename;
    TButton *btnNewType;
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
    void __fastcall btnRenameClick(TObject *Sender);
    void __fastcall cbTypeChange(TObject *Sender);
    void __fastcall btnNewTypeClick(TObject *Sender);
    void __fastcall btnDeleteTypeClick(TObject *Sender);
    void __fastcall edDefSepCharChange(TObject *Sender);
    void __fastcall cbCommaRectClick(TObject *Sender);
    void __fastcall cbCommaRectKeyUpDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall cbCommaRectKeyPress(TObject *Sender, char &Key);
private:    // ���[�U�[�錾
public:     // ���[�U�[�錾
    __fastcall TfrOptionDataFormat(TComponent* Owner);
    void RestoreFromMainForm();
    void StoreToMainForm();
    void RestoreDataPage(int id);
    void StoreDataPage();
    void UpdateOptionTree();
    void UpdateActiveOptionTree();
    void Select(int index);
    TTypeList TypeList;
    int TypeIndex;       // ���ݕ\����
    int ActiveTypeIndex; // MainForm�̃A�N�e�B�u
    bool Renaming;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrOptionDataFormat *frOptionDataFormat;
//---------------------------------------------------------------------------
#endif
