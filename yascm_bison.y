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
%token QUOTE
%token <n> FIXNUM_T
%token <d> FLOATNUM_T
%token FALSE_T
%token TRUE_T
%token <c> CHAR_T
%token <s> STRING_T
%token DOUBLE_QUOTE
%token SYMBOL

%type <s> string

%start exp

%%

exp: object {fprintf(stderr, "exp\n> ");} exp
   | /* NULL */
   ;

string: DOUBLE_QUOTE
      STRING_T
      DOUBLE_QUOTE {$$ = $2;}

number: FIXNUM_T {
      printf("fixnum: %ld\n", $1); 
      }
      | FLOATNUM_T {printf("float\n");}

emptylist: LP RP 

quote_list: QUOTE object {printf("quote:\n");}

object: TRUE_T		{printf("#t\n");}
      | FALSE_T		{printf("#f\n");}
      | CHAR_T		{printf("char: %c\n", $1);}
      | string		{printf("string %s\n", $1);}
      | number		{printf("number\n");}
      | emptylist	{printf("()\n");}
      | quote_list
      | SYMBOL

%%

void yyerror(const char *s)
{
	fprintf(stderr, "error: %s\n", s);
}

