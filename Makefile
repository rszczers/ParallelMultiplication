CC=gcc
LIBS=-std=c99
SRC=counter.c main.c
OUT=test.o

test:
	$(CC) $(SRC) $(LIBS) -o $(OUT)

data:
	bash generate.sh > a.dat; bash generate.sh > b.dat

clean:
	$(RM) *.o *.dat *.out *.swp

