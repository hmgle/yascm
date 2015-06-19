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

object *eval(object *env, object *obj)
{
	/* TODO */
	object *bind;
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
	default:
		debug_print("Unknow type: %d", obj->type);
	}
	return Nil;
}

void object_print(const object *obj)
{
	/* TODO */
	if (!obj) goto end;
	switch (obj->type) {
	case FIXNUM:
		printf("FIXNUM: %ld\n", obj->int_val);
		break;
	case PRIM:
		printf("<primitive>\n");
		break;
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
		if (list = Nil)
			return len;
		list = list->cdr;
		len++;
	}
}

static object *prim_quote(object *env, object *args_list)
{
	if (list_length(args_list) != 1)
		DIE("quote");
	return args_list->car;
}

static object *prin_define(object *env, object *args_list)
{
	/* TODO */
	return NULL;
}

static void define_prim(object *env)
{
	add_primitive(env, "+", prim_plus);
	add_primitive(env, "quote", prim_quote);
	add_primitive(env, "define", prin_define);
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
