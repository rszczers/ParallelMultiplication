CC=mpiicc
LIBS=-mkl
SRC=test.c
OUT=test.o

test:
	$(CC) $(SRC) $(LIBS) -o $(OUT)

clean:
	$(RM) *.o

