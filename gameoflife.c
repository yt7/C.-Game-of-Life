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

typedef struct {
    int x;
    int y;
} point;

char board[ROWS][COLS];
char temp[ROWS][COLS];
SDL_Rect cells[ROWS][COLS]; /* Stores positions of each cell for blits. */

void randomize_board(void);
void blit_board(SDL_Surface* bcell, SDL_Surface* screen);
void print_board(void);
int num_neighbours(int x, int y);
void update_board(void);
void load_glider(int x, int y);
int txt_main(void);
//void loop_callback(void (*callback)(point));
//void randomize_cell(point p);
int gfx_main(void);

int main(int argc, char* argv[]) {
    //int generator;
    
    memset((void *)board, OFF, ROWS * COLS);
    //loop_callback(randomize_cell);
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            temp[x][y] = board[x][y];
        }
    }
    if (argc >= 2 && strncmp(argv[1], "txt", 3) == 0)
        return txt_main();
    return gfx_main();
}

void randomize_board(void) {
    memset((void *)board, OFF, ROWS * COLS);
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (rand() % 5 == 0) {
                board[x][y] = ON;
            }
        }
    }
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

void blit_board(SDL_Surface* bcell, SDL_Surface* screen) {
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
    SDL_WM_SetCaption("Conway's Game of Life - Ryuurei", NULL);
    SDL_Event event;
    int breaker = 0;
    int paused = 1;
    randomize_board();
    SDL_Surface* screen = SDL_SetVideoMode(
                              scr_width, scr_height + 40, 0, 
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
    /* Calculate position of each cell rect for positioning on the screen. */
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            (cells[x][y]).x = (cell_width * x);
            (cells[x][y]).y = (cell_height * y);
        }
    }
    SDL_Surface* bcell = SDL_CreateRGBSurface(
                              SDL_SWSURFACE, cell_width, cell_height, BLACK);
    if (! bcell) {
        perror("BLACK SDL_CreateRGBSurface");
        return EXIT_FAILURE;
    }
    SDL_Flip(screen);
    while (1) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: 
                    breaker = 1;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_KP_ENTER ||
                        event.key.keysym.sym == SDLK_RETURN) {
                        paused = !paused;
                    } else if (event.key.keysym.sym == SDLK_SPACE) {
                        SDL_FillRect(screen, &(screen->clip_rect), bgcolor);
                        randomize_board();
                        SDL_Flip(screen);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    puts("Got mouse event!");
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        puts("SDL_BUTTON_LEFT found!");
                        int tempx = event.button.x;
                        int tempy = event.button.y;
                        printf("Mouse at pos (%d, %d)\n", tempx, tempy);
                        for (int y = 0; y < ROWS; y++) {
                            if (tempy > cells[0][y].y &&
                                tempy < cells[0][y].y + cell_height) {
                                tempy = y;
                                printf("Y pos is %d\n", tempy);
                            }
                        }
                        for (int x = 0; x < COLS; x++ ) {
                            if (tempx > cells[x][0].x &&
                                tempx < cells[x][0].x + cell_width) {
                                tempx = x;
                                printf("X pos is %d\n", tempx);
                            }
                        }
                        if (board[tempx][tempy] == OFF) {
                            board[tempx][tempy] = ON;
                            SDL_BlitSurface(
                                bcell,
                                &(bcell->clip_rect),
                                screen,
                                &cells[tempx][tempy]);
                        }
                    }
                    break;
                default: continue;
            }
            if (breaker)
                break;
        }
        if (breaker)
            break;
        blit_board(bcell, screen);
        SDL_Flip(screen);
        if (!paused) {
            update_board();
            SDL_Delay(250);
            SDL_FillRect(screen, &(screen->clip_rect), bgcolor);
        }
    }
    return EXIT_SUCCESS;
}
