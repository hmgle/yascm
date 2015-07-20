TARGET = yascm

all:: $(TARGET)

yascm: yascm.c yascm_flex.lex.c yascm_bison.tab.c
	cc -O3 $^ -o $@

test:: yascm
	./yascm < tests/tests.scm

clean::
	-rm -f $(TARGET) *.o
