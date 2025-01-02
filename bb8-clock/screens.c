#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_add_state(obj, LV_STATE_DISABLED);
    {
        lv_obj_t *parent_obj = obj;
        {
            // background
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.background = obj;
            lv_obj_set_pos(obj, 1, -1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_star_wars_bg_1);
        }
        {
            // seconds
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.seconds = obj;
            lv_obj_set_pos(obj, 6, 5);
            lv_obj_set_size(obj, 230, 230);
            lv_arc_set_range(obj, 0, 60);
            lv_arc_set_value(obj, 60);
            lv_arc_set_bg_start_angle(obj, 90);
            lv_arc_set_bg_end_angle(obj, 449);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_arc_color(obj, lv_color_hex(0xffea890d), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 3, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffecab22), LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_rounded(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // hour
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hour = obj;
            lv_obj_set_pos(obj, 20, 101);
            lv_obj_set_size(obj, 90, 39);
            lv_label_set_text(obj, "00");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffea890d), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_orbitron_bold_54, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // dots
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.dots = obj;
            lv_obj_set_pos(obj, 114, 101);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, ":");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffea890d), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_orbitron_bold_54, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // minutes
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.minutes = obj;
            lv_obj_set_pos(obj, 130, 101);
            lv_obj_set_size(obj, 90, 39);
            lv_label_set_text(obj, "00");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffea890d), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_orbitron_bold_54, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // date
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.date = obj;
            lv_obj_set_pos(obj, 75, 156);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "21/12/24");
            lv_obj_set_style_text_font(obj, &ui_font_orbitron_bold_20, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // mode
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            objects.mode = obj;
            lv_obj_set_pos(obj, 105, 22);
            lv_obj_set_size(obj, 32, 32);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_mode_up, NULL);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_mode_down, NULL);
            lv_obj_add_event_cb(obj, action_night_mode, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
        }
    }
}

void tick_screen_main() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
