SIZE = 4096 #size of randomly generated test square matrix
NPROC = 144 #number of MPI threads
OMP_THREADS = 1
###############################################################################

PROJECT = pmm
SOURCE_DIR = ./src/
DEBUG_DIR = ./debug/
RESOURCES_DIR = ./resources/
BUILD_DIR = ./build/

# INPUT DATA
PATH_A = $(RESOURCES_DIR)a.dat
PATH_B = $(RESOURCES_DIR)b.dat
#lower and upper bounds of randomly generated test data
MIN = 0
MAX = 10
PROBLEM = $(shell echo $(SIZE)\*$(SIZE) | bc)

# DATA OUTPUT
#OUTPUT_SRUN = $(RESOURCES_DIR)c_seq.dat
#OUTPUT_CRUN = $(RESOURCES_DIR)c_cannon.dat
#OUTPUT_MRUN = $(RESOURCES_DIR)c_mkl.dat
#OUTPUT_OMP = $(RESOURCES_DIR)c_omp.dat
#OUTPUT_CANNON_OMP = $(RESOURCES_DIR)c_cannon_omp.dat
#OUTPUT_CANNON_MKL = $(RESOURCES_DIR)c_cannon_mkl.dat

# PMM
CC = mpiicc
CFLAGS = -std=c99
LIBS = -mkl -openmp
SRC_PMM = $(SOURCE_DIR)main.c \
	$(SOURCE_DIR)load_matrix.c \
	$(SOURCE_DIR)save_matrix.c \
	$(SOURCE_DIR)save_info.c
BUILD_PATH_PMM = $(BUILD_DIR)$(PROJECT)

# GEN
OBJ1 = gen
GCC = gcc
SRC_GEN = $(SOURCE_DIR)generate.c
BUILD_PATH_GEN = $(BUILD_DIR)gen

MPI_OPT = -env I_MPI_DEVICE sock 

.PHONY: clean

all: pmm gen

pmm:
	@$(CC) $(CFLAGS) $(SRC_PMM) $(LIBS) -o $(BUILD_PATH_PMM)

gen:
	@$(GCC) $(CFLAGS) $(SRC_GEN) -o $(BUILD_PATH_GEN)

rebuild_dirtree:
ifeq ($(wildcard $(DEBUG_DIR)),)
	mkdir $(DEBUG_DIR)
endif
ifeq ($(wildcard $(RESOURCES_DIR)),)
	mkdir $(RESOURCES_DIR)
endif
ifeq ($(wildcard $(BUILD_DIR)),)
	mkdir $(BUILD_DIR)
endif

#runs cannon's algorithm
cannon:
	@mpirun \
	$(MPI_OPT) \
	-np $(NPROC) \
	$(BUILD_PATH_PMM) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_CRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=cannon \
	-q \
	-d$(DEBUG_DIR) \
	--omp_threads=$(OMP_THREADS)

#runs cannon's algorithm
cannon_dgemm:
	@mpirun \
	$(MPI_OPT) \
	-np $(NPROC) \
	$(BUILD_PATH_PMM) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_CANNON_MKL) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=cannon_dgemm \
	-q \
	-d$(DEBUG_DIR) \
	--omp_threads=$(OMP_THREADS)

cannon_omp:
	@mpirun \
	$(MPI_OPT) \
	-np $(NPROC) \
	$(BUILD_PATH_PMM) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_CANNON_OMP) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=cannon_omp \
	-q \
	-d$(DEBUG_DIR) \
	--omp_threads=$(OMP_THREADS)

#runs mkl multiplication procedure
mkl:
	@mpirun \
	$(MPI_OPT) \
	-np 1 \
	$(BUILD_PATH_PMM) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_MRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=MKL \
	-q \
	-d$(DEBUG_DIR) \
	--omp_threads=$(OMP_THREADS)

#runs simple sequential algorithm 
seq:
	@mpirun \
	$(MPI_OPT) \
	-np 1 \
	$(BUILD_PATH_PMM) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_SRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=sequential \
	-q \
	-d$(DEBUG_DIR) \
	--omp_threads=$(OMP_THREADS)

#runs simple sequential algorithm 
omp:
	@mpirun \
	$(MPI_OPT) \
	-np 1 \
	$(BUILD_PATH_PMM) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_OMP) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=omp \
	-q \
	-d$(DEBUG_DIR) \
	--omp_threads=$(OMP_THREADS)

help:
	@mpirun \
	$(MPI_OPT) \
	$(BUILD_PATH_PMM) \
	--help

#generates test data
data:
ifeq ($(wildcard $(BUILD_PATH_PMM) $(BUILD_PATH_GEN)),)
	@make all
endif
ifneq ($(wildcard $(PATH_A) $(PATH_B)),)
	@$(RM) $(PATH_A) $(PATH_B)
endif
	@$(BUILD_PATH_GEN) -l $(PROBLEM) -m $(MIN) -M $(MAX) -p$(PATH_A) 
	@sleep 1 #time() changes every 1s
	@$(BUILD_PATH_GEN) -l $(PROBLEM) -m $(MIN) -M $(MAX) -p$(PATH_B)

#rebuilds directory tree
run: rebuild_dirtree
	clear
ifeq ($(wildcard $(BUILD_PATH_GEN)),)
	@make gen 
endif
ifeq ($(wildcard $(PATH_A) $(PATH_B)),)		
	@make data
endif
ifeq ($(wildcard $(BUILD_PATH_PMM)),)
	@make pmm 
endif
	./makeplots.sh $(NPROC)

test:
	./makeplots.sh $(NPROC)

clean:
ifneq ($(wildcard $(BUILD_PATH_PMM)),)
	@$(RM) $(BUILD_PATH_PMM)
endif
ifneq ($(wildcard $(BUILD_PATH_GEN)),)
	@$(RM) $(BUILD_PATH_GEN)
endif
ifneq ($(wildcard $(DEBUG_DIR)*),)
	@$(RM) $(DEBUG_DIR)*
endif
ifneq ($(wildcard $(PATH_A) $(PATH_B)),)
	@$(RM) $(PATH_A) $(PATH_B)
endif
ifneq ($(wildcard $(OUTPUT_SRUN)),)
	@$(RM) $(OUTPUT_SRUN)
endif
ifneq ($(wildcard $(OUTPUT_CRUN)),)
	@$(RM) $(OUTPUT_CRUN)
endif
ifneq ($(wildcard $(OUTPUT_MRUN)),)
	@$(RM) $(OUTPUT_MRUN)
endif
