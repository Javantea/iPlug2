CFLAGS=-I. -I../glad_GLX/include -ggdb -O2 -Wall -fPIC

ifdef GL2
CFLAGS += -I../glad_GL2/src -I../glad_GL2/include
endif
ifdef GL3
CFLAGS += -I../glad_GL3/src -I../glad_GL3/include
endif
# TODO: needs one or the other

xcbt.o: xcbt.c xcbt.h
	gcc -c -o xcbt.o xcbt.c -Wall -ggdb -O2 $(CFLAGS)
