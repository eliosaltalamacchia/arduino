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

static void event_handler_cb_main_mode(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 7, 0, e);
    }
}

static void event_handler_cb_config_alarm_back(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    
    if (event == LV_EVENT_RELEASED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 7, 0, e);
    }
}

static void event_handler_cb_config_alarm_next(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    
    if (event == LV_EVENT_RELEASED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 9, 0, e);
    }
}

static void event_handler_cb_config_background_back_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    
    if (event == LV_EVENT_RELEASED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 2, 0, e);
    }
}

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
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
            lv_obj_add_state(obj, LV_STATE_DISABLED);
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
            lv_obj_set_pos(obj, 105, 187);
            lv_obj_set_size(obj, 32, 36);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_mode_up, NULL);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_mode_down, NULL);
            lv_obj_add_event_cb(obj, event_handler_cb_main_mode, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
        }
    }
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
}

void create_screen_config_alarm() {
    void *flowState = getFlowState(0, 1);
    lv_obj_t *obj = lv_obj_create(0);
    objects.config_alarm = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_add_state(obj, LV_STATE_DISABLED);
    {
        lv_obj_t *parent_obj = obj;
        {
            // background_1
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.background_1 = obj;
            lv_obj_set_pos(obj, 1, -1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_star_wars_bg_1);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // alarm
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.alarm = obj;
            lv_obj_set_pos(obj, 95, 31);
            lv_obj_set_size(obj, 50, 25);
        }
        {
            // sleepMode
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.sleep_mode = obj;
            lv_obj_set_pos(obj, 96, 191);
            lv_obj_set_size(obj, 50, 25);
        }
        {
            // modeTitle
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.mode_title = obj;
            lv_obj_set_pos(obj, 78, 171);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Sleep Mode");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // hourAlarm
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.hour_alarm = obj;
            lv_obj_set_pos(obj, 73, 65);
            lv_obj_set_size(obj, 44, 95);
            lv_roller_set_options(obj, "00\n01\n02", LV_ROLLER_MODE_NORMAL);
            lv_obj_set_style_text_font(obj, &ui_font_orbitron_bold_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffea890d), LV_PART_SELECTED | LV_STATE_DEFAULT);
        }
        {
            // minAlarm
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.min_alarm = obj;
            lv_obj_set_pos(obj, 124, 65);
            lv_obj_set_size(obj, 44, 95);
            lv_roller_set_options(obj, "00\n01\n02\n03", LV_ROLLER_MODE_NORMAL);
            lv_obj_set_style_text_font(obj, &ui_font_orbitron_bold_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffea890d), LV_PART_SELECTED | LV_STATE_DEFAULT);
        }
        {
            // back
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.back = obj;
            lv_obj_set_pos(obj, 13, 108);
            lv_obj_set_size(obj, 38, 25);
            lv_obj_add_event_cb(obj, event_handler_cb_config_alarm_back, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffea890d), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // backTitle
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_title = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "<<");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // next
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.next = obj;
            lv_obj_set_pos(obj, 186, 108);
            lv_obj_set_size(obj, 38, 25);
            lv_obj_add_event_cb(obj, event_handler_cb_config_alarm_next, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffea890d), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // nextTitle
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.next_title = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, ">>");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_config_alarm() {
    void *flowState = getFlowState(0, 1);
}

void create_screen_config_background() {
    void *flowState = getFlowState(0, 2);
    lv_obj_t *obj = lv_obj_create(0);
    objects.config_background = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_add_state(obj, LV_STATE_DISABLED);
    {
        lv_obj_t *parent_obj = obj;
        {
            // background_2
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.background_2 = obj;
            lv_obj_set_pos(obj, 1, -1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_star_wars_bg_1);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // back_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.back_1 = obj;
            lv_obj_set_pos(obj, 17, 108);
            lv_obj_set_size(obj, 38, 25);
            lv_obj_add_event_cb(obj, event_handler_cb_config_background_back_1, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffea890d), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // backTitle_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_title_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "<<");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 61, 78);
            lv_obj_set_size(obj, 121, 85);
            lv_roller_set_options(obj, "Background 1\nBackground 2\nBackground 3", LV_ROLLER_MODE_NORMAL);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffea890d), LV_PART_SELECTED | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_config_background() {
    void *flowState = getFlowState(0, 2);
}


static const char *screen_names[] = { "Main", "ConfigAlarm", "ConfigBackground" };
static const char *object_names[] = { "main", "config_alarm", "config_background", "mode", "back", "next", "back_1", "background", "seconds", "hour", "dots", "minutes", "date", "background_1", "alarm", "sleep_mode", "mode_title", "hour_alarm", "min_alarm", "back_title", "next_title", "background_2", "back_title_1", "obj0" };

void create_screens() {
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_config_alarm();
    create_screen_config_background();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_config_alarm,
    tick_screen_config_background,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
