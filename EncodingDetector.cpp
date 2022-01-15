//---------------------------------------------------------------------------
#pragma hdrstop
#include "EncodingDetector.h"
//---------------------------------------------------------------------------
typedef void funcNext(byte b, int i, int &error, int &hit, int &flag);
//---------------------------------------------------------------------------
void SJIS_Next(byte b, int i, int &error, int &hit, int &flag)
{
  if(flag){
    if(b < 0x40 || b == 0x7f || 0xfc < b){ // not 2nd char
      error++;
    }
    flag = false;
  }else{
    if((0x81 <= b && b <= 0x9f) || (0xe0 <= b && b <= 0xef)){ // 1st char
      flag = true;
    }else if((b & 0x80) == 0){ // 0xxx xxxx
      flag = false;
    }else if(0xef < b){ // Gaiji
      error++;
      flag = true;
    }else{ // Hankaku Kana
      flag = false;
    }
  }
}
//---------------------------------------------------------------------------
void EUC_Next(byte b, int i, int &error, int &hit, int &flag)
{
  if(flag){
    if(b < 0xa1 || 0xfe < b){ error++; } // not 2nd char
    flag = false;
  }else{
    if((b & 0x80) == 0){ // 0xxx xxxx
      flag = false;
    }else if(0xa1 <= b && b <= 0xfe){ // 1st char
      flag = true;
    }else{
      error++;
      flag = true;
    }
  }
}
//---------------------------------------------------------------------------
void JIS_Next(byte b, int i, int &error, int &hit, int &flag)
{
  if(b == 0x1b){ hit++; } // ESC
  else if((b & 0x80) == 0x80){ error++; } // 1xxx xxxx
}
//---------------------------------------------------------------------------
void UTF8_Next(byte b, int i, int &error, int &hit, int &flag)
{
  if((b & 0x80) == 0){          // 0xxx xxxx
    if(flag <= 0){ flag=1; }
    else{ error++; }
  }else if((b & 0xc0) == 0x80){ // 10xx xxxx
    if(flag <= 0){ error++; }
  }else if((b & 0xe0) == 0xc0){ // 110x xxxx
    if(flag <= 0){ flag=2; }
    else{ error++; }
  }else if((b & 0xf0) == 0xe0){ // 1110 xxxx
    if(flag <= 0){ flag=3; }
    else{ error++; }
  }else if((b & 0xf8) == 0xf0){ // 1111 0xxx
    if(flag <= 0){ flag=4; }
    else{ error++; }
  }else if((b & 0xfc) == 0xf8){ // 1111 10xx
    if(flag <= 0){ flag=5; }
    else{ error++; }
  }else if((b & 0xfe) == 0xfc){ // 1111 110x
    if(flag <= 0){ flag=6; }
    else{ error++; }
  }else{
    error++;
  }
  flag--;
}
//---------------------------------------------------------------------------
void UTF16LE_Next(byte b, int i, int &error, int &hit, int &flag)
{
  if((i % 2 == 0) && (b == 0x0a || b == 0x0d)){ // LF, CR
    flag = true;
  }else{
    if(flag && b == 0){
      hit += 2;
    }
    flag = false;
  }
}
//---------------------------------------------------------------------------
void UTF16BE_Next(byte b, int i, int &error, int &hit, int &flag)
{
  if((i % 2 == 0) && (b == 0)){
    flag = true;
  }else{
    if(flag && (b == 0x0a || b == 0x0d)){ // LF, CR
      hit += 2;
    }
    flag = false;
  }
}
//---------------------------------------------------------------------------
int DetectEncode(byte *buf, int len, int def)
{
#define CHARCODE_COUNT 6
  funcNext *next[CHARCODE_COUNT];
  next[CHARCODE_SJIS] = SJIS_Next;
  next[CHARCODE_EUC] = EUC_Next;
  next[CHARCODE_JIS] = JIS_Next;
  next[CHARCODE_UTF8] = UTF8_Next;
  next[CHARCODE_UTF16LE] = UTF16LE_Next;
  next[CHARCODE_UTF16BE] = UTF16BE_Next;

  if(len >= 4){
    if(buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf){
      return CHARCODE_UTF8;
    }else if(buf[0] == 0xff && buf[1] == 0xfe){
      return CHARCODE_UTF16LE;
    }else if(buf[0] == 0xfe && buf[1] == 0xff){
      return CHARCODE_UTF16BE;
    }
  }

  bool live[CHARCODE_COUNT] = { true, true, true, true, true, true };
  int error[CHARCODE_COUNT] = { 0, 0, 0, 0, 0, 0 };
  int hit[CHARCODE_COUNT] = { 0, 0, 0, 0, 0, 0 };
  int flag[CHARCODE_COUNT] = { 0, 0, 0, 0, 0, 0 };

  int livecount = CHARCODE_COUNT;
  int last = len < 1000 ? len : 1000;
  for(int i=0; i<last; i++){
    char b = buf[i];
    for(int enc=0; enc<CHARCODE_COUNT; enc++){
      next[enc](b, i, error[enc], hit[enc], flag[enc]);
      if(live[enc] && error[enc] > 0){ live[enc] = false; livecount--; }
      else if(error[enc] == 0 && hit[enc] >= 2){ return enc; }
    }
    if(livecount == 1){
      for(int enc=0; enc<CHARCODE_COUNT; enc++){
        if(live[enc]){ return enc; }
      }
    }
  }

  int result = (def >= 0 && def < CHARCODE_UTF16LE) ? def : 0;
  int minerr = error[result];
  // UTF-16 は error をカウントしないため比較対象外
  for (int i = 0; i < CHARCODE_UTF16LE; i++) {
    if (error[i] < minerr) {
      result = i;
      minerr = error[i];
    }
  }
  return result;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
