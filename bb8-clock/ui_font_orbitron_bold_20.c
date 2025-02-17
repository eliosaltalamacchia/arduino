/*******************************************************************************
 * Size: 20 px
 * Bpp: 8
 * Opts: undefined
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl.h"
#endif

#ifndef UI_FONT_ORBITRON_BOLD_20
#define UI_FONT_ORBITRON_BOLD_20 1
#endif

#if UI_FONT_ORBITRON_BOLD_20

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+002F "/" */
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x7, 0xc9, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x9c, 0xff, 0x60, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x64, 0xff, 0xff,
    0x5e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x36,
    0xf9, 0xff, 0xd2, 0xa, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x15, 0xe3, 0xff, 0xee, 0x23, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x4, 0xbf, 0xff, 0xfe, 0x49,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8a, 0xff,
    0xff, 0x7d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x55, 0xff, 0xff, 0xb3, 0x2, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x2a, 0xf4, 0xff, 0xdb, 0x10, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xe, 0xda, 0xff, 0xf4,
    0x2c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xb0,
    0xff, 0xff, 0x56, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x7c, 0xff, 0xff, 0x8b, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xe0, 0xff, 0xbf, 0x4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0,
    0xe3, 0x16, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0xbf, 0x36, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0,

    /* U+0030 "0" */
    0x3, 0x8b, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xd4, 0x3a, 0x0, 0x77,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xee, 0x14, 0xd0, 0xff,
    0xf6, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8,
    0xe6, 0xff, 0xff, 0xff, 0x5c, 0xdc, 0xff, 0xc4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4c, 0xf4,
    0xff, 0xff, 0xff, 0x68, 0xdc, 0xff, 0xc4, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x68, 0xfc, 0xff, 0xff,
    0xff, 0xff, 0x68, 0xdc, 0xff, 0xc4, 0x0, 0x0,
    0x0, 0x1, 0x89, 0xff, 0xff, 0xff, 0xbe, 0xff,
    0xff, 0x68, 0xdc, 0xff, 0xc4, 0x0, 0x0, 0x8,
    0xaa, 0xff, 0xff, 0xfe, 0x72, 0x38, 0xff, 0xff,
    0x68, 0xdc, 0xff, 0xc4, 0x0, 0x13, 0xc3, 0xff,
    0xff, 0xf7, 0x53, 0x0, 0x38, 0xff, 0xff, 0x68,
    0xdc, 0xff, 0xc4, 0x22, 0xd9, 0xff, 0xff, 0xea,
    0x38, 0x0, 0x0, 0x38, 0xff, 0xff, 0x68, 0xdc,
    0xff, 0xe3, 0xea, 0xff, 0xff, 0xd9, 0x22, 0x0,
    0x0, 0x0, 0x38, 0xff, 0xff, 0x68, 0xdc, 0xff,
    0xff, 0xff, 0xff, 0xc4, 0x13, 0x0, 0x0, 0x0,
    0x0, 0x38, 0xff, 0xff, 0x68, 0xdc, 0xff, 0xff,
    0xff, 0xaa, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x38, 0xff, 0xff, 0x68, 0xcc, 0xff, 0xff, 0xff,
    0xdd, 0xdc, 0xdc, 0xdc, 0xdc, 0xdc, 0xdc, 0xe7,
    0xff, 0xff, 0x57, 0x70, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xeb, 0xf, 0x2, 0x89, 0xf3, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd4, 0x35,
    0x0,

    /* U+0031 "1" */
    0x0, 0x0, 0x0, 0x1f, 0xeb, 0xff, 0xff, 0x44,
    0x0, 0x0, 0xa, 0xcf, 0xff, 0xff, 0xff, 0x44,
    0x0, 0x1, 0xa9, 0xff, 0xff, 0xff, 0xff, 0x44,
    0x0, 0x76, 0xff, 0xff, 0xff, 0xff, 0xff, 0x44,
    0x49, 0xfd, 0xff, 0xf9, 0x96, 0xff, 0xff, 0x44,
    0x52, 0x64, 0x64, 0x3f, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,
    0x0, 0x0, 0x0, 0x0, 0x60, 0xff, 0xff, 0x44,

    /* U+0032 "2" */
    0x5, 0x92, 0xf4, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xd7, 0x40, 0x0, 0x81,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf4, 0x1b, 0xd3, 0xff,
    0xf2, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
    0xb8, 0xcb, 0xff, 0xff, 0x5f, 0xd2, 0xf4, 0xbb,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x38, 0xff, 0xff, 0x68, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38,
    0xff, 0xff, 0x68, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0xff,
    0xff, 0x68, 0x0, 0x49, 0xab, 0xb8, 0xb8, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xcb, 0xff, 0xff,
    0x5e, 0x51, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf2, 0x18,
    0xc5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xd6, 0x3e, 0x0, 0xdc,
    0xff, 0xc8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xdc, 0xff,
    0xc4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0xdc, 0xff, 0xc4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xdc, 0xff, 0xf2, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
    0xb8, 0xb8, 0x4b, 0xdc, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x68, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x68,

    /* U+0033 "3" */
    0xa, 0xa1, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xef, 0x85, 0x1, 0x0, 0x98,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x69, 0x0, 0xe6, 0xff,
    0xed, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
    0xb8, 0xf9, 0xff, 0xba, 0x0, 0xe1, 0xf0, 0xa5,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0xdc, 0xff, 0xc4, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xdc,
    0xff, 0xc4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xdf, 0xff,
    0xc4, 0x0, 0x0, 0x0, 0x74, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb2,
    0x0, 0x0, 0x0, 0x74, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xea, 0xd,
    0x0, 0x0, 0x53, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
    0xb8, 0xb8, 0xb8, 0xd0, 0xff, 0xff, 0x4a, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x48, 0xff, 0xff, 0x58, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x48, 0xff, 0xff, 0x58, 0xb4, 0xc0, 0x84,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x48, 0xff, 0xff, 0x58, 0xe6, 0xff, 0xed, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xd0,
    0xff, 0xff, 0x4d, 0x93, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xec, 0xf, 0x9, 0x9d, 0xf6, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd0, 0x34,
    0x0,

    /* U+0034 "4" */
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x42, 0xf2, 0xff, 0xcc, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x52, 0xf8, 0xff,
    0xff, 0xcc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x65, 0xfd, 0xff, 0xff, 0xff, 0xcc,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7a,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xcc, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x1, 0x8f, 0xff, 0xff, 0xff,
    0xa0, 0xd6, 0xff, 0xcc, 0x0, 0x0, 0x0, 0x0,
    0x5, 0xa3, 0xff, 0xff, 0xfe, 0x79, 0x0, 0xd0,
    0xff, 0xcc, 0x0, 0x0, 0x0, 0xa, 0xb5, 0xff,
    0xff, 0xf6, 0x54, 0x0, 0x0, 0xd0, 0xff, 0xcc,
    0x0, 0x0, 0x11, 0xc5, 0xff, 0xff, 0xe6, 0x35,
    0x0, 0x0, 0x0, 0xd0, 0xff, 0xcc, 0x0, 0x0,
    0xc3, 0xff, 0xff, 0xff, 0xd5, 0xbc, 0xbc, 0xbc,
    0xbc, 0xf3, 0xff, 0xf2, 0xbc, 0x99, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xd0, 0xe0, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xd0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xd0, 0xff, 0xcc, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0xd0, 0xff, 0xcc, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd0,
    0xff, 0xcc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0xd0, 0xff, 0xcc,
    0x0, 0x0,

    /* U+0035 "5" */
    0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x68, 0xdc,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x68, 0xdc, 0xff,
    0xf2, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0x4b, 0xdc, 0xff, 0xc4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xdc, 0xff, 0xc4, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xdc, 0xff, 0xc7, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd7, 0x42,
    0x0, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x18,
    0xa2, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
    0xbc, 0xbc, 0xbc, 0xce, 0xff, 0xff, 0x5e, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x38, 0xff, 0xff, 0x68, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x38, 0xff, 0xff, 0x68, 0xb6, 0xd4, 0xa2,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x38, 0xff, 0xff, 0x68, 0xd2, 0xff, 0xf2, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xcb,
    0xff, 0xff, 0x5e, 0x7d, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf1, 0x18, 0x4, 0x8e, 0xf3, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd6, 0x3d,
    0x0,

    /* U+0036 "6" */
    0x5, 0x93, 0xf4, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x20, 0x0, 0x0, 0x80,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x20, 0x0, 0x0, 0xd2, 0xff,
    0xf2, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
    0xb8, 0xb8, 0x17, 0x0, 0x0, 0xdc, 0xff, 0xc4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0xdc, 0xff, 0xc4, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0xdc, 0xff, 0xc7, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd7, 0x42,
    0x0, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x18,
    0xdc, 0xff, 0xf0, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
    0xbc, 0xbc, 0xbc, 0xce, 0xff, 0xff, 0x5e, 0xdc,
    0xff, 0xc4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x38, 0xff, 0xff, 0x68, 0xdc, 0xff,
    0xc4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x38, 0xff, 0xff, 0x68, 0xdc, 0xff, 0xc4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x38, 0xff, 0xff, 0x68, 0xd2, 0xff, 0xf2, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xcb,
    0xff, 0xff, 0x5e, 0x7e, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf1, 0x18, 0x4, 0x91, 0xf3, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd6, 0x3d,
    0x0,

    /* U+0037 "7" */
    0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xfa, 0xb1, 0x13, 0x0, 0xf0, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xb2, 0x0, 0xa9, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4,
    0xb4, 0xb4, 0xb4, 0xe4, 0xff, 0xfd, 0x6, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x94, 0xff, 0xff, 0xc, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x94, 0xff, 0xff,
    0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x94, 0xff, 0xff, 0xc, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x94,
    0xff, 0xff, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x94, 0xff, 0xff, 0xc,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x94, 0xff, 0xff, 0xc, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x94, 0xff,
    0xff, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x94, 0xff, 0xff, 0xc, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x94, 0xff, 0xff, 0xc, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x94, 0xff, 0xff,
    0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x94, 0xff, 0xff, 0xc, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x94,
    0xff, 0xff, 0xc,

    /* U+0038 "8" */
    0x3, 0x8d, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xd5, 0x3d, 0x0, 0x77,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf4, 0x1d, 0xcf, 0xff,
    0xf4, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
    0xbc, 0xcf, 0xff, 0xff, 0x64, 0xdc, 0xff, 0xc4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x38, 0xff, 0xff, 0x68, 0xdc, 0xff, 0xc4, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38,
    0xff, 0xff, 0x68, 0xdc, 0xff, 0xc4, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0xff,
    0xff, 0x68, 0xcf, 0xff, 0xe6, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0xa1, 0xff, 0xff,
    0x5a, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1c,
    0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x52, 0xdc,
    0xff, 0xc7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x3b, 0xff, 0xff, 0x68, 0xdc, 0xff,
    0xc4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x38, 0xff, 0xff, 0x68, 0xdc, 0xff, 0xc4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x38, 0xff, 0xff, 0x68, 0xd4, 0xff, 0xf2, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xcb,
    0xff, 0xff, 0x60, 0x84, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf4, 0x1c, 0x5, 0x93, 0xf5, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x41,
    0x0,

    /* U+0039 "9" */
    0xa, 0xa1, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xd0, 0x37, 0x0, 0x97,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xed, 0xf, 0xe6, 0xff,
    0xee, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
    0xb8, 0xd0, 0xff, 0xff, 0x4d, 0xf0, 0xff, 0xb4,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x48, 0xff, 0xff, 0x58, 0xf0, 0xff, 0xb4, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x48,
    0xff, 0xff, 0x58, 0xf0, 0xff, 0xb4, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x48, 0xff,
    0xff, 0x58, 0xe6, 0xff, 0xef, 0xb8, 0xb8, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xcc, 0xff, 0xff,
    0x58, 0x93, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x58,
    0x9, 0x9e, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x58, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x4c, 0xff, 0xff, 0x58, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x48, 0xff, 0xff, 0x58, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x48, 0xff, 0xff, 0x58, 0x9c, 0xb8, 0xb8, 0xb8,
    0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xd0,
    0xff, 0xff, 0x4d, 0x78, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xec, 0xf, 0x3, 0x8d, 0xf4, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd0, 0x34,
    0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 167, .box_w = 11, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 165, .adv_w = 267, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 125, .box_w = 8, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 510, .adv_w = 266, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 735, .adv_w = 264, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 960, .adv_w = 234, .box_w = 14, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1170, .adv_w = 266, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1395, .adv_w = 262, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1620, .adv_w = 211, .box_w = 13, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1815, .adv_w = 267, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2040, .adv_w = 265, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 47, .range_length = 11, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 8,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_orbitron_bold_20 = {
#else
lv_font_t ui_font_orbitron_bold_20 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_ORBITRON_BOLD_20*/

