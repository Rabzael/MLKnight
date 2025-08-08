#include "knight.h"

int cmp_state(int * n1, int * n2) {
  for (int k=0; k<8; k++) {
    if (*(n1+k) != *(n2+k)) return -1;
  }
  return 0;
}

void add_experience(int* surroundings, unsigned int action, int reward, Experience* memory) {
  // Check if the experience is already in memory
  Experience* current = memory;
  while (current != NULL) {
    if (cmp_state(current->state, surroundings) == 0 && current->action == action) {
      // The experience is already in memory, update the reward
      current->reward += reward;
      // Limit the reward to a range
      if (current->reward < -50) current->reward = -50;
      if (current->reward > 50) current->reward = 50;
      return;
    }
    current = current->next_state;
  }

  // Else create a new experience...
  Experience* new_experience = (Experience*)malloc(sizeof(Experience));
  if (new_experience == NULL) {
    printf("Memory allocation failed!\n");
    return;
  }
  for (int j=0; j<8; j++) new_experience->state[j] = surroundings[j];
  new_experience->action = action;
  new_experience->reward = reward;
  new_experience->next_state = NULL;
  
  // ...and add it to memory
  current = memory;
  while (current->next_state != NULL) {
    current = current->next_state;
  }
  current->next_state = new_experience;
}

int weighted_random_choice(float* weights) {
  float total_weight = 0.0;
  float max_weight = 0.0;

  for (int i = 0; i < N_ACTIONS; i++) {
      total_weight += weights[i];
      if (weights[i] > max_weight) max_weight = weights[i];
  }

  // Bonus for best actions
  if (max_weight > 0.0) {
    for (int i = 0; i < N_ACTIONS; i++) {
      if (weights[i] == max_weight) {
        weights[i] += BEST_ACTION_BONUS_WEIGHT;
      }
    }
  }

  // Fallback in case of zero or negative weights
  if (total_weight <= 0.0) return rand() % N_ACTIONS;

  float random_value = ((float)rand() / (float)RAND_MAX) * total_weight;
  float cumulative_weight = 0.0;

  for (int i = 0; i < N_ACTIONS; i++) {
      cumulative_weight += weights[i];
      if (random_value < cumulative_weight) {
          return i;
      }
  }

  return rand() % N_ACTIONS; // Fallback
}

int get_action(int* surroundings, Experience* memory, int previous_action, short verbose) {
  // Initialize the weights for each action
  float weights[N_ACTIONS];
  for (int kw = 0; kw < N_ACTIONS; kw++) {
    weights[kw] = BASE_WEIGHT;
  }
  
  // Look into memory to find the same states and adjust the weights
  Experience* current = memory;  
  while (current != NULL) {
    if (cmp_state(current->state, surroundings) == 0) {
        // the reward associated with the action modifies the probability
        weights[current->action] = (current->reward+50) / 100.0;
    }
    current = current->next_state;
  }

  // Increase the probability of moving in straight lines if the previus action was not bad rewarded
  if (previous_action != -1 && weights[previous_action] >= 0.5) {
    weights[previous_action] += STRAIGHT_LINES_BONUS_WEIGHT;
  }

  int action_taken = weighted_random_choice(weights);
  if (verbose == 1) { 
    float max_weight = 0.0;
    for (int i = 0; i < N_ACTIONS; i++) {
      if (weights[i] > max_weight) max_weight = weights[i];
    }
    if (weights[action_taken] < max_weight) printf("Action taken: NOT the best choice\n");
    else printf("Action taken: the best choice\n");
  }
  return action_taken;
}