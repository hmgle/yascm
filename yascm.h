#ifndef _YASCM_H
#define _YASCM_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define debug_print(fmt, ...) \
	do { \
		fprintf(stderr, "debug_print: %s: %d: %s():" \
			fmt "\n", __FILE__, __LINE__, __func__, \
			##__VA_ARGS__); \
	} while (0)

#define DIE(fmt, ...) \
	do { \
		fprintf(stderr, "debug_print: %s: %d: %s():" \
			fmt "\n", __FILE__, __LINE__, __func__, \
			##__VA_ARGS__); \
		exit(-1); \
	} while (0)

typedef enum {
	FIXNUM,
	FLOATNUM,
	BOOL,
	CHAR,
	STRING,
	PAIR,
	SYMBOL,
	PRIM,
	LAMBDA,
	COMPOUND_PROC,
	ENV,
	OTHER,
} object_type;

typedef struct object_s object;
typedef object *Primitive(object *env, object *args);

struct object_s {
	object_type type;
	union {
		int64_t int_val; /* FIXNUM */
		long double float_val; /* FLOATNUM */
		bool bool_val; /* BOOL */
		char char_val; /* CHAR */
		char *string_val; /* STRING */
		struct { /* PAIR */
			object *car;
			object *cdr;
		};
		struct { /* COMPOUND_PROC or LAMBDA */
			object *parameters;
			object *body;
			object *env;
		};
		struct { /* env frame */
			object *vars;
			object *up;
		};
		Primitive *func;
	};
};

object *cons(object *car, object *cdr);
object *make_bool(bool val);
object *make_char(char val);
object *make_string(const char *val);
object *make_fixnum(int64_t val);
object *make_emptylist(void);
object *make_symbol(const char *name);
object *make_quote(object *obj);
object *make_function(object *parameters, object *body);
object *make_env(object *var, object *up);
object *eval(object *env, object *obj);
void object_print(const object *obj);

#endif
