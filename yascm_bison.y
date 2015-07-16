%{
#include <stdio.h>
#include <stdlib.h>
#include "yascm.h"

void yyerror(struct object_s **obj, const char *s);
%}

%parse-param {struct object_s **obj}

%union {
	struct object_s *var;
	int64_t n;
	long double d;
	char c;
	char *s;
};

%token <var> LP
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
%token <s> SYMBOL_T
%token END_OF_FILE

%type <s> string
%type <var> object
%type <n> number
%type <var> emptylist
%type <var> quote_list
%type <var> pair
%type <var> list_item
%type <var> list_end
%type <var> list

%start exp

%%

/* exp: object {object_print(eval(GENV, $1));} exp */
exp: object {*obj = $1; YYACCEPT;}
   | END_OF_FILE {eof_handle(); YYACCEPT;}

string: DOUBLE_QUOTE STRING_T DOUBLE_QUOTE {$$ = $2;}
      | DOUBLE_QUOTE DOUBLE_QUOTE {$$ = "\"";}

number: FIXNUM_T {$$ = $1;}
      | FLOATNUM_T {printf("float: not support now\n");}

emptylist: LP RP 

quote_list: QUOTE object {$$ = make_quote($2);}

pair: object DOT object {$$ = cons($1, $3);}

list_item: object {$$ = cons($1, make_emptylist());}
	  | object list_item {$$ = cons($1, $2);}
	  | pair {$$ = $1;}

list_end: list_item RP {$$ = $1;}

list: LP list_end {$$ = $2;}

object: TRUE_T		{$$ = make_bool(true);}
      | FALSE_T		{$$ = make_bool(false);}
      | number		{$$ = make_fixnum($1);}
      | CHAR_T		{$$ = make_char($1);}
      | string		{$$ = make_string($1);}
      | SYMBOL_T	{$$ = make_symbol($1);}
      | emptylist	{$$ = make_emptylist();}
      | quote_list	{$$ = $1;}
      | list		{$$ = $1;}

%%

void yyerror(struct object_s **obj, const char *s)
{
	(void)obj;
	fprintf(stderr, "yyerror: %s!\n", s);
	exit(0);
}
