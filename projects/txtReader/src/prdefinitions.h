#ifndef _PRDEFINITIONS_H
#define _PRDEFINITIONS_H

/*General*/
#define MENU_DISCARDED           -1
#define R_INVALID                -1
#define R_SUCCESS                0

/*File browser*/
#define R_FB_PRESSED_EXIT        222
/*Reader return values*/
#define R_READER_INVALID_BGPICT  133
#define R_READER_STRAIGHT_EXIT   127
#define R_READER_NXBOOK          255
#define R_READER_INVALID_SUFFIX  250
#define R_READER_WRONG_FORMAT    254

/*Configuration Menu Items*/
#define N_M_CONF                 10
#define M_CONF_LABEL_FONT_SIZE   0
#define M_CONF_CHECK_FONT_LARGE  1
#define M_CONF_CHECK_FONT_SMALL  2
#define M_CONF_CHECK_BACKSLASH   3
#define M_CONF_CLEAR_SESSION     7
#define M_CONF_APPLY_RETURN      8
#define M_CONF_DISMISS_RETURN    9
#define M_CONF_BGPICT_SETTINGS   5
#define M_CONF_HIDE_HUD          4
#define M_CONF_DRAW_PROGRESSBAR  6
/*BG Picture Menu Items*/
#define N_M_BG                   5
#define M_BG_CHECK_ENABLED       0
#define M_BG_CHOOSE_PICTURE      1
#define M_BG_APPLY               2
#define M_BG_DISMISS             3
#define M_BG_LABEL_CURRENT_PICT  4

/*Main Menu Items*/
#define N_M_MAIN                 5
#define M_MAIN_BROWSE            0
#define M_MAIN_OPEN_RECENT       1
#define M_MAIN_SETTINGS          2
#define M_MAIN_MANAGE_SLOTS      3
#define M_MAIN_ABOUT             4

#endif