CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc

HEADERS = odbc.h customers.h

EXE = menu
OBJ = menu.o customers.o odbc.o


all : $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LDLIBS)

%.o: %.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f *.o core $(EXE)


