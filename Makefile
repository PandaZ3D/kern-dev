MAIN=ffblock
BIN=ffb

all: clean exe

exe:
	gcc -o $(BIN) $(MAIN).c

clean:
	rm -f $(BIN)

run:
	sudo ./$(BIN)
