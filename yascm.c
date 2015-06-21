#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yascm.h"

int yyparse(struct object_s **env);

object *Nil;
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
	debug_print();
	free(obj);
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

object *make_function(object *parameters, object *body)
{
	object *function = create_object(LAMBDA);
	function->parameters = parameters;
	function->body = body;
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

static object *apply(object *env, object *fn, object *args)
{
	/* TODO */
	if (args != Nil && args->type != PAIR)
		DIE("args must be a list");
	if (fn->type == PRIM) {
		return fn->func(env, args);
	}
	debug_print("error");
	return NULL;
}

object *extend_env(object *vars, object *vals, object *base_env)
{
	object *newenv = make_env(Nil, base_env);
	while (vars != Nil && vals != Nil) {
		add_variable(newenv, vars->car, vals->car);
		vars = vars->cdr;
		vals = vals->cdr;
	// add_variable(env, sym, prim);
	}
	// while (args_list != Nil) {
	// 	ret += eval(env, args_list->car)->int_val;
	// 	args_list = args_list->cdr;
	// }
	// return make_fixnum(ret);
	return newenv;
}

object *eval(object *env, object *obj)
{
	/* TODO */
	object *bind;
	object *fn, *args;
	object *newenv, *newobj;
	if (obj == Nil) return Nil;
	switch (obj->type) {
	case FIXNUM:
	case FLOATNUM:
	case BOOL:
	case CHAR:
	case STRING:
		return obj;
	case SYMBOL:
		bind = lookup_variable_val(obj, env);
		if (!bind)
			DIE("not define: %s", obj->string_val);
		return bind->cdr;
	case PAIR:
		fn = eval(env, obj->car);
		args = obj->cdr;
		if (fn->type == PRIM){
			debug_print();
			return apply(env, fn, args);
		} else if (fn->type == COMPOUND_PROC) {
			debug_print();
			newenv = extend_env(fn->parameters, args, env);
			newobj = fn->body;
			return eval(newenv, newobj);
		} else {
			DIE("not COMPOUND_PROC or PRIM");
		}
	case LAMBDA:
		debug_print();
		obj->type = COMPOUND_PROC;
		obj->env = env;
		return obj;
	default:
		debug_print("Unknow type: %d", obj->type);
	}
	return Nil;
}

void object_print(const object *obj)
{
	/* TODO */
	if (!obj) goto end;
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
	case SYMBOL:
		printf("%s\n", obj->string_val);
		break;
	default:
		debug_print("obj type: %d", obj->type);
	}
end:
	printf("> ");
}

static void add_primitive(object *env, char *name, Primitive *func)
{
	object *sym = make_symbol(name);
	object *prim = create_object(PRIM);
	prim->func = func;
	add_variable(env, sym, prim);
}

static object *prim_plus(object *env, object *args_list)
{
	int64_t ret = 0;
	while (args_list != Nil) {
		ret += eval(env, args_list->car)->int_val;
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
	debug_print("args type: %d", args->type);
	if (args->car->type == SYMBOL) {
		debug_print();
		return car(args);
	} else { /* PAIR */
		debug_print();
		return caar(args);
	}
}

static object *def_val(object *args)
{
	debug_print("args type: %d", args->type);
	if (car(args)->type == SYMBOL) {
		debug_print();
		return cadr(args);
	} else { /* PAIR */
		debug_print("cdar(args) type: %d", cdar(args)->type);
		debug_print("cadr(args) type: %d", cadr(args)->type);
		return make_function(cdar(args), cadr(args));
	}
}

static void define_variable(object *var, object *val, object *env)
{
	/* TODO */
	debug_print("var type: %d", var->type);
	debug_print("var val: %s", var->string_val);
	debug_print("val type: %d", val->type);
	object *oldvar = lookup_variable_val(var, env);
	if (oldvar != NULL) {
		debug_print();
		destroy_object(oldvar->cdr);
		oldvar->cdr = val;
		return;
	}
	add_variable(env, var, val);
}

static object *prim_define(object *env, object *args_list)
{
	/* TODO */
	debug_print();
	define_variable(def_var(args_list), eval(env, def_val(args_list)), env);
	return NULL;
}

static void define_prim(object *env)
{
	add_primitive(env, "+", prim_plus);
	add_primitive(env, "quote", prim_quote);
	add_primitive(env, "define", prim_define);
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
	object *genv = make_env(Nil, NULL);
	object *obj;
	Symbol_table = Nil;
	define_prim(genv);
	printf("welcome\n> ");
	for (;;) {
		yyparse(&obj);
		object_print(eval(genv, obj));
	}
	return 0;
}
