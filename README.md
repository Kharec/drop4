# Drop4

A simple implementation of the game Puissance 4, written in C.

Why ? Because I got bored on friday night.

## Implementation details

The board is represented as a 2D array of integers.

The last played piece is stored in the `last_row` and `last_col` variables, allowing us to efficiently check for a win by examining lines extending from that position in 4 directions (horizontal, vertical, and both diagonals).

Game is a struct with the board, the current player, and the last played piece.

What else... I guess input handling is prettty robust, I check most of the possible edge cases.

## Rules

The game is played on a 6x7 grid.

The players take turns dropping their pieces into the grid.

The first player to get 4 pieces in a row (horizontally, vertically, or diagonally) wins.

## How to play

Just compile and run the program.

```bash
make
./drop4
```

## License

BSD 3-Clause License