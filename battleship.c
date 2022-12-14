#include "battleship.h"
#include "helpers.h"

/**
 * @brief Clears all fields on the GameBoard.
 * 
 * Resets GameBoard by reseting important field values back to 0.
 * 
 * @param game_board Target Board
 * @param game_range Target Board Dimension
 */
void board_clear(Cell **game_board, int game_range) {
  for (int i = 0; i < game_range; i++) {
    // game_board[i] = malloc(2 * game_range * sizeof(int)); memleak on reset
    for (int j = 0; j < game_range; j++) {
      game_board[i][j].symbol = WATER;
      game_board[i][j].shipid = -1;
    }
  }
}

/**
 * @brief Sets up ships on the GameBoard.
 * 
 * Places ships specified in the argument.
 * Placement on a per field bases depending on @c position, @c ship_mode and @c direction of ship.
 * 
 * @param game_board Target Board
 * @param ship_type Ship Properties
 * @param position Ship Coordinates
 * @param ship_mode Ship Length
 * @param direction Cardinal Direction
 * @param index Ship ID on the Target Board
 */
void board_fill(Cell **game_board, WaterCraft *ship_type, Coordinate position, int ship_mode, int direction, int index) {
  for (int i = 0; i < ship_mode; i++) {
    int x = direction == 0 ? position.row : (position.row + i);
    int y = direction == 0 ? (position.col + i) : position.col;

    game_board[x][y].symbol = ship_type[ship_mode - 2].id;
    game_board[x][y].shipid = index;
  }
}

/**
 * @brief Generates coordinates for ships on the GameBoard.
 * 
 * Different modes to generate coordinates on GameBaord with given arguments. \n
 * 
 * Random Mode: Generates Coordinates @c genCoords() and sets position @c board_fill() 
 * after passing validation @c is_valid(). \n
 * Manual Mode: Coordinates based on user input from @c getTarget(). \n
 * 
 * Reset Counter @c c resets board if @c isValid can't find a valid position after x tries. \n
 * Prevents endless loops when ships are placed too dense to each other.
 * 
 * @param game_board Target Board
 * @param ship_type Ship Properties
 * @param game_range Target Board Dimension
 * @param ship_mode Ship Length
 * @param ship_total Ship Counter
 * @param rng Generation Mode. 0 == random, 1 == manual
 */
void board_rand(Cell **game_board, WaterCraft *ship_type, int game_range, int *ship_mode, int ship_total, int rng) {
  Coordinate pos;

  int dir = 0;
  int c = 0;
  char dval[3], *vald;

  for (int i = 0; i < ship_total; ++i) {
    while (true) {
      if (rng == 0) {
        c++;
        if (c > 10) {
          board_clear(game_board, game_range);
          board_rand(game_board, ship_type, game_range, ship_mode, ship_total, rng);
          return;
        }
        dir = inRange(0, 1);
        pos = genCoords(dir, game_range, ship_mode[i]);
      } else {
        printf("[%d/%d] Placing %s (%d cells); ", i + 1, ship_total, ship_type[ship_mode[i] - 2].name, ship_mode[i]);
        pos = getTarget(game_range);
        printf("[1] HORIZONTAL\n");
        printf("[2] VERTICAL\n");
        printf(">Choose:");
        do {
          fgets(dval, sizeof(dval), stdin);
          dir = (int)strtol(dval, &vald, 0);
        } while (dir <= 0 || dir > 2);
        dir -= 1;
      }
      if (isvalid(game_board, pos, game_range, dir, ship_mode[i], i)) {
        c = 0;
        break;
      }
    }
    board_fill(game_board, ship_type, pos, ship_mode[i], dir, i);
    if (rng == 1) {
      board_print(game_board, game_board, game_range, true);
    }
  }
}

/**
 * @brief Dialogue to choose placement mode for ships.
 * 
 * Player choose mode in witch to generate ship positions.
 * 
 * @param game_board Target Board
 * @param gameBoard2 CPU Board
 * @param ship_type Ship Properties
 * @param game_range Target Board Dimension
 * @param player_total Player Counter (no CPU)
 * @param ship_mode Ship Length
 * @param ship_total Ship Counter
 */
void board_diag(Cell **game_board, Cell **gameBoard2, WaterCraft *ship_type, int game_range, int player_total, int *ship_mode, int ship_total) {
  char tmp[3], *tmp_;
  int gen = 0;

  for (int i = 0; i < player_total; ++i) {
    if (i == 0 || (i == 1 && player_total == 2)) {
      printf("\nPlayer %d do you want to enter x,y coordinates to place your ships?\n", i + 1);
      printf("[1] NO\n");
      printf("[2] YES\n");
      printf(">Choose:");
      do {
        fgets(tmp, sizeof(tmp), stdin);
        gen = (int)strtol(tmp, &tmp_, 0);
      } while (gen <= 0 || gen > 2);
    }

    if (i == 0) {
      board_rand(game_board, ship_type, game_range, ship_mode, ship_total, gen - 1);
    } else {
      board_rand(gameBoard2, ship_type, game_range, ship_mode, ship_total, gen - 1);
      continue;
    }
    board_rand(gameBoard2, ship_type, game_range, ship_mode, ship_total, gen - 1);
  }
}

/**
 * @brief Prints both GameBoard using ncurses.
 * 
 * Ncurses implementation of showing GameBoards.
 * 
 * @param own Made Shots
 * @param opp Opponent Shots
 * @param game_range Target Board Dimension
 * @param show_all @c Debug Show all ships to both players
 */
void board_printn(Cell **own, Cell **opp, int game_range, bool show_all) {
  initscr();

  box(stdscr, '|', '~');
  // init eight basic colors
  start_color();
  curs_set(0);

  init_pair(1, COLOR_CYAN, COLOR_BLACK); // WATER
  init_pair(2, COLOR_WHITE, COLOR_BLACK); // MISS
  init_pair(3, COLOR_RED, COLOR_BLACK); // HIT
  init_pair(4, COLOR_GREEN, COLOR_BLACK); // FRIENDLY 
  init_pair(5, COLOR_RED, COLOR_BLACK); // ENEMY

  cbreak();
  noecho();
  // keypad(stdscr, TRUE);
  refresh();

  int offset = game_range + 40;

  char ship_syms[6] = {'~', 'x', 's', 'c', 'b', 'r'};

  mvwprintw(stdscr, 1, 3 * 1 + 2, "%s", "TARGET FIELD");
  for (int i = 0; i < game_range; i++) {
    mvwprintw(stdscr, 2, 3 * (i + 1) + 2, "%d", i + 1);
  }

  mvwprintw(stdscr, 1, offset + 3 * 1 + 2, "%s", "PLAYER BATTLESHIPS");
  for (int i = 0; i < game_range; i++) {
    mvwprintw(stdscr, 2, offset + 3 * (i + 1) + 2, "%d", i + 1);
  }

  int nposy, nposx;

  for (int i = 0; i < game_range; i++) {
    nposy = i + 3;
    mvwprintw(stdscr, nposy, 1, "%i", i + 1);
    for (int j = 0; j < game_range; j++) {
      nposx = 3 * (j + 1);
      // if (show_all == true) {
      if (own[i][j].symbol == -1) {
        // MISS
        wattron(stdscr, COLOR_PAIR(2));
        mvwprintw(stdscr, nposy, nposx, "  m  ");
        wattroff(stdscr, COLOR_PAIR(2));
      } else if (own[i][j].symbol == 1) {
        // HIT
        wattron(stdscr, COLOR_PAIR(3));
        mvwprintw(stdscr, nposy, nposx, "  x  ");
        wattroff(stdscr, COLOR_PAIR(3));
      } else {
        // SHIP
        if (show_all == true) {
          wattron(stdscr, COLOR_PAIR(5));
          mvwprintw(stdscr, nposy, nposx, "  %c  ", ship_syms[own[i][j].symbol]);
          wattroff(stdscr, COLOR_PAIR(5));
        } else {
          wattron(stdscr, COLOR_PAIR(1));
          mvwprintw(stdscr, nposy, nposx, "  ~  ");
          wattroff(stdscr, COLOR_PAIR(1));
        }
      }
    }
    for (int j = 0; j < game_range; j++) {
      nposx = 3 * (j + 1);
      mvwprintw(stdscr, nposy, offset, "%i", i + 1);
      if (opp[i][j].symbol == 0) {
        // WATER
        wattron(stdscr, COLOR_PAIR(1));
        mvwprintw(stdscr, nposy, offset + nposx, "  ~  ");
        wattroff(stdscr, COLOR_PAIR(1));
      } else if (opp[i][j].symbol == -1) {
        // MISS
        wattron(stdscr, COLOR_PAIR(2));
        mvwprintw(stdscr, nposy, offset + nposx, "  m  ");
        wattroff(stdscr, COLOR_PAIR(2));
      } else if (opp[i][j].symbol == 1) {
        // HIT
        wattron(stdscr, COLOR_PAIR(3));
        mvwprintw(stdscr, nposy, offset + nposx, "  x  ");
        wattroff(stdscr, COLOR_PAIR(3));
      } else {
        // SHIP
        wattron(stdscr, COLOR_PAIR(4));
        mvwprintw(stdscr, nposy, offset + nposx, "  %c  ", ship_syms[opp[i][j].symbol]);
        wattroff(stdscr, COLOR_PAIR(4));
      }
    }
    // wattron(stdscr,COLOR_PAIR(1));
  }

  box(stdscr, 0, 0);

  getch();
  echo();
  endwin();
}

/**
 * @brief Prints both GameBoards on the console.
 * 
 * @param own Made Shots
 * @param opp Opponent Shots
 * @param game_range Target Board Dimension
 * @param show_all @c Debug Show all ships to both players
 */
void board_print(Cell **own, Cell **opp, int game_range, bool show_all) {
  char ship_syms[6] = {'~', 'x', 's', 'c', 'b', 'r'};
  printf("\n");
  printf("\tTARGET FIELD\t\t\t\t\t\t");
  if (game_range >= 10)
    printf("\t\t");
  printf("MY SHIPS");
  printf("\n");
  printf("\t    ");
  for (int i = 1; i <= game_range; i++) {
    printf("%d ", i);
  }
  printf("\t\t\t\t\t");
  if (game_range < 10)
    printf("\t");
  printf("    ");

  for (int i = 1; i <= game_range; i++) {
    printf("%d ", i);
  }
  printf("\n");
  /* code */
  for (int i = 0; i < game_range; i++) {
    printf("\t");
    printf("%02d ", i + 1);
    for (int j = 0; j < game_range; ++j) {
      if (show_all == true) {
        if (own[i][j].symbol == -1) {
          printf(" %c", 'm');
        } else {
          if (DEBUG || own[i][j].symbol < 2) {
            printf(" %c", ship_syms[own[i][j].symbol]);
          } else {
            printf(" ~");
          }
        }
      } else {
        if (own[i][j].symbol == -1) {
          printf(" %c", 'm');
        } else {
          if (own[i][j].symbol < 2) {
            printf(" %c", ship_syms[own[i][j].symbol]);
          } else {
            printf(" ~");
          }
        }
      }
    }
    printf("\t\t\t\t\t\t");
    printf("%02d ", i + 1);
    for (int j = 0; j < game_range; ++j) {
      if (show_all == true) {
        if (opp[i][j].symbol == -1) {
          printf(" %c", 'm');
        } else {
          printf(" %c", ship_syms[opp[i][j].symbol]);
        }
      } else {
        if (opp[i][j].symbol == -1) {
          printf(" %c", 'm');
        } else {
          printf(" %c", ship_syms[opp[i][j].symbol]);
        }
      }
    }
    printf("\n");
  }
  printf("\n");
}