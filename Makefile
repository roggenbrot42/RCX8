####### Compiler, tools and options
CC            = gcc
CXX           = g++
CFLAGS        = -std=c99 -m64 -pipe -O3 -g -pipe -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4  -W -D_REENTRANT $(DEFINES)
LINK          = g++
LDFLAGS	      = 
####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = src/main.c \
		src/loader.c 
OBJECTS       = main.o \
		loader.o
all: main.o loader.o rcx8
####### Implicit rules

rcx8: main.o loader.o
	$(CC) $(LDFLAGS) -o rcx8 main.o loader.o

.SUFFIXES: .o .c

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Compile

main.o: src/main.c src/program.h \
		src/loader.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o main.o src/main.c

loader.o: src/loader.c src/loader.h \
		src/auxiliary.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o loader.o src/loader.c
	
clean:
	rm *.o *.out core
