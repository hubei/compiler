# DHBW CC directories
OBJDIR = bin
SRCDIR = src

# Executables
CC = gcc
BISON = bison
FLEX = flex
RM = rm
DOXYGEN = doxygen
SPLINT = splint
VALGRIND = valgrind

# Compiler Flags
STANDARDS = -std=c99 -D_POSIX_C_SOURCE=200809L
INCLUDES = -I$(SRCDIR) -I$(OBJDIR) -Iinclude
CFLAGS = -O0 -Wall -Wextra -pedantic -g $(INCLUDES) $(STANDARDS) -Wno-unused-parameter
CFLAGS_GEN = -O2 -g -I$(OBJDIR) -w $(INCLUDES) $(STANDARDS)
FLEXFLAGS = 

.PHONY: all check test debug
all: $(OBJDIR) compiler 

$(OBJDIR):
	mkdir -p $(OBJDIR)

###############################################
# DHBW Compiler                               #
###############################################
DHBWCC_OBJECTS = $(addprefix $(OBJDIR)/, scanner.o parser.o main.o diag.o resource_manager.o symboltable.o address_code.o mips32gen.o typechecking.o generalParserFunc.o)

## Convenient targets for DHBW Compiler
.PHONY: compiler cc_objects cc_parser cc_scanner 
compiler: $(OBJDIR)/dhbwcc
cc_objects: $(GENERATED_OBJECTS)
parser: $(OBJDIR)/parser.c $(OBJDIR)/parser.h
scanner: $(OBJDIR)/scanner.c

$(OBJDIR)/dhbwcc: $(DHBWCC_OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $+
	
$(OBJDIR)/symboltable.o: $(SRCDIR)/symboltable.c $(SRCDIR)/symboltable.h
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJDIR)/address_code.o: $(SRCDIR)/address_code.c $(SRCDIR)/address_code.h
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJDIR)/typechecking.o: $(SRCDIR)/typechecking.c $(SRCDIR)/typechecking.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/generalParserFunc.o: $(SRCDIR)/generalParserFunc.c $(SRCDIR)/generalParserFunc.h
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJDIR)/mips32gen.o: $(SRCDIR)/mips32gen.c $(SRCDIR)/mips32gen.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/diag.o: $(SRCDIR)/diag.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/resource_manager.o: $(SRCDIR)/resource_manager.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/scanner.o: $(OBJDIR)/scanner.c $(OBJDIR)/parser.h
	$(CC) $(CFLAGS_GEN) -c $< -o $@

$(OBJDIR)/parser.o: $(OBJDIR)/parser.c $(OBJDIR)/parser.h
	$(CC) $(CFLAGS_GEN) -c $< -o $@

$(OBJDIR)/parser.c $(OBJDIR)/parser.h: $(SRCDIR)/parser.y
	$(BISON) -v -t --locations --defines=$(OBJDIR)/parser.h \
	  -o $(OBJDIR)/parser.c -rall --report-file=$(OBJDIR)/bison.out $<

$(OBJDIR)/scanner.c: $(SRCDIR)/scanner.l
	$(FLEX) $(FLEXFLAGS) -o $(OBJDIR)/scanner.c $<

.PHONY: clean_compiler
clean_compiler:
	$(RM) -f bin/*


###############################################
# DHBW Compiler RT-Check                      #
###############################################
SPLINT_OPTIONS = -unrecog
VALGRIND_OPTIONS = --leak-check=full

.PHONY: check_compiler
check_compiler:
	$(VALGRIND) $(VALGRIND_OPTIONS) bin/dhbwcc -p a.c

###############################################
# Doxygen Project Documentation               #
###############################################
DOXYGEN_CONFIG = doxygen/config
DOXYGEN_FLAGS = 

.PHONY: doxygen clean_doxygen
doxygen:
	$(DOXYGEN) $(DOXHGEN_FLAGS) $(DOXYGEN_CONFIG)

clean_doxygen:
	$(RM) -rf doxygen/html

.PHONY: clean
clean: clean_compiler clean_doxygen
	
