#include "config.h"

#include "io/file.h"
#include "io/log.h"

#include <string.h>

#define MAX_LINE 100

static const char *INI_FILENAME = "augustus.ini";

// Keep this in the same order as the ints in config.h
static const struct {
    const char *name;
    const bool enabled;
} ini_keys_defaults[CONFIG_MAX_ENTRIES] = {
        {"gameplay_fix_immigration", true},
        {"gameplay_fix_100y_ghosts", true},
        {"gameplay_fix_editor_events", true},
        {"ui_sidebar_info", true},
        {"ui_show_intro_video", false},
        {"ui_smooth_scrolling", true},
        {"ui_walker_waypoints", false},
        {"ui_visual_feedback_on_delete", true},
        {"ui_show_water_structure_range", true},
        {"ui_show_construction_size", true},
        {"ui_zoom", true},
        {"ui_complete_ratings_columns", false},
        {"ui_highlight_legions", true},
        {"ui_rotate_manually", false},
        {"gameplay_change_grandfestival", false},
        {"gameplay_change_jealous_gods", false},
        {"gameplay_change_global_labour", false},
        {"gameplay_change_school_walkers", false},
        {"gameplay_change_retire_at_60", false},
        {"gameplay_change_fixed_workers", false},
        {"gameplay_enable_extra_forts", false},
        {"gameplay_wolves_block", false},
        {"gameplay_dynamic_granaries", false},
        {"gameplay_houses_stockpile_more", false},
        {"gameplay_buyers_dont_distribute", true},
        {"gameplay_change_immediate_delete", false},
        {"gameplay_change_getting_granaries_go_offroad", false},
        {"gameplay_change_granaries_get_double", false},
        {"gameplay_change_tower_sentries_go_offroad", false},
        {"gameplay_change_farms_deliver_close", false},
        {"gameplay_change_only_deliver_to_accepting_granaries", false},
        {"gameplay_change_all_houses_merge", false},
        {"gameplay_change_wine_open_trade_route_counts", false},
        {"gameplay_change_random_mine_or_pit_collapses_take_money", false},
        {"gameplay_change_multiple_barracks", false},
        {"gameplay_change_warehouses_dont_accept", false},
        {"gameplay_change_houses_dont_expand_into_gardens", false},

        ///

        {"gameplay_fix_irrigation_range", true},
        {"gameplay_fix_farm_produce_quantity", true},
        {"ui_keep_camera_inertia", true},
        {"gameplay_change_watercarriers_firefight", true},
        {"gameplay_change_cart_instant_unload", false},
        {"gameplay_change_understaffed_accept_goods", false},
        {"gameplay_change_multiple_temple_complexes", false},
        {"gameplay_change_multiple_monuments", false},
        {"gameplay_change_soil_depletion", true},
        {"gameplay_change_multiple_gatherers", false},
};

static const char *ini_string_keys[] = {
        "ui_language_dir"
};

static bool values[CONFIG_MAX_ENTRIES];
static char string_values[CONFIG_STRING_MAX_ENTRIES][CONFIG_STRING_VALUE_MAX];
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
bool config_get_default_value(int key) {
    return ini_keys_defaults[key].enabled;
}
const char *config_get_default_string_value(int key) {
    return default_string_values[key];
}

void config_set_defaults(void) {
    for (int i = 0; i < CONFIG_MAX_ENTRIES; ++i) {
        values[i] = ini_keys_defaults[i].enabled;
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
                if (strcmp(ini_keys_defaults[i].name, line) == 0) {
                    int value = atoi(&equals[1]);
                    log_info("Config key", ini_keys_defaults[i].name, value);
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
    for (int i = 0; i < CONFIG_MAX_ENTRIES; i++)
        fprintf(fp, "%s=%d\n", ini_keys_defaults[i].name, values[i]);
    for (int i = 0; i < CONFIG_STRING_MAX_ENTRIES; i++)
        fprintf(fp, "%s=%s\n", ini_string_keys[i], string_values[i]);
    file_close(fp);
}
