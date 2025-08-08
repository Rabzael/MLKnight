#include <time.h>

#include "../include/dungeon.h"
#include "../include/knight.h"

#define GAME_MAX_MOVES 5000
#define REWARD_WALL_HIT -10
#define REWARD_COIN_PICKED 10
#define REWARD_EMPTY 0
#define REWARD_EMPTY_WITH_NEAR_COIN -1

void init_knight(Knight* knight, Dungeon_t* dungeon) {
  Position p = random_empty_position(dungeon);
  knight->x = p.x;
  knight->y = p.y;
  knight->coins = 0;
  knight->moves = 0;
}

long memory_size(Experience * memory) {
  long size = 0;
  Experience * current = memory;
  while (current->next_state != NULL) {
    size++;
    current = current->next_state;
  }
  return size;
}

void print_memory(Experience * memory) {
  Experience * current = memory;
  while (current->next_state != NULL) {
    if (current->reward != 0) {
      printf("State: %d %d %d %d %d %d %d %d, Action: %d, Reward: %d\n",
        current->state[0],
        current->state[1],
        current->state[2],
        current->state[3],
        current->state[4],
        current->state[5],
        current->state[6],
        current->state[7],
        current->action,
        current->reward
      );
    }
    current = current->next_state;
  }
}

void print_board(Dungeon_t * board, Knight * knight) {
  for (int y = 0; y < DUNGEON_SIZE; y++) {
    for (int x = 0; x < DUNGEON_SIZE; x++) {
      if (knight != NULL && knight->x == x && knight->y == y) {
        printf("K");
        continue;
      }
      switch (board->board[x][y]) {
        case EMPTY:
          printf(" ");
          break;
        case WALL:
          printf("#");
          break;
        case COIN:
          printf("$");
          break;
      }
    }
    printf("\n");
  }
}

/**
 * Play the game with the knight
 * @param game_mode: 0 if the knight is in training mode, 1 if in interactive mode, 2 interactive verbose mode
 *                   (if the game_mode is 0, the knight will not print anything)
 *                   (if the game_mode is 1, the knight will print its actions and rewards
 *                   and proceeds only if the user presses Enter)
 *                   (if the game_mode is 2, like mode 1 but without waiting for user input)
 * @param history: the knight's experience memory
 * @param dungeon: the dungeon where the knight is playing
 * @param total_moves: pointer to store the total moves made by the knight
 * @param valid_moves: pointer to store the valid moves made by the knight
 */
void game (short game_mode, Experience * history, Dungeon_t * dungeon, unsigned int *total_moves, unsigned int *valid_moves) {
  int wait_user_input = 1;
  
  // Initialize the knight
  Knight knight;
  init_knight(&knight, dungeon);

  // Play the game
  unsigned int l_total_moves = 0;
  int last_action = -1;
  while (dungeon->coins > 0 && l_total_moves < GAME_MAX_MOVES) {
    l_total_moves += 1;

    // Look around
    int surroundings[8] = {0};
    look_around(knight.x, knight.y, surroundings, dungeon);
    
    // Get the action
    int action = get_action(surroundings, history, last_action, game_mode > 1);
    last_action = action;
    
    // Preview the knight's move
    int px = knight.x, py = knight.y;
    switch (action) {
      case MOVE_UP: // up
        py--;
        break;
      case MOVE_DOWN: // down
        py++;
        break;
      case MOVE_LEFT: // left
        px--;
        break;
      case MOVE_RIGHT: // right
        px++;
        break;
      case MOVE_UP_R: // up-right
        py--;
        px++;
        break;
      case MOVE_UP_L: // up-left
        py--;
        px--;
        break;
      case MOVE_DOWN_R: // down-right
        py++;
        px++;
        break;
      case MOVE_DOWN_L: // down-left
        py++;
        px--;
        break;
    }
    
    // Move's outcomes
    if (px < 0 || py < 0 || px >= DUNGEON_SIZE || py >= DUNGEON_SIZE) {
      printf("OUT of bounds!\n");
      exit(1);
    }
    int reward = REWARD_EMPTY;
    if (dungeon->board[px][py] == WALL) {
      // Hit a wall: don't move
      reward = REWARD_WALL_HIT;
    } else {
      // Move the knight
      knight.x = px;
      knight.y = py;
      knight.moves++;
      // Pick coin
      if (dungeon->board[knight.x][knight.y] == COIN) {
        knight.coins++;
        dungeon->coins--;
        dungeon->board[knight.x][knight.y] = EMPTY;
        reward = REWARD_COIN_PICKED;
      } 
      // Empty space with a coin near: slightly negative reward
      else if (
        dungeon->board[knight.x][knight.y] == EMPTY &&
        (surroundings[MOVE_UP] == COIN || surroundings[MOVE_DOWN] == COIN ||
         surroundings[MOVE_LEFT] == COIN || surroundings[MOVE_RIGHT] == COIN ||
         surroundings[MOVE_UP_R] == COIN || surroundings[MOVE_UP_L] == COIN ||
         surroundings[MOVE_DOWN_R] == COIN || surroundings[MOVE_DOWN_L] == COIN)
      ) {
        reward = REWARD_EMPTY_WITH_NEAR_COIN;
      }
    }

    // Update the memory if the reward is not 0
    if (game_mode == 0 && reward != 0)
      add_experience(surroundings, action, reward, history);

    // Interactive mode
    if (game_mode > 0 && wait_user_input) {
      print_board(dungeon, &knight);
      if (game_mode == 2) {
        printf("Action: %d, Reward: %d, Total moves: %d, Valid moves: %d\n", action, reward, l_total_moves, knight.moves);
      }
      printf("Press Enter to continue single-step, a to automatic, q to exit...");
      char in = getchar();
      if (in == 'q') break;
      if (in == 'a') wait_user_input = 0;
    }
  }

  if (valid_moves != NULL) *valid_moves = knight.moves;
  if (total_moves != NULL) *total_moves = l_total_moves;
}
 /**
  * Create a first set of memory / experiences on random generated boards
  */
void training(Experience * experience, long n_games) {
  // Play a number of games
  unsigned int g_total_moves = 0;
  unsigned int g_valid_moves = 0;
  float average_efficiency = 0.0;

  Dungeon_t * random_dungeon = (Dungeon_t*)malloc(sizeof(Dungeon_t));
  if (random_dungeon == NULL) {
    printf("Memory allocation failed!\n");
    return;
  }

  printf("Training... [ ]");
  char * progress = "|/-\\";
  fflush(stdout);
  for (int i = 0; i < n_games; i++) {
    if (i % 25 == 0) {
      printf("\b\b%c]", progress[i % 4]);
      fflush(stdout);
    }
    init_random_board(random_dungeon);
    game(0, experience, random_dungeon, &g_total_moves, &g_valid_moves);

    // Update the average efficiency
    float game_efficiency = ((float)g_valid_moves / g_total_moves) * 100;
    average_efficiency += (game_efficiency - average_efficiency) / (i + 1);
  }
  free(random_dungeon);

  printf("\nTraining complete:\n\t- played %ld games on random boards\n\t- memory size (positions saved): %ld\n\t- average efficiency: %f%%\n"
    , n_games
    , memory_size(experience)
    , average_efficiency
  );
  fflush(stdout);
}

int main(int argc, char *argv[]) {

  /** INITIALIZATION 
   * Set arguments and initialize values
   */
  if (argc < 2) {
    printf("Usage: %s [-v] <number of training games>\n", argv[0]);
    return 1;
  }

  // Check for verbose mode
  int verbose_mode = 0;
  if (strcmp(argv[1], "-v") == 0) {
    verbose_mode = 1;
    printf("Verbose mode enabled.\n");
    if (argc < 3) {
      printf("Usage: %s [-v] <number of training games>\n", argv[0]);
      return 1;
    }
    argv++; // Skip the verbose flag
  }

  long training_games;  // Number of games to play for training
  sscanf(argv[1], "%ld", &training_games);
  srand(time(NULL));

  // Initialize the memory
  Experience * memory = (Experience*)malloc(sizeof(Experience));
  if (memory == NULL) {
    printf("Memory allocation failed!\n");
    return 1;
  }
  for (int j=0; j<8; j++) memory->state[j] = 0;
  memory->next_state = NULL;

  /** TRAINING */
  training(memory, training_games);

  /** PLAY */
  printf("\nGenerating a board...\n");
  Dungeon_t * user_dungeon = (Dungeon_t*)malloc(sizeof(Dungeon_t));
  if (user_dungeon == NULL) {
    printf("Memory allocation failed!\n");
    return 1;
  }
  init_random_board(user_dungeon);
  unsigned int total_moves = 0;
  unsigned int valid_moves = 0;
  game(1+verbose_mode, memory, user_dungeon, &total_moves, &valid_moves);
  printf("\nGame over!\n\t- total moves: %d\n\t- valid moves: %d\n\t- coins left: %d\n\t- moves efficiency: %f%%\n"
    , total_moves
    , valid_moves
    , count_coins(user_dungeon)
    , ((float)valid_moves / total_moves) * 100
  );
  free(user_dungeon);

  /** CLOSING */
  // Free the memory
  Experience * current = memory;
  if (memory->next_state == NULL) {
    free(memory);
  } else while (current->next_state != NULL) {
    Experience * next = current->next_state;
    free(current);   
    current = next; 
  }

  return 0;
}