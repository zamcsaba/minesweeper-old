#include "headers/game.h"
#include "headers/graphics.h"
#include "headers/leaderboard.h"

static SDL_TimerID timer;

//#region PRIVATE

/**
 * Allocate memory used to store field data
 */
void allocate_field_memory(FieldSet *fieldSet) {
    fieldSet->fields = (Field **) malloc(fieldSet->rows * sizeof(Field *));
    fieldSet->fields[0] = (Field *) malloc(fieldSet->rows * fieldSet->cols * sizeof(Field));
    for (int i = 1; i < fieldSet->rows; i++) {
        fieldSet->fields[i] = fieldSet->fields[0] + (i * fieldSet->cols);
    }
}

/**
 * Reset field to its default state and to a given position
 * @param field The field to be resetted
 * @param x field position x
 * @param y field position y
 */
void reset_field(Field *field, int x, int y) {
    field->type = FIELD_STANDARD;
    field->is_mine = false;
    field->x = x;
    field->y = y;
    field->neighbour_mines_count = 0;
}

/**
 * Initializes the fieldset, allocates memory and resets the fields.
 */
void init_field_set(Game *game) {
    game->fieldSet.rows = game->params.rows;
    game->fieldSet.cols = game->params.cols;

    // Allocate if not allocated yet
    if (game->fieldSet.fields == NULL) {
        allocate_field_memory(&game->fieldSet);
    }

    // Reset
    for (int i = 0; i < game->fieldSet.rows; ++i) {
        for (int j = 0; j < game->fieldSet.cols; ++j) {
            reset_field(&game->fieldSet.fields[i][j], i, j);
        }
    }
}

/**
 * Place mines randomly except on the specified field
 * @param start Field not to place a mine on
 */
void place_mines(Field start, Game *game) {
    for (int i = 0; i < game->params.mine_count; i++) {
        bool search_for_position = true;
        while (search_for_position) {
            int row = rand() % game->fieldSet.rows;
            int col = rand() % game->fieldSet.cols;

            Field *current = &game->fieldSet.fields[row][col];

            if (!current->is_mine && (abs(current->x - start.x) > 1 || abs(current->y - start.y) > 1)) {
                current->is_mine = true;
                search_for_position = false;
            }
        }
    }
}

/**
 * Finds a field by mouse coordinates.
 * @param x Mouse X
 * @param y Mouse Y
 * @return reference to the found field, or NULL if there is no field there.
 */
Field *find_field(int x, int y, FieldSet fieldSet) {
    int fieldX, fieldY;
    position_to_field_coordinates(x, y, fieldSet.rows, fieldSet.cols, &fieldX, &fieldY);

    if (fieldX > -1 && fieldY > -1)
        return &fieldSet.fields[fieldY][fieldX];
    else
        return NULL;
}

/**
 * Reveal all mines in the mine area
 */
void reveal_mines(FieldSet fieldSet) {
    for (int i = 0; i < fieldSet.rows; ++i) {
        for (int j = 0; j < fieldSet.cols; ++j) {
            Field *current = &fieldSet.fields[i][j];
            if (current->type == FIELD_FLAGGED || (current->is_mine && current->type != FIELD_MINE_EXPLODED)) {
                if (current->type == FIELD_FLAGGED)
                    current->type = current->is_mine ? FIELD_FLAGGED : FIELD_MINE_CROSSED;
                else
                    current->type = FIELD_MINE_REVEALED;
            }
        }
    }
}

/**
 * Counts the neighbor mines of a field.
 * @param field Field to count the neighbors mines of
 * @return Number of neighbor mines
 */
int count_neighbour_mines(Field field, FieldSet fieldSet) {
    int cnt = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;

            if (is_in_range(field.x + i, field.y + j, 0, fieldSet.rows - 1, 0, fieldSet.cols - 1)) {
                if (fieldSet.fields[field.x + i][field.y + j].is_mine)
                    cnt++;
            }
        }
    }

    return cnt;
}

/**
 * Autodiscover empty fields started with a given empty field.
 * This function runs until there are empty fields without being intersected by a non-empty field.
 * @param field
 */
void autodiscover_empty_fields(Field *field, FieldSet fieldSet) {
    int cnt;
    if ((cnt = count_neighbour_mines(*field, fieldSet)) > 0) {
        field->neighbour_mines_count = cnt;
        field->type = FIELD_NUMBERED;
        return;
    }

    field->type = FIELD_EMPTY;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;

            if (is_in_range(field->x + i, field->y + j, 0, fieldSet.rows - 1, 0, fieldSet.cols - 1)) {
                if (fieldSet.fields[field->x + i][field->y + j].type == FIELD_STANDARD)
                    autodiscover_empty_fields(&fieldSet.fields[field->x + i][field->y + j], fieldSet);
            }
        }
    }
}

/**
 * Checks if the player won the game
 * @return
 */
bool is_winner(FieldSet fieldSet) {
    for (int i = 0; i < fieldSet.rows; ++i) {
        for (int j = 0; j < fieldSet.cols; ++j) {
            if (fieldSet.fields[i][j].type == FIELD_STANDARD && !fieldSet.fields[i][j].is_mine)
                return false;
        }
    }
    return true;
}

/**
 * Handles the event when a player won.
 * Stops the timer, sets the game as finished and saves the game
 */
void handle_winning(Game *game) {
    draw_face(FACE_SUNGLASSES);
    game->state.game_finished = true;
    SDL_RemoveTimer(timer);
    game->state.game_started = false;

    save_game_results(game->params.player_name, game->state.time_elapsed);
}

/**
 * Reset the parameters and state of the game
 * @param game
 */
void reset_game(Game *game) {
    SDL_RemoveTimer(timer);

    game->state.game_started = false;
    game->state.game_finished = false;
    game->state.time_elapsed = 0;
    game->state.mines_found = 0;
    game->state.face_pressed = false;
    game->state.pressed_field = NULL;


    init_field_set(game);

    reset_screen();
    draw_fields(game->fieldSet);
    draw_mine_counter(game->params.mine_count);
    draw_time_counter(game->params.time_limit);
    draw_face(FACE_SMILING);
    draw_button("Eredmények");
}

/**
 * Timer callback
 * @param ms elapsed milliseconds
 * @param param game object
 * @return
 */
Uint32 handle_timer(Uint32 ms, void *param) {
    Game *game = (Game *) param;

    if (game->state.game_started && !game->state.game_finished) {
        game->state.time_elapsed++;

        if (game->params.time_limit > 0 && game->state.time_elapsed > game->params.time_limit) {
            game->state.game_finished = true;
            SDL_RemoveTimer(timer);
            draw_face(FACE_DEAD);
        }

        draw_time_counter(game->state.time_elapsed);
        refresh_screen();
    }

    return ms;
}

/**
 * Handle the mouse down event
 * @param button
 * @param x
 * @param y
 * @param game
 */
void handle_mouse_down(Uint8 button, int x, int y, Game *game) {
    if (button == SDL_BUTTON_LEFT) {
        Field *field = find_field(x, y, game->fieldSet);
        if (field != NULL && !game->state.leaderboard_opened) {
            if (game->state.game_finished)
                return;

            if (field->type == FIELD_STANDARD) {
                draw_face(FACE_THREATENED);
                field->type = FIELD_EMPTY;
                game->state.pressed_field = field;

                draw_fields(game->fieldSet);
            }
        } else {
            game->state.pressed_field = NULL;
            if (is_position_on_face(x, y) && !game->state.leaderboard_opened) {
                draw_face(FACE_SMILING_PRESSED);
                game->state.face_pressed = true;
            }
            if (is_position_on_button(x, y)) {
                if (!game->state.game_finished && game->state.game_started)
                    return;

                if (!game->state.leaderboard_opened) {
                    game->state.leaderboard_opened = true;
                    load_top_results();
                    show_leaderboard();
                } else {
                    reset_game(game);
                    game->state.leaderboard_opened = false;
                }
            }
        }
    } else if (button == SDL_BUTTON_RIGHT && !game->state.leaderboard_opened) {
        Field *field = find_field(x, y, game->fieldSet);
        if (field != NULL) {
            // Handle only if game is ongoing
            if (game->state.game_finished)
                return;

            if (field->type == FIELD_STANDARD && (game->params.mine_count - game->state.mines_found) > 0) {
                field->type = FIELD_FLAGGED;
                draw_mine_counter(game->params.mine_count - ++game->state.mines_found);
            } else if (field->type == FIELD_FLAGGED) {
                draw_mine_counter(game->params.mine_count - --game->state.mines_found);
                field->type = FIELD_QUESTION;
            } else if (field->type == FIELD_QUESTION) {
                field->type = FIELD_STANDARD;
            }

            draw_fields(game->fieldSet);
        }
    }


    if (game->state.game_started && !game->state.game_finished && is_winner(game->fieldSet)) {
        handle_winning(game);
    }
}

/**
 * Handler for the mouse up event
 * @param button
 * @param x
 * @param y
 * @param game
 */
void handle_mouse_up(Uint8 button, int x, int y, Game *game) {
    if (button == SDL_BUTTON_LEFT && !game->state.leaderboard_opened) {
        if (!game->state.face_pressed && game->state.game_finished)
            return;

        if (game->state.pressed_field != NULL) {
            game->state.pressed_field->type = FIELD_STANDARD;
            game->state.pressed_field = NULL;
        }

        Field *field = find_field(x, y, game->fieldSet);
        if (field != NULL) {
            if (!game->state.game_started) {
                place_mines(*field, game);
                game->state.game_started = true;
                timer = SDL_AddTimer(1000, handle_timer, game);
                autodiscover_empty_fields(field, game->fieldSet);
            }

            if (field->type == FIELD_FLAGGED)
                return;

            if (field->is_mine) {
                game->state.game_finished = true;
                SDL_RemoveTimer(timer);
                field->type = FIELD_MINE_EXPLODED;
                reveal_mines(game->fieldSet);
                draw_face(FACE_DEAD);
            } else {
                field->neighbour_mines_count = count_neighbour_mines(*field, game->fieldSet);
                if (field->neighbour_mines_count == 0)
                    autodiscover_empty_fields(field, game->fieldSet);
                else
                    field->type = FIELD_NUMBERED;

                if (game->state.game_started && !game->state.game_finished && is_winner(game->fieldSet)) {
                    handle_winning(game);
                } else
                    draw_face(FACE_SMILING);
            }
            draw_fields(game->fieldSet);
        } else if (game->state.face_pressed) {
            game->state.face_pressed = false;
            reset_game(game);
        }
    }
}

/**
 * Free the memory allocated for the fields
 * @param fieldSet
 */
void free_field_memory(FieldSet *fieldSet) {
    free(fieldSet->fields[0]);
    free(fieldSet->fields);
}

//#endregion

void start_game(GameParams p_params) {

    Game game;
    game.params = p_params;
    game.fieldSet.fields = NULL;


    // Init graphics and leaderboard
    init_game_window(game.params.rows, game.params.cols);
    init_leaderboard(game.params);
    load_top_results();

    reset_game(&game);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                quit = true;

                free_field_memory(&game.fieldSet);
                free_graphics_memory();
                close_leaderboard();
                break;

            case SDL_MOUSEBUTTONDOWN:
                handle_mouse_down(event.button.button, event.button.x, event.button.y, &game);
                break;

            case SDL_MOUSEBUTTONUP:
                handle_mouse_up(event.button.button, event.button.x, event.button.y, &game);
                break;

            default:
                break;
        }

        refresh_screen();
    }
}