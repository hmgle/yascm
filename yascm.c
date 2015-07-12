#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yascm.h"

extern FILE *yyin;
int yyparse(struct object_s **env);

object *Nil;
object *Else;
object *Ok;
object *Unspecified;
static object FALSE = {
	.type = BOOL,
	.bool_val = false,
};
static object TRUE = {
	.type = BOOL,
	.bool_val = true,
};
static object *Symbol_table;
static bool NOT_END = true;

void eof_handle(void)
{
	NOT_END = false;
}

static object *create_object(int type)
{
	object *obj = calloc(1, sizeof(*obj));
	if (obj == NULL) {
		fprintf(stderr, "calloc fail!\n");
		exit(1);
	}
	obj->type = type;
	return obj;
}

static object *car(object *pair)
{
	return pair->car;
}

static object *cdr(object *pair)
{
	return pair->cdr;
}

#define cadr(obj)	car(cdr(obj))
#define cdar(obj)	cdr(car(obj))
#define caar(obj)	car(car(obj))
#define caddr(obj)	car(cdr(cdr(obj)))

object *cons(object *car, object *cdr)
{
	object *pair = create_object(PAIR);
	pair->car = car;
	pair->cdr = cdr;
	return pair;
}

/* return ((x . y) . a) */
static object *acons(object *x, object *y, object *a)
{
	return cons(cons(x, y), a);
}

static void add_variable(object *env, object *sym, object *val)
{
	env->vars = acons(sym, val, env->vars);
}

object *make_bool(bool val)
{
	/* XXX: Efficient but no safe */
	return val ? &TRUE : &FALSE;
}

object *make_char(char val)
{
	object *obj = create_object(CHAR);
	obj->char_val = val;
	return obj;
}

object *make_string(const char *val)
{
	size_t len = strlen(val) - 1; /* ignore the last DOUBLE_QUOTE */
	object *obj = create_object(STRING);
	obj->string_val = malloc(len + 1);
	assert(obj->string_val != NULL);
	strncpy(obj->string_val, val, len);
	obj->string_val[len] = '\0';
	return obj;
}

object *make_fixnum(int64_t val)
{
	object *obj = create_object(FIXNUM);
	obj->int_val = val;
	return obj;
}

object *make_emptylist(void)
{
	return Nil;
}

object *make_quote(object *obj)
{
	return cons(make_symbol("quote"), cons(obj, Nil));
}

object *_make_symbol(const char *name)
{
	assert(name != NULL);
	object *obj = create_object(SYMBOL);
	obj->string_val = strdup(name);
	return obj;
}

object *make_symbol(const char *name)
{
	object *p;
	for (p = Symbol_table; p != Nil; p = p->cdr)
		if (strcmp(name, p->car->string_val) == 0)
			return p->car;
	object *sym = _make_symbol(name);
	Symbol_table = cons(sym, Symbol_table);
	return sym;
}

object *make_function(object *parameters, object *body, object *env)
{
	object *function = create_object(COMPOUND_PROC);
	function->parameters = parameters;
	function->body = body;
	function->env = env;
	return function;
}

static object *lookup_variable_val(object *var, object *env)
{
	object *p, *cell;
	for (p = env; p; p = p->up) {
		for (cell = p->vars; cell != Nil; cell = cell->cdr) {
			object *bind = cell->car;
			if (var == bind->car)
				return bind;
		}
	}
	return NULL;
}

static object *list_of_val(object *args, object *env)
{
	if (args == Nil)
		return Nil;
	return cons(eval(env, car(args)), list_of_val(cdr(args), env));
}

static object *apply(object *env, object *fn, object *args)
{
	object *eval_args;
	if (args != Nil && args->type != PAIR)
		DIE("args must be a list");
	if (fn->type == PRIM) {
		eval_args = list_of_val(args, env);
		return fn->func(env, eval_args);
	} else if (fn->type == KEYWORD) {
		return fn->func(env, args);
	}
	debug_print("error");
	return NULL;
}

object *extend_env(object *vars, object *vals, object *base_env)
{
	object *newenv = make_env(Nil, base_env);
	while (vars != Nil && vals != Nil) {
		if (vars->type == SYMBOL) {
			add_variable(newenv, vars, vals);
			return newenv;
		}
		add_variable(newenv, vars->car, vals->car);
		vars = vars->cdr;
		vals = vals->cdr;
	}
	return newenv;
}

static bool is_the_last_arg(object *args)
{
	return (args->cdr == Nil) ? true : false;
}

object *eval(object *env, object *obj)
{
	object *bind;
	object *fn, *args;
	object *newenv, *newobj;
	object *eval_args;
	if (obj == Nil) return Nil;
	switch (obj->type) {
	case SYMBOL:
		bind = lookup_variable_val(obj, env);
		if (!bind)
			return Nil;
		return bind->cdr;
	case PAIR:
		fn = eval(env, obj->car);
		args = obj->cdr;
		if (fn->type == PRIM || fn->type == KEYWORD) {
			return apply(env, fn, args);
		} else if (fn->type == COMPOUND_PROC) {
			eval_args = list_of_val(args, env);
			newenv = extend_env(fn->parameters, eval_args, fn->env);
			newobj = fn->body;
			while (!is_the_last_arg(newobj)) {
				(void)eval(newenv, newobj->car);
				newobj = newobj->cdr;
			}
			return eval(newenv, newobj->car);
		} else {
			return obj; /* list */
		}
	default:
		return obj;
	}
}

static void pair_print(const object *pair)
{
	const object *car_pair = pair->car;
	const object *cdr_pair = pair->cdr;
	object_print(car_pair);
	if (cdr_pair == Nil) {
		return;
	} else if (cdr_pair->type == PAIR) {
		printf(" ");
		pair_print(cdr_pair);
	} else {
		printf(" . ");
		object_print(cdr_pair);
	}
}

void object_print(const object *obj)
{
	if (!obj) return;
	switch (obj->type) {
	case FIXNUM:
		printf("%ld", obj->int_val);
		break;
	case KEYWORD:
		printf("<keywork>");
		break;
	case PRIM:
		printf("<primitive>");
		break;
	case BOOL:
		printf("#%c", obj->bool_val ? 't' : 'f');
		break;
	case CHAR:
		printf("#\\%c", obj->char_val);
		break;
	case STRING:
		printf("\"%s\"", obj->string_val);
		break;
	case SYMBOL:
		printf("%s", obj->string_val);
		break;
	case PAIR:
		printf("(");
		pair_print(obj);
		printf(")");
		break;
	case COMPOUND_PROC:
		printf("<proc>");
		break;
	default:
		if (obj == Nil)
			printf("()");
		else if (obj == Ok)
			fprintf(stderr, "; ok");
		else if (obj == Unspecified)
			(void)0;
		else
			printf("type: %d", obj->type);
	}
}

static void add_primitive(object *env, char *name, Primitive *func,
			  object_type type)
{
	object *sym = make_symbol(name);
	object *prim = create_object(type);
	prim->func = func;
	add_variable(env, sym, prim);
}

static object *prim_plus(object *env, object *args_list)
{
	int64_t ret = 0;
	while (args_list != Nil) {
		ret += (car(args_list)->int_val);
		args_list = args_list->cdr;
	}
	return make_fixnum(ret);
}

static int list_length(object *list)
{
	int len = 0;
	for (;;) {
		if (list == Nil)
			return len;
		list = list->cdr;
		len++;
	}
}

static object *prim_sub(object *env, object *args_list)
{
	int64_t ret;
	if (list_length(args_list) == 1)
		return make_fixnum(-car(args_list)->int_val);
	ret = car(args_list)->int_val;
	args_list = args_list->cdr;
	while (args_list != Nil) {
		ret -= (car(args_list)->int_val);
		args_list = args_list->cdr;
	}
	return make_fixnum(ret);
}

static object *prim_mul(object *env, object *args_list)
{
	int64_t ret = 1;
	while (args_list != Nil) {
		ret *= (car(args_list)->int_val);
		args_list = args_list->cdr;
	}
	return make_fixnum(ret);
}

static object *prim_cons(object *env, object *args_list)
{
	return cons(car(args_list), cadr(args_list));
}

static object *prim_car(object *env, object *args_list)
{
	return caar(args_list);
}

static object *prim_cdr(object *env, object *args_list)
{
	return cdar(args_list);
}

static object *prim_list(object *env, object *args_list)
{
	return args_list;
}

static object *prim_quote(object *env, object *args_list)
{
	if (list_length(args_list) != 1)
		DIE("quote");
	return args_list->car;
}

static object *def_var(object *args)
{
	if (args->car->type == SYMBOL)
		return car(args);
	else /* PAIR */
		return caar(args);
}

static object *def_val(object *args, object *env)
{
	if (car(args)->type == PAIR)
		return make_function(cdar(args), cdr(args), env);
	else
		return eval(env, cadr(args));
}

static void define_variable(object *var, object *val, object *env)
{
	object *oldvar = lookup_variable_val(var, env);
	if (oldvar != NULL) {
		oldvar->cdr = val;
		return;
	}
	add_variable(env, var, val);
}

static object *set_var(object *args)
{
	return car(args);
}

static object *set_val(object *args)
{
	return cadr(args);
}

static void set_var_val(object *var, object *val, object *env)
{
	object *oldvar = lookup_variable_val(var, env);
	if (oldvar == NULL)
		DIE("unbound variable");
	oldvar->cdr = val;
}

static object *prim_define(object *env, object *args_list)
{
	define_variable(def_var(args_list), def_val(args_list, env), env);
	return Ok;
}

static object *prim_lambda(object *env, object *args_list)
{
	return make_function(car(args_list), cdr(args_list), env);
}

static object *prim_let(object *env, object *args_list)
{
	object *parameters;
	object *exps;
	object **para_end = &parameters;
	object **exps_end = &exps;
	object *let_var_exp = car(args_list);
	object *let_body = cdr(args_list);
	while (let_var_exp != Nil) {
		*para_end = cons(caar(let_var_exp), Nil);
		para_end = &((*para_end)->cdr);
		*exps_end = cons(car(cdar(let_var_exp)), Nil);
		exps_end = &((*exps_end)->cdr);
		let_var_exp = cdr(let_var_exp);
	}
	object *lambda = make_function(parameters, let_body, env);
	return eval(env, cons(lambda, exps));
}

static object *prim_set(object *env, object *args_list)
{
	set_var_val(set_var(args_list), eval(env, set_val(args_list)), env);
	return Ok;
}

static bool is_false(object *obj)
{
	if (obj->type != BOOL)
		return false;
	if (obj->bool_val != false)
		return false;
	return true;
}

static bool is_true(object *obj)
{
	return !is_false(obj);
}

static object *prim_and(object *env, object *args_list)
{
	object *obj;
	object *ret;
	while (args_list != Nil) {
		obj = car(args_list);
		ret = eval(env, obj);
		if (is_false(ret))
			return ret;
		args_list = args_list->cdr;
	}
	return ret;
}

static object *prim_or(object *env, object *args_list)
{
	object *obj;
	object *ret;
	while (args_list != Nil) {
		obj = car(args_list);
		ret = eval(env, obj);
		if (is_true(ret))
			return ret;
		args_list = args_list->cdr;
	}
	return ret;
}

static object *prim_begin(object *env, object *args_list)
{
	object *obj;
	object *ret;
	while (args_list != Nil) {
		obj = car(args_list);
		ret = eval(env, obj);
		args_list = args_list->cdr;
	}
	return ret;
}

static object *prim_if(object *env, object *args_list)
{
	object *predicate = eval(env, car(args_list));
	if (is_true(predicate))
		return eval(env, cadr(args_list));
	if (is_the_last_arg(args_list->cdr))
		return Unspecified;
	return eval(env, caddr(args_list));
}

static bool is_else(object *sym)
{
	return (sym == Else) ? true : false;
}

static object *eval_args_list(object *env, object *args_list)
{
	object *arg;
	for (arg = args_list; !is_the_last_arg(arg); arg = arg->cdr)
		(void)eval(env, arg->car);
	return eval(env, arg->car);
}

static object *prim_cond(object *env, object *args_list)
{
	object *pairs = args_list;
	object *predicate;
	for (pairs = args_list; pairs != Nil; pairs = pairs->cdr) {
		predicate = eval(env, caar(pairs));
		if (!is_the_last_arg(pairs)) {
			if (predicate->bool_val)
				return eval_args_list(env, cdar(pairs));
		} else {
			if (is_else(predicate))
				return eval_args_list(env, cdar(pairs));
			else if (predicate->bool_val)
				return eval_args_list(env, cdar(pairs));
		}
	}
	return Unspecified;
}

static object *prim_is_null(object *env, object *args_list)
{
	return make_bool((args_list->car == Nil) ? true : false);
}

static object *prim_is_boolean(object *env, object *args_list)
{
	return make_bool((args_list->car->type == BOOL) ? true : false);
}

static object *prim_is_pair(object *env, object *args_list)
{
	return make_bool((args_list->car->type == PAIR) ? true : false);
}

static object *prim_is_symbol(object *env, object *args_list)
{
	return make_bool((args_list->car->type == SYMBOL) ? true : false);
}

static object *prim_is_number(object *env, object *args_list)
{
	return make_bool((args_list->car->type == FIXNUM ||
			args_list->car->type == FLOATNUM) ? true : false);
}

static object *prim_is_char(object *env, object *args_list)
{
	return make_bool((args_list->car->type == CHAR) ? true : false);
}

static object *prim_is_string(object *env, object *args_list)
{
	return make_bool((args_list->car->type == STRING) ? true : false);
}

static object *prim_is_procedure(object *env, object *args_list)
{
	return make_bool((args_list->car->type == COMPOUND_PROC ||
			args_list->car->type == PRIM) ? true : false);
}

static object *prim_is_eq(object *env, object *args_list)
{
	object *obj;
	object *first = args_list->car;
	while (args_list != Nil) {
		obj = args_list->car;
		if (obj->type != first->type)
			return make_bool(false);
		switch (first->type) {
		case FIXNUM:
			if (obj->int_val != first->int_val)
				return make_bool(false);
			break;
		case BOOL:
			if (obj->bool_val != first->bool_val)
				return make_bool(false);
			break;
		case CHAR:
			if (obj->char_val != first->char_val)
				return make_bool(false);
			break;
		case STRING:
			if (strcmp(obj->string_val, first->string_val))
				return make_bool(false);
			break;
		default:
			if (obj != first)
				return make_bool(false);
		}
		args_list = args_list->cdr;
	}
	return make_bool(true);
}

static object *prim_is_num_eq(object *env, object *args_list)
{
	int64_t first = args_list->car->int_val;
	while (args_list != Nil) {
		if (args_list->car->int_val != first)
			return make_bool(false);
		args_list = args_list->cdr;
	}
	return make_bool(true);
}

static object *prim_is_num_gt(object *env, object *args_list)
{
	int64_t next;
	int64_t first = args_list->car->int_val;
	args_list = args_list->cdr;
	while (args_list != Nil) {
		next = args_list->car->int_val;
		if (next >= first)
			return make_bool(false);
		first = next;
		args_list = args_list->cdr;
	}
	return make_bool(true);
}

static object *load_file(const char *filename, object *env)
{
	object *obj;
	FILE *oldin = (yyin == NULL) ? stdin : yyin;
	fprintf(stderr, "; loading %s\n", filename);
	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "fopen() %s fail!\n", filename);
		return Nil;
	}
	yyrestart(f);
	while (NOT_END) {
		yyparse(&obj);
		eval(env, obj);
	}
	fclose(f);
	fprintf(stderr, "; done loading %s\n", filename);
	NOT_END = true;
	yyrestart(oldin);
	return Ok;
}

static object *prim_eval(object *env, object *args_list)
{
	return eval(env, car(args_list));
}

static object *prim_load(object *env, object *args_list)
{
	return load_file(car(args_list)->string_val, env);
}

static object *prim_display(object *env, object *args_list)
{
	if (args_list->car->type == STRING)
		printf("%s", args_list->car->string_val);
	else
		object_print(car(args_list));
	return Unspecified;
}

static object *prim_newline(object *env, object *args_list)
{
	printf("\n");
	return Ok;
}

static void define_prim(object *env)
{
	add_primitive(env, "define", prim_define, KEYWORD);
	add_primitive(env, "lambda", prim_lambda, KEYWORD);
	add_primitive(env, "let", prim_let, KEYWORD);
	add_primitive(env, "set!", prim_set, KEYWORD);
	add_primitive(env, "and", prim_and, KEYWORD);
	add_primitive(env, "or", prim_or, KEYWORD);
	add_primitive(env, "begin", prim_begin, KEYWORD);
	add_primitive(env, "if", prim_if, KEYWORD);
	add_primitive(env, "cond", prim_cond, KEYWORD);
	add_primitive(env, "quote", prim_quote, KEYWORD);

	add_primitive(env, "+", prim_plus, PRIM);
	add_primitive(env, "-", prim_sub, PRIM);
	add_primitive(env, "*", prim_mul, PRIM);
	add_primitive(env, "cons", prim_cons, PRIM);
	add_primitive(env, "car", prim_car, PRIM);
	add_primitive(env, "cdr", prim_cdr, PRIM);
	add_primitive(env, "list", prim_list, PRIM);
	add_primitive(env, "null?", prim_is_null, PRIM);
	add_primitive(env, "boolean?", prim_is_boolean, PRIM);
	add_primitive(env, "pair?", prim_is_pair, PRIM);
	add_primitive(env, "symbol?", prim_is_symbol, PRIM);
	add_primitive(env, "number?", prim_is_number, PRIM);
	add_primitive(env, "char?", prim_is_char, PRIM);
	add_primitive(env, "string?", prim_is_string, PRIM);
	add_primitive(env, "procedure?", prim_is_procedure, PRIM);
	add_primitive(env, "eq?", prim_is_eq, PRIM);
	add_primitive(env, "=", prim_is_num_eq, PRIM);
	add_primitive(env, ">", prim_is_num_gt, PRIM);
	add_primitive(env, "eval", prim_eval, PRIM);
	add_primitive(env, "load", prim_load, PRIM);
	add_primitive(env, "display", prim_display, PRIM);
	add_primitive(env, "newline", prim_newline, PRIM);
}

object *make_env(object *var, object *up)
{
	object *env = create_object(ENV);
	env->vars = var;
	env->up = up;
	return env;
}

int main(int argc, char **argv)
{
	Nil = create_object(OTHER);
	Else = create_object(OTHER);
	Ok = create_object(OTHER);
	Unspecified = create_object(OTHER);
	object *genv = make_env(Nil, NULL);
	object *obj;
	Symbol_table = Nil;
	define_prim(genv);
	add_variable(genv, make_symbol("else"), Else);
	(void)load_file("stdlib.scm", genv);
	fprintf(stderr, "welcome\n> ");
	while (NOT_END) {
		yyparse(&obj);
		object_print(eval(genv, obj));
		printf("\n> ");
	}
	return 0;
}
