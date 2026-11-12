CC = gcc -g
ODBC_CFLAGS ?= $(shell odbc_config --cflags 2>/dev/null)
ODBC_LIBS ?= $(shell odbc_config --libs 2>/dev/null)
CFLAGS = -Wall -Wextra -pedantic -ansi $(ODBC_CFLAGS)
LDLIBS = $(if $(ODBC_LIBS),$(ODBC_LIBS),-lodbc)

export PGDATABASE := classicmodels
export PGUSER := alumnodb
export PGPASSWORD := alumnodb
export PGCLIENTENCODING := LATIN9
export PGHOST := localhost

DBNAME = $(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists

HEADERS = odbc.h products.h orders.h customers.h
OBJ = menu.o odbc.o products.o orders.o customers.o
EXE = menu

.PHONY: all compile dropdb createdb restore clean splint

# 1. Borrar, crear y poblar la base de datos
all: dropdb createdb restore

dropdb:
	@echo Eliminando BBDD...
	@$(DROPDB) $(DBNAME)
	rm -f *.log

createdb:
	@echo Creando BBDD...
	@$(CREATEDB) $(DBNAME)

restore:
	@echo Poblando BBDD...
	@cat $(DBNAME).sql | $(PSQL)

# 2. Compilar el código
compile: $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LDLIBS)

%.o: %.c $(HEADERS)
	@echo Compilando $<...
	$(CC) $(CFLAGS) -c $< -o $@

splint:
	splint -nullpass *.c *.h > splint.log

clean:
	rm -f *.o core $(EXE)
