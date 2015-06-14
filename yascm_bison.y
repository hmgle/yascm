%{
#include <stdio.h>
#include "yascm.h"

void yyerror(struct object_s *env, const char *s);
%}

%parse-param {struct object_s *env}

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
%token SYMBOL_T

%type <s> string
%type <var> object

%start exp

%%

exp: object {fprintf(stderr, "exp\n> ");} exp
   | /* NULL */
   ;

string: DOUBLE_QUOTE STRING_T DOUBLE_QUOTE {$$ = $2;}

number: FIXNUM_T {
      printf("fixnum: %ld\n", $1); 
      }
      | FLOATNUM_T {printf("float\n");}

emptylist: LP RP 

quote_list: QUOTE object {printf("quote:\n");}

pairs_list: object
	  | object pairs_list

pairs_end: pairs_list RP

pairs: LP pairs_end

object: TRUE_T		{$$ = make_bool(true); printf("#t\n");}
      | FALSE_T		{$$ = make_bool(false); printf("#f\n");}
      | CHAR_T		{$$ = make_char($1); printf("char: %c\n", $1);}
      | string		{$$ = make_string($1); printf("string %s\n", $1);}
      | number		{printf("number\n");}
      | emptylist	{printf("()\n");}
      | quote_list	{printf("quote_list\n");}
      | SYMBOL_T	{printf("symbol\n");}
      | pairs		{printf("pairs\n");}

%%

void yyerror(struct object_s *env, const char *s)
{
	(void)env;
	fprintf(stderr, "error: %s\n", s);
}

