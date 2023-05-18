#ifndef _STATMAN_H
#define _STATMAN_H
extern void stat_write(int len);
extern void stat_read(int len);
extern void stat_clear();
extern void stat_page();
extern void stat_paginate();
extern void stat_set();
extern void show_stat();
extern void stat_book();
#endif