#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
//#include <math.h>
#include <ncurses.h>

#define DEBUG 0
#define NCURS 1
#define MAX_SHIPS 7

#define HIT 1
#define WATER 0
#define MISS -1

/*
 * Ship properties.
 */
typedef struct watercraft {
    short id;
    char symbol;
    char *size;
    char *name;
} WaterCraft;

typedef struct coordinate {
    int row;
    int col;
} Coordinate;

typedef struct cell {
    int shipid;
    int symbol;
} Cell;

void board_clear(Cell **game_board, int game_range);
void board_print(Cell **game_board, Cell **game_board2, int game_range, bool show_all);
void board_printn(Cell **game_board, Cell **game_board2, int game_range, bool show_all);
void board_diag(Cell **game_board, Cell **gameBoard2, WaterCraft *ship_type, int game_range, int player_total, int *ship_mode, int ship_total);
void board_rand(Cell **game_board, WaterCraft *ship_type, int game_range, int *ship_mode, int ship_total, int mode);
void board_fill(Cell **game_board, WaterCraft *ship_type, Coordinate position, int ship_mode, int direction, int index);

#endif
