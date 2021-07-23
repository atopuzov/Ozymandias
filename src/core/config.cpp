#include "config.h"

#include "core/file.h"
#include "core/log.h"

#include <stdio.h>
#include <string.h>

#define MAX_LINE 100

static const char *INI_FILENAME = "augustus.ini";

// Keep this in the same order as the ints in config.h
static const char *ini_keys[] = {
        "gameplay_fix_immigration",
        "gameplay_fix_100y_ghosts",
        "gameplay_fix_editor_events",
        "ui_sidebar_info",
        "ui_show_intro_video",
        "ui_smooth_scrolling",
        "ui_walker_waypoints",
        "ui_visual_feedback_on_delete",
        "ui_show_water_structure_range",
        "ui_show_construction_size",
        "ui_zoom",
        "ui_complete_ratings_columns",
        "ui_highlight_legions",
        "ui_rotate_manually",
        "gameplay_change_grandfestival",
        "gameplay_change_jealous_gods",
        "gameplay_change_global_labour",
        "gameplay_change_school_walkers",
        "gameplay_change_retire_at_60",
        "gameplay_change_fixed_workers",
        "gameplay_enable_extra_forts",
        "gameplay_wolves_block",
        "gameplay_dynamic_granaries",
        "gameplay_houses_stockpile_more",
        "gameplay_buyers_dont_distribute",
        "gameplay_change_immediate_delete",
        "gameplay_change_getting_granaries_go_offroad",
        "gameplay_change_granaries_get_double",
        "gameplay_change_tower_sentries_go_offroad",
        "gameplay_change_farms_deliver_close",
        "gameplay_change_only_deliver_to_accepting_granaries",
        "gameplay_change_all_houses_merge",
        "gameplay_change_wine_open_trade_route_counts",
        "gameplay_change_random_mine_or_pit_collapses_take_money",
        "gameplay_change_multiple_barracks",
        "gameplay_change_warehouses_dont_accept",
        "gameplay_change_houses_dont_expand_into_gardens",

};

static const char *ini_string_keys[] = {
        "ui_language_dir"
};

static int values[CONFIG_MAX_ENTRIES];
static char string_values[CONFIG_STRING_MAX_ENTRIES][CONFIG_STRING_VALUE_MAX];

#define CONFIG_DEFAULT_GP_FIX_IMMIGRATION_BUG 1
#define CONFIG_DEFAULT_GP_FIX_100_YEAR_GHOSTS 1
#define CONFIG_DEFAULT_GP_FIX_EDITOR_EVENTS 1
#define CONFIG_DEFAULT_UI_SIDEBAR_INFO 1
#define CONFIG_DEFAULT_UI_SHOW_INTRO_VIDEO 0
#define CONFIG_DEFAULT_UI_SMOOTH_SCROLLING 1
#define CONFIG_DEFAULT_UI_WALKER_WAYPOINTS 0
#define CONFIG_DEFAULT_UI_VISUAL_FEEDBACK_ON_DELETE 1
#define CONFIG_DEFAULT_UI_SHOW_WATER_STRUCTURE_RANGE 1
#define CONFIG_DEFAULT_UI_SHOW_CONSTRUCTION_SIZE 1
#define CONFIG_DEFAULT_UI_ZOOM 1
#define CONFIG_DEFAULT_UI_COMPLETE_RATING_COLUMNS 0
#define CONFIG_DEFAULT_UI_HIGHLIGHT_LEGIONS 1
#define CONFIG_DEFAULT_UI_ROTATE_MANUALLY 0
#define CONFIG_DEFAULT_GP_CH_GRANDFESTIVAL 0
#define CONFIG_DEFAULT_GP_CH_JEALOUS_GODS 0
#define CONFIG_DEFAULT_GP_CH_GLOBAL_LABOUR 0
#define CONFIG_DEFAULT_GP_CH_SCHOOL_WALKERS 0
#define CONFIG_DEFAULT_GP_CH_RETIRE_AT_60 0
#define CONFIG_DEFAULT_GP_CH_FIXED_WORKERS 0
#define CONFIG_DEFAULT_GP_CH_EXTRA_FORTS 0
#define CONFIG_DEFAULT_GP_CH_WOLVES_BLOCK 0
#define CONFIG_DEFAULT_GP_CH_DYNAMIC_GRANARIES 0
#define CONFIG_DEFAULT_GP_CH_MORE_STOCKPILE 0
#define CONFIG_DEFAULT_GP_CH_NO_BUYER_DISTRIBUTION 0
#define CONFIG_DEFAULT_GP_CH_IMMEDIATELY_DELETE_BUILDINGS 0
#define CONFIG_DEFAULT_GP_CH_GETTING_GRANARIES_GO_OFFROAD 0
#define CONFIG_DEFAULT_GP_CH_GRANARIES_GET_DOUBLE 0
#define CONFIG_DEFAULT_GP_CH_TOWER_SENTRIES_GO_OFFROAD 0
#define CONFIG_DEFAULT_GP_CH_FARMS_DELIVER_CLOSE 0
#define CONFIG_DEFAULT_GP_CH_DELIVER_ONLY_TO_ACCEPTING_GRANARIES 0
#define CONFIG_DEFAULT_GP_CH_ALL_HOUSES_MERGE 0
#define CONFIG_DEFAULT_GP_CH_WINE_COUNTS_IF_OPEN_TRADE_ROUTE 0
#define CONFIG_DEFAULT_GP_CH_RANDOM_COLLAPSES_TAKE_MONEY 0
#define CONFIG_DEFAULT_GP_CH_MULTIPLE_BARRACKS 0
#define CONFIG_DEFAULT_GP_CH_WAREHOUSES_DONT_ACCEPT 0
#define CONFIG_DEFAULT_GP_CH_HOUSES_DONT_EXPAND_INTO_GARDENS 0

static int default_values[CONFIG_MAX_ENTRIES] = {
        CONFIG_DEFAULT_GP_FIX_IMMIGRATION_BUG,
        CONFIG_DEFAULT_GP_FIX_100_YEAR_GHOSTS,
        CONFIG_DEFAULT_GP_FIX_EDITOR_EVENTS,
        CONFIG_DEFAULT_UI_SIDEBAR_INFO,
        CONFIG_DEFAULT_UI_SHOW_INTRO_VIDEO,
        CONFIG_DEFAULT_UI_SMOOTH_SCROLLING,
        CONFIG_DEFAULT_UI_WALKER_WAYPOINTS,
        CONFIG_DEFAULT_UI_VISUAL_FEEDBACK_ON_DELETE,
        CONFIG_DEFAULT_UI_SHOW_WATER_STRUCTURE_RANGE,
        CONFIG_DEFAULT_UI_SHOW_CONSTRUCTION_SIZE,
        CONFIG_DEFAULT_UI_ZOOM,
        CONFIG_DEFAULT_UI_COMPLETE_RATING_COLUMNS,
        CONFIG_DEFAULT_UI_HIGHLIGHT_LEGIONS,
        CONFIG_DEFAULT_UI_ROTATE_MANUALLY,
        CONFIG_DEFAULT_GP_CH_GRANDFESTIVAL,
        CONFIG_DEFAULT_GP_CH_JEALOUS_GODS,
        CONFIG_DEFAULT_GP_CH_GLOBAL_LABOUR,
        CONFIG_DEFAULT_GP_CH_SCHOOL_WALKERS,
        CONFIG_DEFAULT_GP_CH_RETIRE_AT_60,
        CONFIG_DEFAULT_GP_CH_FIXED_WORKERS,
        CONFIG_DEFAULT_GP_CH_EXTRA_FORTS,
        CONFIG_DEFAULT_GP_CH_WOLVES_BLOCK,
        CONFIG_DEFAULT_GP_CH_DYNAMIC_GRANARIES,
        CONFIG_DEFAULT_GP_CH_MORE_STOCKPILE,
        CONFIG_DEFAULT_GP_CH_NO_BUYER_DISTRIBUTION,
        CONFIG_DEFAULT_GP_CH_IMMEDIATELY_DELETE_BUILDINGS,
        CONFIG_DEFAULT_GP_CH_GETTING_GRANARIES_GO_OFFROAD,
        CONFIG_DEFAULT_GP_CH_GRANARIES_GET_DOUBLE,
        CONFIG_DEFAULT_GP_CH_TOWER_SENTRIES_GO_OFFROAD,
        CONFIG_DEFAULT_GP_CH_FARMS_DELIVER_CLOSE,
        CONFIG_DEFAULT_GP_CH_DELIVER_ONLY_TO_ACCEPTING_GRANARIES,
        CONFIG_DEFAULT_GP_CH_ALL_HOUSES_MERGE,
        CONFIG_DEFAULT_GP_CH_WINE_COUNTS_IF_OPEN_TRADE_ROUTE,
        CONFIG_DEFAULT_GP_CH_RANDOM_COLLAPSES_TAKE_MONEY,
        CONFIG_DEFAULT_GP_CH_MULTIPLE_BARRACKS,
        CONFIG_DEFAULT_GP_CH_WAREHOUSES_DONT_ACCEPT,
        CONFIG_DEFAULT_GP_CH_HOUSES_DONT_EXPAND_INTO_GARDENS
};

static char default_string_values[CONFIG_STRING_MAX_ENTRIES][CONFIG_STRING_VALUE_MAX];

int config_get(int key) {
    return values[key];
}
void config_set(int key, int value) {
    values[key] = value;
}

const char *config_get_string(int key) {
    return string_values[key];
}
void config_set_string(int key, const char *value) {
    if (!value)
        string_values[key][0] = 0;
    else {
        strncpy(string_values[key], value, CONFIG_STRING_VALUE_MAX - 1);
    }
}
int config_get_default_value(int key) {
    return default_values[key];
}
const char *config_get_default_string_value(int key) {
    return default_string_values[key];
}

void config_set_defaults(void) {
    for (int i = 0; i < CONFIG_MAX_ENTRIES; ++i) {
        values[i] = default_values[i];
    }
    strncpy(string_values[CONFIG_STRING_UI_LANGUAGE_DIR], default_string_values[CONFIG_STRING_UI_LANGUAGE_DIR],
            CONFIG_STRING_VALUE_MAX);
}
void config_load(void) {
    config_set_defaults();
    FILE *fp = file_open(INI_FILENAME, "rt");
    if (!fp)
        return;
    char line_buffer[MAX_LINE];
    char *line;
    while ((line = fgets(line_buffer, MAX_LINE, fp))) {
        // Remove newline from string
        size_t size = strlen(line);
        while (size > 0 && (line[size - 1] == '\n' || line[size - 1] == '\r')) {
            line[--size] = 0;
        }
        char *equals = strchr(line, '=');
        if (equals) {
            *equals = 0;
            for (int i = 0; i < CONFIG_MAX_ENTRIES; i++) {
                if (strcmp(ini_keys[i], line) == 0) {
                    int value = atoi(&equals[1]);
                    log_info("Config key", ini_keys[i], value);
                    values[i] = value;
                    break;
                }
            }
            for (int i = 0; i < CONFIG_STRING_MAX_ENTRIES; i++) {
                if (strcmp(ini_string_keys[i], line) == 0) {
                    const char *value = &equals[1];
                    log_info("Config key", ini_string_keys[i], 0);
                    log_info("Config value", value, 0);
                    strncpy(string_values[i], value, CONFIG_STRING_VALUE_MAX);
                    break;
                }
            }
        }
    }
    file_close(fp);
}
void config_save(void) {
    FILE *fp = file_open(INI_FILENAME, "wt");
    if (!fp) {
        log_error("Unable to write configuration file", INI_FILENAME, 0);
        return;
    }
    for (int i = 0; i < CONFIG_MAX_ENTRIES; i++) {
        fprintf(fp, "%s=%d\n", ini_keys[i], values[i]);
    }
    for (int i = 0; i < CONFIG_STRING_MAX_ENTRIES; i++) {
        fprintf(fp, "%s=%s\n", ini_string_keys[i], string_values[i]);
    }
    file_close(fp);
}