//---------------------------------------------------------------------------
#include <vcl.h>
#include <tchar.h>
#pragma hdrstop
USEFORM("OptionLaunch.cpp", frOptionLaunch); /* TFrame: File Type */
USEFORM("OptionBehavior.cpp", frOptionBehavior); /* TFrame: File Type */
USEFORM("OptionFile.cpp", frOptionFile); /* TFrame: File Type */
USEFORM("OptionBackUp.cpp", frOptionBackUp); /* TFrame: File Type */
USEFORM("Option.cpp", fmOption);
USEFORM("OptionDataFormat.cpp", frOptionDataFormat); /* TFrame: File Type */
USEFORM("OptionColor.cpp", frOptionColor); /* TFrame: File Type */
USEFORM("MultiLineInputBox.cpp", fmMultiLineInputBox);
USEFORM("PasteDlg.cpp", fmPasteDialog);
USEFORM("Find.cpp", fmFind);
USEFORM("OptionView.cpp", frOptionView); /* TFrame: File Type */
USEFORM("KeyCustomize.cpp", fmKey);
USEFORM("Letter.cpp", fmLetter);
USEFORM("Size.cpp", fmSize);
USEFORM("Print.cpp", fmPrint);
USEFORM("Preview.cpp", fmPreview);
USEFORM("Sort.cpp", fmSort);
USEFORM("MainForm.cpp", fmMain);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
  try
  {
    Application->Initialize();
    Application->HelpFile = "";
    Application->Title = "Cassava";
    Application->CreateForm(__classid(TfmMain), &fmMain);
         Application->CreateForm(__classid(TfmFind), &fmFind);
         Application->CreateForm(__classid(TfmSort), &fmSort);
         Application->CreateForm(__classid(TfmPasteDialog), &fmPasteDialog);
         Application->CreateForm(__classid(TfrOptionDataFormat), &frOptionDataFormat);
         Application->CreateForm(__classid(TfrOptionBackUp), &frOptionBackUp);
         Application->CreateForm(__classid(TfrOptionLaunch), &frOptionLaunch);
         Application->CreateForm(__classid(TfrOptionBehavior), &frOptionBehavior);
         Application->CreateForm(__classid(TfrOptionColor), &frOptionColor);
         Application->CreateForm(__classid(TfrOptionFile), &frOptionFile);
         Application->CreateForm(__classid(TfrOptionView), &frOptionView);
         Application->CreateForm(__classid(TfmOption), &fmOption);
         Application->Run();
  }
  catch (Exception &exception)
  {
    Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------
