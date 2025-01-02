#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_star_wars_bg_1;
extern const lv_img_dsc_t img_star_wars_bg_2;
extern const lv_img_dsc_t img_star_wars_bg_3;
extern const lv_img_dsc_t img_alarm;
extern const lv_img_dsc_t img_mode_down;
extern const lv_img_dsc_t img_mode_up;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[6];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/