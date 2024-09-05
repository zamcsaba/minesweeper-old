#include "headers/graphics.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static int window_width;
static int window_height;

static GraphicsAssets assets;

static SDL_Rect button_position;

static const char *WINDOW_NAME = "Minesweeper";
static const char *FONT_NAME = "LiberationSerif-Regular.ttf";

static const int FIELD_SIZE = 16;
static const int BORDER_SIZE = 10;
static const int NUMBER_WIDTH = 13;
static const int NUMBER_HEIGHT = 23;
static const int FACE_SIZE = 26;
static const int LARGE_FONT_SIZE = 24;
static const int SMALL_FONT_SIZE = 12;

//#region PRIVATE

/**
 * Init Tiles
 * Loads tilesets into texture files
 *
 * (Private)
 */
void init_tiles() {
    assets.face_tiles = IMG_LoadTexture(renderer, "faces.png");
    assets.field_tiles = IMG_LoadTexture(renderer, "fields.png");
    assets.number_tiles = IMG_LoadTexture(renderer, "numbers.png");
    assets.mine_number_tiles = IMG_LoadTexture(renderer, "mine_numbers.png");
}

/**
 * Draw Field
 *
 * @param field Type of field to draw
 * @param x X
 * @param y Y
 *
 * (Private)
 */
void draw_field(FieldType field, int x, int y) {
    SDL_Rect src = {field * FIELD_SIZE, 0, FIELD_SIZE, FIELD_SIZE};
    SDL_Rect dest = {x, y, FIELD_SIZE, FIELD_SIZE};
    SDL_RenderCopy(renderer, assets.field_tiles, &src, &dest);
}

/**
 * Draw Counter Number
 * @param number Number 0-9
 * @param x X
 * @param y Y
 *
 * (Private)
 */
void draw_counter_number(int number, int x, int y) {
    SDL_Rect src = {number * NUMBER_WIDTH, 0, NUMBER_WIDTH, NUMBER_HEIGHT};
    SDL_Rect dest = {x, y, NUMBER_WIDTH, NUMBER_HEIGHT};
    SDL_RenderCopy(renderer, assets.number_tiles, &src, &dest);
}

/**
 * Draw Mine Number
 * @param number Number 0-8
 * @param x X
 * @param y Y
 *
 * (Private)
 */
void draw_mine_number(int number, int x, int y) {
    SDL_Rect src = {(number - 1) * FIELD_SIZE, 0, FIELD_SIZE, FIELD_SIZE};
    SDL_Rect dest = {x, y, FIELD_SIZE, FIELD_SIZE};
    SDL_RenderCopy(renderer, assets.mine_number_tiles, &src, &dest);
}

void draw_text(char *text, SDL_Rect *position, bool large) {
    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(large ? assets.font_large : assets.font_small, text,
                                                       assets.black);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    position->w = text_surface->w;
    position->h = text_surface->h;

    SDL_RenderCopy(renderer, text_texture, NULL, position);
    refresh_screen();

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

//#endregion

void init_game_window(int rows, int cols) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("SDL cannot start: %s", SDL_GetError());
        exit(1);
    }

    window_width = (cols * FIELD_SIZE) + (2 * BORDER_SIZE);
    window_height = (rows * FIELD_SIZE) + (5 * BORDER_SIZE) + NUMBER_HEIGHT;

    window = SDL_CreateWindow(
            WINDOW_NAME,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window_width, window_height, 0
    );
    if (window == NULL) {
        SDL_Log("Window cannot be created: %s", SDL_GetError());
        exit(2);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Renderer cannot be created: %s", SDL_GetError());
        exit(3);
    }

    TTF_Init();
    assets.font_small = TTF_OpenFont(FONT_NAME, SMALL_FONT_SIZE);
    if (assets.font_small == NULL) {
        SDL_Log("Font cannot be opened: %s", TTF_GetError());
        exit(4);
    }
    assets.font_large = TTF_OpenFont(FONT_NAME, LARGE_FONT_SIZE);
    if (assets.font_large == NULL) {
        SDL_Log("Font cannot be opened: %s", TTF_GetError());
        exit(4);
    }

    SDL_Color black = {30, 30, 30};
    assets.black = black;

    init_tiles();
    reset_screen();

    draw_button("Eredmények");
    SDL_RenderPresent(renderer);
}

void draw_face(FaceType face) {
    SDL_Rect src = {face * FACE_SIZE, 0, FACE_SIZE, FACE_SIZE};
    SDL_Rect dest = {(window_width - FACE_SIZE) / 2, 2 * BORDER_SIZE, FACE_SIZE, FACE_SIZE};
    SDL_RenderCopy(renderer, assets.face_tiles, &src, &dest);
}

void draw_fields(FieldSet fieldSet) {
    for (int i = 0; i < fieldSet.rows; i++) {
        for (int j = 0; j < fieldSet.cols; j++) {
            if (fieldSet.fields[i][j].neighbour_mines_count == 0) {
                draw_field(fieldSet.fields[i][j].type, (j * FIELD_SIZE) + BORDER_SIZE,
                           (i * FIELD_SIZE) + (4 * BORDER_SIZE) + NUMBER_HEIGHT);
            } else {
                draw_mine_number(fieldSet.fields[i][j].neighbour_mines_count, (j * FIELD_SIZE) + BORDER_SIZE,
                                 (i * FIELD_SIZE) + (4 * BORDER_SIZE) + NUMBER_HEIGHT);
            }
        }
    }
}

void draw_mine_counter(int mine_count) {
    for (int i = 2, j = mine_count; i >= 0; i--, j /= 10) {
        draw_counter_number(j % 10, (i * NUMBER_WIDTH) + BORDER_SIZE, 2 * BORDER_SIZE);
    }
}

void draw_time_counter(int seconds) {
    for (int i = 2, j = seconds; i >= 0; i--, j /= 10) {
        draw_counter_number(j % 10, window_width - ((3 - i) * NUMBER_WIDTH) - BORDER_SIZE, 2 * BORDER_SIZE);
    }
}

void draw_leaderboard(Result results[], int length) {
    reset_screen();
    draw_button("Játék");

    for (int i = 0; i < length; ++i) {
        char res[128 + 1];
        sprintf(res, "%d. %s, %d mp.", i + 1, results[i].player_name, results[i].seconds);

        bool is_large_needed = window_width > 300;

        SDL_Rect pos;
        pos.x = BORDER_SIZE;
        pos.y = (4 * BORDER_SIZE) + i * (LARGE_FONT_SIZE + 12);
        draw_text(res, &pos, is_large_needed);
    }
}

void draw_button(char *text) {
    button_position.x = BORDER_SIZE;
    button_position.y = 4;

    draw_text(text, &button_position, false);
}

void refresh_screen() {
    SDL_RenderPresent(renderer);
}

void reset_screen() {
    SDL_SetRenderDrawColor(renderer, 189, 189, 189, 255);
    SDL_RenderClear(renderer);
}

void position_to_field_coordinates(int posX, int posY, int rows, int cols, int *coordX, int *coordY) {
    // If x,y is not in the minefield
    bool is_in = true;
    if (posX < BORDER_SIZE || posX > BORDER_SIZE + (cols * FIELD_SIZE))
        is_in = false;
    if (posY < (4 * BORDER_SIZE) + NUMBER_HEIGHT || posY > (4 * BORDER_SIZE) + NUMBER_HEIGHT + (rows * FIELD_SIZE))
        is_in = false;
    if (!is_in) {
        *coordX = -1;
        *coordY = -1;
        return;
    }

    *coordX = (posX - BORDER_SIZE) / FIELD_SIZE;
    *coordY = (posY - (4 * BORDER_SIZE) - NUMBER_HEIGHT) / FIELD_SIZE;
}

bool is_in_range(int x, int y, int minX, int maxX, int minY, int maxY) {
    return x >= minX && y >= minY && x <= maxX && y <= maxY;
}

bool is_position_on_face(int posX, int posY) {
    int minX = (window_width - FACE_SIZE) / 2;
    int maxX = (window_width + FACE_SIZE) / 2;
    int minY = 2 * BORDER_SIZE;
    int maxY = (2 * BORDER_SIZE) + FACE_SIZE;

    return is_in_range(posX, posY, minX, maxX, minY, maxY);
}

bool is_position_on_button(int posX, int posY) {
    return is_in_range(posX, posY, button_position.x, button_position.x + button_position.w, button_position.y,
                       button_position.y + button_position.h);
}

void free_graphics_memory() {
    // Textures
    SDL_DestroyTexture(assets.face_tiles);
    SDL_DestroyTexture(assets.field_tiles);
    SDL_DestroyTexture(assets.number_tiles);
    SDL_DestroyTexture(assets.mine_number_tiles);

    // Fonts
    TTF_CloseFont(assets.font_small);
    TTF_CloseFont(assets.font_large);

    // Renderer & Window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}