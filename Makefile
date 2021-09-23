CC=gcc
CFLAGS=-Wall -g

PROJECT=gsu_shell
OBJECTS=parser.o process.o

$(PROJECT): $(PROJECT).c $(OBJECTS)

%.o: %.c %.h

doc:
	doxygen

clean:
	rm -rf *.o $(PROJECT)
