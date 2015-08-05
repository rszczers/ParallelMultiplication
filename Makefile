PROJECT = pmm
CC = mpiicc
OPTIONS = -std=c99
LIBS = -mkl
SRC = ./src/main.c \
	  ./src/load_matrix.c \
	  ./src/save_matrix.c \
	  ./src/save_info.c
BUILD_PATH = ./build/$(PROJECT).o

SIZE = 512 
NPROC = 4

PATH_A = ./resources/a.dat
PATH_B = ./resources/b.dat
OUTPUT_SRUN = ./resources/c_seq.dat
OUTPUT_CRUN = ./resources/c_cannon.dat
OUTPUT_MRUN = ./resources/c_mkl.dat
DEBUG_DIR = ./debug/

.PHONY: clean

all:
	@$(CC) $(OPTIONS) $(SRC) $(LIBS) -o $(BUILD_PATH)

cannon:
	@mpirun -np $(NPROC) \
	$(BUILD_PATH) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_CRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=cannon \
	-q \
	-d$(DEBUG_DIR)

mkl:
	@mpirun -np 1 \
	$(BUILD_PATH) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_MRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=MKL \
	-q \
	-d$(DEBUG_DIR)

seq:
	@mpirun -np 1 \
	$(BUILD_PATH) \
	-A $(PATH_A) \
	-B $(PATH_B) \
	-C$(OUTPUT_SRUN) \
	-m $(SIZE) \
	-n $(SIZE) \
	-k $(SIZE) \
	--method=sequential \
	-q \
	-d$(DEBUG_DIR)

data:
ifneq ($(wildcard $(PATH_A) $(PATH_B)),)
	@$(RM) $(PATH_A) $(PATH_B)
endif
	@bash ./src/generate.sh $(SIZE) $(PATH_A); \
	bash ./src/generate.sh $(SIZE) $(PATH_B) 

run:
	clear
ifeq ($(wildcard $(PATH_A) $(PATH_B)),)		
	@make data
endif
ifeq ($(wildcard $(BUILD_PATH)),)
	@make all
endif
	@make cannon

clean:
ifneq ($(wildcard $(BUILD_PATH) $(DEBUG_DIR)*),)
	@$(RM) $(BUILD_PATH) $(PATH_A) $(PATH_B) $(DEBUG_DIR)*
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
