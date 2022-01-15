//---------------------------------------------------------------------------
#ifndef MacroOpeCodeH
#define MacroOpeCodeH
//---------------------------------------------------------------------------
#define CMO_Pointer  'P' /* "*"  */
#define CMO_Goto     'g' /* if,while */
#define CMO_Minus    'm' /* "-"  */
#define CMO_Inc      'i' /* "++" */
#define CMO_Dec      'd' /* "--" */
#define CMO_Eq       'Q' /* "==" */
#define CMO_PSubst   'p' /* "=&" */
#define CMO_NEq      'N' /* "!=" */
#define CMO_LEq      'L' /* "<=" */
#define CMO_GEq      'G' /* ">=" */
//---------------------------------------------------------------------------
char CMOCode(AnsiString str){
  if(str == "++") return 'i';
  else if(str == "--") return 'd';
  else if(str == "==") return 'Q';
  else if(str == "=&") return 'p';
  else if(str == "!=") return 'N';
  else if(str == "<=" || str == "=<") return 'L';
  else if(str == ">=" || str == "=>") return 'G';
  else if(str == "&&") return '&';
  else if(str == "||") return '|';
  return '\0';
}
//---------------------------------------------------------------------------
#endif

 