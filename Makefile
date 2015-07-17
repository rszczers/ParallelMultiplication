PROJECT=test
CC=mpiicc
OPTIONS=-std=c99
LIBS=-mkl
SRC=./src/main.c ./src/bar.c ./src/load_matrix.c ./src/save_matrix.c 
OUT=./build/test.o
SIZE=256
NPROC=4

test:
	$(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT)

make mrun:
	mpirun -np $(NPROC) ./build/test.o -A ./resources/a.dat -B ./resources/b.dat -m $(SIZE) -n $(SIZE) -k $(SIZE) --method=cannon -v 

make srun:
	mpirun -np 1 ./build/test.o -A ./resources/a.dat -B ./resources/b.dat -m $(SIZE) -n $(SIZE) -k $(SIZE) --method=sequential -v 

data:
	bash ./src/generate.sh > ./resources/a.dat; bash ./src/generate.sh > ./resources/b.dat

run:
	clear; printf "Generowanie próbnych danych. Proszę poczekać...\n"; make data; printf "Trwa kompilowanie. Proszę poczekać...\n"; $(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT); clear; $(OUT)

clean:
	$(RM) ./build/*.o ./resources/*.dat ./build/*.out ./src/*.swp
