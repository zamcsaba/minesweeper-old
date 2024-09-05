#ifndef MINESWEEPER_GRAHICS_H
#define MINESWEEPER_GRAHICS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "game.h"
#include "leaderboard.h"

typedef struct GraphicsAssets {
    SDL_Texture *face_tiles;
    SDL_Texture *field_tiles;
    SDL_Texture *number_tiles;
    SDL_Texture *mine_number_tiles;

    TTF_Font *font_small;
    TTF_Font *font_large;
    SDL_Color black;
} GraphicsAssets;

/**
 * Init Game Graphics
 * Initializes the window and the renderer.
 * Loads tiles and calculates window width and height.
 *
 * @param rows Number of rows
 * @param cols Number of columns
 */
void init_game_window(int rows, int cols);

/**
 * Draw Face
 *
 * @param face The type of face to draw
 */
void draw_face(FaceType face);

/**
 * Draw Fields
 * Draw all the fields to the game area.
 * If a field is revealed and has neighbour mines, a number is drawn instead.
 *
 * @param fieldSet Set of fields in the game area
 */
void draw_fields(FieldSet fieldSet);

/**
 * Draw mine counter
 * Draws the 3-digit mine counter to the top left corner of the screen
 * @param mine_count
 */
void draw_mine_counter(int mine_count);

/**
 * Draw time counter
 * Draws the 3-digit time counter to the top right corner of the screen
 * @param seconds
 */
void draw_time_counter(int seconds);

/**
 * Draws the leaderboard to the screen
 * @param results array containing the results
 * @param length number of results to draw
 */
void draw_leaderboard(Result results[], int length);

/**
 * Draws the leaderboard/game button with the given text
 * @param text
 */
void draw_button(char *text);

/**
 * Refresh the screen
 */
void refresh_screen();

/**
 * Resets the screen
 */
void reset_screen();

/**
 * Position to Field Coordinates
 *
 * Converts a mouse position to field coordinates.
 * Sets coordX and coordY to -1 if the position is not in the field area
 *
 * @param posX Mouse position X
 * @param posY Mouse position Y
 * @param rows Number of rows
 * @param cols Number of cols
 * @param coordX X Coordinate
 * @param coordY Y Coordinate
 */
void position_to_field_coordinates(int posX, int posY, int rows, int cols, int *coordX, int *coordY);

/**
 * Checks if a coordinate is in a specified rande
 * @param x X
 * @param y Y
 * @param minX Min X
 * @param maxX Max X
 * @param minY Min Y
 * @param maxY Max Y
 * @return true if the coordinates are in the given range
 */
bool is_in_range(int x, int y, int minX, int maxX, int minY, int maxY);

/**
 * Check if mouse position intersects the face
 * @param posX Mouse X
 * @param posY Mouse Y
 * @return true if the mouse is on the face
 */
bool is_position_on_face(int posX, int posY);

/**
 * Check if mouse position intersects the menu button
 * @param posX Mouse X
 * @param posY Mouse Y
 * @return true if the mouse is on the menu button
 */
bool is_position_on_button(int posX, int posY);

/**
 * Destroys graphics elements, frees graphics memory
 */
void free_graphics_memory();

#endif //MINESWEEPER_GRAHICS_H
