#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *config_alarm;
    lv_obj_t *config_background;
    lv_obj_t *mode;
    lv_obj_t *back;
    lv_obj_t *next;
    lv_obj_t *back_1;
    lv_obj_t *background;
    lv_obj_t *seconds;
    lv_obj_t *hour;
    lv_obj_t *dots;
    lv_obj_t *minutes;
    lv_obj_t *date;
    lv_obj_t *background_1;
    lv_obj_t *alarm;
    lv_obj_t *sleep_mode;
    lv_obj_t *mode_title;
    lv_obj_t *hour_alarm;
    lv_obj_t *min_alarm;
    lv_obj_t *back_title;
    lv_obj_t *next_title;
    lv_obj_t *background_2;
    lv_obj_t *back_title_1;
    lv_obj_t *obj0;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_CONFIG_ALARM = 2,
    SCREEN_ID_CONFIG_BACKGROUND = 3,
};

void create_screen_main();
void tick_screen_main();

void create_screen_config_alarm();
void tick_screen_config_alarm();

void create_screen_config_background();
void tick_screen_config_background();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/