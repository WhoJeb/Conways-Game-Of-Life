#include <stdio.h>
#include <unistd.h>

const int ROWS = 45;
const int COLS = 60;
const int DECAY = 3;

// New constants for cell states
#define ALIVE 1
#define DEAD 0
#define DYING -1

struct voxel {
  int life;
};

struct cell {
  int life;
  int state;  // ALIVE, DEAD, DYING
  int decay_timer;  // Number of steps before it becomes DEAD
};

void RefreshScreen() {
  printf("\033[2J"); // Clears the entire screen
  printf("\033[H");  // Moves the cursor to the top-left corner (0,0)
}

// Helper function to count live or dying neighbors of a cell
int count_neighbors(struct cell arr[ROWS][COLS], int rows, int cols, int x, int y) {
  int neighbors = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int nx = x + i;
      int ny = y + j;

      if ((i == 0 && j == 0) || nx < 0 || nx >= rows || ny < 0 || ny >= cols)
        continue;

      if (arr[nx][ny].state == ALIVE || arr[nx][ny].state == DYING) {
        neighbors++;
      }
    }
  }
  return neighbors;
}

// Initialize the grid to a starting state
void make_arr(struct cell arr[ROWS][COLS], int rows, int cols, struct voxel v) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      arr[i][j].state = DEAD;  // Default all cells to dead
      arr[i][j].decay_timer = 0;  // No decay for dead cells
    }
  }

  // Example: Initializing a few cells as alive and others as dying
  arr[11][11].state = ALIVE;
  arr[11][12].state = ALIVE;
  arr[12][11].state = ALIVE;
  arr[12][12].state = ALIVE;
  arr[12][13].state = ALIVE;
  arr[13][13].state = ALIVE;
  arr[13][12].state = ALIVE;
  arr[14][14].state = ALIVE;
  arr[15][14].state = ALIVE;

  // Adding a partially dead (dying) cell
  arr[3][3].state = DYING;
  arr[3][3].decay_timer = 2;  // This cell will take 2 steps to fully die
}

// Print the grid to the screen
void print_arr(struct cell arr[ROWS][COLS], int rows, int cols) {
  RefreshScreen();
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (arr[i][j].state == ALIVE) {
        printf("O ");  // Alive cell
      } else if (arr[i][j].state == DYING) {
        printf("X ");  // Dying cell
      } else {
        printf(". ");  // Dead cell
      }
    }
    printf("\n");
  }
  sleep(1);  // Slow down the simulation for visibility
}

// Update the grid based on Conway's Game of Life rules
void play(struct cell current[ROWS][COLS], struct cell next[ROWS][COLS], int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int neighbors = count_neighbors(current, rows, cols, i, j);

      if (current[i][j].state == ALIVE) {  // Alive cell
        if (neighbors < 2 || neighbors > 3) {
          next[i][j].state = DYING;  // Cell dies and starts dying
          next[i][j].decay_timer = DECAY;  // Start the decay timer
        } else {
          next[i][j].state = ALIVE;  // Cell survives
          next[i][j].decay_timer = 0;  // Reset decay timer
        }
      } else if (current[i][j].state == DYING) {  // Dying cell
        // Dying cells eventually become dead after 1 cycle
        if (current[i][j].decay_timer == 0) {
          next[i][j].state = DEAD;
          next[i][j].decay_timer = 0;  // Reset decay timer
        } else {
          next[i][j].state = DYING;
          next[i][j].decay_timer = current[i][j].decay_timer - 1;
        }
      } else {  // Dead cell
        if (neighbors == 3) {
          next[i][j].state = ALIVE;  // Cell becomes alive (reproduction)
          next[i][j].decay_timer = 0;
        } else {
          next[i][j].state = DEAD;  // Cell stays dead
          next[i][j].decay_timer = 0;
        }
      }
    }
  }
}

int main() {
  struct cell current[ROWS][COLS], next[ROWS][COLS];
  struct voxel vox;
  vox.life = ALIVE;  // Alive cells have value 1

  make_arr(current, ROWS, COLS, vox);  // Initialize the grid

  while (1) {
    print_arr(current, ROWS, COLS);  // Display the current state
    play(current, next, ROWS, COLS);  // Calculate the next state

    // Copy next state into current state
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        current[i][j] = next[i][j];
      }
    }
  }

  return 0;
}
