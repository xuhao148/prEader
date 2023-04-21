
#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/file.h>
#include <string.h>
#include <preader/filedialog.h>
#include <extra_calls.h>
#include <preader/ui.h>
#include <preader/i18n.h>
#include <preader/reader.h>
#include "prdefinitions.h"

extern SessionConfig cfg;
extern int modified_cfg;

void drawList(int slot_begin, int chosen);

void slot_manager(void) {

    EnableStatusArea(3);
    int slot_begin = 0;
    int chosen = 0;
    int key;
    char buf[64];
    int should_exit = 0;

while (!should_exit) {
    Bdisp_AllClr_VRAM();
    rect(0,0,383,215,63389);
    rect(0,0,383,31,14937);
    rect(11,41+chosen*39,372,79+chosen*39,35689);
    ProcessPrintChars(936);
    PrintCXY(5,5,"存储槽位管理器",0x40|0x20,-1,COLOR_WHITE,COLOR_BLACK,1,0);
    drawList(slot_begin,chosen);
    printMiniSingleLineCutOffUnprintables(11,198-24,"[EXE]设为当前 [DEL]删除该记录 [OPTN]交换",384,35689,63389,1);
    ProcessPrintChars(0);
    GetKey(&key);
    int ret;
    switch(key) {
        case KEY_CTRL_LEFT:
            slot_begin -= 4;
            if (slot_begin < 0) slot_begin = 28;
            break;
        case KEY_CTRL_RIGHT:
            slot_begin += 4;
            if (slot_begin >= 32) slot_begin = 0;
            break;
        case KEY_CTRL_UP:
            chosen--;
            if (chosen<0) chosen=3;
            break;
        case KEY_CTRL_DOWN:
            chosen++;
            if (chosen>=4) chosen=0;
            break;
        case KEY_CTRL_EXIT:
            should_exit = 1;
            break;
        case KEY_CTRL_EXE:
            if (cfg.book_records[slot_begin+chosen].book_path[0]) {
            modified_cfg = 1;
            cfg.last_book = &cfg.book_records[slot_begin+chosen];
            }
            break;
        case KEY_CTRL_DEL:
            if (cfg.book_records[slot_begin+chosen].book_path[0]) {
            ret = msgbox("确定要删除吗？\n[F1]确定 [EXIT]取消","警告",60,1,COLOR_GOLD);
            if (ret == KEY_CTRL_F1) {
                modified_cfg = 1;
                cfg.book_records[slot_begin+chosen].book_path[0] = 0;
                cfg.book_records[slot_begin+chosen].last_byte_location = -1;
                cfg.book_records[slot_begin+chosen].last_location = -1;
                cfg.n_book_records--;
                /* Dealing with CURRENT book record */
                if (cfg.has_last_book && cfg.last_book == &cfg.book_records[slot_begin+chosen]) {
                    cfg.has_last_book = 0;
                }
            }
            }
            break;
        case KEY_CTRL_OPTN:
            {
                int optn_key;
                int needs_to_exit = 0;
                int current_exchange = chosen+slot_begin;
                int optn_slot_begin = slot_begin;
                int optn_chosen = chosen;
                while (!needs_to_exit) {
                    Bdisp_AllClr_VRAM();
                    rect(0,0,383,215,63389);
                    rect(0,0,383,31,11968);
                    rect(11,41+optn_chosen*39,372,79+optn_chosen*39,35689);
                    ProcessPrintChars(936);
                    sprintf(buf,"要将#%d与哪个交换？",current_exchange+1);
                    PrintCXY(5,5,buf,0x40|0x20,-1,COLOR_WHITE,COLOR_BLACK,1,0);
                    ProcessPrintChars(0);
                    drawList(optn_slot_begin,optn_chosen);
                    printMiniSingleLineCutOffUnprintables(11,198-24,"[EXE]确定  [EXIT]取消",384,35689,63389,1);
                    GetKey(&optn_key);
                    switch (optn_key) {
                        case KEY_CTRL_LEFT:
                            optn_slot_begin -= 4;
                            if (optn_slot_begin < 0) optn_slot_begin = 28;
                            break;
                        case KEY_CTRL_RIGHT:
                            optn_slot_begin += 4;
                            if (optn_slot_begin >= 32) optn_slot_begin = 0;
                            break;
                        case KEY_CTRL_UP:
                            optn_chosen--;
                            if (optn_chosen<0) optn_chosen=3;
                            break;
                        case KEY_CTRL_DOWN:
                            optn_chosen++;
                            if (optn_chosen>=4) optn_chosen=0;
                            break;
                        case KEY_CTRL_EXIT:
                            optn_chosen = -1;
                            needs_to_exit = 1;
                            break;
                        case KEY_CTRL_EXE:
                            needs_to_exit = 1;
                            break;
                    }
                }
                if (optn_chosen >= 0) {
                    if (optn_slot_begin+optn_chosen != chosen+slot_begin) {
                    modified_cfg = 1;
                    BookRecord br_temp;
                    if (cfg.last_book == &cfg.book_records[current_exchange]) cfg.last_book = &cfg.book_records[optn_chosen+optn_slot_begin];
                    else if (cfg.last_book == &cfg.book_records[optn_chosen+optn_slot_begin]) cfg.last_book = &cfg.book_records[current_exchange];
                    memcpy(&br_temp,&cfg.book_records[optn_chosen+optn_slot_begin],sizeof(BookRecord));
                    memcpy(&cfg.book_records[optn_chosen+optn_slot_begin],&cfg.book_records[current_exchange],sizeof(BookRecord));
                    memcpy(&cfg.book_records[current_exchange],&br_temp,sizeof(BookRecord));
                    }
                } else {
                    optn_chosen = 0;
                }
                chosen = optn_chosen;
                slot_begin = optn_slot_begin;
            }
            break;
    }
}
    EnableStatusArea(0);
    return;
}

void drawList(int slot_begin, int chosen) {
    char buf[64];
    for (int d=0; d<4; d++) {
        sprintf(buf,"%d",slot_begin+d+1);
        color_t text_color = (d==chosen)?63389:35689;
        draw_custom_font_16x32(12,42+39*d,buf,text_color);
        if (cfg.book_records[slot_begin+d].book_path[0]) {
            strcpy(buf,cfg.book_records[slot_begin+d].book_path);
            duplicateBackSlashes(buf);
            printMiniSingleLineInRestrictedLineWidth(47,43-24+39*d,buf,371-43+1,text_color,COLOR_BLACK,1);
            int mmx = 48, mmy = 63-24+39*d;
            sprintf(buf,"Page %d",cfg.book_records[slot_begin+d].last_location+1);
            Bdisp_MMPrint(mmx,mmy,buf,2,-1,0,0,text_color,COLOR_BLACK,1,0);
            buf[1] = 0;
            buf[0] = cfg.book_records[slot_begin+d].bookmark_version?'S':'L';
            mmx = 136;
            Bdisp_MMPrint(mmx,mmy,buf,2,-1,0,0,text_color,COLOR_BLACK,1,0);
            int bookmark_cnt = 0;
            for (int i=0; i<8; i++) if (cfg.book_records[slot_begin+d].bookmarks[i].page_location >= 0) bookmark_cnt++;
            sprintf(buf,"%d bookmark(s)",bookmark_cnt);
            mmx = 248;
            Bdisp_MMPrint(mmx,mmy,buf,2,-1,0,0,text_color,COLOR_BLACK,1,0);
            strcpy(buf,"[CUR]");
            if (cfg.has_last_book && cfg.last_book == &cfg.book_records[slot_begin+d])
            {
                mmx = 187;
                Bdisp_MMPrint(mmx,mmy,buf,2,-1,0,0,text_color,COLOR_BLACK,1,0);
            }
        } else {
            printMiniSingleLineInRestrictedLineWidth(47,43-24+39*d,"(空)",371-43+1,text_color,COLOR_BLACK,1);
        }
    }
}