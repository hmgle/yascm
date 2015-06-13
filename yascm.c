#include <stdio.h>
#include <stdlib.h>
#include "yascm.h"

int yyparse(struct object_s *env);

static struct object_s *create_object(void)
{
	struct object_s *obj = calloc(1, sizeof(*obj));
	if (obj == NULL) {
		fprintf(stderr, "calloc fail!\n");
		exit(1);
	}
	return obj;
}

struct object_s *make_fixnum(int64_t val)
{
	struct object_s *obj = create_object();
	obj->type = FIXNUM;
	obj->data.int_val = val;
	return obj;
}

void object_print(const struct object_s *obj)
{
	/* TODO */
	if (obj->type == FIXNUM)
		printf("FIXNUM: %ld\n", obj->data.int_val);
}

int main(int argc, char **argv)
{
	struct object_s *env;
	printf("welcome\n> ");
	yyparse(env);
	return 0;
}
