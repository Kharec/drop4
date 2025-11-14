#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 6
#define COLS 7
#define EMPTY 0
#define P1 1
#define P2 2
#define WIN_LENGTH 4
#define NUM_DIRECTIONS 4

typedef struct {
  int board[ROWS][COLS];
  int current_player;
  int last_row;
  int last_col;
} Game;

void init_game(Game *game) {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      game->board[i][j] = EMPTY;
    }
  }
  game->current_player = P1;
  game->last_row = -1;
  game->last_col = -1;
}

void print_board(Game *game) {
  printf("\n");
  for (int j = 0; j < COLS; j++) {
    printf(" %d ", j);
  }
  printf("\n");
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      char symbol = game->board[i][j] == EMPTY ? '.'
                    : game->board[i][j] == P1  ? 'X'
                                               : 'O';
      printf(" %c ", symbol);
    }
    printf("\n");
  }
  printf("\n");
}

void switch_player(Game *game) {
  game->current_player = game->current_player == P1 ? P2 : P1;
}

int drop_piece(Game *game, int col) {
  for (int i = ROWS - 1; i >= 0; i--) {
    if (game->board[i][col] == EMPTY) {
      game->board[i][col] = game->current_player;
      game->last_row = i;
      game->last_col = col;
      return 1;
    }
  }
  return 0;
}

int is_valid_move(Game *game, int col) {
  return col >= 0 && col < COLS && game->board[0][col] == EMPTY;
}

int is_winning_move(Game *game) {
  if (game->last_row < 0 || game->last_col < 0) {
    return 0;
  }

  int row = game->last_row;
  int col = game->last_col;
  int player = game->board[row][col];
  const int directions[NUM_DIRECTIONS][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

  for (int dir = 0; dir < NUM_DIRECTIONS; dir++) {
    int count = 1;
    for (int offset = 1; offset < WIN_LENGTH; offset++) {
      int new_row = row + directions[dir][0] * offset;
      int new_col = col + directions[dir][1] * offset;
      if (new_row < 0 || new_row >= ROWS || new_col < 0 || new_col >= COLS)
        break;
      if (game->board[new_row][new_col] != player)
        break;
      count++;
    }
    if (count >= WIN_LENGTH)
      return 1;
    for (int offset = 1; offset < WIN_LENGTH; offset++) {
      int new_row = row - directions[dir][0] * offset;
      int new_col = col - directions[dir][1] * offset;
      if (new_row < 0 || new_row >= ROWS || new_col < 0 || new_col >= COLS)
        break;
      if (game->board[new_row][new_col] != player)
        break;
      count++;
      if (count >= WIN_LENGTH)
        return 1;
    }
  }
  return 0;
}

int is_board_full(Game *game) {
  for (int j = 0; j < COLS; j++) {
    if (game->board[0][j] == EMPTY)
      return 0;
  }
  return 1;
}

static void trim_whitespace(char *str) {
  size_t len = strlen(str);
  while (len > 0 && isspace((unsigned char)str[len - 1])) {
    str[len - 1] = '\0';
    len--;
  }
  size_t start = 0;
  while (str[start] != '\0' && isspace((unsigned char)str[start])) {
    start++;
  }
  if (start > 0) {
    len = strlen(str + start);
    memmove(str, str + start, len + 1);
  }
}

static int is_quit_command(const char *str) {
  char temp[64];
  strncpy(temp, str, sizeof(temp) - 1);
  temp[sizeof(temp) - 1] = '\0';
  trim_whitespace(temp);
  size_t len = strlen(temp);
  if (len == 0)
    return 0;

  if (len == 1 && (temp[0] == 'q' || temp[0] == 'Q'))
    return 1;
  if (len == 4) {
    if ((temp[0] == 'q' || temp[0] == 'Q') &&
        (temp[1] == 'u' || temp[1] == 'U') &&
        (temp[2] == 'i' || temp[2] == 'I') &&
        (temp[3] == 't' || temp[3] == 'T')) {
      return 1;
    }
  }
  return 0;
}

int get_column_input(Game *game) {
  int col;
  char buffer[64];
  while (1) {
    printf("Player %d, enter a column (0-%d) or 'q' to quit: ",
           game->current_player, COLS - 1);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      return -1;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] != '\n') {
      int c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      printf("Error: Input too long. Please enter a number between 0 and %d.\n",
             COLS - 1);
      continue;
    }

    if (is_quit_command(buffer)) {
      return -1;
    }

    char *endptr;
    errno = 0;
    long col_long = strtol(buffer, &endptr, 10);

    if (errno == ERANGE || col_long < INT_MIN || col_long > INT_MAX) {
      printf(
          "Error: Number too large. Please enter a number between 0 and %d.\n",
          COLS - 1);
      continue;
    }

    if (endptr == buffer) {
      printf("Error: Invalid input. Please enter a number between 0 and %d, or "
             "'q' to quit.\n",
             COLS - 1);
      continue;
    }

    while (*endptr != '\0' && isspace((unsigned char)*endptr)) {
      endptr++;
    }

    if (*endptr != '\0') {
      printf("Error: Invalid characters after number. Please enter only a "
             "number between 0 and %d.\n",
             COLS - 1);
      continue;
    }

    col = (int)col_long;

    if (col < 0 || col >= COLS) {
      printf("Error: Column %d is out of range. Please enter a number between "
             "0 and %d.\n",
             col, COLS - 1);
      continue;
    }

    if (!is_valid_move(game, col)) {
      printf("Error: Column %d is full. Please choose another column.\n", col);
      continue;
    }

    return col;
  }
}

int main() {
  Game game;
  init_game(&game);
  print_board(&game);

  while (1) {
    int col = get_column_input(&game);
    if (col < 0) {
      printf("\nGame interrupted.\n");
      break;
    }

    drop_piece(&game, col);
    print_board(&game);
    if (is_winning_move(&game)) {
      printf("Player %d wins!\n", game.current_player);
      break;
    }
    if (is_board_full(&game)) {
      printf("It's a draw! The board is full.\n");
      break;
    }
    switch_player(&game);
  }

  return 0;
}
