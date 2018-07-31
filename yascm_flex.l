/*
 * "yascm_flex.l" yascm lexer.
 * Copyright (C) 2015 Hmgle <dustgle@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
;.*$	/* Scheme comment */
<<EOF>>	{return END_OF_FILE;}

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
<EXPECT_STRING>([^\\\"]|\\.)* {
	yylval.s = yytext;
	return STRING_T;
}
<EXPECT_STRING>\" {BEGIN(INITIAL); return DOUBLE_QUOTE;}

{IDENT}+  {yylval.s = yytext; return SYMBOL_T;}
%%
