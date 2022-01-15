//---------------------------------------------------------------------------
#ifndef MacroOpeCodeH
#define MacroOpeCodeH
//---------------------------------------------------------------------------
#define CMO_Goto     'g' /* if,while */
#define CMO_Minus    'm' /* "-"  */
#define CMO_Inc      'i' /* "++" */
#define CMO_Dec      'd' /* "--" */
#define CMO_Eq       'Q' /* "==" */
#define CMO_Add      'A' /* "+=" */
#define CMO_Subtract 'S' /* "-=" */
#define CMO_Multiply 'M' /* "*=" */
#define CMO_Divide   'D' /* "/=" */
#define CMO_NEq      'N' /* "!=" */
#define CMO_LEq      'L' /* "<=" */
#define CMO_GEq      'G' /* ">=" */
#define CMO_In       'n' /* "in" */
//---------------------------------------------------------------------------
char CMOCode(String str){
  if(str == "++") return CMO_Inc;
  else if(str == "--") return CMO_Dec;
  else if(str == "==") return CMO_Eq;
  else if(str == "+=") return CMO_Add;
  else if(str == "-=") return CMO_Subtract;
  else if(str == "*=") return CMO_Multiply;
  else if(str == "/=") return CMO_Divide;
  else if(str == "!=") return CMO_NEq;
  else if(str == "<=" || str == "=<") return CMO_LEq;
  else if(str == ">=" || str == "=>") return CMO_GEq;
  else if(str == "&&") return '&';
  else if(str == "||") return '|';
  else if(str == "in") return CMO_In;
  return '\0';
}
//---------------------------------------------------------------------------
#endif
