#ifndef _READER_H
#define _READER_H
#include <preader/common_definitions.h>
extern int read_book(char *fpath);
extern int check_magic_paging(char *str);
extern int check_pagefile_validity(int fileHandle);
extern void get_file_basename(const char *pathname, char *filename, char *suffix, char *parentdir);
#endif