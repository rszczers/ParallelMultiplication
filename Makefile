PROJECT=test
CC=gcc
OPTIONS=-std=c99
LIBS=
SRC=./src/bar.c ./src/load_matrix.c ./src/main.c
OUT=./build/test.o

test:
	$(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT)

data:
	bash ./src/generate.sh > ./resources/a.dat; bash ./src/generate.sh > ./resources/b.dat

run:
	clear; printf "Generowanie próbnych danych. Proszę poczekać...\n"; make data; printf "Trwa kompilowanie. Proszę poczekać...\n"; $(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT); clear; $(OUT)

clean:
	$(RM) ./build/*.o ./resources/*.dat ./build/*.out ./src/*.swp