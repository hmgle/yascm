#include <stdio.h>

typedef enum {
    NUM,
    SYMBOL,
} atom_type;

struct atom_s {
    atom_type type;
    union {
        int num;
        char *symbol;
    } data;
};

struct sexp_s {
    union {
        struct atom_s atom;
        struct {
            struct sexp_s *car;
            struct sexp_s *cdr;
        } sexp;
    } data;
};

int main(int argc, char **argv)
{
    return 0;
}
