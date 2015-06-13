%{
#include "yascm_bison.tab.h"
%}

%option noyywrap
%option always-interactive

DIG	[0-9]+
EOL	\n|\r\n|\n\r|\r
WS	{EOL}|[[:blank:]]
IDENT	[!$%&*+\-./:<=>?@^_~[:alnum:]]

CHAR_VAL (newline|space|[^{WS}]|x[0-9A-F]+)

%x EXPECT_CHAR
%x EXPECT_STRING

%%

"("	{return LP;}
")"	{return RP;}
"."	{return DOT;}
"'"	{return QUOTE;}
"#f"	{return FALSE_T;}
"#t"	{return TRUE_T;}
{WS}*	/* do nothing with white space */

[-]?{DIG} {
	yylval.n = strtoll(yytext, NULL, 10);
	return FIXNUM_T;
}
[-]?{DIG}?"."{DIG}	{return FLOATNUM_T;}

#\\	{BEGIN(EXPECT_CHAR);}
<EXPECT_CHAR>{CHAR_VAL} {
	BEGIN(INITIAL); 
	yylval.c = *yytext;
	return CHAR_T;
}

\"	{BEGIN(EXPECT_STRING); return DOUBLE_QUOTE;}
<EXPECT_STRING>(\\n|\\["]|\\\\|[^\\"])* {
	yylval.s = yytext;
	return STRING_T;
}
<EXPECT_STRING>\" {BEGIN(INITIAL); return DOUBLE_QUOTE;}

{IDENT}+  { return SYMBOL; }
%%
