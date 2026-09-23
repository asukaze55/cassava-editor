//---------------------------------------------------------------------------
#ifndef KeyCustomizeH
#define KeyCustomizeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TMenuShortCutChange {
private:
  TShortCut GetShortCut() const {
    return ::ShortCut(Key, Shift);
  }
  void SetShortCut(TShortCut SC) {
    ShortCutToKey(SC, Key, Shift);
  }
  bool GetModified() const {
    if (!MenuItem) {
      return false;
    }
    Word currentKey;
    TShiftState currentShift;
    ShortCutToKey(MenuItem->ShortCut, currentKey, currentShift);
    if (currentKey == '\0' && Key == '\0') {
      return false;
    }
    return (currentKey != Key || currentShift != Shift);
  }
public:
  TMenuItem *MenuItem;
  Word Key;
  TShiftState Shift;
  String Caption;
  __property TShortCut ShortCut = {read=GetShortCut, write=SetShortCut};
  __property bool Modified = {read=GetModified};

  TMenuShortCutChange(TMenuItem *mi)
      : MenuItem(mi), ShortCut(mi->ShortCut), Caption(mi->Caption) {}
};
//---------------------------------------------------------------------------
class TfmKey : public TForm
{
__published:	// IDE 管理のコンポーネント
  TTreeView *tvMenu;
  TPanel *Panel1;
  TBitBtn *btnOK;
  TBitBtn *btnCancel;
  TCheckBox *cbCtrl;
  TCheckBox *cbShift;
  TRadioGroup *rgSCKey;
  TEdit *edSCKey;
  TCheckBox *cbAlt;
  TStaticText *StaticText1;
  TStaticText *stUseSC;
  TBitBtn *btnSave;
  TBitBtn *btnOpen;
  TOpenDialog *dlgOpen;
  TEdit *edSelected;
  TSaveDialog *dlgSave;
  TEdit *csFNumber;
  TUpDown *udFNumber;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall tvMenuChange(TObject *Sender, TTreeNode *Node);
  void __fastcall rgSCKeyClick(TObject *Sender);
  void __fastcall btnOKClick(TObject *Sender);
  void __fastcall edSCKeyKeyPress(TObject *Sender, wchar_t &Key);
  void __fastcall btnSaveClick(TObject *Sender);
  void __fastcall btnOpenClick(TObject *Sender);
  void __fastcall tvMenuChanging(TObject *Sender, TTreeNode *Node,
      bool &AllowChange);
  void __fastcall edSelectedChange(TObject *Sender);
private:	// ユーザー宣言
  std::map<TTreeNode*, TMenuShortCutChange> Changes;
  void AddMenu(TTreeNode* Node, TMenuItem* MenuItem);
  TMenuShortCutChange* UpdateChange();
  int KeyToIndex(Word Key);
public:		// ユーザー宣言
  __fastcall TfmKey(TComponent* Owner);
  void MakeTree();
  void UpdateMenu();
  bool SaveKey(String FileName);
  bool LoadKey(String FileName);
};
//---------------------------------------------------------------------------
#endif
