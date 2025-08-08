#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdlib.h>
#include <stdio.h>

#define DUNGEON_SIZE 20

// Define the symbols for the board
#define EMPTY 0
#define WALL  1
#define COIN  2

/* A position on the board */
typedef struct {
  unsigned int x,y;
} Position;

/* The board */
typedef struct {
  int board[DUNGEON_SIZE][DUNGEON_SIZE];
  unsigned int coins;
} Dungeon_t;

// Initialize the board with walls and coins
void init_random_board(Dungeon_t*);

// Return a random empty position on the board
Position random_empty_position(Dungeon_t*);

// Copy the surroundings of a position
void look_around(int x, int y, int* surroundings, Dungeon_t*);

// Count coins on the board
unsigned int count_coins(Dungeon_t*);

#endif // DUNGEON_H