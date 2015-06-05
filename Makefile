CFLAGS += -Wall

TARGET = yascm

all:: $(TARGET)

yascm: yascm.o

clean::
	-rm -f $(TARGET) *.o
