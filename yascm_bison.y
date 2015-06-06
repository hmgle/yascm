%{
#include <stdio.h>
#include "yascm.h"

void yyerror(const char *s);
%}

%union {
	struct object_s *var;
};

%token LP
%token RP
%token DOT
%token FIXNUM_T
%token FLOATNUM_T
%token FALSE_T
%token TRUE_T
%token CHAR_T
%token STRING_T
%token DOUBLE_QUOTE

%%

exp: object {fprintf(stderr, "exp\n"); YYACCEPT;} exp
   | /* NULL */
   ;

boolean: TRUE_T {printf("true\n");}
       | FALSE_T {printf("false\n");}

string: DOUBLE_QUOTE
      STRING_T {printf("string\n");}
      DOUBLE_QUOTE

number: FIXNUM_T {printf("fixnum\n");}
      | FLOATNUM_T {printf("float\n");}

object: boolean		{printf("boolean\n");}
      | CHAR_T		{printf("char\n");}
      | string		{printf("string\n");}
      | number		{printf("number\n");}

%%

void yyerror(const char *s)
{
	fprintf(stderr, "error: %s\n", s);
}

