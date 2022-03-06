## Grupo 006
## Pedro Trindade 56342
## Vicente Sousa 55386
## João Anjos 54476

## Define vars

INCLUDEDIR = include
OBJDIR = obj
SRCDIR = src
BINDIR = bin

## Define objects needed for compilation

REQUIREDOBJ = client.o driver.o main.o memory.o process.o restaurant.o

## Create flags var
FLAGS = 

## Main target

all: clean compile

## add debug flag then proceed to do a full compilation
debug: FLAGS +=-g -Wall
debug: all

## create obj files
%.o: $(SRCDIR)/*/%.c
	$(CC) $(FLAGS) -o $(addprefix $(OBJDIR)/,$@) -c $< -I $(INCLUDEDIR)

## create obj files depth 2
%.o: $(SRCDIR)/*/*/%.c
	$(CC) $(FLAGS) -o $(addprefix $(OBJDIR)/,$@) -c $< -I $(INCLUDEDIR)

## TODO: E PARA FAZER ISTO SEUS MERDAS
compile:

## clean folders
clean:
	rm -rf $(OBJDIR)/*
	rm -rf $(BINDIR)/*

## find stuff marked to do in the project
wegucci:
	find . | grep -R TODO
