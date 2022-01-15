//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("Paste.res");
USEFORM("PasteDlg.cpp", fmPasteDialog);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TfmPasteDialog), &fmPasteDialog);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
