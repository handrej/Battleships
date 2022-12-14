#include "battleship.h"

#ifndef BATTLESHIPS_HELPERS_H
#define BATTLESHIPS_HELPERS_H

/*
 * Holds player statistics.
 */
typedef struct stats {
    int hit;
    int shots;
    int miss;
    double ratio;
    int total;
    int won;
    int lost;
} Stats;

#define STATSFILE "stats.txt"
#define LBUFFER 32
/*
 * Order when parsing stats file.
 */
#define HITS 1
#define TOTAL 2
#define RATIO 3
#define PLAYED 4
#define WON 5
#define LOST 6

Coordinate getTarget(int game_range);
Coordinate genCoords(int direction, int game_range, int offset);
bool isvalid(Cell **gameBoard, Coordinate position, int game_range, int direction, int size, int index);
int checkShot(Cell **gameBoard, Coordinate target);
int inRange(int lower, int upper);
void parseStats(Stats gameStats[2]);
void writeStats(Stats gameStats[2]);

#endif //BATTLESHIPS_HELPERS_H
