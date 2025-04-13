/*
* @Author: karlosiric
* @Date:   2025-04-13 11:08:27
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-04-13 19:15:04
*/


/* main.c of the tetris game */

// first we need all of the includes for this tetris projects
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h> // for sleeping we will need this as well
#include <termios.h> // since I am on macOS I need this for terminal controlling
#include <fcntl.h>  
#include <errno.h>

// here I define some macros that I will be using in the game
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define EMPTY_CELL ' '
#define FILLED_CELL '#'

// first we need the function for setting up the terminal on mac
// since the terminal is only used for in the mode where a user types something and then presses the enter key
// termios.h allows us to use the terminal in a more fundamental way specifically designed for when making games like tetris
// of course it is possible to use ncurses as well instead of termios but in a modified version maybe later


/* input handling we have: - Canonical Input Mode -> line based (typically not ver very good, for text adventure games mostly)
 *                         - Non-canonical Input Mode -> more control over input, so better than canonical
 *                         - RAW Input mode -> this is the best one
 *                         
 *  Also important to turn off echo as well
 *  
 *  When it comes to Sound, well it is a terminal game so no sounds really to implement
 *  So using termios which allows us to disable the echo and to enable the non-canonical mode input
 *  Also using ANSII escape modes
*/


/************************** 
 * Function declarations: *
 **************************/

void setupTerminal();

void resetTerminal();

int keybit();

int getch();

void initBoard();












void setupTerminal() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); 
    /*
    // here we are performing a bit of bitwise operations for disabling CANONICAL mode and ECHO mode , while keeping others
    // so basically (ICANON and bitwise OR uses both of them and stores them into some bit number like they are both enabled..)
    // after that we do ~ flipping everything to on except these two we put them to 0
    // after that we do bitwise & AND which keeps other flags on and these to off
    // here we need to do bitwise OR because what that does is it binds these two flags into a single where both are enabled
    // if c_lflag for ICANNON in bit is 0000 1000 (8 decimal) and if ECHO is 0000 0001 (1 decimal) then doing this we get a new result both being enabled
    */

    tcsetattr(0, TCSANOW, &term); // this is setting to immediately to the terminal, 0 is the file descriptor 
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK); // this controls the file descriptor properties
}

void resetTerminal() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO; 
    // here a bit different syntax because we want to enable it all again so we do bitwise OR
    // if we have all flags are for example 1111 1111 all enabled, then here we do bitwise OR and we get all of them,
    // when using bitwise AND we only get those that we want, so those with 0 stay 0 or disabled... takes a bit of thinking to see how this works
    tcsetattr(0, TCSANOW, &term);
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK); // here we use & and bitwise NOT this turns off the blocking, now the terminal waits for the input again
}
/*

// we make a function for checking if a key is pressed in the terminal in the first place

*/

int keybit() {
    char ch;
    int bytes = read(0, &ch, 1); 
    /*
    // this is to read nbytes from the file descriptor so here reading from stdinput so fd is 0
    // so if it returns correctly it needs to return nbytes so if we press arrow keys that is one key each containing some value and that value is char so 1 byte 
    // if correct we return that, and if it is not read correctly it will return -1 and errno is called
    */
    if (bytes == 1) {
        return 1;
    } else {
        perror("Error reading correct number of bytes");
        return EXIT_FAILURE;
    }
}

// function to get the pressed key 
int getch() {
    char ch;
    read(0, &ch, 1);
    return ch;
}

//this will be our game board
char board[BOARD_HEIGHT][BOARD_WIDTH]; 

// now we initialize that board and set it all to empty, we populat the board
void initBoard() {
    for (int x = 0; x < BOARD_HEIGHT; x++) {
        for (int y = 0; y < BOARD_WIDTH; y++) {
            board[x][y] = EMPTY_CELL;
        }
    }
}

/* 
 * In this next step we need to make tetrominos themselves
 * For this we need to do the following, let's make an enum that will hold the names of the shapes
 */
typedef enum {
    I_SHAPE,
    O_SHAPE,
    T_SHAPE,
    J_SHAPE,
    L_SHAPE,
    S_SHAPE,
    Z_SHAPE,
    NUM_OF_SHAPES // to see how many shapes we will have get it
} e_TetrominoType; // e_ indicates it is an enum so we can remember better

typedef struct {
    int x, y;
    e_TetrominoType type;
    int rotation; // this goes from 0 to 3, because we can have 0 deg which is neutral position, after that we have 90 deg, 180 deg, 270deg
} s_Tetromino;

// here 1 represents an area that is filled and 0 represents and area which is empty cell, empty space
const int TETROMINO_SHAPE[NUM_OF_SHAPES][4][4][4] = {
    // I_SHAPE now first this is at index 0
    {
        // default we can call it rotation 0
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // rotation 1
        {
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0}
        },
        // rotation 2
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0}
        },
        // rotation 3
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
        }
    },
    // now we have the next shape and so forth this is how we do it for 
    

    // O SHAPE
    {   
        // rotation 0, default rotation
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // rotation 1
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // rotation 2 
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // rotation 3
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }
    },


    // T Shape
    {
        // rotation 0 default
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // rotation 1
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0}
        },
        // rotation 2
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0}
        },
        // rotation 3
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0}
        }
    },

    // adding shape J
    {
        // rotation 0 default rotation
        {
            {0, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },    
        // rotation 1 
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
        },
        // rotation 2
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 0}
        },
        // rotation 3
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0}
        }
    },

    // L shape
    {
        // rotation 0, default
        {
            {0, 0, 0, 0},
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // rotation 1
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0}
        },
        // rotation 2
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {1, 0, 0, 0}
        },
        // rotation 3
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
        }
    },

    // S shape
    {
        // rotation 0 default one
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // rotation 1
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0}
        },
        // rotation 2
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0}
        },
    // Rotation 3
        {
            {0, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0}
        }
    },

    // Z Shape
    {
            // Rotation 0
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 0, 0},
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0}
        },
        // Rotation 3
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {1, 0, 0, 0}
        }
    }
};

// lets now make a function where we actually make a new tetromino
void createTetronino(s_Tetromino *tetromino) {
    tetromino->type = rand() % NUM_OF_SHAPES; // here we decide which type it is going to be, something randomlly of course
    tetromino->rotation = 0; // initially it is set to 0
    tetromino->x = BOARD_WIDTH / 2 - 2; // this is needed in order to center the tetromino whichever we will be using.
    tetromino->y = 0; // we start at the very top of the board
}

// we also need a function which checks whether this tetromino is in the right position
bool is_in_valid_position(s_Tetromino *tetromino) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            // we make a tetromino and we check if this one exists
            if (TETROMINO_SHAPE[tetromino->type][tetromino->rotation][tetromino->x][tetromino->y]) { // if it is a valid one, type and rotation will be passed when we create the piece
                int boardX = tetromino->x + x;
                int boardY = tetromino->y + y;
                // we check if it is outside of the board and so forth...
                if (boardX < 0 || boardX >= BOARD_WIDTH || boardY < 0 || boardY >= BOARD_HEIGHT) {
                    return false;
                }
                // now we check if it is overlapping with some existing blocks on the board already
                if (board[boardX][boardY] != EMPTY_CELL) {
                    return false; // position is invalid, we have a collision
                }

            }
        }
    }
    // if we got to this part so far, that means 
    return true;
}

// now we need to implement the movement of the tetromino's
bool moveTetrominoDown(s_Tetromino *tetromino) {
    tetromino->y++;
    if (!is_in_valid_position(tetromino)) {
        tetromino->y--;
        return false;
    }

    return true; // here we are done, we can move it
}


bool moveTetrominoLeft(s_Tetromino *tetromino) {
    tetromino->x--; 
    if (!is_in_valid_position(tetromino)) {
        tetromino->x++; // if we can't go left anymore
        return false;
    }

    return true;
}

bool moveTetrominoRight(s_Tetromino *tetromino) {
    tetromino->x++; // moving right so positive
    if (!is_in_valid_position(tetromino)) {
        tetromino->x--;
        return false;
    }

    return true;
}

// now we need to do the tetromino rotations
bool rotateTetromino(s_Tetromino *tetromino) {
    
}


























int main(void) {


    return 0;
}
