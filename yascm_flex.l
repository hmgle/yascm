%{
#include "yascm_bison.tab.h"
%}

%option noyywrap
%option always-interactive

DIG	[0-9]+
EOL	\n|\r\n|\n\r|\r
WS	{EOL}|[[:blank:]]

CHAR_VAL (newline|space|[^{WS}])

%x EXPECT_CHAR
%x EXPECT_STRING

%%

"("	{return LP;}
")"	{return RP;}
"."	{return DOT;}
"#f"	{return FALSE_T;}
"#t"	{return TRUE_T;}
{WS}*	/* do nothing with white space */

[-]?{DIG} {
	struct object_s *obj = make_fixnum(atoi(yytext));
	yylval.var = obj;
	return FIXNUM_T;
}
[-]?{DIG}?"."{DIG}	{return FLOATNUM_T;}

#\\	{BEGIN(EXPECT_CHAR);}
<EXPECT_CHAR>{CHAR_VAL} {
	BEGIN(INITIAL); 
	return CHAR_T;
}

\"	{BEGIN(EXPECT_STRING); return DOUBLE_QUOTE;}
<EXPECT_STRING>(\\n|\\["]|\\\\|[^\\"])* {return STRING_T;}
<EXPECT_STRING>\" {BEGIN(INITIAL); return DOUBLE_QUOTE;}

%%
