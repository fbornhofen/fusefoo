CC=gcc
INC=/opt/local/include
LIB=/opt/local/lib
CFLAGS=-I $(INC) -L $(LIB) -lm -lfuse -D_FILE_OFFSET_BITS=64

all: fusefoo.c
	$(CC) -o fusefoo fusefoo.c $(CFLAGS)
