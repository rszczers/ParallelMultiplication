CC=gcc
OPTIONS=-std=c99
LIBS=
SRC=bar.c counter.c main.c
OUT=./build/test.o

test:
	$(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT)

data:
	bash generate.sh > ./resources/a.dat; bash generate.sh > ./resources/b.dat

run:
	clear; printf "Generowanie próbnych danych. Proszę poczekać...\n"; make data; printf "Trwa kompilowanie. Proszę poczekać...\n"; $(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT); clear; ./$(OUT)

clean:
	$(RM) *.o *.dat *.out *.swp

