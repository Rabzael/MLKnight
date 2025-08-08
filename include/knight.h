#ifndef KNIGHT_H
#define KNIGHT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_ACTIONS  8

#define BASE_WEIGHT 0.5
#define STRAIGHT_LINES_BONUS_WEIGHT 0.2
#define BEST_ACTION_BONUS_WEIGHT 0.3

#define MOVE_UP    0
#define MOVE_DOWN  1
#define MOVE_LEFT  2
#define MOVE_RIGHT 3
#define MOVE_UP_R  4
#define MOVE_UP_L  5
#define MOVE_DOWN_R 6
#define MOVE_DOWN_L 7

/* A knight */
typedef struct {
  int x,y;    // position
  int coins;  // coins collected
  int moves;  // moves made
} Knight;

/* A single memory of the knight
  * The state represents the surroundings of the knight, starting from the top-left corner.
  * The action is an integer representing the action taken by the knight.
  * The reward is an integer representing the result of the action: negative, neutral (0) or positive.
  */
typedef struct Experience {
  int state[8];     // surroundings of the knight, starting from the top-left corner
  unsigned int action;       // action taken by the knight
  int  reward;       // result
  struct Experience* next_state;   // next experience
} Experience;

// Save an experience in memory, if it's not already there
void add_experience(int* surroundings, unsigned int action, int reward, Experience* memory);

/* Get the next action of the knight
  * Look into memory to find the best action for the state
  * If found, use the reward associated with the action to decide
  * Pick a random action if the state is not in memory
  */
int get_action(int* surroundings, Experience* memory, int prev_action, short verbose);

/* Compares two states, returns 0 if are equals, -1 if not*/
int cmp_state(int * n1, int * n2);

#endif // KNIGHT_H