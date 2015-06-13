#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yascm.h"

int yyparse(struct object_s *env);

static struct object_s *Nil;
static struct object_s *Symbol_table;

static struct object_s *create_object(int type)
{
	struct object_s *obj = calloc(1, sizeof(*obj));
	if (obj == NULL) {
		fprintf(stderr, "calloc fail!\n");
		exit(1);
	}
	obj->type = type;
	return obj;
}

static struct object_s *cons(struct object_s *car, struct object_s *cdr)
{
	struct object_s *pair = create_object(PAIR);
	pair->data.car = car;
	pair->data.cdr = cdr;
	return pair;
}

struct object_s *make_fixnum(int64_t val)
{
	struct object_s *obj = create_object(FIXNUM);
	obj->data.int_val = val;
	return obj;
}

struct object_s *_make_symbol(const char *name)
{
	struct object_s *obj = create_object(SYMBOL);
	obj->data.string_val = strdup(name);
	return obj;
}

struct object_s *make_symbol(const char *name)
{
	struct object_s *p;
	for (p = Symbol_table; p != Nil; p = p->data.cdr)
		if (strcmp(name, p->data.car->data.string_val) == 0)
			return p->data.car;
	struct object_s *sym = _make_symbol(name);
	Symbol_table = cons(sym, Symbol_table);
	return sym;
}

void object_print(const struct object_s *obj)
{
	/* TODO */
	if (obj->type == FIXNUM)
		printf("FIXNUM: %ld\n", obj->data.int_val);
}

static void add_primitive(struct object_s *env, char *name, Primitive *fn)
{
}

struct object_s *make_env(struct object_s *var, struct object_s *up)
{
	struct object_s *env = create_object(ENV);
	env->data.car = var;
	env->data.cdr = up;
	return env;
}

int main(int argc, char **argv)
{
	struct object_s *env = make_env(Nil, NULL);
	Symbol_table = Nil;
	printf("welcome\n> ");
	yyparse(env);
	return 0;
}
