PROJECT=test
CC=mpiicc
OPTIONS=-std=c99
LIBS=-mkl
SRC=./src/main.c ./src/bar.c ./src/load_matrix.c ./src/save_matrix.c ./src/sequential.c ./src/naive.c
OUT=./build/test.o

test:
	$(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT)

make mrun:
	mpirun -np 6 ./build/test.o -A ./resources/a.dat -B ./resources/b.dat -m 8 -n 8 -k 8 --method=naive

data:
	bash ./src/generate.sh > ./resources/a.dat; bash ./src/generate.sh > ./resources/b.dat

run:
	clear; printf "Generowanie próbnych danych. Proszę poczekać...\n"; make data; printf "Trwa kompilowanie. Proszę poczekać...\n"; $(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT); clear; $(OUT)

clean:
	$(RM) ./build/*.o ./resources/*.dat ./build/*.out ./src/*.swp
