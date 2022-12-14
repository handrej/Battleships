#include "helpers.h"

/**
 * @brief Generates Coordinates on the GameBoard.
 * 
 * Chooses coordinates in a range between 0 -> Board Dimension
 * 
 * @param direction Cardinal Direction
 * @param game_range Target Board Dimension
 * @param offset Ship Length
 * @return Coordinate 
 */
Coordinate genCoords(int direction, int game_range, int offset) {
  Coordinate position;
  int x = direction ? game_range : game_range - offset;
  int y = direction ? game_range - offset : game_range;

  position.row = inRange(0, x);
  position.col = inRange(0, y);

  return position;
}

/**
 * @brief Get the Target object by fetching user input.
 * 
 * Fetch input x,y coordinates and check out of bounds.
 * 
 * @param game_range Game Board Dimension
 * @return Coordinate 
 */
Coordinate getTarget(int game_range) {
  Coordinate target;
  target.row = 0;
  target.col = 0;
  printf("Please enter coordinates (1-%d,1-%d):\n", game_range, game_range);
  char tmp[10], *tmp_;

  do {
    fgets(tmp, sizeof(tmp), stdin);
    int i = 0;
    char *p = strtok(tmp, ",");
    while (p != NULL) {
      if (i == 0) {
        target.row = (int)strtol(p, &tmp_, 0);
      } else if (i == 1) {
        target.col = (int)strtol(p, &tmp_, 0);
      }
      p = strtok(NULL, ",");
      i++;
    }
  } while ((target.row <= 0 || target.row > game_range) ||
           (target.col <= 0 || target.col > game_range));
  // offset for user input
  target.row -= 1;
  target.col -= 1;

  return target;
}

/**
 * @brief Pick a number in a range of two numbers.
 * 
 * @param lower min
 * @param upper max
 * @return int 
 */
int inRange(int lower, int upper) {
  return (rand() % (upper - lower + 1)) + lower;
}

/**
 * @brief Returns hit type of targeted GameBoard field.
 * 
 * @param gameBoard Target Board
 * @param target Coordinates
 * @return int 
 */
int checkShot(Cell **gameBoard, Coordinate target) {
  int hit;
  switch (gameBoard[target.row][target.col].symbol) {
  case WATER:
    hit = -1;
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    hit = 1;
    break;
  case HIT:
  case MISS:
  default:
    hit = 0;
    break;
  }
  return hit;
}

/**
 * @brief Verify ship coordinates.
 * 
 * Checks if fields around given ship coordinates match ruleset. \n
 * 1. Cells have to be water or matching ship (id) from previous iteration. \n
 * 2. Cells around coordinates need to be water.\n
 * 
 * Depending on starting @c position and @c direction check every cell and next cell. \n
 * 
 * HORIZONTAL expands to the EAST. VERTICAL expands to the SOUTH.
 *
 * 
 * @param gameBoard Target Board
 * @param position Ship Coordinates
 * @param game_range Target Board Dimension
 * @param direction Cardinal Direction
 * @param size Ship Length
 * @param index Ship ID on the Target Board
 * @return true 
 * @return false 
 */
bool isvalid(Cell **gameBoard, Coordinate position, int game_range, int direction, int size, int index) {
  for (int i = 0; i < size; i++) {
    if (direction == 0) { /*HORIZONTAL*/
      int x = position.row;
      int y = position.col + i;

      if (y >= game_range)
        return false;
      // EAST LEADING
      if ((*(gameBoard + x) + y)->symbol != WATER &&
          (*(gameBoard + x) + y)->shipid != index)
        return false;
      if (y + 1 < game_range) {
        if ((*(gameBoard + x) + y + 1)->symbol != WATER &&
            (*(gameBoard + x) + y + 1)->shipid != index)
          return false;
      }
      // NORTH
      if (x - 1 >= 0) {
        if ((*(gameBoard + x - 1) + y)->symbol != WATER &&
            (*(gameBoard + x - 1) + y)->shipid != index)
          return false;
        if (y + 1 < game_range) {
          if ((*(gameBoard + x - 1) + y + 1)->symbol != WATER &&
              (*(gameBoard + x - 1) + y + 1)->shipid != index)
            return false;
        }
      }
      // SOUTH
      if (x + 1 < game_range) {
        if ((*(gameBoard + x + 1) + y)->symbol != WATER &&
            (*(gameBoard + x + 1) + y)->shipid != index)
          return false;
        if (y + 1 < game_range) {
          if ((*(gameBoard + x + 1) + y + 1)->symbol != WATER &&
              (*(gameBoard + x + 1) + y + 1)->shipid != index)
            return false;
        }
      }
      // WEST
      if (position.col - 1 >= 0 && x - 1 >= 0) {
        if ((*(gameBoard + x - 1) + position.col - 1)->symbol != WATER &&
            (*(gameBoard + x - 1) + position.col - 1)->shipid != index)
          return false;
      }
      if (position.col - 1 >= 0) {
        if ((*(gameBoard + x) + position.col - 1)->symbol != WATER &&
            (*(gameBoard + x) + position.col - 1)->shipid != index)
          return false;
      }
      if (position.col - 1 >= 0 && x + 1 < game_range) {
        if ((*(gameBoard + x + 1) + position.col - 1)->symbol != WATER &&
            (*(gameBoard + x + 1) + position.col - 1)->shipid != index)
          return false;
      }

    } else if (direction == 1) { /*VERTICAL*/
      int x = position.row + i;
      int y = position.col;

      if (x >= game_range)
        return false;
      // SOUTH LEADING
      if ((*(gameBoard + x) + y)->symbol != WATER &&
          (*(gameBoard + x) + y)->shipid != index)
        return false;
      if (x + 1 < game_range) {
        if ((*(gameBoard + x + 1) + y)->symbol != WATER &&
            (*(gameBoard + x + 1) + y)->shipid != index)
          return false;
      }
      // NORTH
      if (position.row - 1 >= 0 && y - 1 >= 0) {
        if ((*(gameBoard + position.row - 1) + y - 1)->symbol != WATER &&
            (*(gameBoard + position.row - 1) + y - 1)->shipid != index)
          return false;
      }
      if (position.row - 1 >= 0) {
        if ((*(gameBoard + position.row - 1) + y)->symbol != WATER &&
            (*(gameBoard + position.row - 1) + y)->shipid != index)
          return false;
        if (y + 1 < game_range) {
          if ((*(gameBoard + position.row - 1) + y + 1)->symbol != WATER &&
              (*(gameBoard + position.row - 1) + y + 1)->shipid != index)
            return false;
        }
      }
      // EAST
      if (y + 1 < game_range) {
        if ((*(gameBoard + x) + y + 1)->symbol != WATER &&
            (*(gameBoard + x) + y + 1)->shipid != index)
          return false;
        if (x + 1 < game_range) {
          if ((*(gameBoard + x + 1) + y + 1)->symbol != WATER &&
              (*(gameBoard + x + 1) + y + 1)->shipid != index)
            return false;
        }
      }
      // WEST
      if (y - 1 >= 0) { // check out of bounds
        if ((*(gameBoard + x) + y - 1)->symbol != WATER &&
            (*(gameBoard + x) + y - 1)->shipid != index)
          return false;
        if (x + 1 < game_range) {
          if ((*(gameBoard + x + 1) + y - 1)->symbol != WATER &&
              (*(gameBoard + x + 1) + y - 1)->shipid != index)
            return false;
        }
      }
    }
  }
  return true;
}

/**
 * @brief Write statistics for both players into file
 * 
 * Outputs player statistics into stats.txt file. Each row has an own identifier string with specific text value.
 * 
 * @param pstats_ Player stats
 */
void writeStats(Stats pstats_[2]) {
  FILE *fw = NULL;

  if ((fw = fopen(STATSFILE, "w")) == NULL) {
    printf("\n!Error while handling stats file!\n");
    exit(1);
  }

  char *identifier[8] = {
      "PLAYER", "HITS:", "TOTAL:", "RATIO:", "PLAYED:", "WON:", "LOST:"};

  int tmp_val = 0;
  float tmp_valf = 0.0;

  fprintf(fw, "%s\n", "STATS FILE BATTLESHIPS");
  for (int i = 0; i < 2; i++) {
    fprintf(fw, "\n%s\t\t%d:\n", "PLAYER", i + 1);
    for (int j = 1; j < 7; j++) {
      switch (j) {
      case HITS:
        fprintf(fw, "-TARGET\n");
        tmp_val = pstats_[i].hit;
        break;
      case TOTAL:
        tmp_val = pstats_[i].shots;
        break;
      case RATIO:
        tmp_valf = pstats_[i].ratio;
        break;
      case PLAYED:
        fprintf(fw, "-GAMES\n");
        tmp_val = pstats_[i].total;
        break;
      case WON:
        tmp_val = pstats_[i].won;
        break;
      case LOST:
        tmp_val = pstats_[i].lost;
        break;
      default:
        break;
      }
      if (j == RATIO) {
        // consider the first three digits for approximation
        fprintf(fw, "%s\t\t%.3g\n", identifier[j], tmp_valf);
        continue;
      }

      fprintf(fw, "%s\t\t%d\n", identifier[j], tmp_val);
    }
  }
  fflush(fw);
  fclose(fw);
}

/**
 * @brief Read existing stats filef
 * 
 * Opens read handler on existing stats file and parses the content by matching the identifiers to their line values and adding them to the stats of the current game.
 * Creates a new file if stats.txt fails to open.
 * 
 * @param pstats_ Player stats
 */
void parseStats(Stats pstats_[2]) {

  char *tmp_;

  FILE *fr = NULL;

  char stats_buff[LBUFFER];

  char *identifier[8] = {
      "PLAYER", "HITS:", "TOTAL:", "RATIO:", "PLAYED:", "WON:", "LOST:"};

  if ((fr = fopen(STATSFILE, "r")) == NULL) {
    printf("\nCreating new stats file (stats.txt).\n");
    writeStats(pstats_);
    exit(0);
  }

  int file_value = 0, player_atm = -1, toggle_stats = -1;
  float file_valuef = 0.0;

  while (fgets(stats_buff, LBUFFER, fr)) {
    char *x = strtok(stats_buff, "\t\t");
    while (x != NULL) {

      if (toggle_stats == 0) {
        player_atm += 1;
        toggle_stats = -1;
      }

      if (toggle_stats > 0) {
        if (toggle_stats == RATIO) {
          file_valuef = strtof(x, &tmp_);
        } else {
          file_value = (int)strtol(x, &tmp_, 0);
        }

        switch (toggle_stats) {
        case HITS:
          pstats_[player_atm].hit += file_value;
          break;
        case TOTAL:
          pstats_[player_atm].shots += file_value;
          break;
        case RATIO:
          pstats_[player_atm].ratio += file_valuef;
          break;
        case PLAYED:
          pstats_[player_atm].total += file_value;
          break;
        case WON:
          pstats_[player_atm].won += file_value;
          break;
        case LOST:
          pstats_[player_atm].lost += file_value;
          break;
        default:
          break;
        }
      }

      for (int i = 0; i < 7; i++) {
        if (strcmp(x, identifier[i]) == 0) {
          toggle_stats = i;
        }
      }
      x = strtok(NULL, " ");
    }
  }

  fflush(fr);
  fclose(fr);

  writeStats(pstats_);

  printf("> Updated stats.\n");
}