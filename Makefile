

BIN = ./streamcap

SRC = streamcap.c

LDFLAGS = -lcurl

$(BIN): $(SRC)
	gcc -o $(BIN) -O3 $(SRC) $(LDFLAGS)
