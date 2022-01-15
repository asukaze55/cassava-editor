//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USEFORM("Letter.cpp", fmLetter);
USEFORM("MainForm.cpp", fmMain);
USEFORM("Preview.cpp", fmPreview);
USEFORM("KeyCustomize.cpp", fmKey);
USEFORM("Print.cpp", fmPrint);
USEFORM("Size.cpp", fmSize);
USEFORM("Find.cpp", fmFind);
USEFORM("PasteDlg.cpp", fmPasteDialog);
USEFORM("Option.cpp", fmOption);
USEFORM("OptionDataFormat.cpp", frOptionDataFormat); /* TFrame: File Type */
USEFORM("OptionBackUp.cpp", frOptionBackUp); /* TFrame: File Type */
USEFORM("OptionLaunch.cpp", frOptionLaunch); /* TFrame: File Type */
USEFORM("OptionBehavior.cpp", frOptionBehavior); /* TFrame: File Type */
USEFORM("OptionColor.cpp", frOptionColor); /* TFrame: File Type */
USEFORM("OptionFile.cpp", frOptionFile); /* TFrame: File Type */
USEFORM("MultiLineInputBox.cpp", fmMultiLineInputBox);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
    Application->Initialize();
    Application->HelpFile = "";
    Application->Title = "Cassava";
    Application->CreateForm(__classid(TfmMain), &fmMain);
     Application->CreateForm(__classid(TfmFind), &fmFind);
     Application->CreateForm(__classid(TfmPasteDialog), &fmPasteDialog);
     Application->CreateForm(__classid(TfrOptionDataFormat), &frOptionDataFormat);
     Application->CreateForm(__classid(TfrOptionBackUp), &frOptionBackUp);
     Application->CreateForm(__classid(TfrOptionLaunch), &frOptionLaunch);
     Application->CreateForm(__classid(TfrOptionBehavior), &frOptionBehavior);
     Application->CreateForm(__classid(TfrOptionColor), &frOptionColor);
     Application->CreateForm(__classid(TfrOptionFile), &frOptionFile);
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
