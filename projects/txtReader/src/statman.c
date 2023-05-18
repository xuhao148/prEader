#include <preader/common_definitions.h>
#include <preader/ui.h>
#include <string.h>
#include "prdefinitions.h"
#include <fxcg/keyboard.h>
#include <extra_calls.h>

extern SessionConfig cfg;
extern int modified_cfg;

void stat_write(int len) {
    modified_cfg = 1;
    cfg.bytes_written += len;
}
void stat_read(int len) {
    modified_cfg = 1;
    cfg.bytes_read += len;
}
void stat_clear() {
    modified_cfg = 1;
    cfg.bytes_read = 0;
    cfg.bytes_written = 0;
    cfg.pagination_files_created = 0;
    cfg.pages_read = 0;
    cfg.settings_modified = 0;
    cfg.books_added = 0;
}
void stat_page() {
    modified_cfg = 1;
    cfg.pages_read++;
}
void stat_paginate() {
    modified_cfg = 1;
    cfg.pagination_files_created++;
}
void stat_set() {
    modified_cfg = 1;
    cfg.settings_modified++;
}
void stat_book() {
    modified_cfg = 1;
    cfg.books_added++;
}

int __statman_get_digit(unsigned long long a) {
    if (a==0) return 1;
    int cnt = 0;
    while (a > 0) {
        cnt++;
        a = a / 10;
    }
    return cnt;
}

void __statman_fix6_div(unsigned long long a, unsigned long long  b, char *res) {
    char resbuf[32];
    char major[14];
    char minor[6];
    unsigned long long d_major = a / b;
    unsigned long long d_minor = (a * 1000000 / b) % 1000000;
    unsigned int digits = __statman_get_digit(d_minor);
    int flg = 0;
    while (flg < (6-digits)) {
        minor[flg] = '0';
        flg++;
    }
    sprintf(major,"%u",(unsigned int)d_major);
    sprintf(&minor[flg],"%u",(unsigned int)d_minor);
    strcpy(resbuf,major);
    strcat(resbuf,".");
    strcat(resbuf,minor);
    strcpy(res,resbuf);
}
void show_stat() {
    while (1) {
    Bdisp_AllClr_VRAM();
    rect(0,0,383,215,cfg.color_scheme[CI_MENU_BG]);
    printCXYSingleLineCutOffUnprintables(0,0,"ͳ����Ϣ",384,cfg.color_scheme[CI_MENU_BG_CHOSEN],cfg.color_scheme[CI_MENU_BG],0);
    char buf[256];
    char divres[24];
    __statman_fix6_div(cfg.bytes_written,32*1024*1024,divres);
    sprintf(buf,"д���ֽڣ�%u ��%s�Σ�",cfg.bytes_written,divres);
    printMiniSingleLineInRestrictedLineWidth(2,27,buf,384,COLOR_BLUE,cfg.color_scheme[CI_MENU_BG],0);
    __statman_fix6_div(cfg.bytes_read,32*1024*1024,divres);
    sprintf(buf,"��ȡ�ֽڣ�%u ��%s�Σ�",cfg.bytes_read,divres);
    printMiniSingleLineInRestrictedLineWidth(2,47,buf,384,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_BG],0);
    sprintf(buf,"��ȡҳ�棺%uҳ",cfg.pages_read);
    printMiniSingleLineInRestrictedLineWidth(2,67,buf,384,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_BG],0);
    sprintf(buf,"���ɷ�ҳ�ļ���%u��",cfg.pagination_files_created);
    printMiniSingleLineInRestrictedLineWidth(2,87,buf,384,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_BG],0);
    sprintf(buf,"�������ã�%u��",cfg.settings_modified);
    printMiniSingleLineInRestrictedLineWidth(2,107,buf,384,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_BG],0);
    sprintf(buf,"����鼮��%u��",cfg.books_added);
    printMiniSingleLineInRestrictedLineWidth(2,127,buf,384,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_BG],0);
    int item_chosen = 0;
    int key;
    int choosing = 1;
    while (choosing) {
        rect(0,165+24,383,165+24+18,cfg.color_scheme[CI_MENU_BG]);
        char *labels[] = {"����","�������ʲô?","����"};
        int x = 2, y = 165;
        for (int i=0; i<3; i++) {
            color_t bg, fg;
            if (item_chosen == i) {
                bg = cfg.color_scheme[CI_MENU_BG_CHOSEN];
                fg = cfg.color_scheme[CI_MENU_FG_CHOSEN];
            } else {
                bg = cfg.color_scheme[CI_MENU_BG];
                fg = cfg.color_scheme[CI_MENU_FG];
            }
            ProcessPrintChars(936);
            PrintMini(&x,&y,labels[i],0,-1,0,0,fg,bg,1,0);
            x+=16;
            ProcessPrintChars(0);
        }
        rect(0,205,383,209,cfg.color_scheme[CI_MENU_BG]);
        GetKey(&key);
        switch (key) {
            case KEY_CTRL_LEFT:
                item_chosen--;
                if (item_chosen < 0) item_chosen = 2;
                break;
            case KEY_CTRL_RIGHT:
                item_chosen++;
                if (item_chosen >= 3) item_chosen = 0;
                break;
            case KEY_CTRL_EXE:
                if (item_chosen == 0) {
                    return;
                } else if (item_chosen == 1) {
                    __statman_show_help();
                    choosing = 0;
                } else if (item_chosen == 2) {
                    __statman_confirm_clear();
                    choosing = 0;
                }
                break;
            case KEY_CTRL_EXIT:
                return;
        }
    }
    }
    return;
}

void __statman_show_help() {
    int key;
    const static char *help_text = 
"��ҳ����ʾ������Ϊ��Ӧ�����й����в�����һЩ�����ļ�¼�����У�д���ֽڡ���ȡ�ֽ�ֻ����Ӧ�õ�������ȡ��д�������\n\
CASIO Prizmϵ�м�����������NOR Flash�������������ڲ����ڶ�ȡ�Ŷ��������Ͼ������޴εĶ�ȡ������\n\
Ȼ��������Ȼ��������Flash�Ĳ�д���������������Ҫ���ã����Ǽ�¼��Ӧ�ö�Flash���е�д��������Ա�������ʹ�ñ�Ӧ�á�������˵��Flash\n\
���Ա�ȫд����ǧ�Ρ������޼ۣ�����������";
    drawDialog(5,28,371,209);
    printMiniMultiLineNCutOffUnprintables(5,4,help_text,371-5+1,10,COLOR_BLACK,COLOR_WHITE,0);
    GetKey(&key);
}

void __statman_confirm_clear() {
    int ret = msgbox("ȷ��Ҫ�����\n[EXE]ȷ�� [EXIT]ȡ��","��ʾ",64,1,COLOR_GOLD);
    if (ret == KEY_CTRL_EXE) {
        stat_clear();
        infobox("�����㡣",38,1);
    }
}