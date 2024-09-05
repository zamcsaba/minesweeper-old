#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

#include <stdlib.h>
#include <stdbool.h>

#include <SDL_mouse.h>
#include <SDL_events.h>
#include <SDL_timer.h>
#include <SDL_quit.h>

typedef enum FaceType {
    FACE_SMILING,
    FACE_SMILING_PRESSED,
    FACE_THREATENED,
    FACE_SUNGLASSES,
    FACE_DEAD
} FaceType;

typedef enum FieldType {
    FIELD_STANDARD,
    FIELD_EMPTY,
    FIELD_FLAGGED,
    FIELD_QUESTION,
    FIELD_QUESTION_PRESSED,
    FIELD_MINE_REVEALED,
    FIELD_MINE_EXPLODED,
    FIELD_MINE_CROSSED,
    FIELD_NUMBERED
} FieldType;

typedef struct Field {
    FieldType type;
    bool is_mine;
    int x;
    int y;
    int neighbour_mines_count;
} Field;

typedef struct FieldSet {
    Field **fields;
    int rows;
    int cols;
} FieldSet;

typedef struct GameParams {
    char *player_name;
    int cols;
    int rows;
    int mine_count;
    int time_limit;
} GameParams;

typedef struct GameState {
    int mines_found;
    int time_elapsed;
    bool game_started;
    bool game_finished;
    bool leaderboard_opened;
    bool face_pressed;
    Field *pressed_field;
} GameState;

typedef struct Game {
    FieldSet fieldSet;
    GameParams params;
    GameState state;
} Game;

/**
 * Starts the game. Initializes event loop, allocates memory, prepares the game.
 * @param params Game parameters
 */
void start_game(GameParams params);

#endif //MINESWEEPER_GAME_H
