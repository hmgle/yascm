#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yascm.h"

int yyparse(struct object_s *env);

static object *Nil;
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

static object *cons(object *car, object *cdr)
{
	object *pair = create_object(PAIR);
	pair->data.car = car;
	pair->data.cdr = cdr;
	return pair;
}

/* return ((x . y) . a) */
static object *acons(object *x, object *y, object *a)
{
	return cons(cons(x, y), a);
}

// static void add_variable(struct object_s)

object *make_fixnum(int64_t val)
{
	object *obj = create_object(FIXNUM);
	obj->data.int_val = val;
	return obj;
}

object *_make_symbol(const char *name)
{
	object *obj = create_object(SYMBOL);
	obj->data.string_val = strdup(name);
	return obj;
}

object *make_symbol(const char *name)
{
	object *p;
	for (p = Symbol_table; p != Nil; p = p->data.cdr)
		if (strcmp(name, p->data.car->data.string_val) == 0)
			return p->data.car;
	object *sym = _make_symbol(name);
	Symbol_table = cons(sym, Symbol_table);
	return sym;
}

void object_print(const object *obj)
{
	/* TODO */
	if (obj->type == FIXNUM)
		printf("FIXNUM: %ld\n", obj->data.int_val);
}

static void add_primitive(object *env, char *name, Primitive *fn)
{
}

object *make_env(object *var, object *up)
{
	object *env = create_object(ENV);
	env->data.car = var;
	env->data.cdr = up;
	return env;
}

int main(int argc, char **argv)
{
	object *env = make_env(Nil, NULL);
	Symbol_table = Nil;
	printf("welcome\n> ");
	yyparse(env);
	return 0;
}
