//---------------------------------------------------------------------------
#pragma hdrstop
#include "Version.h"
#include "HTTPConnection.h"
#include "AutoOpen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
String Version::Current()
{
  return "2.8a4";
}
//---------------------------------------------------------------------------
String Version::CurrentDate()
{
  return "2026/03/21";
}
//---------------------------------------------------------------------------
bool Version::CurrentIsBeta()
{
  return true;
}
//---------------------------------------------------------------------------
String Version::CurrentText()
{
  String text = Current();
  int pre = text.Pos("pre");
  if(pre > 0){
    text = text.SubString(0, pre - 1) + " pre";
  }
  TReplaceFlags replaceAll = TReplaceFlags() << rfReplaceAll;
  text = StringReplace(text, L"a", L" α", replaceAll);
  text = StringReplace(text, L"b", L" β", replaceAll);
  text = StringReplace(text, L"rc", L" RC", replaceAll);
#if _WIN64
  text += " (64 bit)";
#else
  text += " (32 bit)";
#endif
  return text;
}
//---------------------------------------------------------------------------
int Version::Compare(String ver1, String ver2)
{
  TReplaceFlags replaceAll = TReplaceFlags() << rfReplaceAll;
  int num1 = StringReplace(ver1, "/", "", replaceAll).ToIntDef(0);
  int num2 = StringReplace(ver2, "/", "", replaceAll).ToIntDef(0);
  return num1 - num2;
}
//---------------------------------------------------------------------------
class UpdateCheckThread : public TThread
{
protected:
  void __fastcall Execute();
public:
  __fastcall UpdateCheckThread() : TThread(true) {}
private:
  String newVer;
  String newDate;
  String newUrl;
  void __fastcall MessageBox();
};
//---------------------------------------------------------------------------
void __fastcall UpdateCheckThread::Execute()
{
  TStringList *list = new TStringList();
  list->Text = HttpsGet("www.asukaze.net", 443,
                        "/soft/cassava/update.cgi?ver=" + Version::Current());
  if (list->Count < 6) {
    newVer = "";
    newDate = "";
    newUrl = "";
  } else if (!Version::CurrentIsBeta()) {
    newVer = list->Strings[0];
    newDate = list->Strings[1];
    newUrl = list->Strings[2];
  } else {
    newVer = list->Strings[3];
    newDate = list->Strings[4];
    newUrl = list->Strings[5];
  }
  delete list;
  Synchronize(&MessageBox);
}
//---------------------------------------------------------------------------
void __fastcall UpdateCheckThread::MessageBox()
{
  constexpr wchar_t title[] = L"更新の確認";
  if (newUrl == "") {
    Application->MessageBox(L"情報の取得に失敗しました。", title, 0);
    return;
  }

  if (Version::Compare(newDate, Version::CurrentDate()) > 0) {
    String message = L"バージョン " + newVer + L" が見つかりました。\n" +
        newUrl + L"を開きますか？";
    int mr = Application->MessageBox(message.c_str(), title, MB_OKCANCEL);
    if (mr == IDOK) {
      AutoOpen(newUrl, "");
    }
  } else {
    Application->MessageBox(L"更新は見つかりませんでした。", title, 0);
  }
}
//---------------------------------------------------------------------------
void Version::UpdateCheck()
{
  TThread *thread = new UpdateCheckThread();
  thread->FreeOnTerminate = true;
  thread->Start();
}
//---------------------------------------------------------------------------

