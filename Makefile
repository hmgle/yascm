CFLAGS += -Wall

TARGET = yascm

all:: $(TARGET)

yascm: yascm_flex.l yascm_bison.y yascm.h yascm.c
	bison -d yascm_bison.y
	flex -o yascm_flex.lex.c yascm_flex.l
	cc yascm_bison.tab.c yascm_flex.lex.c yascm.c -o $@

clean::
	-rm -f $(TARGET) *.o
