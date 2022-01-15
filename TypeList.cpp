//---------------------------------------------------------------------------
#pragma hdrstop
#include "TypeList.h"
//---------------------------------------------------------------------------
String Ascii2Ctrl(String str){
  AnsiString r;
  for(int i=1; i<=str.Length(); i++){
	if(str[i] == '\\' && i < str.Length()){
	  i++;
	  switch(str[i]){
		case 'n':  r += "\n";   break;
		case 't':  r += "\t";   break;
		case '_':  r += " ";    break;
		default:   r += str[i]; break;
	  }
	}else{
	  r += str[i];
	}
  }
  return r;
}
//---------------------------------------------------------------------------
String Ctrl2Ascii(String str){
  AnsiString r;
  for(int i=1; i<=str.Length(); i++){
    switch(str[i]){
      case '\n': r += "\\n"; break;
      case '\t': r += "\\t"; break;
      case ' ':  r += "\\_"; break;
      case '\\': r += "\\\\"; break;
      default:   r += str[i]; break;
    }
  }
  return r;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
