#include "headers/leaderboard.h"
#include "headers/graphics.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define MAXIMUM_RESULTS 5

static const char *DEFAULT_FILENAME = "results.csv";

static GameParams params;

static FILE *fp;
static Result top_results[MAXIMUM_RESULTS];
static int results_loaded = 0;

//#region PRIVATE

/**
 * Writes the header of the results CSV file
 */
void write_header() {
    fprintf(fp, "Jatekosnev,Ido,Aknak_szama,Szelesseg,Magassag\n");
}

/**
 * Checks if a file is empty (its data length is 0)
 * @return true if the file is empty
 */
bool is_file_empty() {
    fseek(fp, 0, SEEK_END);

    if (ftell(fp) == 0)
        return true;
    else
        return false;
}

/**
 * Inserts a result record to a given index into the (sorted!) top_results array.
 * The maximum index is 4!
 *
 * @param result The record to be added
 * @param index The target index
 */
void insert_to_results_at(Result result, int index) {
    if (index > MAXIMUM_RESULTS - 1 || index < 0)
        return;

    for (int i = MAXIMUM_RESULTS - 1; i > index; i--) {
        top_results[i] = top_results[i - 1];
    }
    top_results[index] = result;
}

/**
 * Puts a result into the top_results.
 * Finds the approppriate position for the result and if it should be in the top 5, inserts it.
 *
 * @param result the record to be added
 */
void put_result(Result result) {
    if (results_loaded == 0) {
        top_results[0] = result;
    } else {
        int insert_index = 0;
        for (int i = 0; i < min(MAXIMUM_RESULTS, results_loaded); i++) {
            if (top_results[i].seconds < result.seconds)
                insert_index++;
        }

        if (insert_index < MAXIMUM_RESULTS)
            insert_to_results_at(result, insert_index);
    }

    results_loaded++;
}

//#endregion

bool init_leaderboard(GameParams p_params) {
    params = p_params;

    fp = fopen(DEFAULT_FILENAME, "a+");

    if (fp == NULL)
        return false;

    return true;
}

void save_game_results(char *player_name, int seconds) {
    if (is_file_empty())
        write_header();

    fprintf(fp, "%s,%d,%d,%d,%d\n", player_name, seconds, params.mine_count, params.cols, params.rows);
}

void load_top_results() {
    fseek(fp, 0, SEEK_SET);
    results_loaded = 0;

    char line[128 + 1];
    bool is_header_line = true;
    while (fgets(line, 128, fp) != NULL) {
        if (is_header_line) {
            is_header_line = false;
            continue;
        }

        Result res;
        sscanf(line, "%[^,],%d,%d,%d,%d", res.player_name, &res.seconds, &res.mines, &res.cols, &res.rows);

        if (res.cols == params.cols && res.rows == params.rows && res.mines == params.mine_count)
            put_result(res);
    }
}

void show_leaderboard() {
    draw_leaderboard(top_results, min(MAXIMUM_RESULTS, results_loaded));
}

void close_leaderboard() {
    fclose(fp);
}