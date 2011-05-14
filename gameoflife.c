#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define ROWS 60
#define COLS 60
#define OFF 0
#define ON 1
#define BLACK 8, 0, 0, 0, 0
#define scr_width 600
#define scr_height 600
#define cell_width (scr_width / ROWS)
#define cell_height (scr_height / COLS)

/** Note to self:
  * Always refer to the board as board[x][y] as to follow the general
  * standard for specifying coordinates. When looping, y must be declared
  * first in the outer loop so that it represents the rows while x, declared
  * within the y loop becomes the variable representing each column value.
 */

/** Bugs
  * = Severity -> Low
  *   - When ROWS or COLS is greater than the other, only a squared area
  *     is actually updated or a segfault is fired.
 */

static char board[ROWS][COLS];
static char temp[ROWS][COLS];

void print_board(void);
int num_neighbours(int x, int y);
void update_board(void);
void load_glider(int x, int y);
int txt_main(void);
int gfx_main(void);

int main(int argc, char* argv[]) {
    int generator;
    
    srand(time(NULL));
    memset((void *)board, OFF, ROWS * COLS);
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            board[x][y] = OFF;
        }
    } 
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            generator = rand() % 5;
            if (generator == 0)
                board[x][y] = ON;
        }
    }
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            temp[x][y] = board[x][y];
        }
    }
    if (argc < 2)
        return gfx_main();
    if (strncmp(argv[1], "txt", 3) >= 0) 
        return txt_main();
    else if (strncmp(argv[1], "gfx", 3) >= 0) 
        return gfx_main();
    else
        return gfx_main();
    return 0;
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

int num_neighbours(int x, int y) {
    int num_adj = 0;
    int tmpy;
    int tmpx;

    /* Check vertical neighbours. */
    if (y-1 < 0)
        tmpy = ROWS - 1;
    else
        tmpy = y - 1;
    if (board[x][tmpy] == ON) num_adj++;
    if (y+1 >= ROWS)
        tmpy = 0;
    else
        tmpy = y + 1;
    if (board[x][tmpy] == ON) num_adj++;
    /*Check horizontal neighbours. */
    if (x-1 < 0)
        tmpx = COLS - 1;
    else
        tmpx = x - 1;
    if (board[tmpx][y] == ON) num_adj++;
    if (x+1 >= COLS)
        tmpx = 0;
    else
        tmpx = x + 1;
    if (board[tmpx][y] == ON) num_adj++;
    /* Check upper diagonal neighbours. */
    if (y-1 < 0)
        tmpy = ROWS - 1;
    else
        tmpy = y - 1;
    if (x-1 < 0)
        tmpx = COLS - 1;
    else
        tmpx = x - 1;
    if (board[tmpx][tmpy] == ON) num_adj++;
    if (x+1 >= COLS)
        tmpx = 0;
    else
        tmpx = x + 1;
    if (board[tmpx][tmpy] == ON) num_adj++;
    /* Check lower diagonal neighbours. */
    if (y+1 >= ROWS)
        tmpy = 0;
    else
        tmpy = y + 1;
    if (x+1 >= COLS)
        tmpx = 0;
    else
        tmpx = x + 1;
    if (board[tmpx][tmpy] == ON) num_adj++;
    if (x-1 < 0)
        tmpx = COLS - 1;
    else
        tmpx = x - 1;
    if (board[tmpx][tmpy] == ON) num_adj++;
    return num_adj;
}
void update_board(void) {
    int neighbours = 0;

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            neighbours = num_neighbours(x, y);
            if (neighbours < 2 && board[x][y] == ON) {
                temp[x][y] = OFF; /* Dies by underpopulation. */
            } else if (neighbours > 3 && board[x][y] == ON) {
                temp[x][y] = OFF; /* Dies by overpopulation. */
            } else if (neighbours == 3 && board[x][y] == OFF) {
                temp[x][y] = ON; /* Become alive because of reproduction. */
            }
            /* Otherwise the cell lives with just the right company. */
        }
    }
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            board[x][y] = temp[x][y];
        }
    }
}

int txt_main(void) {
    while (1) {
        print_board();
        update_board();
        printf("\n\n\n\n\n\n");
        SDL_Delay(250);
    }
    return EXIT_SUCCESS;
}

int gfx_main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Rect cells[ROWS][COLS]; /* Stores positions of each cell for blits. */
    SDL_Event event;
    int breaker = 0;
    SDL_Surface* screen = SDL_SetVideoMode(
                              scr_width, scr_height, 0, 
                              SDL_SWSURFACE | SDL_DOUBLEBUF);
    if (! screen) {
        perror("SDL_SetVideoMode");
        return EXIT_FAILURE;
    }
    Uint32 bgcolor = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
    if (SDL_FillRect(screen, &(screen->clip_rect), bgcolor) == -1) {
        perror("SDL_FillRect");
        return EXIT_FAILURE;
    }
    SDL_Flip(screen);
    SDL_Delay(5000);
    /* Calculate position of each cell rect for positioning on the screen. */
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            (cells[x][y]).x = cell_width * x;
            (cells[x][y]).y = cell_height * y;
        }
    }
    SDL_Surface* bcell = SDL_CreateRGBSurface(
                              SDL_SWSURFACE, cell_width, cell_height, BLACK);
    if (! bcell) {
        perror("BLACK SDL_CreateRGBSurface");
        return EXIT_FAILURE;
    }
    while (1) {
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                if (board[x][y] == ON) {
                    SDL_BlitSurface(
                        bcell, 
                        &(bcell->clip_rect), 
                        screen, 
                        &cells[x][y]);
                }
            }
        }
        SDL_Flip(screen);
        update_board();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: breaker = 1;
                default: continue;
            }
            if (breaker)
                break;
        }
        if (breaker)
            break;
        SDL_Delay(250);
        SDL_FillRect(screen, &(screen->clip_rect), bgcolor);
    }
    return EXIT_SUCCESS;
}
