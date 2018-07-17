MAIN=ffblock
BIN=ffb

all: exe

exe:
	gcc -o $(BIN) $(MAIN).c

clean:
	rm -f $(BIN)

run:
	sudo ./$(BIN)

re: clean exe run
