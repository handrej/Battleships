/**
 * @file main.c
 * @author Andrej Hanausek (ic19b032@technikum-wien.at)
 * @brief C implementation of the Classic Battleships Navy game. Two players compete with eachother to sink all ship in the opponents fleet. 
 * @version 1.1
 * @date 2020-01-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "battleship.h"
#include "helpers.h"

int main(int argc, char *argv[]) {
  int game_mode, player_total, player_current, direct, hitype, hitship, game_round = 0, sub_round = 1;
  int hit_total = 0, hit_count[2] = {0};

  int ship_mode[MAX_SHIPS] = {2, 2, 3, 3, 4, 5, 4};

  int ship_sunk[2][MAX_SHIPS] = {{2, 2, 3, 3, 4, 5, 4}, {2, 2, 3, 3, 4, 5, 4}};

  WaterCraft ship_type[4] = {
      {2, 's', "small", "Submarine"},
      {3, 'c', "medium", "Cruiser"},
      {4, 'b', "large", "Battleship"},
      {5, 'r', "huge", "Carrier"},
  };
  Coordinate target;
  // needs bigger buffer or bleeds into next input
  char tmp[3], *tmp_;

  Stats pstats_[2] = {0};

  /*
   * GAME START
   * */
  printf("####### BATTLESHIPS #######\n");
  printf("\nHow many human players are participating [1] or [2]?\n");
  // scanf("%d", &players);
  printf(">Enter Option:");
  do {
    fgets(tmp, sizeof(tmp), stdin);
    player_total = (int)strtol(tmp, &tmp_, 0);
  } while (player_total <= 0 || player_total > 2);
  if (*tmp_ != '\n' && *tmp_ != '\0') {
    fprintf(stderr, "Invalid input\n");
    return -1;
  }

  printf("\nChoose game mode:\n");
  printf("[1] Easy: 5x5 with 3 ships\n");
  printf("[2] Medium: 7x7 with 4 ships\n");
  printf("[3] Hard: 10x10 with 5 ships\n");
  printf("[4] Ultra: 13x13 with 7 ships\n");
  printf("[0] Exit\n");
  const int gm[4] = {DEBUG ? 2 : 5, 7, 10, 13};
  printf(">Enter Option:");
  do {
    fgets(tmp, sizeof(tmp), stdin);
    game_mode = (int)strtol(tmp, &tmp_, 0);
  } while (game_mode < 0 || game_mode > 4);
  if (*tmp_ != '\n' && *tmp_ != '\0') {
    fprintf(stderr, "Invalid input\n");
    return -1;
  }
  if (game_mode == 0)
    return 0;

  const int game_range = gm[game_mode - 1];
  // const int ship_total = (int)ceil((float)game_range / 2);
  const int ship_total = (game_range / 2) + ((game_range % 2) != 0);
  /*
   * GENERATE PLAYFIELDS
   * */
  Cell **player_1;
  Cell **player_2;

  // first dimension
  player_1 = malloc(game_range * sizeof(int *));
  if (player_1 == NULL) {
    fprintf(stderr, "Out of Memory\n");
    return -1;
  }
  for (int i = 0; i < game_range; i++) {
    player_1[i] = malloc(2 * game_range * sizeof(int));
  }
  board_clear(player_1, game_range);
  // first dimension
  player_2 = malloc(game_range * sizeof(int *));
  if (player_2 == NULL) {
    fprintf(stderr, "Out of Memory\n");
    return -1;
  }
  for (int i = 0; i < game_range; i++) {
    player_2[i] = malloc(2 * game_range * sizeof(int));
  }
  board_clear(player_2, game_range);
  // 'randomize' seeder based on time
  srand(time(0));

  board_diag(player_1, player_2, ship_type, game_range, player_total, ship_mode,
             ship_total);

  if (DEBUG) {
    printf("\n<DEBUG> PLAYER1:\n");
    board_print(player_1, player_2, game_range, true);
    printf("\n<DEBUG> PLAYER2:\n");
    board_print(player_2, player_1, game_range, true);
  }

  printf("\nGame starts!\n");
  player_current = inRange(0, 1);
  printf(">Player %d has been selected to go first.\n\n", player_current + 1);

  for (int j = 0; j < ship_total; ++j) {
    hit_total += ship_mode[j];
  }

  //double hitmissratio;
  //double flatoverflow;
  /*
   * GAME FLOW
   * */
  while (hit_count[0] != hit_total || hit_count[1] != hit_total) {
    hitype = 0;
    game_round++;
    if ((game_round % 2) == 1) {
      printf("********************");
      printf("\n** ROUND %d STARTS\n", sub_round);
      printf("********************");
      sub_round++;
    }

    switch (player_current) {
    case 0:
      printf("\nPLAYER %d'S TURN\n", player_current + 1);
      do {
        target = getTarget(game_range);
        hitype = checkShot(player_2, target);
      } while (hitype == 0);
      break;
    case 1:
      if (player_total == 2) {
        printf("\nPLAYER %d'S TURN\n", player_current + 1);
        do {
          target = getTarget(game_range);
          hitype = checkShot(player_1, target);
        } while (hitype == 0);
        break;
      } else {
        printf("\nCPU'S TURN\n");
        do {
          direct = inRange(0, 1);
          target = genCoords(direct, game_range - 1, 0);
          hitype = checkShot(player_1, target);
        } while (hitype == 0);
      }
      break;
    default:
      break;
    }
    pstats_[player_current].shots++;
    /*
     * prompts player if it's a hit or miss
     * */
    if (hitype == 1) {
      printf("(%d, %d) Target Hit!\n", target.row + 1, target.col + 1);
      hitship = player_current == 0 ? player_2[target.row][target.col].shipid : player_1[target.row][target.col].shipid;
      if (hitship > -1) {
        if (--ship_sunk[!player_current][hitship] == 0) {
          printf(">>Target Destroyed [Player %d's %s ship (%d cells)]!\n",
                 !player_current + 1, ship_type[ship_mode[hitship] - 2].size,
                 ship_mode[hitship]);
        }
      }
      hit_count[player_current]++;
      pstats_[player_current].hit++;
    } else {
      printf("(%d, %d) Target Miss!\n", target.row + 1, target.col + 1);
      pstats_[player_current].miss++;
    }
    // update board symbol
    if (player_current == 0) {
      player_2[target.row][target.col].symbol = hitype;
    } else {
      player_1[target.row][target.col].symbol = hitype;
    }
    /*
     * End of rounds
     * */
    if (hit_count[player_current] == hit_total) {
      printf("\n> Player %d wins!\n", player_current + 1);
      pstats_[player_current].won++;
      pstats_[!player_current].lost++;
      for (int c = 0; c < 2; c++) {
        pstats_[c].total++;
        pstats_[c].ratio = (pstats_[c].hit == 0 ? 0.0 : (double)pstats_[c].hit / (double)pstats_[c].shots);
      }
      break;
    }
    /*
     * display boards after each round
     */
    if (player_current == 0) {
      if (NCURS)
      {
        board_printn(player_2, player_1, game_range, DEBUG);
      }else{
        board_print(player_2, player_1, game_range, DEBUG);
      }
    } else {
      if (player_total == 2) {
        if (NCURS)
        {
          board_printn(player_1, player_2, game_range, DEBUG);
        }else{
          board_print(player_1, player_2, game_range, DEBUG);
        }
      }
    }
    // alternate players
    player_current = !player_current;
  }

  /*
   * MEMORY CLEANUP
   * */
  for (int i = 0; i < game_range; i++) {
    // free the second dimension
    free(player_1[i]);
    free(player_2[i]);
  }
  // free memory
  free(player_1);
  free(player_2);

  player_1 = NULL;
  player_2 = NULL;

  /*
   * LOG ENDGAME STATS
   * */
  parseStats(pstats_);

  exit(0);
}
