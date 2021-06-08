# compiler to use
CC=gcc
# flags to use
CFLAGS=-Wall -pedantic -ansi
#oject directory
ODIR=obj
#tests directory
TDIR=tests
#include directory
IDIR=include
# depndensis (all .h files)
DEPS=$(wildcard $(IDIR)/*.h)
# all c source files in current directory (./)
CFILES=$(filter %.c,$(wildcard *))
# getting all object files that need to be created
OBJ=$(CFILES:%.c=$(ODIR)/%.o)


all : depndensis assembler.out copy_for_test

#make executable file
assembler.out : $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)

#creating all object files
$(ODIR)/%.o : %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

#creating object directory if not exists
depndensis :
	mkdir -p $(ODIR)

copy_for_test:
	cp assembler.out $(TDIR)

#phoney clean function to clean object and executable file
.PHONY: deepclean clean depndensis all reset copy_for_test


# DANGEROUS DEEP CLEAN
deepclean : clean
	rm *.ent *.ext *.obj *.as

#cleaning funtion
clean :
	rm *.out $(ODIR)/*.o $(TDIR)/*.out

# deletes all .ent .ext .ob in tests
reset:
	rm $(TDIR)/*.ent $(TDIR)/*.ext $(TDIR)/*.ob