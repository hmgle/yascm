#include <stdio.h>
#include "yascm.h"

int yyparse(void);

int main(int argc, char **argv)
{
	yyparse();
	return 0;
}
