#ifndef _READER_H
#define _READER_H

typedef struct _shiori {
    long byte_location;
    long page_location;
    char preview[16];
} Shiori;
typedef struct _book_record {
    char book_path[32];
    long last_location; // Unit: page
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
    int  draw_progressbar; //0: Do not draw it  1: draw it
    char bgpict_path[32];
    BookRecord book_records[32];
    BookRecord *last_book;
} SessionConfig;
typedef struct _paging_data_header {
    char magic[6];
    int version; /* 0 : <= 16 pages; 1 : <= 64 pages; 2 : <= 256 pages; 3 : <= 1024 pages; 4 : <= 8192 pages */
    int n_pages_avail;
    int font; /* 0 : large; 1 : small */
} PagingDataHeader;

typedef struct _paging_data_ver0 {
    PagingDataHeader hdr;
    int pages[16];
} PD0;

typedef struct _paging_data_ver1 {
    PagingDataHeader hdr;
    int pages[64];
} PD1;

typedef struct _paging_data_ver2 {
    PagingDataHeader hdr;
    int pages[256];
} PD2;

typedef struct _paging_data_ver3 {
    PagingDataHeader hdr;
    int pages[1024];
} PD3;

typedef struct _paging_data_ver4 {
    PagingDataHeader hdr;
    int pages[8192];
} PD4;
extern int read_book(char *fpath);
extern int check_magic_paging(char *str);
extern int check_pagefile_validity(int fileHandle);
extern void get_file_basename(const char *pathname, char *filename, char *suffix, char *parentdir);
#endif