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
SIZE = 1024 
NPROC = 4
OUTPUT_SRUN = ./resources/c_srun.dat
OUTPUT_CRUN = ./resources/c_crun.dat
OUTPUT_MRUN = ./resources/c_mrun.dat
DEBUG_DIR = ./debug/

test:
	$(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT)

make crun:
	mpirun -np $(NPROC) \
	$(BUILD_PATH) \
	-A ./resources/a.dat \
	-B ./resources/b.dat \
	-C$(OUTPUT_CRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=cannon \
	-q \
	-d$(DEBUG_DIR)

make mrun:
	mpirun -np 1 \
	$(BUILD_PATH) \
	-A ./resources/a.dat \
	-B ./resources/b.dat \
	-C$(OUTPUT_MRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=MKL \
	-q \
	-d$(DEBUG_DIR)

make srun:
	mpirun -np 1 \
	$(BUILD_PATH) \
	-A ./resources/a.dat \
	-B ./resources/b.dat \
	-C$(OUTPUT_SRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=sequential \
	-q \
	-d$(DEBUG_DIR)

data:
	rm ./resources/a.dat; \
	rm ./resources/b.dat; \
	rm ./resources/c.dat; \
	bash ./src/generate.sh $(SIZE) ./resources/a.dat; \
	bash ./src/generate.sh $(SIZE) ./resources/b.dat

run:
	clear; printf "Generowanie próbnych danych. Proszę poczekać...\n"; make data; printf "Trwa kompilowanie. Proszę poczekać...\n"; $(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(OUT); clear; $(OUT)

clean:
	$(RM) ./build/*.o ./resources/c* ./build/*.out ./src/*.swp ./debug/*
