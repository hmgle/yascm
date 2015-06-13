#ifndef _YASCM_H
#define _YASCM_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	FIXNUM,
	FLOATNUM,
	BOOL,
	CHAR,
	STRING,
	PAIR,
} object_type;

struct object_s {
	object_type type;
	union {
		int64_t int_val; /* FIXNUM */
		long double float_val; /* FLOATNUM */
		bool bool_val; /* BOOL */
		char char_val; /* CHAR */
		char *string_val; /* STRING */
		struct { /* PAIR */
			struct object_s *car;
			struct object_s *cdr;
		};
	} data;
};

struct object_s *make_fixnum(int64_t val);
void object_print(const struct object_s *obj);

#endif
