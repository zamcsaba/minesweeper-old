#include <stdlib.h>
#include <stdbool.h>
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_events.h>
#include <SDL_timer.h>
#include "headers/graphics.h"
#include "headers/game.h"
#include "headers/leaderboard.h"

static char *DEFAULT_PLAYER_NAME = "Anonymous";
static const int DEFAULT_MINE_FIELD_WIDTH = 9;
static const int DEFAULT_MINE_FIELD_HEIGHT = 9;
static const int DEFAULT_MINE_COUNT = 10;
static const int DEFAULT_TIME_LIMIT = 0;

int main(int argc, char *argv[]) {
    GameParams params;

    // Get game parameters
    params.player_name = argc > 1 ? argv[1] : DEFAULT_PLAYER_NAME;
    params.cols = argc > 2 ? atoi(argv[2]) : DEFAULT_MINE_FIELD_WIDTH;
    params.rows = argc > 3 ? atoi(argv[3]) : DEFAULT_MINE_FIELD_HEIGHT;
    params.mine_count = argc > 4 ? atoi(argv[4]) : DEFAULT_MINE_COUNT;
    params.time_limit = argc > 5 ? atoi(argv[5]) : DEFAULT_TIME_LIMIT;

    // Start the game
    start_game(params);

    return 0;
}