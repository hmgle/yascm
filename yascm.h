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
} object_type;

struct object_s {
	object_type type;
	union {
		int64_t int_val;
		long double float_val;
		bool bool_val;
		char char_val;
		char *string_val;
	} data;
};

struct object_s *make_fixnum(int64_t val);

#endif
