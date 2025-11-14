BIN = drop4

all: $(BIN)

$(BIN): drop4.c
	gcc -Wall -Wextra -Werror -std=c23 -o $(BIN) drop4.c

clean:
	rm -f $(BIN)