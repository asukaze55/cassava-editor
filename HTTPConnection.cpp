//---------------------------------------------------------------------------
#pragma hdrstop
#include "HTTPConnection.h"
#include <windows.h>
#include <Wininet.h>
#include "Version.h"
//---------------------------------------------------------------------------
#pragma link "wininet.lib"
#pragma package(smart_init)
//---------------------------------------------------------------------------
String HttpsGet(String host, int port, String path)
{
  String result = "";
  String agent = (String)"CassavaEditor/" + Version::Current();

  HINTERNET hInet;
  HINTERNET hConn;
  HINTERNET hReq;
  char szBuf[1024];
  DWORD dwSize;
  DWORD dwIndex;
  BOOL bret;

  hInet = ::InternetOpen(agent.c_str(),
    INTERNET_OPEN_TYPE_PRECONFIG, TEXT(""), TEXT(""), 0);
  if(hInet == NULL) {
    return "";
  }

  hConn = ::InternetConnect(hInet, host.c_str(), port,
    NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
  if(hConn == NULL) {
    ::InternetCloseHandle(hInet);
    return "";
  }

  DWORD dwFlags = INTERNET_FLAG_RELOAD
                | INTERNET_FLAG_DONT_CACHE
                | INTERNET_FLAG_NO_AUTO_REDIRECT
                | INTERNET_FLAG_SECURE
                | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
                | INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
  hReq = ::HttpOpenRequest(hConn, TEXT("GET"), path.c_str(), NULL, NULL, NULL,
                           dwFlags, NULL);
  if(hReq == NULL) {
    ::InternetCloseHandle(hConn);
    ::InternetCloseHandle(hInet);
    return "";
  }

  if(::HttpSendRequest(hReq, NULL, 0, NULL, 0) != TRUE) {
    ::InternetCloseHandle(hReq);
    ::InternetCloseHandle(hConn);
    ::InternetCloseHandle(hInet);
    return "";
  }

  bret = TRUE;
  dwIndex = 0;
  while(bret) {
    dwSize = sizeof(szBuf) - 1;
    memset(szBuf, 0x00, sizeof(szBuf));
    bret = ::HttpQueryInfo(hReq,
      HTTP_QUERY_RAW_HEADERS_CRLF | HTTP_QUERY_FLAG_NUMBER,
      szBuf, &dwSize, &dwIndex);
    dwIndex++;
  }

  bret = TRUE;
  while(bret) {
    dwSize = 0;
    memset(szBuf, 0x00, sizeof(szBuf));
    bret = ::InternetReadFile(hReq, szBuf, sizeof(szBuf) - 1, &dwSize);
    if(dwSize == 0) {
      break;
    }else if(bret){
      result += szBuf;
    }
  }

  ::InternetCloseHandle(hReq);
  ::InternetCloseHandle(hConn);
  ::InternetCloseHandle(hInet);

  return result;
}
//---------------------------------------------------------------------------

