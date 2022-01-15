//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USEFORM("Letter.cpp", fmLetter);
USEFORM("MainForm.cpp", fmMain);
USEFORM("Preview.cpp", fmPreview);
USEFORM("KeyCustomize.cpp", fmKey);
USEFORM("Option.cpp", fmOption);
USEFORM("Print.cpp", fmPrint);
USEFORM("Size.cpp", fmSize);
USEFORM("Find.cpp", fmFind);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
    Application->Initialize();
    Application->HelpFile = "";
    Application->Title = "Cassava";
    Application->CreateForm(__classid(TfmMain), &fmMain);
                 Application->CreateForm(__classid(TfmOption), &fmOption);
                 Application->CreateForm(__classid(TfmFind), &fmFind);
                 Application->Run();
  }
  catch (Exception &exception)
  {
    Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------
