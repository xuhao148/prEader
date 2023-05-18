#include <fxcg/display.h>
#ifndef _PRDR_UI_H
#define _PRDR_UI_H

#define AT(x,y) &vramaddr[(y)*384+(x)]

typedef struct _menuitem {
    int enabled;
    char *label;
} MenuItem;

extern void rect(int x1, int y1, int x2, int y2, short color);
extern void drawDialog(int x1, int y1, int x2, int y2);
extern  int flexibleMenu(int left, int top, color_t bgcolor, 
                 int fontsize, color_t txtcolor, color_t txtcolorhi, color_t txtunavail,
                 color_t bgcolorhi, int theme, int itemwidth, int linespace, int n_items,
                 MenuItem entries[], int items_in_screen, int defaultitem, int isStatusBarOn,
                 int usescrollbar
                 );
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
extern int msgbox(char *infomsg, char *title, int height, int statusbarenabled, color_t titlecolor);
extern void fatal_unhandled_out_of_bound(char *detail);
extern void fatal_unhandled_npe(char *detail);
#endif