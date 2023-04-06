/* Some UI components (other than File Dialog) that are used multiple times. */
#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <stddef.h>
#include <extra_calls.h>
#include <string.h>

#define AT(x,y) &vramaddr[(y)*384+(x)]

const char *SYM_checked = "\xe6\xa9";
const char *SYM_unchecked = "\xe6\xa5";
const char *SYM_triangle = "\xe6\x9e";
const char *SYM_radiooff = "\xe6\xa3";
const char *SYM_radioon = "\xe6\xa4";

static short *vramaddr = NULL;
int printMiniSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
int printCXYSingleLineInRestrictedLineWidth(int x, int y, char *str, int width, color_t fgcolor, color_t bgcolor, int transparentbg);
char *lineCopy(char *dest, const char *src, int maxn);
/*
    Dialogs
     These functions helps to draw simple dialogs so that some controls (like MENU and LineGet copied from WikiPrizm)
*/
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
     */
    int type;
    /*
        Define the index of the item.
        Each item should be assigned an arbitary but unique prop_index, which will be returned in its complexMenuItemResult,
        except for Radiobutton menu items -- those in the same group should be assigned the same prop_index.
    */
    int prop_index;
    /* Note that the label will be cut off if when is too long. */
    char *label;
    /* Checkbox items and radiobutton items use it; 0 for off and 1 for on */
    char value;
} complexMenuItem;


/* Draws a simple dialog with gray shadow under it. */
void drawDialog(int x1, int y1, int x2, int y2) {
    if (vramaddr == NULL) vramaddr = GetVRAMAddress();
    rect(x1+1,y1+2,x2+3,y2+4,COLOR_GRAY); /* Shadow */
    rect(x1-1,y1-1,x2+1,y2+1,COLOR_BLACK); /* Border */
    rect(x1,y1,x2,y2,COLOR_WHITE); /* Panel */
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
                 int usescrollbar
                 )
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
            if (fontsize == 1)
            {
                printMiniSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth+dwidth,txtcolor_s,bgcolor,1);
            } else {
                printCXYSingleLineInRestrictedLineWidth(left,top+i*itemheight_with_linespace,entries[current_scope+i].label,itemwidth+dwidth,txtcolor_s,bgcolor,1);
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

        /* Controls key operations. This uses getkey, thus allowing user to return to MENU.
        Therefore, remember closing all your files before invoking a menu! (Or invoke DisableMenuBlahblah)*/

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
                /* Where things gets complicated...*/
                current_item = current_item_on_screen + current_scope;
                switch (entries[current_item].type) {
                    case 0:case 3:
                    return current_item;
                    case 1:
                    entries[current_item].value = 1 - entries[current_item].value;break;
                    case 2:
                    for (int i=0; i<n_items; i++) {
                        if (entries[i].type == 2 && entries[i].prop_index == entries[current_item].prop_index) {
                            if (i == current_item) entries[i].value = 1;
                            else entries[i].value = 0;
                        }
                    }
                    break;
                    default: LoadVRAM_1();return -1;
                }
                break;
            case KEY_CTRL_EXIT:
                LoadVRAM_1(); return -1;
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
    } while (key != KEY_CTRL_EXIT && key != KEY_CTRL_EXE);
    LoadVRAM_1();
    return key;
}

void fatal_wip() {
    fatal_error("（未实现）",38,1);
}

void info_wip() {
    infobox("（未实现）",38,1);
}