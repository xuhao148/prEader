/* Some UI components (other than File Dialog) that are used multiple times. 
 You can use it in other programs, as long as you have the SessionConfig thing.
*/
#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <stddef.h>
#include <extra_calls.h>
#include <string.h>
#include "prdefinitions.h"
#include <preader/reader.h>
#include <preader/common_definitions.h>

#define AT(x,y) &vramaddr[(y)*384+(x)]

extern SessionConfig cfg;

const char *SYM_checked = "\xe6\xa9";
const char *SYM_unchecked = "\xe6\xa5";
const char *SYM_triangle = "\xe6\x9e";
const char *SYM_radiooff = "\xe6\xa3";
const char *SYM_radioon = "\xe6\xa4";

const int font_data[] = {
120,204,204,204,204,204,204,204,204,204,204,204,120,0,0,0,
112,48,48,48,48,48,48,48,48,48,48,48,48,0,0,0,
248,12,12,12,24,48,96,192,192,192,192,192,252,0,0,0,
248,12,12,12,12,56,12,12,12,12,12,12,248,0,0,0,
192,204,204,204,204,252,12,12,12,12,12,12,12,0,0,0,
252,192,192,192,192,248,12,12,12,12,12,12,248,0,0,0,
120,192,192,192,192,248,204,204,204,204,204,204,120,0,0,0,
252,204,12,12,24,24,24,48,48,48,48,48,48,0,0,0,
120,204,204,204,204,120,204,204,204,204,204,204,120,0,0,0,
120,204,204,204,204,204,124,12,12,12,12,12,120,0,0,0,
252,192,192,192,192,240,192,192,192,192,192,192,252,0,0,0,
120,204,204,204,204,204,204,204,204,204,204,204,120,0,0,0,
252,192,192,192,192,240,192,192,192,192,192,192,192,0,0,0,
204,204,204,216,24,56,48,112,96,108,204,204,204,0,0,0,
};
const char char_table[] = {'0','1','2','3','4','5','6','7','8','9','E','O','F','%'};

const int large_font_data[] = {
32760,65532,65532,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,65532,65532,32760,
4032,4032,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,960,
32760,65532,65532,57372,57372,57372,57372,28,28,28,28,28,60,124,248,496,992,1984,3968,7936,15872,31744,63488,61440,57344,57344,57344,57344,57344,65532,65532,65532,
65532,65532,65532,28,28,28,28,28,60,124,248,496,992,1984,992,496,248,124,60,28,28,28,28,28,28,28,28,57372,57372,65532,65532,32760,
57344,57344,57344,57344,57344,57344,57344,57344,57344,57344,57792,57792,57792,57792,65528,65528,65528,448,448,448,448,448,448,448,448,448,448,448,448,448,448,448,
65532,65532,65532,57344,57344,57344,57344,57344,57344,57344,57344,57344,57344,57344,65528,65532,65532,28,28,28,28,28,28,28,28,28,28,28,28,65532,65532,65528,
32764,65532,65532,57344,57344,57344,57344,57344,57344,57344,57344,57344,57344,57344,65528,65532,65532,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,32764,32764,32760,
65532,65532,65532,60,60,56,120,120,120,112,240,240,240,224,224,480,480,448,448,448,960,896,896,896,1920,1792,3840,3840,3840,7680,7680,7680,
32760,65532,65532,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,65532,32760,65532,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,32764,32764,32760,
32760,65532,65532,57372,57372,57372,57372,57372,57372,57372,57372,57372,57372,65532,65532,32764,28,28,28,28,28,28,28,28,28,28,28,28,28,32764,32764,32760,
};
const char large_char_table[] = {
'0','1','2','3','4','5','6','7','8','9',};

#define SIZEOF_CHAR_TABLE 14
#define SIZEOF_LARGE_CHAR_TABLE 10


static short *vramaddr = NULL;
int printMiniSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
int printCXYSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
void __uicomp_draw_tabitem(int left, int top, int max_width, int use_smallfont, int highlighted, complexMenuItem tabitem);
char *lineCopy(char *dest, const char *src, int maxn);


/* Draws a simple dialog with gray shadow under it. */
void drawDialog(int x1, int y1, int x2, int y2) {
    if (vramaddr == NULL) vramaddr = GetVRAMAddress();
    rect(x1+1,y1+2,x2+3,y2+4,cfg.color_scheme[CI_DIALOG_SHADOW]); /* Shadow */
    rect(x1-1,y1-1,x2+1,y2+1,cfg.color_scheme[CI_DIALOG_BORDER]); /* Border */
    rect(x1,y1,x2,y2,cfg.color_scheme[CI_DIALOG_BG]); /* Panel */
    Bdisp_PutDisp_DD();
}

/*Draws a rectangle with color ranged (x1,y1) - (x2,y2) (Both inclusive).*/
void rect(int x1, int y1, int x2, int y2, short color) {
    
    if (vramaddr == NULL) vramaddr = GetVRAMAddress();
    short *vram_ptr;
    for (int y=y1; y<=y2; y++) {
        vram_ptr = AT(x1,y);
        for (int x=x1; x<=x2; x++) {
            *vram_ptr = color;
            vram_ptr++;
        }
    }
}


/*
 Menus
    These are functions that help you display a menu that is pretty flexible in appearance.
    Note that unlike the dialog functions, these functions use PrintCXY and its friends, so you have to tell them
    whether you are using a status bar or not.
 */

/* Draws a menu and returns the index.
   This is a pretty flexible function.
*/
int flexibleMenu(int left, int top, color_t bgcolor, 
                 int fontsize, color_t txtcolor, color_t txtcolorhi, color_t txtunavail,
                 color_t bgcolorhi, int theme, int itemwidth, int linespace, int n_items,
                 MenuItem entries[], int items_in_screen, int defaultitem, int isStatusBarOn,
                 int usescrollbar
                 )
{
    SaveVRAM_1();
    int key;

    int current_scope;
    int current_item_on_screen;
    int rect_y_offset = 0;
    int right = left + itemwidth - 1;
    int fontheight = fontsize==1?18:24;
    if (right >= 384) right = 383;
    int bottom = top + linespace * (items_in_screen - 1) + fontheight * items_in_screen - 1;
    if (top >= 216) top = 215;
    if (bottom >= 216) bottom = 215;

    /*Initializing variables*/
    int itemheight_with_linespace = fontheight + linespace;
    if (isStatusBarOn) rect_y_offset = 24;
    /* Determine how the menu is initially shown. "Scope" means the first item on screen currently; "Item on screen" means the (n-1)th item on screen. */
    if (defaultitem < items_in_screen) {
        current_scope = 0;
        current_item_on_screen = defaultitem;
    } else {
        current_scope = defaultitem - items_in_screen + 1;
        current_item_on_screen = items_in_screen - 1;
    }

    while (1)
    {
        /* Clears up te space for the menu items. */
        rect(left,top+rect_y_offset,right,bottom+rect_y_offset,bgcolor);
        /* Draws the items, with different colors like when highlighted, etc. */
        for (int i=0; i<items_in_screen && current_scope+i < n_items; i++) {
            int txtcolor_s;
            /*Draws the cursor*/
            if (i == current_item_on_screen) {
                if (theme == 1) {
                    rect(left,top+rect_y_offset+i*itemheight_with_linespace+fontheight-2,left+itemwidth-1,top+rect_y_offset+i*itemheight_with_linespace+fontheight-1,bgcolorhi);
                } else {
                    rect(left,top+rect_y_offset+i*itemheight_with_linespace,left+itemwidth-1,top+rect_y_offset+i*itemheight_with_linespace+fontheight-1,bgcolorhi);
                }
                txtcolor_s=txtcolorhi;
            } else {
                if (entries[current_scope+i].enabled)
                    txtcolor_s=txtcolor;
                else
                    txtcolor_s=txtunavail;
            }

            /*Draws the items*/
            if (fontsize == 1)
            {
                printMiniSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth,txtcolor_s,bgcolor,1);
            } else {
                printCXYSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth,txtcolor_s,bgcolor,1);
            }
        }
        /* Draws the scroll bar. This will always appear on the right of the whole menu.
           If there are too few items, the scrollbar won't be created and shown. */
        if (usescrollbar && n_items > items_in_screen) {
            struct scrollbar csb = {0};
            csb.indicatormaximum = n_items - items_in_screen + 1;
            csb.indicatorheight = 1;
            csb.indicatorpos = current_scope;
            csb.barleft = right + 1;
            csb.bartop = top;
            csb.barheight = bottom-top;
            csb.barwidth = 6;
            Scrollbar(&csb);
        }

        /* Controls key operations. This uses getkey, thus allowing user to return to MENU.
        Therefore, remember closing all your files before invoking a menu!*/

        GetKey(&key);
        int current_item;
        switch (key) {
            case KEY_CTRL_DOWN:
                do {
                current_item = current_scope + current_item_on_screen;
                if (current_item == n_items - 1) {
                    current_scope = 0;
                    current_item_on_screen = 0;
                } else {
                    if (current_item_on_screen < items_in_screen - 1) current_item_on_screen++;
                    else {current_scope++;}
                }
                current_item = current_scope + current_item_on_screen;
                } while (!entries[current_item].enabled);
                break;
            case KEY_CTRL_UP:
                current_item = current_scope + current_item_on_screen;
                do {
                if (current_item == 0) {
                    if (n_items > items_in_screen) {
                        current_scope = n_items - items_in_screen;
                        current_item_on_screen = items_in_screen - 1;
                    } else {
                        current_item_on_screen = n_items - 1;
                    }
                } else {
                    if (current_item_on_screen > 0) current_item_on_screen--;
                    else {current_scope--;}
                }
                current_item = current_scope + current_item_on_screen;
                } while (!entries[current_item].enabled);
                break;
            case KEY_CTRL_EXE:
                LoadVRAM_1(); return current_scope + current_item_on_screen;
            case KEY_CTRL_EXIT:
                LoadVRAM_1(); return -1;
        }
    }
}

/* If normal items are chosen, returns its array subscription. If user used EXIT, returns -1.
   The modification of the user reflects on the changes of values of entries[].*/
int flexibleMenu_complex(int left, int top, color_t bgcolor, 
                 int fontsize, color_t txtcolor, color_t txtcolorhi, color_t txtunavail,
                 color_t bgcolorhi, int theme, int itemwidth, int linespace, int n_items,
                 complexMenuItem entries[], int items_in_screen, int defaultitem, int isStatusBarOn,
                 int usescrollbar, int (*callback_agent)(complexMenuItem *, callbackData *, int *), void (*callback_memopt)(complexMenuItem *, int, int))
{
    SaveVRAM_1();
    int key;
    int result_index = 0;
    int current_scope;
    int current_item_on_screen;
    int rect_y_offset = 0;
    int right = left + itemwidth - 1;
    int fontheight = fontsize==1?18:24;
    if (right >= 384) right = 383;
    int bottom = top + linespace * (items_in_screen - 1) + fontheight * items_in_screen - 1;
    if (top >= 216) top = 215;
    if (bottom >= 216) bottom = 215;

    /*Initializing variables*/
    int itemheight_with_linespace = fontheight + linespace;
    if (isStatusBarOn) rect_y_offset = 24;
    /* Determine how the menu is initially shown. "Scope" means the first item on screen currently; "Item on screen" means the (n-1)th item on screen. */
    if (defaultitem < items_in_screen) {
        current_scope = 0;
        current_item_on_screen = defaultitem;
    } else {
        current_scope = defaultitem - items_in_screen + 1;
        current_item_on_screen = items_in_screen - 1;
    }

    while (1)
    {
        /* Clears up te space for the menu items. */
        rect(left,top+rect_y_offset,right,bottom+rect_y_offset,bgcolor);
        /* Draws the items, with different colors like when highlighted, etc. */
        for (int i=0; i<items_in_screen && current_scope+i < n_items; i++) {
            int txtcolor_s;
            /*Draws the cursor*/
            if (i == current_item_on_screen) {
                if (theme == 1) {
                    rect(left,top+rect_y_offset+i*itemheight_with_linespace+fontheight-2,left+itemwidth-1,top+rect_y_offset+i*itemheight_with_linespace+fontheight-1,bgcolorhi);
                } else {
                    rect(left,top+rect_y_offset+i*itemheight_with_linespace,left+itemwidth-1,top+rect_y_offset+i*itemheight_with_linespace+fontheight-1,bgcolorhi);
                }
                txtcolor_s=txtcolorhi;
            } else {
                if (entries[current_scope+i].enabled)
                    txtcolor_s=txtcolor;
                else
                    txtcolor_s=txtunavail;
            }

            /*Draws the items*/
            {
                int dwidth = entries[current_scope+i].type == 0 ? 0 : (fontsize==1?-16:-18);
                
                /* Draws the extra component */
                switch (entries[current_scope+i].type) {
                    case 1:
                        if (entries[current_scope+i].value) {
                            if (fontsize) {
                                int l = left+itemwidth+dwidth;
                                int t = top+i*itemheight_with_linespace;
                                PrintMini(&l,&t,SYM_checked,0x02,-1,0,0,txtcolor_s,bgcolor,1,0);
                            } else {
                                PrintCXY(left+itemwidth+dwidth,top+i*itemheight_with_linespace,SYM_checked,0x20,-1,txtcolor_s,bgcolor,1,0);
                            }
                        } else {
                            if (fontsize) {
                                int l = left+itemwidth+dwidth;
                                int t = top+i*itemheight_with_linespace;
                                PrintMini(&l,&t,SYM_unchecked,0x02,-1,0,0,txtcolor_s,bgcolor,1,0);
                            } else {
                                PrintCXY(left+itemwidth+dwidth,top+i*itemheight_with_linespace,SYM_unchecked,0x20,-1,txtcolor_s,bgcolor,1,0);
                            }
                        }
                        break;
                    case 2:
                        if (entries[current_scope+i].value) {
                            if (fontsize) {
                                int l = left+itemwidth+dwidth;
                                int t = top+i*itemheight_with_linespace;
                                PrintMini(&l,&t,SYM_radioon,0x02,-1,0,0,txtcolor_s,bgcolor,1,0);
                            } else {
                                PrintCXY(left+itemwidth+dwidth,top+i*itemheight_with_linespace,SYM_radioon,0x20,-1,txtcolor_s,bgcolor,1,0);
                            }
                        } else {
                            if (fontsize) {
                                int l = left+itemwidth+dwidth;
                                int t = top+i*itemheight_with_linespace;
                                PrintMini(&l,&t,SYM_radiooff,0x02,-1,0,0,txtcolor_s,bgcolor,1,0);
                            } else {
                                PrintCXY(left+itemwidth+dwidth,top+i*itemheight_with_linespace,SYM_radiooff,0x20,-1,txtcolor_s,bgcolor,1,0);
                            }
                        }
                        break;
                    case 3:
                        if (fontsize) {
                            int l = left+itemwidth+dwidth;
                            int t = top+i*itemheight_with_linespace;
                            PrintMini(&l,&t,SYM_triangle,0x02,-1,0,0,txtcolor_s,bgcolor,1,0);
                        } else {
                            PrintCXY(left+itemwidth+dwidth,top+i*itemheight_with_linespace,SYM_triangle,0x20,-1,txtcolor_s,bgcolor,1,0);
                        }
                        break;
                }
            /* Draws the main text */
            if (entries[current_scope+i].type >= 0 && entries[current_scope+i].type <= 4)
            if (fontsize == 1 || entries[current_scope+i].type == 4)
            {
                printMiniSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth+dwidth,txtcolor_s,bgcolor,1);
            } else {
                printCXYSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth+dwidth,txtcolor_s,bgcolor,1);
            } else {
                
                /* Special controls */
                int itemType = entries[current_scope+i].type;
                int itemProp = entries[current_scope+i].prop_index;
                switch (itemType) {
                    case TMC_SLIDER:
                        {
                        /* Clears the bg */
                        rect(left,top+rect_y_offset+i*itemheight_with_linespace,left+itemwidth-1,top+rect_y_offset+i*itemheight_with_linespace+fontheight-1,bgcolor);
                        sliderProperties *props = entries[current_scope+i].item_based_properties;
                        int realBarLeft = left + 3 + 1, realBarRight = left + itemwidth - 1 - 4, realBarTop = top+i*itemheight_with_linespace+(fontsize?8:10), realBarBottom = top+i*itemheight_with_linespace+(realBarBottom = fontsize?8:12);
                        realBarTop += 24;
                        realBarBottom += 24;
                        if (props->show_label) {
                            if (fontsize)
                                printMiniSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,props->label_width,txtcolor,bgcolor,1);
                            else
                                printCXYSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,props->label_width,txtcolor,bgcolor,1);
                            realBarLeft += props->label_width;
                        }
                        if (props->show_value) {
                            realBarRight -= props->value_width;
                            char buf[64];
                            sprintf(buf,"%d",entries[current_scope+i].value);
                            if (fontsize)
                                printMiniSingleLineInRestrictedLineWidth(realBarRight+1,top+i*itemheight_with_linespace,buf,props->value_width,txtcolor,bgcolor,1);
                            else
                                printCXYSingleLineInRestrictedLineWidth(realBarRight+1,top+i*itemheight_with_linespace,buf,props->value_width,txtcolor,bgcolor,1);
                        }
                        
                        /* Drawing the slider */
                        /* Border */
                        rect(realBarLeft-1,realBarTop-1,realBarRight+1,realBarBottom+1,cfg.color_scheme[CI_MENU_FG]);
                        /* Background */
                        rect(realBarLeft,realBarTop,realBarRight,realBarBottom,52959);
                        /* Foreground */
                        int realBarLastPixel = realBarLeft+(realBarRight-realBarLeft)*(entries[current_scope+i].value-props->min)/(props->max-props->min);
                        rect(realBarLeft,realBarTop,realBarLastPixel,realBarBottom,25823);
                        /* Knob */
                        color_t knobBorder = (current_item_on_screen==current_scope+i)?cfg.color_scheme[CI_MENU_FG_CHOSEN]:cfg.color_scheme[CI_MENU_FG];
                        color_t knobBg = (current_item_on_screen==current_scope+i)?cfg.color_scheme[CI_MENU_BG_CHOSEN]:COLOR_WHITE;
                        if (fontsize) {
                            rect(realBarLastPixel-1,realBarTop-2,realBarLastPixel+2,realBarBottom+2,knobBorder);
                            rect(realBarLastPixel,realBarTop-1,realBarLastPixel+1,realBarBottom+1,knobBg);
                        } else {
                            rect(realBarLastPixel-2,realBarTop-2,realBarLastPixel+3,realBarBottom+2,knobBorder);
                            rect(realBarLastPixel-1,realBarTop-1,realBarLastPixel+2,realBarBottom+1,knobBg);
                        }
                        }
                        break;
                    case TMC_COLORVIEW:
                        switch (itemProp) {
                            case 1:
                                if (fontsize) {
                                    printMiniSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth,(entries[current_scope+i].value==COLOR_WHITE)?COLOR_BLACK:COLOR_WHITE,entries[current_scope+i].value,0);
                                } else {
                                    printCXYSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth,(entries[current_scope+i].value==COLOR_WHITE)?COLOR_BLACK:COLOR_WHITE,entries[current_scope+i].value,0);
                                }
                                break;
                            case 2:
                                if (fontsize) {
                                    printMiniSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth,entries[current_scope+i].value,bgcolor,1);
                                } else {
                                    printCXYSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth,entries[current_scope+i].value,bgcolor,1);
                                }
                                break;
                            case 0: default:
                            if (fontsize) {
                                printMiniSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth-45,txtcolor_s,bgcolor,1);
                                rect(left+itemwidth-36-1,top+i*itemheight_with_linespace+24+3-1,left+itemwidth-5+1,top+i*itemheight_with_linespace+24+15+1,COLOR_BLACK);
                                rect(left+itemwidth-36,top+i*itemheight_with_linespace+24+3,left+itemwidth-5,top+i*itemheight_with_linespace+24+15,entries[current_scope+i].value);
                            } else {
                                printCXYSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth-60,txtcolor_s,bgcolor,1);
                                rect(left+itemwidth-51-1,top+i*itemheight_with_linespace+24+5-1,left+itemwidth-6+1,top+i*itemheight_with_linespace+24+19+1,COLOR_BLACK);
                                rect(left+itemwidth-51,top+i*itemheight_with_linespace+24+5,left+itemwidth-6,top+i*itemheight_with_linespace+24+19,entries[current_scope+i].value);
                            }
                                break;
                        }
                        break;
                    case TMC_TABITEM:
                        __uicomp_draw_tabitem(left,top+i*itemheight_with_linespace,itemwidth,fontsize,i==current_item_on_screen,entries[current_scope+i]);
                        break;
                }

            }
            
            
            }
        }
        /*
            ！！！！！！！！！！！！！！！
            TODO HERE
            ！！！！！！！！！！！！！！！
        */
        /* Draws the scroll bar. This will always appear on the right of the whole menu.
           If there are too few items, the scrollbar won't be created and shown. */
        if (usescrollbar && n_items > items_in_screen) {
            struct scrollbar csb = {0};
            csb.indicatormaximum = n_items - items_in_screen + 1;
            csb.indicatorheight = 1;
            csb.indicatorpos = current_scope;
            csb.barleft = right + 1;
            csb.bartop = top;
            csb.barheight = bottom-top;
            csb.barwidth = 6;
            Scrollbar(&csb);
        }
        int callback_ret = R_CALLBACK_NORMAL;
        /* Callback function is called before waiting for the next input */
        if (callback_agent != NULL) {
            callbackData cd;
            cd.current_item_on_screen = current_item_on_screen;
            cd.current_scope = current_scope;
            callback_ret = (*callback_agent)(entries,&cd,&key);
        }
        if (callback_ret == R_CALLBACK_BREAK) return R_MENU_INTERRUPTED;
        /* Controls key operations. This uses getkey, thus allowing user to return to MENU.
        Therefore, remember closing all your files before invoking a menu! (Or invoke DisableMenuBlahblah)*/
        if (callback_ret == R_CALLBACK_NORMAL)
            GetKey(&key);
        if (callback_ret == R_CALLBACK_NORMAL || callback_ret == R_CALLBACK_OVERRIDE_KEY)
        {
        int current_item;
        switch (key) {
            case KEY_CTRL_DOWN:
                do {
                current_item = current_scope + current_item_on_screen;
                if (current_item == n_items - 1) {
                    current_scope = 0;
                    current_item_on_screen = 0;
                } else {
                    if (current_item_on_screen < items_in_screen - 1) current_item_on_screen++;
                    else {current_scope++;}
                }
                current_item = current_scope + current_item_on_screen;
                } while (!entries[current_item].enabled);
                break;
            case KEY_CTRL_UP:
                current_item = current_scope + current_item_on_screen;
                do {
                if (current_item == 0) {
                    if (n_items > items_in_screen) {
                        current_scope = n_items - items_in_screen;
                        current_item_on_screen = items_in_screen - 1;
                    } else {
                        current_item_on_screen = n_items - 1;
                    }
                } else {
                    if (current_item_on_screen > 0) current_item_on_screen--;
                    else {current_scope--;}
                }
                current_item = current_scope + current_item_on_screen;
                } while (!entries[current_item].enabled);
                break;
            case KEY_CTRL_EXE:
                /* Where things gets complicated...*/
                current_item = current_item_on_screen + current_scope;
                switch (entries[current_item].type) {
                    case 0:case 3:case 4:case 6:
                    LoadVRAM_1();
                    return current_item;
                    case 1:
                    entries[current_item].value = 1 - entries[current_item].value;
                    if (callback_memopt) {
                        (*callback_memopt)(entries,current_item,entries[current_item].value);
                    }
                    break;
                    case 2:
                    for (int i=0; i<n_items; i++) {
                        if (entries[i].type == 2 && entries[i].prop_index == entries[current_item].prop_index) {
                            if (i == current_item) entries[i].value = 1;
                            else entries[i].value = 0;
                        }
                        if (callback_memopt) {
                            (*callback_memopt)(entries,i,entries[i].value);
                        }
                    }
                    break;
                    case 5: case TMC_TABITEM:
                    break;
                    default: LoadVRAM_1(); return -1;
                }
                break;
            case KEY_CTRL_RIGHT:
                current_item = current_item_on_screen + current_scope;
                switch (entries[current_item].type) {
                    case TMC_TRIANGLE:
                        return current_item;
                    case TMC_SLIDER:
                        {
                        sliderProperties *prop = entries[current_item].item_based_properties;
                        int val = entries[current_item].value;
                        int max = prop->max;
                        int min = prop->min;
                        int step = prop->step;
                        val += step;
                        if (val > max) val = max;
                        if (val < min) val = min;
                        entries[current_item].value = val;
                        if (callback_memopt) {
                            (*callback_memopt)(entries,current_item,entries[current_item].value);
                        }
                        }
                        break;
                    case TMC_TABITEM:
                        {
                            tabularProperties *tabprop = entries[current_item].item_based_properties;
                            int n_items = tabprop->n_items;
                            entries[current_item].value++;
                            if (entries[current_item].value >= n_items) {
                                entries[current_item].value = 0;
                            }
                            if (callback_memopt) {
                                (*callback_memopt)(entries,current_item,entries[current_item].value);
                            }
                        }
                        break;
                }
                break;
            case KEY_CTRL_LEFT:
                current_item = current_item_on_screen + current_scope;
                switch (entries[current_item].type) {
                    case TMC_SLIDER:
                        {
                        sliderProperties *prop = entries[current_item].item_based_properties;
                        int val = entries[current_item].value;
                        int max = prop->max;
                        int min = prop->min;
                        int step = prop->step;
                        val -= step;
                        if (val < min) val = min;
                        if (val > max) val = max;
                        entries[current_item].value = val;
                        if (callback_memopt) {
                            (*callback_memopt)(entries,current_item,entries[current_item].value);
                        }
                        }
                        break;
                    case TMC_TABITEM:
                    {
                        tabularProperties *tabprop = entries[current_item].item_based_properties;
                        int n_items = tabprop->n_items;
                        entries[current_item].value--;
                        if (entries[current_item].value < 0) {
                            entries[current_item].value = n_items - 1;
                        }
                        if (callback_memopt) {
                            (*callback_memopt)(entries,current_item,entries[current_item].value);
                        }
                    }
                    break;
                }
                break;
            case KEY_CTRL_EXIT:
                LoadVRAM_1(); return -1;
        }
        }
    }
}

int printCXYSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg) {
    char buf[64] = {0};
    int bytes_to_print = bytesYouCanPrintInALineGB2312(str,width,0);
    int bytes_to_print_period = bytesYouCanPrintInALineGB2312(str,width-24,0);
    if (bytes_to_print < strlen(str))
    {
        strncpy(buf,str,bytes_to_print_period);
        strcat(buf,"…");
    } else {
        strcpy(buf,str);
    }
    ProcessPrintChars(936);
    PrintCXY(x,y,buf,transparentbg?0x20:0,-1,fgcolor,bgcolor,1,0);
    ProcessPrintChars(0);
    return 0;
}

int printMiniSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg) {
    char buf[128] = {0};
    int bytes_to_print = bytesYouCanPrintInALineGB2312(str,width,1);
    int bytes_to_print_period = bytesYouCanPrintInALineGB2312(str,width-16,1);
    if (bytes_to_print < strlen(str))
    {
        strncpy(buf,str,bytes_to_print_period);
        strcat(buf,"…");
    } else {
        strcpy(buf,str);
    }
    ProcessPrintChars(936);
    int xv = x, yv = y;
    PrintMini(&xv,&yv,buf,transparentbg?0x02:0,-1,0,0,fgcolor,bgcolor,1,0);
    ProcessPrintChars(0);
    return 0;
}

/* Returns bytes printed, gotten from bytesYouCanPrintInALineGB2312.*/
int printCXYSingleLineCutOffUnprintables(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg)
{
    char buf[64] = {0};
    int bytes_to_print = bytesYouCanPrintInALineGB2312(str,width,0);
    strncpy(buf,str,bytes_to_print);
    buf[bytes_to_print] = 0;
    ProcessPrintChars(936);
    PrintCXY(x,y,buf,transparentbg?0x20:0,-1,fgcolor,bgcolor,1,0);
    ProcessPrintChars(0);
    return bytes_to_print;
}

int printMiniSingleLineCutOffUnprintables(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg)
{
    char buf[128] = {0};
    int bytes_to_print = bytesYouCanPrintInALineGB2312(str,width,1);
    strncpy(buf,str,bytes_to_print);
    buf[bytes_to_print] = 0;
    ProcessPrintChars(936);
    int xv = x, yv = y;
    PrintMini(&xv,&yv,buf,transparentbg?0x02:0,-1,0,0,fgcolor,bgcolor,1,0);
    ProcessPrintChars(0);
    return bytes_to_print;
}

/*Prints multiline texts. When there are more texts than maxline, they are cut off.
\n will be treated as newlines, while \r will be treated as ordinary characters. So convert CRLF to CR!
Not recommended for txt reader's main textview -- it supports only 128 chars per line. 
*/
int printCXYMultiLineNCutOffUnprintables(int x, int y, char *str, int width, int maxline, color_t fgcolor, color_t bgcolor, int transparentbg)
{
    ProcessPrintChars(936);
    char buf[128];
    char *ptr = lineCopy(buf,str,128);
    int flg = 0;
    int lines_printed = 0;
    while (lines_printed < maxline) {
        flg += printCXYSingleLineCutOffUnprintables(x,y,&buf[flg],width,fgcolor,bgcolor,transparentbg);
        if (!ptr && !buf[flg]) break;
        lines_printed++;
        y+=24;
        if (!buf[flg] && ptr) {ptr = lineCopy(buf,ptr,128); flg = 0;}
    }
    ProcessPrintChars(0);
    return lines_printed;
}

int printMiniMultiLineNCutOffUnprintables(int x, int y, char *str, int width, int maxline, color_t fgcolor, color_t bgcolor, int transparentbg)
{
    ProcessPrintChars(936);
    char buf[256];
    char *ptr = lineCopy(buf,str,256);
    int flg = 0;
    int key;
    int lines_printed = 0;
    while (lines_printed < maxline) {
        flg += printMiniSingleLineCutOffUnprintables(x,y,&buf[flg],width,fgcolor,bgcolor,transparentbg);
        if (!ptr && !buf[flg]) break;
        lines_printed++;
        y+=18;
        if (!buf[flg] && ptr) {ptr = lineCopy(buf,ptr,256); flg = 0;}
    }
    ProcessPrintChars(0);
    return lines_printed;
}

/*Copies the first line of src into dest. Returns a pointer at the next line. NULL if nothing else is to printed.*/
char *lineCopy(char *dest, const char *src, int maxn) {
    int flg = 0;
    while (src[flg] && (src[flg] != '\r' && src[flg] != '\n' ) && flg < maxn) flg++;
    strncpy(dest,src,flg);
    dest[flg] = 0;
    while ((src[flg] == '\r' || src[flg] == '\n') && src[flg] && flg < maxn) flg++;
    if (!src[flg]) return NULL;
    return &src[flg];
}

/* Returns how many bytes you can print in a line in GB2312 font.
   0=regular !0=mini
   Maybe its not that effecient... but whatever.*/
int bytesYouCanPrintInALineGB2312(char *str, int px, int ismini)
{    
    int flg = 0;
    int width_consumed = 0;
    unsigned char ascchar;
    unsigned short glyph_info;
    int last_char_is_wchar = 0;
    ProcessPrintChars(936);
    while (width_consumed < px && str[flg]) {
        ascchar = str[flg];
        if (ascchar >= 0xA1 && ascchar <= 0xF7) {
            last_char_is_wchar = 1;
            if (ismini)
                glyph_info=16;
            else
                glyph_info=24;
            flg++;
        } else {
            if (ascchar == '\\') {
                last_char_is_wchar = 1;
                flg++;
                ascchar = str[flg];
                if (ascchar == '\\')
                {
                    if (ismini)
                        glyph_info = 9;
                    else
                        glyph_info = 18;
                } else {
                    glyph_info = 0;
                }
            } else {
            last_char_is_wchar = 0;
            if (ismini)
                GetMiniGlyphPtr(ascchar,&glyph_info);
            else
                glyph_info = 18;
            }
        }
        width_consumed+=glyph_info;
        flg++;
    }
    ProcessPrintChars(0);
    if (width_consumed > px) return flg-(last_char_is_wchar?2:1);
    else return flg;
}

void fatal_error(char *errmsg, int height, int statusbarenabled) {
    int key;
    int dh = statusbarenabled?24:0;
    int top = dh+(LCD_HEIGHT_PX-dh)/2-height/2-height%2;
    int bottom = dh+(LCD_HEIGHT_PX-dh)/2+height/2;
    drawDialog(57,top,316,bottom);
    EnableGetkeyToMainFunctionReturn();
    printMiniSingleLineCutOffUnprintables(57,top-dh,"致命错误",316-57+1,COLOR_RED,COLOR_WHITE,0);
    printMiniSingleLineCutOffUnprintables(58,top-dh,"致命错误",316-57,COLOR_RED,COLOR_WHITE,1);
    printMiniMultiLineNCutOffUnprintables(57,top-dh+18,errmsg,316-57+1,height/18-1,COLOR_BLACK,COLOR_WHITE,0);
    while (1) {
        GetKey(&key);
    }
    return;
}

int infobox(char *infomsg, int height, int statusbarenabled) {
    int key;
    int dh = statusbarenabled?24:0;
    int top = dh+(LCD_HEIGHT_PX-dh)/2-height/2-height%2;
    int bottom = dh+(LCD_HEIGHT_PX-dh)/2+height/2;
    SaveVRAM_1();
    drawDialog(57,top,316,bottom);
    printMiniSingleLineCutOffUnprintables(57,top-dh,"信息",316-57+1,COLOR_BLUE,COLOR_WHITE,0);
    printMiniSingleLineCutOffUnprintables(58,top-dh,"信息",316-57,COLOR_BLUE,COLOR_WHITE,1);
    printMiniMultiLineNCutOffUnprintables(57,top-dh+18,infomsg,316-57+1,height/18-1,COLOR_BLACK,COLOR_WHITE,0);
    do {
        GetKey(&key);
    } while (key != KEY_CTRL_EXIT && key != KEY_CTRL_EXE);
    LoadVRAM_1();
    return key;
}
int msgbox(char *infomsg, char *title, int height, int statusbarenabled, color_t titlecolor) {
    int key;
    int dh = statusbarenabled?24:0;
    int top = dh+(LCD_HEIGHT_PX-dh)/2-height/2-height%2;
    int bottom = dh+(LCD_HEIGHT_PX-dh)/2+height/2;
    SaveVRAM_1();
    drawDialog(57,top,316,bottom);
    printMiniSingleLineCutOffUnprintables(57,top-dh,title,316-57+1,titlecolor,COLOR_WHITE,0);
    printMiniSingleLineCutOffUnprintables(58,top-dh,title,316-57,titlecolor,COLOR_WHITE,1);
    printMiniMultiLineNCutOffUnprintables(57,top-dh+18,infomsg,316-57+1,height/18-1,COLOR_BLACK,COLOR_WHITE,0);
    do {
        GetKey(&key);
    } while (key != KEY_CTRL_EXIT && key != KEY_CTRL_EXE && key != KEY_CTRL_F1);
    LoadVRAM_1();
    return key;
}

void fatal_wip() {
    fatal_error("（未实现）",38,1);
}

void info_wip() {
    infobox("（未实现）",38,1);
}

int info_error(char *infomsg, int height, int statusbarenabled)
{
    int key;
    int dh = statusbarenabled?24:0;
    int top = dh+(LCD_HEIGHT_PX-dh)/2-height/2-height%2;
    int bottom = dh+(LCD_HEIGHT_PX-dh)/2+height/2;
    SaveVRAM_1();
    drawDialog(57,top,316,bottom);
    printMiniSingleLineCutOffUnprintables(57,top-dh,"错误",316-57+1,COLOR_RED,COLOR_WHITE,0);
    printMiniSingleLineCutOffUnprintables(58,top-dh,"错误",316-57,COLOR_RED,COLOR_WHITE,1);
    printMiniMultiLineNCutOffUnprintables(57,top-dh+18,infomsg,316-57+1,height/18-1,COLOR_BLACK,COLOR_WHITE,0);
    do {
        GetKey(&key);
    } while (key != KEY_CTRL_EXIT && key != KEY_CTRL_EXE);
    LoadVRAM_1();
    return key;
}

void register_menuitem_complex(complexMenuItem *i, char i1, int i2, int i3, char *i4, int i5) {
    i->enabled = i1;
    i->type = i2;
    i->prop_index = i3;
    i->label = i4;
    i->value = i5;
}

void register_menuitem_normal(MenuItem *i, int i1, char *i2) {
    i->enabled = i1;
    i->label = i2;
}

void draw_custom_font_8x16(int x, int y, char *str, color_t color) {
    static short *vram = NULL;
    int flg = 0;
    if (vram == NULL) vram = GetVRAMAddress();
    while (str[flg]) {
        int i = 0;
        while (i < SIZEOF_CHAR_TABLE && char_table[i] != str[flg]) i++;
        if (i == SIZEOF_CHAR_TABLE) {info_error("绘制自制字体字符串时出现未定义字符。",50,1); break;}
        int dx,dy;
        for (dy=0;dy<16;dy++) {
            for (dx=0; dx<8; dx++) {
                int ax = x+dx;
                int ay = y+dy;
                if (ax >= 0 && ax < 384 && ay >= 0 && ay <= 216) {
                    if (0 <= ay && ay < 216 && 0 <= ax && ax < 384)
                        if (font_data[i*16+dy] & (1<<(7-dx))) vram[ay*384+ax] = color; 
                }
            }
        }
        flg++;
        x += 8;
    }
}

void draw_custom_font_16x32(int x, int y, char *str, color_t color) {
    static short *vram = NULL;
    int flg = 0;
    if (vram == NULL) vram = GetVRAMAddress();
    while (str[flg]) {
        int i = 0;
        while (i < SIZEOF_LARGE_CHAR_TABLE && large_char_table[i] != str[flg]) i++;
        if (i == SIZEOF_LARGE_CHAR_TABLE) {info_error("绘制自制字体字符串时出现未定义字符。",50,1); break;}
        int dx,dy;
        for (dy=0;dy<32;dy++) {
            for (dx=0; dx<16; dx++) {
                int ax = x+dx;
                int ay = y+dy;
                if (ax >= 0 && ax < 384 && ay >= 0 && ay <= 216) {
                    if (0 <= ay && ay < 216 && 0 <= ax && ax < 384)
                        if (large_font_data[i*32+dy] & (1<<(15-dx))) vram[ay*384+ax] = color; 
                }
            }
        }
        flg++;
        x += 16;
    }
}

int __uicomp_isum(int arr[], int n) {
    int sum = 0;
    for (int i=0; i<n; i++) {
        sum += arr[i];
    }
    return sum;
}

void __uicomp_rect_s(int x1, int y1, int x2, int y2, color_t col) {
    rect(x1,y1+24,x2,y2+24,col);
}

int __uicomp_getwidth_mini(char *str) {
    ProcessPrintChars(936);
    int x=0, y=0;
    PrintMini(&x,&y,str,0,-1,0,0,COLOR_BLACK,COLOR_WHITE,0,0);
    ProcessPrintChars(0);
    return x;
}

void __uicomp_draw_tabitem(int left, int top, int max_width, int use_smallfont, int highlighted, complexMenuItem tabitem) {
    tabularProperties *props = tabitem.item_based_properties;
    char *main_label = tabitem.label;
    int main_label_width = props->width_main_label;
    int n_items = props->n_items;
    char use_main_label = props->show_main_label;
    int width_available_for_ui = max_width - (use_main_label?main_label_width:0) - 4 - 4 - (n_items - 1);
    if (width_available_for_ui <= 8) return;
    int ui_left = left + (use_main_label?main_label_width:0) + 2;
    int ui_right = left + max_width - 1 - 2;
    if (ui_left >= ui_right) return;
    int real_item_width[n_items];
    for (int i=0; i<n_items; i++) {
        real_item_width[i] = props->items[i].desired_width;
        if (real_item_width[i] == -1) {
            real_item_width[i] = __uicomp_getwidth_mini(props->items[i].label)+2;
        }
    }
    while (__uicomp_isum(real_item_width,n_items) > width_available_for_ui) {
        for (int i=0; i<n_items; i++) {
            real_item_width[i]--;
            if (real_item_width[i] < 0) real_item_width[i] = 0;
        }
    }
    int real_item_width_sum = __uicomp_isum(real_item_width,n_items);
    int ui_mid = (ui_left + ui_right) / 2;
    int ui_cursor = ui_mid - (real_item_width_sum) / 2 - (n_items / 2 - 1) - 2;
    int chosen = tabitem.value;
    
    color_t bg;
    color_t txt_on, txt_off;
    color_t trans;
    if (highlighted) {
        bg = cfg.color_scheme[CI_MENU_FG_CHOSEN];
        txt_on = cfg.color_scheme[CI_MENU_BG_CHOSEN];
        txt_off = cfg.color_scheme[CI_MENU_FG_CHOSEN];
        trans = cfg.color_scheme[CI_MENU_BG_CHOSEN];
    } else {
        bg = cfg.color_scheme[CI_MENU_BG_CHOSEN];
        txt_on = cfg.color_scheme[CI_MENU_FG_CHOSEN];
        txt_off = cfg.color_scheme[CI_MENU_BG_CHOSEN];
        trans = cfg.color_scheme[CI_MENU_BG];
    }
    /* Start drawing... */
    if (use_smallfont) {
        if (props->show_main_label) {
            printMiniSingleLineInRestrictedLineWidth(left,top,tabitem.label,props->width_main_label,highlighted?cfg.color_scheme[CI_MENU_FG_CHOSEN]:cfg.color_scheme[CI_MENU_FG],COLOR_WHITE,1);
        }
            /* Left border */
    __uicomp_rect_s(ui_cursor,top+1,ui_cursor,top+16,bg);
    __uicomp_rect_s(ui_cursor+1,top,ui_cursor+1,top+17,bg);
    if (chosen != 0) {
        __uicomp_rect_s(ui_cursor+1,top+1,ui_cursor+1,top+16,trans);
    }
    ui_cursor += 2;
    /* Items */
    for (int i=0; i<n_items; i++) {
        __uicomp_rect_s(ui_cursor,top,ui_cursor+real_item_width[i]-1,top+17,bg);
        if (chosen != i) {
            __uicomp_rect_s(ui_cursor,top+1,ui_cursor+real_item_width[i]-1,top+16,trans);
        }
        int txt_width = __uicomp_getwidth_mini(props->items[i].label);
        if (txt_width > real_item_width[i]) txt_width = real_item_width[i];
        int txt_start = (ui_cursor+ui_cursor+real_item_width[i]-1)/2-(txt_width/2);
        if (chosen == i) {
            printMiniSingleLineInRestrictedLineWidth(txt_start,top,props->items[i].label,real_item_width[i],txt_on,COLOR_WHITE,1);
        } else {
            printMiniSingleLineInRestrictedLineWidth(txt_start,top,props->items[i].label,real_item_width[i],txt_off,COLOR_WHITE,1);
        }
        ui_cursor += real_item_width[i];
        if (i < n_items-1) {
            __uicomp_rect_s(ui_cursor,top,ui_cursor,top+17,bg);
            ui_cursor++;
        }
    }
    /* Right border */
    __uicomp_rect_s(ui_cursor,top,ui_cursor,top+17,bg);
    __uicomp_rect_s(ui_cursor+1,top+1,ui_cursor+1,top+16,bg);
    if (chosen != n_items - 1) {
        __uicomp_rect_s(ui_cursor,top+1,ui_cursor,top+16,trans);
    }
    } else {
        if (props->show_main_label) {
            printCXYSingleLineInRestrictedLineWidth(left,top,tabitem.label,props->width_main_label,highlighted?cfg.color_scheme[CI_MENU_FG_CHOSEN]:cfg.color_scheme[CI_MENU_FG],COLOR_WHITE,1);
        }
    /* Left border */
    __uicomp_rect_s(ui_cursor,top+2,ui_cursor,top+21,bg);
    __uicomp_rect_s(ui_cursor+1,top+1,ui_cursor+1,top+22,bg);
    if (chosen != 0) {
        __uicomp_rect_s(ui_cursor+1,top+2,ui_cursor+1,top+21,trans);
    }
    ui_cursor += 2;
    /* Items */
    for (int i=0; i<n_items; i++) {
        __uicomp_rect_s(ui_cursor,top,ui_cursor+real_item_width[i]-1,top+23,bg);
        if (chosen != i) {
            __uicomp_rect_s(ui_cursor,top+1,ui_cursor+real_item_width[i]-1,top+22,trans);
        }
        int txt_width = __uicomp_getwidth_mini(props->items[i].label);
        if (txt_width > real_item_width[i]) txt_width = real_item_width[i];
        int txt_start = (ui_cursor+ui_cursor+real_item_width[i]-1)/2-(txt_width/2);
        if (chosen == i) {
            printMiniSingleLineInRestrictedLineWidth(txt_start,top+3,props->items[i].label,real_item_width[i],txt_on,COLOR_WHITE,1);
        } else {
            printMiniSingleLineInRestrictedLineWidth(txt_start,top+3,props->items[i].label,real_item_width[i],txt_off,COLOR_WHITE,1);
        }
        ui_cursor += real_item_width[i];
        if (i < n_items-1) {
            __uicomp_rect_s(ui_cursor,top,ui_cursor,top+23,bg);
            ui_cursor++;
        }
    }
    /* Right border */
    __uicomp_rect_s(ui_cursor,top+1,ui_cursor,top+22,bg);
    __uicomp_rect_s(ui_cursor+1,top+2,ui_cursor+1,top+21,bg);
    if (chosen != n_items - 1) {
        __uicomp_rect_s(ui_cursor,top+2,ui_cursor,top+21,trans);
    }
    }
}