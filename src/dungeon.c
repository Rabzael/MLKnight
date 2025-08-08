#include "dungeon.h"

void init_random_board(Dungeon_t * board) {
  // Initialize the board with walls
  for (int i = 0; i < DUNGEON_SIZE; i++) {
      board->board[i][0] = WALL;
      board->board[i][DUNGEON_SIZE - 1] = WALL;
      board->board[0][i] = WALL;
      board->board[DUNGEON_SIZE - 1][i] = WALL;
  }
  
  // Clean the rest of the board
  int free_cells = 0;
  for (int i = 1; i < DUNGEON_SIZE - 1; i++) {
    for (int j = 1; j < DUNGEON_SIZE - 1; j++) {
      board->board[i][j] = EMPTY;
      free_cells++;
    }
  }

  // Place the internal walls
  unsigned int n_internal_walls = free_cells > 0 ? (rand() % free_cells) / 4 : 0;
  while (n_internal_walls > 0) {
    Position p = random_empty_position(board);
    if (p.x==-1 && p.y==-1) {
      printf("Could not place all internal walls\n");
      break;
    }
    board->board[p.x][p.y] = WALL;
    n_internal_walls--;
    free_cells--;
  }
  
  // Place the coins
  unsigned int n_coins = free_cells > 0 ? (rand() % free_cells) / 6 : 0;
  n_coins = n_coins < 1 && free_cells > 0 ? 1 : n_coins; // Ensure at least 1 coin
  unsigned int placed_coins = 0;
  while (n_coins > 0) {
    Position p = random_empty_position(board);
    if (p.x==-1 && p.y==-1) {
      printf("Could not place all coins\n");
      break;
    }
    board->board[p.x][p.y] = COIN;
    n_coins--;
    placed_coins++;
  }
  board->coins = placed_coins;
}

Position random_empty_position(Dungeon_t * dungeon) {
  unsigned long max_attempts = DUNGEON_SIZE * DUNGEON_SIZE * 2;
  Position p;
  do {
    max_attempts--;
    p.x = rand() % DUNGEON_SIZE;
    p.y = rand() % DUNGEON_SIZE;
  } while (dungeon->board[p.x][p.y] != EMPTY && max_attempts > 0);
  if (max_attempts == 0) {
    p.x = -1;
    p.y = -1;
  }
  return p;
}

void look_around(int x, int y, int* surroundings, Dungeon_t * dungeon) {
  surroundings[0] = x-1 < 0 || y-1 < 0 ? WALL : dungeon->board[x-1][y-1]; // up-left
  surroundings[1] = y-1 < 0 ? WALL : dungeon->board[x][y-1]; // up
  surroundings[2] = x+1 > DUNGEON_SIZE-1 || y-1 < 0 ? WALL : dungeon->board[x+1][y-1]; // up-right
  surroundings[3] = x+1 > DUNGEON_SIZE-1 ? WALL : dungeon->board[x+1][y]; // right
  surroundings[4] = x+1 > DUNGEON_SIZE-1 || y+1 > DUNGEON_SIZE-1 ? WALL : dungeon->board[x+1][y+1]; // down-right
  surroundings[5] = y+1 > DUNGEON_SIZE-1 ? WALL : dungeon->board[x][y+1]; // down
  surroundings[6] = x-1 < 0 || y+1 > DUNGEON_SIZE-1 ? WALL : dungeon->board[x-1][y+1]; // down-left
  surroundings[7] = x-1 < 0 ? WALL : dungeon->board[x-1][y]; // left
}

unsigned int count_coins(Dungeon_t * dungeon) {
  unsigned int count = 0;
  for (int i = 0; i < DUNGEON_SIZE; i++) {
    for (int j = 0; j < DUNGEON_SIZE; j++) {
      if (dungeon->board[i][j] == COIN) {
        count++;
      }
    }
  }
  return count;
}