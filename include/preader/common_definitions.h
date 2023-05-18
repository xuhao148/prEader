#ifndef _PREADER_COMMON_DEFINITIONS
#define _PREADER_COMMON_DEFINITIONS
#include <fxcg/display.h>

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

typedef struct _tabular_items {
    int desired_width; /* When the possible width is less than desired, all the items
                          will be shrunken by the same ratio until they fit.
                          When an item has a width of zero, it won't be displayed. */
    char *label;
} tabularItems;

typedef struct _tabular_properties {
    int n_items;
    tabularItems *items;
    int width_main_label;
    char show_main_label;
} tabularProperties;

typedef struct
{
  unsigned short id, type;
  unsigned long fsize, dsize;
  unsigned int property;
  unsigned long address;
} file_type_t;

typedef struct {
    file_type_t fProperties;
    char pathname[32];
} file_info_t;

typedef struct _shiori {
    long byte_location;
    short page_location;
    char preview[16];
} Shiori;
typedef struct _book_record {
    char book_path[32];
    short last_location; // Unit: page
    long last_byte_location; // Unit: byte
    int bookmark_version; // 0 : Large font; 1 : Small font
    Shiori bookmarks[8];
} BookRecord;
typedef struct _session_config {
    char magic[8];
    char has_last_book; // 0: No last book 1: Has last book
    int  font_size; // 0:Standard 1:Mini Other values will be treated as 0
    int  n_book_records;
    int  process_backslashes; // 0: Do not process backslashes (It will be treated as escape sequences)  1: process backslashes (\ -> \\)
    int  use_bgpict;
    int  hide_ui;
    unsigned int  extra_settings; // 32 switches that are used later (maybe)... See the definition in prdefinitions.h.
    int  draw_progressbar; //0: Do not draw it  1: draw it
    char bgpict_path[32];
    BookRecord book_records[32];
    BookRecord *last_book;
    color_t color_scheme[10];
    unsigned int bytes_read;
    unsigned int bytes_written;
    unsigned int pages_read;
    unsigned int books_added;
    unsigned int pagination_files_created;
    unsigned int settings_modified;
} SessionConfig;
typedef struct _paging_data_header {
    char magic[6];
    //int version; /* (abandoned) 0 : <= 16 pages; 1 : <= 64 pages; 2 : <= 256 pages; 3 : <= 1024 pages; 4 : <= 8192 pages */
    int n_pages_avail;
    int font; /* 0 : large; 1 : small */
} PagingDataHeader;

typedef struct _paging_data_ver6 {
    PagingDataHeader hdr;
    int pages[8192];
} PDCache; 

typedef struct{
  unsigned int i1; // unknown, set to zero
  unsigned int indicatormaximum; // maximum logical indicator range
  unsigned int indicatorheight; // height of the indicator in units of indicatormaximum
  unsigned int indicatorpos; // indicator position in units of indicatormaxiumum
  unsigned int i5; // unknown, set to zero
  unsigned short barleft; // left position of the bar in pixels
  unsigned short bartop; // top position of the bar in pixels
  unsigned short barheight; // height of the bar in pixels
  unsigned short barwidth; // width of the bar in pixels
} TScrollbar;

#endif