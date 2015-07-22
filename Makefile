PROJECT = test
CC = mpiicc
OPTIONS = -std=c99
LIBS = -mkl
SRC = ./src/main.c \
	  ./src/load_matrix.c \
	  ./src/save_matrix.c \
	  ./src/save_info.c
BUILD_PATH = ./build/test.o
OUT = ./build/test.o
SIZE = 16
NPROC = 4 
OUTPUT = ./resources/c.dat
DEBUG_DIR = ./debug/

test:
	$(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT)

make crun:
	mpirun -np $(NPROC) \
	$(BUILD_PATH) \
	-A ./resources/a.dat \
   	-B ./resources/b.dat \
	-C$(OUTPUT) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=cannon \
	-v \
   	-d$(DEBUG_DIR)

make mrun:
	mpirun -np $(NPROC) \
	$(BUILD_PATH) \
	-A ./resources/a.dat \
	-B ./resources/b.dat \
	-C$(OUTPUT) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=MKL \
	-v \
	-d$(DEBUG_DIR)

make srun:
	mpirun -np 1 \
	$(BUILD_PATH) \
	-A ./resources/a.dat \
	-B ./resources/b.dat \
	-C$(OUTPUT) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=sequential \
	-v \
	-d$(DEBUG_DIR)

data:
	bash ./src/generate.sh $(SIZE) > ./resources/a.dat; \
	bash ./src/generate.sh $(SIZE) > ./resources/b.dat

run:
	clear; printf "Generowanie próbnych danych. Proszę poczekać...\n"; make data; printf "Trwa kompilowanie. Proszę poczekać...\n"; $(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT); clear; $(OUT)

clean:
	$(RM) ./build/*.o ./resources/*.dat ./build/*.out ./src/*.swp
