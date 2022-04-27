#include "map_editor.h"

#include "city/view.h"
#include "core/config.h"
#include "editor/tool.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/menu.h"
#include "graphics/window.h"
#include "input/scroll.h"
#include "input/zoom.h"
#include "map/figure.h"
#include "map/grid.h"
#include "map/image.h"
#include "map/point.h"
#include "map/property.h"
#include "sound/city.h"
#include "sound/effect.h"
#include "widget/map_editor_tool.h"
#include "widget/city/tile_draw.h"

static struct {
    map_point current_tile;
    int selected_grid_offset;
    int new_start_grid_offset;
    int capture_input;
} data;

static struct {
    time_millis last_water_animation_time;
    int advance_water_animation;

    int image_id_water_first;
    int image_id_water_last;

    int image_id_deepwater_first;
    int image_id_deepwater_last;
} draw_context;

static void init_draw_context(void) {
    draw_context.advance_water_animation = 0;
    time_millis now = time_get_millis();
    if (now - draw_context.last_water_animation_time > 60) {
        draw_context.last_water_animation_time = now;
        draw_context.advance_water_animation = 1;
    }
    draw_context.image_id_water_first = image_id_from_group(GROUP_TERRAIN_WATER);
    draw_context.image_id_water_last = 5 + draw_context.image_id_water_first;
    draw_context.image_id_deepwater_first = image_id_from_group(GROUP_TERRAIN_DEEPWATER);
    draw_context.image_id_deepwater_last = 89 + draw_context.image_id_deepwater_first;
}

static void draw_flags(int x, int y, int grid_offset) {
    int figure_id = map_figure_at(grid_offset);
    while (figure_id) {
        figure *f = figure_get(figure_id);
        if (!f->is_ghost)
            f->city_draw_figure(x, y, 0);

        if (figure_id != f->next_figure)
            figure_id = f->next_figure;
        else
            figure_id = 0;
    }
}

static void set_city_scaled_clip_rectangle(void) {
    int x, y, width, height;
    city_view_get_scaled_viewport(&x, &y, &width, &height);
    graphics_set_clip_rectangle(x, y, width, height);
}

static void update_zoom_level(void) {
    int zoom = city_view_get_scale();
    pixel_coordinate offset;
    city_view_get_camera_position(&offset.x, &offset.y);
    if (zoom_update_value(&zoom, &offset)) {
        city_view_set_scale(zoom);
        city_view_go_to_position(offset.x, offset.y, true);
        sound_city_decay_views();
    }
}

void widget_map_editor_draw(void) {
    if (config_get(CONFIG_UI_ZOOM)) {
        update_zoom_level();
        graphics_set_active_canvas(CANVAS_CITY);
    }
    set_city_scaled_clip_rectangle();

    init_draw_context();
    city_view_foreach_map_tile(draw_footprint);
    city_view_foreach_valid_map_tile(draw_flags, draw_top, 0);
    map_editor_tool_draw(&data.current_tile);

    graphics_set_active_canvas(CANVAS_UI);
}

static void update_city_view_coords(int x, int y, map_point *tile) {
    view_tile view;
    if (city_view_pixels_to_view_tile(x, y, &view)) {
        tile->grid_offset = city_view_tile_to_grid_offset(&view);
        city_view_set_selected_view_tile(&view);
        tile->x = map_grid_offset_to_x(tile->grid_offset);
        tile->y = map_grid_offset_to_y(tile->grid_offset);
    } else {
        tile->grid_offset = tile->x = tile->y = 0;
    }
}

static void scroll_map(const mouse *m) {
    pixel_coordinate delta;
    if (scroll_get_delta(m, &delta, SCROLL_TYPE_CITY)) {
        city_view_scroll(delta.x, delta.y);
        sound_city_decay_views();
    }
}

static int input_coords_in_map(int x, int y) {
    int x_offset, y_offset, width, height;
    city_view_get_unscaled_viewport(&x_offset, &y_offset, &width, &height);

    x -= x_offset;
    y -= y_offset;

    return (x >= 0 && x < width && y >= 0 && y < height);
}

static void handle_touch_scroll(const touch *t) {
    if (editor_tool_is_active()) {
        if (t->has_started) {
            int x_offset, y_offset, width, height;
            city_view_get_unscaled_viewport(&x_offset, &y_offset, &width, &height);
            scroll_set_custom_margins(x_offset, y_offset, width, height);
        }
        if (t->has_ended)
            scroll_restore_margins();

        return;
    }
    scroll_restore_margins();

    if (!data.capture_input)
        return;
    int was_click = touch_was_click(get_latest_touch());
    if (t->has_started || was_click) {
        scroll_drag_start(1);
        return;
    }

    if (!touch_not_click(t))
        return;

    if (t->has_ended)
        scroll_drag_end();

}

static void handle_touch_zoom(const touch *first, const touch *last) {
    if (touch_not_click(first))
        zoom_update_touch(first, last, city_view_get_scale());

    if (first->has_ended || last->has_ended)
        zoom_end_touch();

}

static void handle_last_touch(void) {
    const touch *last = get_latest_touch();
    if (!last->in_use)
        return;
    if (touch_was_click(last)) {
        editor_tool_deactivate();
        return;
    }
    if (touch_not_click(last))
        handle_touch_zoom(get_earliest_touch(), last);

}

static bool handle_cancel_construction_button(const touch *t) {
    if (!editor_tool_is_active())
        return false;

    int x, y, width, height;
    city_view_get_unscaled_viewport(&x, &y, &width, &height);
    int box_size = 5 * 16;
    width -= box_size;

    if (t->current_point.x < width || t->current_point.x >= width + box_size ||
        t->current_point.y < 24 || t->current_point.y >= 40 + box_size) {
        return false;
    }
    editor_tool_deactivate();
    return true;
}

static void handle_first_touch(map_point *tile) {
    const touch *first = get_earliest_touch();

    if (touch_was_click(first)) {
        if (handle_cancel_construction_button(first))
            return;
    }

    handle_touch_scroll(first);

    if (!input_coords_in_map(first->current_point.x, first->current_point.y))
        return;

    if (editor_tool_is_updatable()) {
        if (!editor_tool_is_in_use()) {
            if (first->has_started) {
                editor_tool_start_use(tile);
                data.new_start_grid_offset = 0;
            }
        } else {
            if (first->has_started) {
                if (data.selected_grid_offset != tile->grid_offset)
                    data.new_start_grid_offset = tile->grid_offset;

            }
            if (touch_not_click(first) && data.new_start_grid_offset) {
                data.new_start_grid_offset = 0;
                data.selected_grid_offset = 0;
                editor_tool_deactivate();
                editor_tool_start_use(tile);
            }
            editor_tool_update_use(tile);
            if (data.selected_grid_offset != tile->grid_offset)
                data.selected_grid_offset = 0;

            if (first->has_ended) {
                if (data.selected_grid_offset == tile->grid_offset) {
                    editor_tool_end_use(tile);
                    widget_map_editor_clear_current_tile();
                    data.new_start_grid_offset = 0;
                } else {
                    data.selected_grid_offset = tile->grid_offset;
                }
            }
        }
        return;
    }

    if (editor_tool_is_brush()) {
        if (first->has_started)
            editor_tool_start_use(tile);

        editor_tool_update_use(tile);
        if (first->has_ended)
            editor_tool_end_use(tile);

        return;
    }

    if (touch_was_click(first) && first->has_ended && data.capture_input &&
        data.selected_grid_offset == tile->grid_offset) {
        editor_tool_start_use(tile);
        editor_tool_update_use(tile);
        editor_tool_end_use(tile);
        widget_map_editor_clear_current_tile();
    } else if (first->has_ended)
        data.selected_grid_offset = tile->grid_offset;

}

static void handle_touch(void) {
    const touch *first = get_earliest_touch();
    if (!first->in_use) {
        scroll_restore_margins();
        return;
    }

    map_point *tile = &data.current_tile;
    if (!editor_tool_is_in_use() || input_coords_in_map(first->current_point.x, first->current_point.y))
        update_city_view_coords(first->current_point.x, first->current_point.y, tile);


    if (first->has_started && input_coords_in_map(first->current_point.x, first->current_point.y)) {
        data.capture_input = 1;
        scroll_restore_margins();
    }

    handle_last_touch();
    handle_first_touch(tile);

    if (first->has_ended)
        data.capture_input = 0;

}

void widget_map_editor_handle_input(const mouse *m, const hotkeys *h) {
    scroll_map(m);

    if (m->is_touch) {
        zoom_map(m);
        handle_touch();
    } else {
        if (m->right.went_down && input_coords_in_map(m->x, m->y) && !editor_tool_is_active())
            scroll_drag_start(0);

        if (m->right.went_up) {
            if (!editor_tool_is_active()) {
                int has_scrolled = scroll_drag_end();
                if (!has_scrolled)
                    editor_tool_deactivate();

            } else {
                editor_tool_deactivate();
            }
        }
    }

    if (h->escape_pressed) {
        if (editor_tool_is_active())
            editor_tool_deactivate();
        else {
            hotkey_handle_escape();
        }
        return;
    }

    map_point *tile = &data.current_tile;
    update_city_view_coords(m->x, m->y, tile);

    if (!tile->grid_offset)
        return;

    if (m->left.went_down) {
        if (!editor_tool_is_in_use())
            editor_tool_start_use(tile);

        editor_tool_update_use(tile);
    } else if (m->left.is_down || editor_tool_is_in_use())
        editor_tool_update_use(tile);

    if (m->left.went_up) {
        editor_tool_end_use(tile);
        sound_effect_play(SOUND_EFFECT_BUILD);
    }
}

void widget_map_editor_clear_current_tile(void) {
    data.selected_grid_offset = 0;
    data.current_tile.grid_offset = 0;
}
