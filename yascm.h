/*
 * "yascm.h" yascm data types and external functions.
 * Copyright (C) 2015 Hmgle <dustgle@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
		debug_print(fmt, ##__VA_ARGS__); \
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
	KEYWORD,
	PRIM,
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
		struct { /* COMPOUND_PROC */
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
object *make_function(object *parameters, object *body, object *env);
object *make_env(object *var, object *up);
object *eval(object *env, object *obj);
void object_print(const object *obj);
void eof_handle(void);

#endif
