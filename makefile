RM = rm
CC = gcc
CFLAGS = -c -Wall -std=c99 -O3 -pedantic

#timto se sestavi spustitelny soubor z *.o binarek
prekladac: main.o lexikalni_analyzator.o syntakticky_analyzator.o vyrazy.o ial.o funkce.o instrukce.o tagenerator.o vestfunk.o clean.o garbage_collector.o
	$(CC) main.o lexikalni_analyzator.o syntakticky_analyzator.o vyrazy.o ial.o funkce.o instrukce.o tagenerator.o  vestfunk.o clean.o garbage_collector.o -lm -o prekladac

clean:
	$(RM)   *.o