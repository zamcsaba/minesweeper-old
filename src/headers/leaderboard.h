#ifndef MINESWEEPER_LEADERBOARD_H
#define MINESWEEPER_LEADERBOARD_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

typedef struct Result {
    char player_name[100 + 1];
    int seconds;
    int mines;
    int cols;
    int rows;
} Result;

/**
 * Initializes the leaderboard.
 *
 * @param p_cols Number of columns
 * @param p_rows Number of rows
 * @param p_mines Number of mines
 * @return
 */
bool init_leaderboard(GameParams params);

/**
 * Save the results of a game the player won.
 *
 * @param player_name Player name
 * @param seconds Elapsed seconds
 */
void save_game_results(char *player_name, int seconds);

/**
 * Load the top results from the file the results are stored
 */
void load_top_results();

/**
 * Shows the leaderbord on the game area
 */
void show_leaderboard();

/**
 * Closes the leaderboard.
 * Frees up memory and closes the file.
 */
void close_leaderboard();

#endif //MINESWEEPER_LEADERBOARD_H
