#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yascm.h"

int yyparse(struct object_s **env);

object *Nil;
object *Else;
object *Ok;
static object *Symbol_table;

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

static void destroy_object(object *obj)
{
	/* TODO */
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
#define cddr(obj)	cdr(cdr(obj))
#define caadr(obj)	car(car(cdr(obj)))
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
	object *obj = create_object(BOOL);
	obj->bool_val = val;
	return obj;
}

object *make_char(char val)
{
	object *obj = create_object(CHAR);
	obj->char_val = val;
	return obj;
}

object *make_string(const char *val)
{
	object *obj = create_object(STRING);
	obj->string_val = malloc(strlen(val) + 1);
	assert(obj->string_val != NULL);
	strcpy(obj->string_val, val);
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
		add_variable(newenv, vars->car, eval(base_env, vals->car));
		vars = vars->cdr;
		vals = vals->cdr;
	}
	return newenv;
}

static bool is_the_last_arg(object *args);
object *eval(object *env, object *obj)
{
	object *bind;
	object *fn, *args;
	object *newenv, *newobj;
	object *eval_args;
	if (obj == Nil) return Nil;
	switch (obj->type) {
	case FIXNUM:
	case FLOATNUM:
	case BOOL:
	case CHAR:
	case STRING:
	case OTHER:
	case COMPOUND_PROC:
		return obj;
	case SYMBOL:
		bind = lookup_variable_val(obj, env);
		if (!bind)
			DIE("not define: %s", obj->string_val);
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
			DIE("not COMPOUND_PROC or PRIM: %d", fn->type);
		}
	default:
		debug_print("Unknow type: %d", obj->type);
	}
	return Nil;
}

void object_print(const object *obj)
{
	/* TODO */
	if (!obj)
		goto end;
	if (obj == Nil) {
		printf("()\n");
		goto end;
	}
	switch (obj->type) {
	case FIXNUM:
		printf("FIXNUM: %ld\n", obj->int_val);
		break;
	case PRIM:
		printf("<primitive>\n");
		break;
	case BOOL:
		printf("#%c\n", obj->bool_val ? 't' : 'f');
		break;
	case SYMBOL:
		printf("%s\n", obj->string_val);
		break;
	case COMPOUND_PROC:
		printf("<proc>\n");
		break;
	default:
		debug_print("obj type: %d", obj->type);
	}
end:
	printf("> ");
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

static object *prim_quote(object *env, object *args_list)
{
	if (list_length(args_list) != 1) {
		DIE("quote");
	}
	return args_list->car;
}

static object *def_var(object *args)
{
	if (args->car->type == SYMBOL) {
		return car(args);
	} else { /* PAIR */
		return caar(args);
	}
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
		destroy_object(oldvar->cdr);
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
	destroy_object(oldvar->cdr);
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

static object *prim_if(object *env, object *args_list)
{
	object *predicate = eval(env, car(args_list));
	if (is_true(predicate))
		return eval(env, cadr(args_list));
	return eval(env, caddr(args_list));
}

static bool is_the_last_arg(object *args)
{
	return (args->cdr == Nil) ? true : false;
}

static bool is_else(object *sym)
{
	return (sym == Else) ? true : false;
}

static object *prim_cond(object *env, object *args_list)
{
	object *pairs = args_list;
	object *predicate;
	while (pairs != Nil) {
		predicate = eval(env, caar(pairs));
		if (!is_the_last_arg(pairs)) {
			if (predicate->bool_val) {
				return eval(env, car(cdar(pairs)));
			}
		} else {
			if (is_else(predicate)) {
				return eval(env, car(cdar(pairs)));
			} else if (predicate->bool_val) {
				return eval(env, car(cdar(pairs)));
			}
		}
		pairs = pairs->cdr;
	}
	DIE("cond error exp!");
}

static object *prim_is_eq(object *env, object *args_list)
{
	object *obj;
	object *first = eval(env, args_list->car);
	while (args_list != Nil) {
		obj = eval(env, args_list->car);
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
	int64_t first = eval(env, args_list->car)->int_val;
	while (args_list != Nil) {
		if (eval(env, args_list->car)->int_val != first)
			return make_bool(false);
		args_list = args_list->cdr;
	}
	return make_bool(true);
}

static object *prim_is_num_gt(object *env, object *args_list)
{
	int64_t next;
	int64_t first = eval(env, args_list->car)->int_val;
	args_list = args_list->cdr;
	while (args_list != Nil) {
		next = eval(env, args_list->car)->int_val;
		if (next >= first)
			return make_bool(false);
		first = next;
		args_list = args_list->cdr;
	}
	return make_bool(true);
}

static void define_prim(object *env)
{
	add_primitive(env, "define", prim_define, KEYWORD);
	add_primitive(env, "lambda", prim_lambda, KEYWORD);
	add_primitive(env, "set!", prim_set, KEYWORD);
	add_primitive(env, "if", prim_if, KEYWORD);
	add_primitive(env, "cond", prim_cond, KEYWORD);

	add_primitive(env, "+", prim_plus, PRIM);
	add_primitive(env, "quote", prim_quote, PRIM);
	add_primitive(env, "eq?", prim_is_eq, PRIM);
	add_primitive(env, "=", prim_is_num_eq, PRIM);
	add_primitive(env, ">", prim_is_num_gt, PRIM);
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
	object *genv = make_env(Nil, NULL);
	object *obj;
	Symbol_table = Nil;
	define_prim(genv);
	add_variable(genv, make_symbol("else"), Else);
	printf("welcome\n> ");
	for (;;) {
		yyparse(&obj);
		object_print(eval(genv, obj));
	}
	return 0;
}
