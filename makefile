## Grupo 006
## Pedro Trindade 56342
## Vicente Sousa 55386
## João Anjos 54476

## Define vars

INCLUDEDIR = include
OBJDIR = obj
SRCDIR = src
BINDIR = bin
OUTNAME = MAGNAEATS

## Define objects needed for compilation

REQUIREDOBJ = client.o configuration.o driver.o file-private.o main.o memory.o memory-private.o process.o restaurant.o synchronization.o 

## Create flags var
FLAGS = -fdiagnostics-color=always 

## Main target

all: clean setup compile

## add debug flag then proceed to do a full compilation
debug: FLAGS +=-g -Wall
debug: all

## create obj files
%.o: $(SRCDIR)/%.c
	$(CC) $(FLAGS) -o $(addprefix $(OBJDIR)/,$@) -c $< -I $(INCLUDEDIR) -lrt -lpthread

## compiles the obj files to an executable
compile: $(REQUIREDOBJ)
	$(CC) $(FLAGS) $(addprefix $(OBJDIR)/,$^) -o $(BINDIR)/$(OUTNAME) -lrt -lpthread

## clean folders
clean:
	rm -rf $(OBJDIR)/*
	rm -rf $(BINDIR)/*

setup:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

## find stuff marked to do in the project
wegucci:
	cd src && find . | grep -R TODO

grind:
	valgrind --leak-check=full --show-leak-kinds=all ./bin/MAGNAEATS
