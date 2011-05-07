#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL/SDL.h>

#define ROWS 50
#define COLS 60
#define OFF 0
#define ON 1

/** Note to self:
  * Always refer to the board as board[x][y] as to follow the general
  * standard for specifying coordinates. When looping, y must be declared
  * first in the outer loop so that it represents the rows while x, declared
  * within the y loop becomes the variable representing each column value.
 */

static char board[ROWS][COLS];

void print_board(void);
short num_neighbours(short x, short y);
void update_board(void);

int main(int argc, char* argv[]) {
    short num_adj;
    short generator;

    srand(time(NULL));
    memset((void *)board, OFF, ROWS * COLS);
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            board[x][y] = OFF;
        }
    }
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            generator = (short)(rand() % 3);
            if (generator == 0)
                board[x][y] = ON;
        }
    }
    print_board();
    for (int i = 0; i < 15; i++) {
        SDL_Delay(2500);
        update_board();
        print_board();
        printf("\n\n");
    }
    return EXIT_SUCCESS;
}

void print_board(void) {
    printf("|");
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (board[x][y] == ON)
                printf("X");
            else
                printf(" ");
            printf("|");
        }
        if (y < (ROWS - 1)) printf("\n|");
    }
    printf("\n");
}

short num_neighbours(short x, short y) {
    short num_adj = 0;

    if (y-1 >= 0) 
        if (board[x][y-1] == ON) num_adj++;
    if (y+1 < ROWS)
        if (board[x][y+1] == ON) num_adj++;
    if (x-1 >= 0)
        if (board[x-1][y] == ON) num_adj++;
    if (x+1 < COLS)
        if (board[x+1][y] == ON) num_adj++;
    return num_adj;
}

void update_board(void) { 
    short neighbours = 0;

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            neighbours = num_neighbours(x, y);
            if (neighbours < 2 && board[x][y] == ON) {
                board[x][y] = OFF; /* Dies by underpopulation. */
            } else if (neighbours > 3 && board[x][y] == ON) {
                board[x][y] = OFF; /* Dies by overpopulation. */
            } else if (neighbours == 3 && board[x][y] == OFF) {
                board[x][y] = ON; /* Becomes alive because of reproduction. */
            }
            /* Otherwise the cell lives with just the right company. */
        }
    }
}
