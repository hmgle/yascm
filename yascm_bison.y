%{
#include <stdio.h>
#include "yascm.h"

void yyerror(const char *s);
%}

%union {
	struct object_s *var;
	int64_t n;
	long double d;
	char c;
	char *s;
};

%token LP
%token RP
%token DOT
%token <n> FIXNUM_T
%token <d> FLOATNUM_T
%token FALSE_T
%token TRUE_T
%token <c> CHAR_T
%token <s> STRING_T
%token DOUBLE_QUOTE

%start exp

%%

exp: object {fprintf(stderr, "exp\n"); YYACCEPT;} exp
   | /* NULL */
   ;

boolean: TRUE_T {printf("true\n");}
       | FALSE_T {printf("false\n");}

string: DOUBLE_QUOTE
      STRING_T {printf("string: %s\n", $2);}
      DOUBLE_QUOTE

number: FIXNUM_T {
      printf("fixnum: %ld\n", $1); 
      }
      | FLOATNUM_T {printf("float\n");}

emptylist: LP RP 

object: boolean		{printf("boolean\n");}
      | CHAR_T		{printf("char: %c\n", $1);}
      | string		{printf("string\n");}
      | number		{printf("number\n");}
      | emptylist	{printf("()\n");}

%%

void yyerror(const char *s)
{
	fprintf(stderr, "error: %s\n", s);
}

