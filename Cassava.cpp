//---------------------------------------------------------------------------
#include <vcl.h>
#include <tchar.h>
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
#pragma hdrstop
USEFORM("OptionDataFormat.cpp", frOptionDataFormat); /* TFrame: File Type */
USEFORM("OptionColor.cpp", frOptionColor); /* TFrame: File Type */
USEFORM("OptionLaunch.cpp", frOptionLaunch); /* TFrame: File Type */
USEFORM("OptionFile.cpp", frOptionFile); /* TFrame: File Type */
USEFORM("Option.cpp", fmOption);
USEFORM("MultiLineInputBox.cpp", fmMultiLineInputBox);
USEFORM("OptionBehavior.cpp", frOptionBehavior); /* TFrame: File Type */
USEFORM("OptionBackUp.cpp", frOptionBackUp); /* TFrame: File Type */
USEFORM("Sort.cpp", fmSort);
USEFORM("Print.cpp", fmPrint);
USEFORM("PasteDlg.cpp", fmPasteDialog);
USEFORM("OptionView.cpp", frOptionView); /* TFrame: File Type */
USEFORM("Letter.cpp", fmLetter);
USEFORM("MainForm.cpp", fmMain);
USEFORM("Find.cpp", fmFind);
USEFORM("KeyCustomize.cpp", fmKey);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
  try
  {
    Application->Initialize();
    Application->HelpFile = "";
    Application->Title = "Cassava Editor";
    Application->CreateForm(__classid(TfmMain), &fmMain);
    Application->CreateForm(__classid(TfmFind), &fmFind);
    Application->CreateForm(__classid(TfmSort), &fmSort);
    Application->CreateForm(__classid(TfmPasteDialog), &fmPasteDialog);
    FSetExceptMask(femALLEXCEPT);
    Application->Run();
  }
  catch (Exception &exception)
  {
    Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------
