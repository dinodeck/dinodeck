CFLAGS=	-W -Wall -I.. -pthread -g
CC= gcc

all:
	OS=`uname`; \
	  test "$$OS" = Linux && LIBS="-ldl" ; \
	  $(CC) $(CFLAGS) hello.c ../mongoose.c  $$LIBS $(ADD) -o hello;
	  $(CC) $(CFLAGS) chat.c ../mongoose.c  $$LIBS $(ADD) -o chat
