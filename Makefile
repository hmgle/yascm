TARGET = yascm

all:: $(TARGET)

yascm: yascm.c yascm_flex.lex.c yascm_bison.tab.c
	cc $^ -o $@

clean::
	-rm -f $(TARGET) *.o
