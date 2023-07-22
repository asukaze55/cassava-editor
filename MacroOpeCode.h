//---------------------------------------------------------------------------
#ifndef MacroOpeCodeH
#define MacroOpeCodeH
//---------------------------------------------------------------------------
#define CMO_IfThen   '?' /* Used in if, while */
#define CMO_Goto     'g' /* Clears stack. Used in if, while */
#define CMO_Jump     'j' /* Keeps stack. Used in ?: */
#define CMO_Return   '}' /* return */
#define CMO_Cell     ']' /* [x,y] */
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
#define CMO_ObjKey   'k' /* ":" */
#define CMO_DefParam 'p' /* Jump if the stack size >= specified size */
#define CMO_VarArg   'v' /* Converts stack to an ArrayLike object */
//---------------------------------------------------------------------------
static char CMOCode(String str){
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
