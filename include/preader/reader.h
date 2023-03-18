#ifndef _READER_H
#define _READER_H

typedef struct _shiori {
    long byte_location;
    char preview[16];
} Shiori;
typedef struct _book_record {
    char book_path[32];
    long last_location; // Unit: byte
    Shiori bookmarks[8];
} BookRecord;
typedef struct _session_config {
    char magic[8];
    char has_last_book; // 0: No last book 1: Has last book
    int  font_size; // 0:Standard 1:Mini Other values will be treated as 0
    int  n_book_records;
    int  process_backslashes; // 0: Do not process backslashes (It will be treated as escape sequences)  1: process backslashes (\ -> \\)
    BookRecord book_records[32];
    BookRecord *last_book;
} SessionConfig;

extern int read_book(char *fpath);
#endif