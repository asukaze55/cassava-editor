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
        void __fastcall edSCKeyKeyPress(TObject *Sender, char &Key);
        void __fastcall btnSaveClick(TObject *Sender);
        void __fastcall btnOpenClick(TObject *Sender);
        void __fastcall tvMenuChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange);
        void __fastcall edSelectedChange(TObject *Sender);
private:	// ユーザー宣言
public:		// ユーザー宣言
        class TMenuShortCut{
          private:
            TShortCut GetMShortCut() { return ShortCut(Key, Shift); }
            void SetMShortCut(TShortCut SC) { ShortCutToKey(SC, Key, Shift); }
            bool GetModified() {
              if(!MenuItem) return false;
              Word TheKey; TShiftState TheShift;
              ShortCutToKey(MenuItem->ShortCut, TheKey, TheShift);
              if(TheKey == '\0' && Key == '\0') return false;
              return (TheKey != Key || TheShift != Shift);
            }
          public:
            TMenuItem *MenuItem;
            Word Key;
            TShiftState Shift;
            String Caption;
            __property TShortCut MShortCut
              = {read=GetMShortCut, write=SetMShortCut};
            __property bool Modified = {read=GetModified};
            TMenuShortCut(){ MenuItem = nullptr;}
            TMenuShortCut(TMenuItem *mi){
              MenuItem = mi; MShortCut = mi->ShortCut;
              Caption = mi->Caption;
            }
        };

        __fastcall TfmKey(TComponent* Owner);
        void MakeTree();
        void AddMenu(TTreeNode* Node, TMenuItem* MenuItem);
        void SetMSC(TMenuShortCut *MSC);
        int KeyToIndex(Word Key);
        void MenuUpDate();
        bool SaveKey(String FileName);
        bool LoadKey(String FileName);
        TMenuShortCut *NowMSC;
};
//---------------------------------------------------------------------------
extern PACKAGE TfmKey *fmKey;
//---------------------------------------------------------------------------
#endif
