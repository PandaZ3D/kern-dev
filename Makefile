MAIN=ffblock
BIN=ffb

all: exe

exe:
	gcc -o $(BIN) $(MAIN).c

clean:
	rm -f $(BIN)

re: clean exe
