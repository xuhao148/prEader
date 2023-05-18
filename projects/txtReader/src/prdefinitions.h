#ifndef _PRDEFINITIONS_H
#define _PRDEFINITIONS_H


/* Settings Bit Switches */
#define BS_USE_STRICT_RENDERER   1

/* General */
#define MENU_DISCARDED           -1
#define R_MENU_INTERRUPTED       -2
#define R_INVALID                -1
#define R_SUCCESS                0

/* Color Schemes */

#define N_COLORS              10
#define CI_MENU_BG            0
#define CI_MENU_BG_CHOSEN     1
#define CI_MENU_FG            2
#define CI_MENU_FG_CHOSEN     3
#define CI_MENU_FG_UNAVAIL    4
#define CI_DIALOG_BG          5
#define CI_DIALOG_BORDER      6
#define CI_DIALOG_SHADOW      7
#define CI_READER_BG          8
#define CI_READER_FG          9

#define COLOR_MENU_BG            65405
#define COLOR_MENU_BG_CHOSEN     60115
#define COLOR_MENU_FG            COLOR_BLACK
#define COLOR_MENU_FG_CHOSEN     COLOR_WHITE
#define COLOR_MENU_FG_UNAVAIL    45577
#define COLOR_DIALOG_BG          COLOR_WHITE
#define COLOR_DIALOG_BORDER      COLOR_BLACK
#define COLOR_DIALOG_SHADOW      COLOR_GRAY
#define COLOR_READER_BG          COLOR_WHITE
#define COLOR_READER_FG          COLOR_BLACK

/*File browser*/
#define R_FB_PRESSED_EXIT        222
/*Reader return values*/
#define R_READER_INVALID_BGPICT  133
#define R_READER_STRAIGHT_EXIT   127
#define R_READER_NXBOOK          255
#define R_READER_INVALID_SUFFIX  250
#define R_READER_WRONG_FORMAT    254
/*Callback return values*/
#define R_CALLBACK_NORMAL        0
#define R_CALLBACK_OVERRIDE_KEY  1
#define R_CALLBACK_CONTINUE      2
#define R_CALLBACK_BREAK         3

/*Configuration Menu Items*/
#define N_M_CONF                 12
#define M_CONF_TAB_FONT_SIZE     0
#define M_CONF_COLOR_SCHEME      1
#define M_CONF_USE_STRICT_RNDR   2
#define M_CONF_CHECK_BACKSLASH   3
#define M_CONF_HIDE_HUD          4
#define M_CONF_BGPICT_SETTINGS   5
#define M_CONF_DRAW_PROGRESSBAR  6
#define M_CONF_CLEAR_SESSION     7
#define M_CONF_BACKUP_CONFIG     8
#define M_CONF_RESTORE_CONFIG    9
#define M_CONF_APPLY_RETURN      10
#define M_CONF_DISMISS_RETURN    11

/*BG Picture Menu Items*/
#define N_M_BG                   5
#define M_BG_CHECK_ENABLED       0
#define M_BG_CHOOSE_PICTURE      1
#define M_BG_APPLY               2
#define M_BG_DISMISS             3
#define M_BG_LABEL_CURRENT_PICT  4

/*Main Menu Items*/
#define N_M_MAIN                 6
#define M_MAIN_BROWSE            0
#define M_MAIN_OPEN_RECENT       1
#define M_MAIN_SETTINGS          2
#define M_MAIN_MANAGE_SLOTS      3
#define M_MAIN_ABOUT             5
#define M_MAIN_STATS             4

/* Menu item types */
#define TMC_STANDARD            0
#define TMC_CHECKBOX            1
#define TMC_RADIOBOX            2
#define TMC_TRIANGLE            3
#define TMC_TINYFONT            4
#define TMC_SLIDER              5
#define TMC_COLORVIEW           6
#define TMC_TABITEM             7



#endif