#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <stddef.h>
#include <extra_calls.h>
#include <string.h>
#include "prdefinitions.h"
#include <preader/reader.h>
#include <preader/ui.h>

extern SessionConfig cfg;

void callback_setpreview(complexMenuItem *menu, int idx, int val);
color_t rgb565_to_colort(int r, int g, int b);
color_t preset_color_chooser(color_t current_color);

typedef struct _preset_color {
    int id;
    char *name;
    color_t color;
} presetColor;

typedef struct _preset_palette {
    int id;
    char *name;
    int n_colors;
    color_t *colors;
} presetPalette;

color_t pp_color_reader_01[] = {COLOR_WHITE,COLOR_BLACK};
color_t pp_color_reader_02[] = {COLOR_BLACK,COLOR_WHITE};
color_t pp_color_reader_03[] = {54835,COLOR_BLACK};
color_t pp_color_reader_04[] = {6539,COLOR_WHITE};

presetPalette palettes_reader[] = {{0,"常规",2,pp_color_reader_01},
                                    {1,"夜间",2,pp_color_reader_02},
                                    {2,"纸张",2,pp_color_reader_03},
                                    {3,"深蓝",2,pp_color_reader_04}};
#define N_PALETTES_READER 4

color_t pp_color_ui_pink[] = {65405,60115,COLOR_BLACK,COLOR_WHITE,45577,COLOR_WHITE,COLOR_BLACK,COLOR_GRAY};
color_t pp_color_ui_classic[] = {COLOR_WHITE,COLOR_CYAN,COLOR_BLACK,COLOR_RED,COLOR_GRAY,COLOR_WHITE,COLOR_BLACK,COLOR_GRAY};

presetPalette palettes_ui[] = {{0,"b站粉",10,pp_color_ui_pink},
                                {1,"经典",10,pp_color_ui_classic}};
#define N_PALETTES_UI     2

#define N_PRESET_COLORS 16
presetColor preset_colors[N_PRESET_COLORS] = {
    0,"踯躅色",60115,
    1,"樱色",65405,
    2,"黑色",0,
    3,"白色",65535,
    4,"水色",44796,
    5,"胭脂色",45577,
    6,"青色",COLOR_CYAN,
    7,"红色",COLOR_RED,
    8,"青绿色",1395,
    9,"露草色",13595,
    10,"浅缥色",9044,
    11,"绀青色",6539,
    12,"赤橙色",62148,
    13,"御纳户色",1074,
    14,"绿色",COLOR_GREEN,
    15,"蓝色",COLOR_BLUE
};

color_t pick_a_color(color_t current_color) {
    int r,g,b;
    DisplayStatusArea();
    colort_to_rgb565(current_color,&r,&g,&b);
    complexMenuItem colorChooser[7] = {0};
    sliderProperties colorConstraintRB = {0,31,1,20,50,1,1};
    sliderProperties colorConstraintG = {0,63,1,20,50,1,1};
    register_menuitem_complex(&colorChooser[0],1,5,0,"R",r);
    register_menuitem_complex(&colorChooser[1],1,5,0,"G",g);
    register_menuitem_complex(&colorChooser[2],1,5,0,"B",b);
    colorChooser[0].item_based_properties = &colorConstraintRB;
    colorChooser[1].item_based_properties = &colorConstraintG;
    colorChooser[2].item_based_properties = &colorConstraintRB;
    register_menuitem_complex(&colorChooser[3],0,6,0,"预览",current_color);
    register_menuitem_complex(&colorChooser[4],1,3,0,"选用预设色",0);
    register_menuitem_complex(&colorChooser[5],1,0,0,"确定",0);
    register_menuitem_complex(&colorChooser[6],1,0,0,"取消",0);
    while (1) {
    drawDialog(30,5+24,30+300-1,5+24+6*26);
    int ret = flexibleMenu_complex(30,5,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,300,2,6,colorChooser,6,4,1,0,NULL,callback_setpreview);
    if (ret == 5) return colorChooser[3].value;
    else if (ret == 4) {
        color_t the_color = preset_color_chooser(colorChooser[3].value);
        colorChooser[3].value = the_color;
        colort_to_rgb565(the_color,&r,&g,&b);
        colorChooser[0].value = r;
        colorChooser[1].value = g;
        colorChooser[2].value = b;
    } else return current_color;
    }   
}

void colort_to_rgb565(color_t color, int *r, int *g, int *b) {
    *r = (color>>11)&31;
    *g = (color>>5)&63;
    *b = color&31;
}

color_t rgb565_to_colort(int r, int g, int b) {
    color_t res = 0;
    res |= r;
    res <<= 6;
    res |= g;
    res <<= 5;
    res |= b;
    return res;
}

void callback_setpreview(complexMenuItem *menu, int idx, int val) {
    menu[3].value = rgb565_to_colort(menu[0].value,menu[1].value,menu[2].value);
}

color_t preset_color_chooser(color_t current_color) {
    complexMenuItem presetColors[N_PRESET_COLORS];
    for (int i=0; i<N_PRESET_COLORS; i++) {
        register_menuitem_complex(&presetColors[i],1,6,0,preset_colors[i].name,preset_colors[i].color);
    }
    drawDialog(30,5+24,30+300-1,5+24+6*26);
    int ret = flexibleMenu_complex(30,5,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,300,2,N_PRESET_COLORS,presetColors,6,0,1,1,NULL,NULL);
    if (ret == -1) return current_color;
    else return preset_colors[ret].color;
}

int preset_chooser(presetPalette *palettes, int n_palettes, color_t *dest) {
    complexMenuItem presetPalettes[n_palettes];
    for (int i=0; i<n_palettes; i++) {
        register_menuitem_complex(&presetPalettes[i],1,0,0,palettes[i].name,0);
    }
    drawDialog(30,5+24,30+300-1,5+24+6*26);
    int ret = flexibleMenu_complex(30,5,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,300,2,n_palettes,presetPalettes,6,0,1,1,NULL,NULL);
    if (ret == -1) return -1;
    else {
        for (int j=0; j<palettes[ret].n_colors; j++) {
            dest[j] = palettes[ret].colors[j];
        }
        return 0;
    }
}

