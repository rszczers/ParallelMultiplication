CC=gcc
LIBS=-std=c99
SRC=bar.c counter.c main.c
OUT=test.o

test:
	$(CC) $(SRC) $(LIBS) -o $(OUT)

data:
	bash generate.sh > a.dat; bash generate.sh > b.dat

run:
	clear; printf "Generowanie próbnych danych. Proszę poczekać...\n"; bash generate.sh > a.dat; bash generate.sh > b.dat; printf "Trwa kompilowanie. Proszę poczekać...\n"; $(CC) $(SRC) $(LIBS) -o $(OUT); clear; ./$(OUT)

clean:
	$(RM) *.o *.dat *.out *.swp

