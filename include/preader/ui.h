#include <fxcg/display.h>
#ifndef _PRDR_UI_H
#define _PRDR_UI_H

#define AT(x,y) &vramaddr[(y)*384+(x)]

typedef struct _menuitem {
    int enabled;
    char *label;
} MenuItem;

typedef struct _menuitem_complex {
    char enabled;
    /* 
        Specifies the type of the menu item.
        0 - Standard menu item. When user chooses the item, its result will be {type:0,index:[prop_index],value:1}
        1 - Checkbox menu item. Whether user changes the item or not, it will be included in the result array, with value
            {type:1, index:[prop_index of the item], value:[1 if on / 0 if not on]}
        2 - Radiobutton menu item. Whether user changes the item or not, it will be included in the result array, with value
            {type:2, index:[prop_index of the item], value:[array subscription of the item]}
        3 - Menu item with black triangle on the right. When user chooses the item, its result will be {type:0,value:[prop_index]}
        4 - Standard menu item, but with forced small font whenever the original one is.]
        5 - Slider. Extra properties is typed sliderProperties, see below.
        6 - Color Viewer. prop_index is used as style settings:
            0 - A label on the left and a color block on the right.
            1 - A label with colored bg and white text (Black text when in white color).
            2 - Text colored in the corresponding color with current bg.     */
    int type;
    /*
        Define the index of the item.
        Most of the time it can be arbitary,
        except for Radiobutton menu items -- those in the same group should be assigned the same prop_index.
    */
    int prop_index;
    /* Note that the label will be cut off if when is too long. */
    char *label;
    /* Checkbox items and radiobutton items use it; 0 for off and 1 for on */
    int value;
    /* A pointer to a data structure determined by its type. */
    void *item_based_properties;
} complexMenuItem;

typedef struct _callback_data {
    int current_scope;
    int current_item_on_screen;
} callbackData;

typedef struct _slider_properties {
    int min;
    int max;
    int step;
    int label_width;
    int value_width;
    char show_label;
    char show_value;
} sliderProperties;

extern void rect(int x1, int y1, int x2, int y2, short color);
extern void drawDialog(int x1, int y1, int x2, int y2);
extern  int flexibleMenu(int left, int top, color_t bgcolor, 
                 int fontsize, color_t txtcolor, color_t txtcolorhi, color_t txtunavail,
                 color_t bgcolorhi, int theme, int itemwidth, int linespace, int n_items,
                 MenuItem entries[], int items_in_screen, int defaultitem, int isStatusBarOn,
                 int usescrollbar
                 );
extern int flexibleMenu_complex(int left, int top, color_t bgcolor, 
                 int fontsize, color_t txtcolor, color_t txtcolorhi, color_t txtunavail,
                 color_t bgcolorhi, int theme, int itemwidth, int linespace, int n_items,
                 complexMenuItem entries[], int items_in_screen, int defaultitem, int isStatusBarOn,
                 int usescrollbar, void (*callback_function)(complexMenuItem *, callbackData *, int *)
                 , void (*callback_memopt)(complexMenuItem *, int, int));
extern int printCXYSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
extern int printMiniSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
extern int printCXYSingleLineCutOffUnprintables(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
extern int printMiniSingleLineCutOffUnprintables(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
extern int printCXYMultiLineNCutOffUnprintables(int x, int y, char *str, int width, int maxline, color_t fgcolor, color_t bgcolor, int transparentbg);
extern int printMiniMultiLineNCutOffUnprintables(int x, int y, char *str, int width, int maxline, color_t fgcolor, color_t bgcolor, int transparentbg);
extern int bytesYouCanPrintInALineGB2312(char *str, int px, int ismini);
extern void fatal_error(char *errmsg, int height, int statusbarenabled);
extern int infobox(char *infomsg, int height, int statusbarenabled);
extern void fatal_wip();
extern void info_wip();
extern int info_error(char *infomsg, int height, int statusbarenabled);
extern int msgbox(char *infomsg, char *title, int height, int statusbarenabled, color_t titlecolor);
extern void register_menuitem_complex(complexMenuItem *i, char i1, int i2, int i3, char *i4, int i5);
extern void register_menuitem_normal(MenuItem *i, int i1, char *i2);
extern void draw_custom_font_8x16(int x, int y, char *str, color_t color);
extern void draw_custom_font_16x32(int x, int y, char *str, color_t color);
#endif