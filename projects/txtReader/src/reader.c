#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <preader/ui.h>
#include <preader/filedialog.h>
#include <string.h>
#include <fxcg/file.h>
#include <fxcg/rtc.h>
#include <stdio.h>
#include <preader/i18n.h>
#include <extra_calls.h>
#include <preader/filedialog.h>
#include <preader/textinput.h>
#include <stdlib.h>
#include "prdefinitions.h"
#include "statman.h"

static color_t fgcolor,bgcolor;
const static int zero = 0;

const static color_t progressbar_texture_blue[] = {
48991,48991,24191,48991,17983,24191,7647,17983,7647,
48991,48991,48991,24191,24191,17983,17983,7647,7647,
};

const static color_t progressbar_texture_gray[] = {
40179,40179,40179,50744,40179,59164,50744,65535,65535,
40179,40179,40179,40179,50744,50744,59164,59164,65535
};

int check_magic_paging(char *str);
void rect_progressbar(int x1, int y1, int x2, color_t texture[]);
extern SessionConfig cfg;
extern int modified_cfg;

int read_book(char *fpath) {
    EnableStatusArea(3);
    short wszFilePath[128];
    Bfile_StrToName_ncpy(wszFilePath,fpath,64);

    /* Goes into the main reading window now... */
    DisableGetkeyToMainFunctionReturn();
    
    char pagedataname[64];
    /* Checks whether paging data is available. The paging data is named \\fls0\*.spd in mini font and \\fls0\*.lpd in normal font.*/
    {
    char basename[32];
    char suffix[8];
    char parentdir[64];
    get_file_basename(fpath,basename,suffix,parentdir);
    if (strcmp(suffix,"txt") && strcmp(suffix,"TXT")) {
        sprintf(parentdir,"不支持的后缀（.%s）。\n请选择其它文件。",suffix);
        infobox(parentdir,72,1);
        EnableGetkeyToMainFunctionReturn();
        return R_READER_INVALID_SUFFIX;
    }
    strcpy(pagedataname,"\\\\fls0\\@PRDR\\");
    strcat(pagedataname,basename);
    strcat(pagedataname,".");
    if (cfg.font_size) {
        strcat(pagedataname,"spd");
    } else {
        strcat(pagedataname,"lpd");
    }
    }

    int fhPageData;
    {
        unsigned short wszPageDataPath[128];
        Bfile_StrToName_ncpy(wszPageDataPath,pagedataname,64);
        fhPageData = Bfile_OpenFile_OS(wszPageDataPath,READ,0);
    }
    int fhBookFileHandle = Bfile_OpenFile_OS(wszFilePath,READ,0);
    if (fhBookFileHandle < 0) {
        char errbuf[256];
        char pathbuf[72];
        strcpy(pathbuf,fpath);
        duplicateBackSlashes(pathbuf);
        sprintf(errbuf,"打开文件%s时错误（ret=%d)\n请检查文件系统，并再试。",pathbuf,fhBookFileHandle);
        info_error(errbuf,100,1);
        return R_READER_NXBOOK;
    }
    PDCache current_page_data = {{{'P','A','G','D','T','A'},-1,-1,cfg.font_size}};
    if (fhPageData < 0) {
        unsigned short wszPageDataPath[128];
        Bfile_StrToName_ncpy(wszPageDataPath,pagedataname,64);
        start_paging(fhBookFileHandle,&current_page_data,&cfg);
        int cfret;
        int filesize;
        filesize = sizeof(PagingDataHeader)+4*current_page_data.hdr.n_pages_avail;
        Bdisp_AllClr_VRAM();
        PrintXY(1,1,"\x3\xa8正在创建并写入分页文件……",0,0);
        Bdisp_PutDisp_DD();
        cfret = Bfile_CreateEntry_OS(wszPageDataPath,CREATEMODE_FILE,&zero);
        if (cfret < 0) {
            char errbuf[64];
            sprintf(errbuf,"无法创建分页文件（errno=%d）\n请检查文件系统。",cfret);
            Bfile_CloseFile_OS(fhBookFileHandle); fatal_error(errbuf,72,1);
        }
        fhPageData = Bfile_OpenFile_OS(wszPageDataPath,WRITE,0);
        if (fhPageData < 0) {
            char errbuf[64];
            sprintf(errbuf,"无法写入分页文件（errno=%d）\n请检查文件系统。",cfret);
           Bfile_CloseFile_OS(fhBookFileHandle); fatal_error(errbuf,72,1);
        }
        Bdisp_PutDisp_DD();
        stat_paginate();
        Bfile_WriteFile_OS(fhPageData,&current_page_data,filesize);
        stat_write(filesize);
        Bfile_CloseFile_OS(fhPageData);
    } else {
        //never forget closing files!
        Bfile_ReadFile_OS(fhPageData,&current_page_data.hdr,sizeof(current_page_data.hdr),0);
        stat_read(sizeof(current_page_data.hdr));
        if (!check_magic_paging(current_page_data.hdr.magic)) {
            Bfile_CloseFile_OS(fhBookFileHandle);Bfile_CloseFile_OS(fhPageData); info_error("不正确的分页文件（MAGIC错误）。\n请于文件管理器中删除分页文件，再使用本软件重建分页文件。",100,1);
            EnableGetkeyToMainFunctionReturn();return R_READER_WRONG_FORMAT;
        }
        int filesize = sizeof(PagingDataHeader) + 4 * current_page_data.hdr.n_pages_avail;
        int real_filesize = Bfile_GetFileSize_OS(fhPageData);
        if (real_filesize != filesize) {
            Bfile_CloseFile_OS(fhBookFileHandle);
            Bfile_CloseFile_OS(fhPageData);
            infobox("不正确的分页文件（大小错误）。\n请于文件管理器中删除%s，再使用本软件重建分页文件。",100,1);EnableGetkeyToMainFunctionReturn();return R_READER_WRONG_FORMAT;
        }
        if (current_page_data.hdr.font != cfg.font_size) {
            Bfile_CloseFile_OS(fhBookFileHandle);
            Bfile_CloseFile_OS(fhPageData);
            infobox("不正确的分页文件（对应字体错误）。\n请于文件管理器中删除%s，再使用本软件重建分页文件。",100,1);EnableGetkeyToMainFunctionReturn();return R_READER_WRONG_FORMAT;
        }
        Bfile_ReadFile_OS(fhPageData,&current_page_data,filesize,0);
        stat_read(filesize);
        Bfile_CloseFile_OS(fhPageData);
    }


    /* Prepare the reading environment.*/
    char filename_real[96];
    char buf[32];
    char status_msg[96];
    strcpy(filename_real,fpath);
    if (getSystemLanguage() == 5) duplicateBackSlashes(filename_real);
    int in_reading = 1;
    int current_page = 0;
    int key;
    int quit = 0;

    /* Check whether there is a record for this book in the session config. */
    int index_cfg = -1;
    for (int i=0; i<32; i++) {
        if (strcmp(cfg.book_records[i].book_path,fpath) == 0) {
            index_cfg = i; break;
        }
    }
    if (index_cfg == -1) {
        if (cfg.n_book_records < 32) {
            /* Adds the record. */
            modified_cfg = 1;
            index_cfg = 0;
            while (index_cfg < 32 && cfg.book_records[index_cfg].book_path[0]) index_cfg++; //Looks for the first empty slot
            if (index_cfg == 32) {info_error("配置文件空间声称剩余空间与实际剩余空间不一致。\n此书仍可以阅读，但无法保存书签及最后阅读的位置。\n请考虑重建配置文件。",100,1); index_cfg = -1;}
             else {cfg.n_book_records++;
             stat_book();
            strcpy(cfg.book_records[index_cfg].book_path,fpath);
            cfg.book_records[index_cfg].last_location = 0;
            cfg.book_records[index_cfg].bookmark_version = cfg.font_size;
            for (int i=0; i<8; i++) {
                cfg.book_records[index_cfg].bookmarks[i].byte_location = -1;
                cfg.book_records[index_cfg].bookmarks[i].page_location = -1;
            }
             }
        } else {
            infobox("配置文件空间已满，无法添加此书的记录。\n此书仍可以阅读，但无法保存书签及最后阅读的位置。\n请考虑重建配置文件。",100,1);
        }
    } else {
        if (cfg.book_records[index_cfg].bookmark_version != cfg.font_size)
        {
            /* Last location conversion */
            int p;
            for (p=0; p<current_page_data.hdr.n_pages_avail; p++) {
                if (current_page_data.pages[p] > cfg.book_records[index_cfg].last_byte_location) {
                    p--; break;
                }
            }
            if (p == current_page_data.hdr.n_pages_avail) p--;
            cfg.book_records[index_cfg].last_location = p;
            cfg.book_records[index_cfg].last_byte_location = current_page_data.pages[p];
            /* Bookmark conversion */
            for (int i=0; i<8; i++) {
                if (cfg.book_records[index_cfg].bookmarks[i].byte_location >= 0) {
                    int p;
                    for (p=0; p<current_page_data.hdr.n_pages_avail; p++) {
                        if (current_page_data.pages[p] > cfg.book_records[index_cfg].bookmarks[i].byte_location) {
                            p--; break;
                        }
                    }
                    if (p == current_page_data.hdr.n_pages_avail) p--;
                    cfg.book_records[index_cfg].bookmarks[i].page_location = p;
                    //cfg.book_records[index_cfg].bookmarks[i].byte_location = current_page_data.pages[p];
                }
            }
            cfg.book_records[index_cfg].bookmark_version = cfg.font_size;

        }
        current_page = cfg.book_records[index_cfg].last_location;
    }

    /* Open the image file. */
    int bgImageFileFh;
    if (cfg.use_bgpict)
    {
        unsigned short wchar_path[64];
        Bfile_StrToName_ncpy(wchar_path,cfg.bgpict_path,32);
        bgImageFileFh = Bfile_OpenFile_OS(wchar_path,READ,0);
        if (bgImageFileFh < 0) {
            Bfile_CloseFile_OS(fhBookFileHandle);
            fatal_error("无法打开背景图片文件。",64,1);
        }
        int fsize = Bfile_GetFileSize_OS(bgImageFileFh);
        if (fsize != 384*216*2) {
            Bfile_CloseFile_OS(bgImageFileFh);
            Bfile_CloseFile_OS(fhBookFileHandle);
            info_error("指定的图片文件大小错误。\n请重新选择一个图片。",100,1);
            return R_READER_INVALID_BGPICT;
        }
    }

    unsigned short *vram = GetVRAMAddress();
    if (cfg.use_bgpict)
    {
        Bfile_ReadFile_OS(bgImageFileFh,vram,384*216*2,0);
        stat_read(384*216*2);
        SaveVRAM_1();
    }
    /* Start the reading loop. */
    //DefineStatusAreaFlags(1,0,0,0);

    bgcolor = cfg.color_scheme[CI_READER_BG];
    fgcolor = cfg.color_scheme[CI_READER_FG];
    DrawFrame(bgcolor);
    int page_needs_refreshing = 1;
    while (in_reading) {
        if (page_needs_refreshing) {
            rect(0,0,383,215,bgcolor);
        }
        /*
        if (index_cfg == -1)
            sprintf(status_msg,"[!] %s %d/%d %d%%",filename_real,current_page+1,current_page_data.hdr.n_pages_avail,100*(current_page+1)/current_page_data.hdr.n_pages_avail);
        else
            sprintf(status_msg,"[%d] %s %d/%d %d%%",index_cfg,filename_real,current_page+1,current_page_data.hdr.n_pages_avail,100*(current_page+1)/current_page_data.hdr.n_pages_avail);
        DefineStatusMessage(status_msg,0,TEXT_COLOR_BLACK,0);
        */
        if (cfg.use_bgpict && page_needs_refreshing) LoadVRAM_1();
        if (!cfg.hide_ui)
        {
            /* Draws the status area manually. */
            rect(0,0,383,19,cfg.color_scheme[CI_MENU_BG_CHOSEN]);
            rect(6,1,23,17,cfg.color_scheme[CI_MENU_FG_UNAVAIL]);
            char buf[8];
            sprintf(buf,"%d",index_cfg+1);
            if (index_cfg >= 9)
                draw_custom_font_8x16(8,3,buf,cfg.color_scheme[CI_MENU_FG_CHOSEN]);
            else if (0 <= index_cfg && index_cfg <= 8)
                draw_custom_font_8x16(12,3,buf,cfg.color_scheme[CI_MENU_FG_CHOSEN]);
            else if (index_cfg < 0) {
                rect(13,2,11,6,cfg.color_scheme[CI_MENU_FG_CHOSEN]);
                rect(14,7,10,11,cfg.color_scheme[CI_MENU_FG_CHOSEN]);
                rect(14,14,10,16,cfg.color_scheme[CI_MENU_FG_CHOSEN]);
            }
            Bdisp_MMPrint(27,2,fpath,0x40|0x2,-1,0,0,cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_BG_CHOSEN],1,0);
            rect(203,0,383,19,cfg.color_scheme[CI_MENU_FG_UNAVAIL]);
            sprintf(buf,"%d",current_page+1);
            Bdisp_MMPrint(206,2,buf,0x40|0x2,-1,0,0,cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_BG_CHOSEN],1,0);
            rect(245,0,383,19,cfg.color_scheme[CI_MENU_FG_UNAVAIL]);
            Bdisp_MMPrint(245,2,"/",0x40|0x2,-1,0,0,cfg.color_scheme[CI_MENU_BG_CHOSEN],cfg.color_scheme[CI_MENU_BG_CHOSEN],1,0);
            sprintf(buf,"%d",current_page_data.hdr.n_pages_avail);
            Bdisp_MMPrint(253,2,buf,0x40|0x2,-1,0,0,cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_BG_CHOSEN],1,0);
            rect(294,0,383,19,cfg.color_scheme[CI_MENU_FG_UNAVAIL]);
            int percentage = (current_page+1) * 1000 / current_page_data.hdr.n_pages_avail;
            int dot = percentage % 10;
            percentage /= 10;
            sprintf(buf,"%d.%d%%",percentage,dot);
            Bdisp_MMPrint(293,2,buf,0x40|0x2,-1,0,0,cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_BG_CHOSEN],1,0);
            unsigned int h,m,s,ms;
            RTC_GetTime(&h,&m,&s,&ms);
            sprintf(buf,"%d%d:%d%d",(h&0xF0)>>4,(h&0x0F),(m&0xF0)>>4,(m&0x0F));
            Bdisp_MMPrint(342,2,buf,0x40|0x2,-1,0,0,cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_BG_CHOSEN],1,0);
        }
        if (page_needs_refreshing) {
            draw_one_page(fhBookFileHandle,current_page_data.pages[current_page],cfg.font_size,0,cfg.process_backslashes);
            stat_page();
        }
        void *fkey_menu;
        void *fkey_jump;
        GetFKeyPtr(396,&fkey_menu);
        GetFKeyPtr(508,&fkey_jump);
        if (!cfg.hide_ui) {
        FKey_Display(0,fkey_menu);
        FKey_Display(1,fkey_jump);
        }
        /* Drawing the progress bar */
        if (cfg.draw_progressbar) {
            if (cfg.hide_ui) {
                rect(3,198,357,208,COLOR_BLACK);
                //rect(4,199,356,207,COLOR_WHITE);
                rect_progressbar(4,199,356,progressbar_texture_gray);
                int d_pix = 3+(356-4+1)*(current_page+1)/(current_page_data.hdr.n_pages_avail);
                //rect(4,199,d_pix>356?356:d_pix,207,0xBF7D);
                rect_progressbar(4,199,d_pix>356?356:d_pix,progressbar_texture_blue);
            } else {
                rect(129,198,357,208,COLOR_BLACK);
                //rect(130,199,356,207,COLOR_WHITE);
                rect_progressbar(130,199,356,progressbar_texture_gray);
                int d_pix = 130+(356-130+1)*(current_page+1)/(current_page_data.hdr.n_pages_avail);
                //rect(130,199,d_pix>356?356:d_pix,207,0xBF7D);
                rect_progressbar(130,199,d_pix>356?356:d_pix,progressbar_texture_blue);
            }
            char sbuf[8] = {0};
            int percentage = 100 * (current_page + 1) / current_page_data.hdr.n_pages_avail;
            if (percentage != 100)
                sprintf(sbuf,"%d%%",percentage);
            else
                strcpy(sbuf,"EOF");
            if (percentage >= 10)
                draw_custom_font_8x16(359,197,sbuf,cfg.color_scheme[CI_READER_FG]);
            else
                draw_custom_font_8x16(359+8,197,sbuf,cfg.color_scheme[CI_READER_FG]);
        }
        GetKey(&key);
        MenuItem menu_f1[] = {1,"显示文件信息",1,"立即重建分页文件",1,"清除此书所有书签",1,"返回主菜单",1,"退出程序"};
        MenuItem menu_f2[] = {1,"按页数",1,"到书签",1,"存储到书签…",1,"删除书签…"};
        if (index_cfg == -1) {menu_f2[1].enabled = 0; menu_f2[2].enabled = 0; menu_f2[3].enabled = 0;}
        int opt_ret;
        switch (key) {
            case KEY_CTRL_EXIT:
                in_reading = 0; break;
            case KEY_CTRL_F1: case KEY_CTRL_MENU:
                page_needs_refreshing = 1;
                drawDialog(20,42,361,189);
rect(20,42,361,189,cfg.color_scheme[CI_MENU_BG]);
                opt_ret = flexibleMenu(20,42-24,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,361-42+1,2,5,menu_f1,5,0,1,0);
                if (opt_ret == 0) {    char buf[128];
    sprintf(buf,"文件路径：%s\n文件句柄：%d\n文件大小：%d字节\n文件页数：%d\n当前页码：%d\n配置文件槽位：%d",filename_real,fhBookFileHandle, Bfile_GetFileSize_OS(fhBookFileHandle),current_page_data.hdr.n_pages_avail,current_page+1,index_cfg);
    if (getSystemLanguage() != 5)
        duplicateBackSlashes(buf);
    msgbox(buf,"文件信息",156,1,COLOR_BLUE);}
                if (opt_ret == 2) {
                    int chs = msgbox("确定清除此书的书签？\n[EXE] 确定 [EXIT] 取消","警告",50,1,COLOR_GOLD);
                    if (chs == KEY_CTRL_EXE) {
                        for (int i=0; i<8; i++) {
                            cfg.book_records[index_cfg].bookmarks[i].byte_location=-1;
                            cfg.book_records[index_cfg].bookmarks[i].page_location=-1;
                        }
                    }
                }
                if (opt_ret == 1) {
                    page_immediately(fhBookFileHandle,fpath,&cfg);
                }
                if (opt_ret == 3) in_reading = 0;
                if (opt_ret == 4) {in_reading = 0; quit = 1;}
                break;
            case KEY_CTRL_F2:
                page_needs_refreshing = 1;
                drawDialog(20,42,361,189);
rect(20,42,361,189,cfg.color_scheme[CI_MENU_BG]);
                opt_ret = flexibleMenu(20,42-24,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,361-42+1,2,4,menu_f2,4,0,1,0);
                switch (opt_ret) {
                    case 0:
                    {
                        int val_page;
                        char buf[16] = {0};
                        drawDialog(50,66,330,175);
                        printCXYSingleLineCutOffUnprintables(50,66-24,"请输入页码：",330-50,COLOR_BLACK,COLOR_WHITE,0);
                         int key = 0,kcol = 0,krow = 0;
                        getTextLine(buf,8,4,6,8,INPUT_MODE_POSINT);
                        val_page = atoi(buf);
                        if (val_page < 1 || val_page > current_page_data.hdr.n_pages_avail) {
                            msgbox("页码不在范围内！","错误",38,1,COLOR_RED);
                        } else {
                            current_page = val_page - 1;
                        }
                    }
                    break;
                    case 1:
                    {
                        int available_bms = 0, first_available = -1;
                        for (int i=0; i<8; i++) {
                            if (cfg.book_records[index_cfg].bookmarks[i].byte_location >= 0)
                                {available_bms++; first_available = i;}
                        }
                        if (!available_bms) {
                            msgbox("没有可用的书签！","错误",38,1,COLOR_RED);
                        } else {
                            MenuItem bookmarks[] = {0,"1:[无记录]",0,"2:[无记录]",0,"3:[无记录]",0,"4:[无记录]",
                                                    0,"5:[无记录]",0,"6:[无记录]",0,"7:[无记录]",0,"8:[无记录]"};
                            char bookmark_labels[8][64];
                            for (int i=0; i<8; i++) {
                                if (cfg.book_records[index_cfg].bookmarks[i].byte_location >= 0) {
                                    bookmarks[i].enabled = 1;
                                    sprintf(bookmark_labels[i],"%d:第%d页 %s",i+1,cfg.book_records[index_cfg].bookmarks[i].page_location+1,cfg.book_records[index_cfg].bookmarks[i].preview);
                                    bookmarks[i].label = bookmark_labels[i];
                                }
                            }
                            drawDialog(50,66,329,197);
rect(50,66,329,197,cfg.color_scheme[CI_MENU_BG]);
                            int choice = flexibleMenu(50,66-24,cfg.color_scheme[CI_MENU_BG],1,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,329-50+1-6,1,8,bookmarks,7,first_available,1,1);
                            if (choice >= 0) current_page = cfg.book_records[index_cfg].bookmarks[choice].page_location;
                        }
                    }
                    break;
                    case 2:
                    {
                            MenuItem bookmarks[] = {1,"1:[无记录]",1,"2:[无记录]",1,"3:[无记录]",1,"4:[无记录]",
                                                    1,"5:[无记录]",1,"6:[无记录]",1,"7:[无记录]",1,"8:[无记录]"};
                            char bookmark_labels[8][64];
                            for (int i=0; i<8; i++) {
                                if (cfg.book_records[index_cfg].bookmarks[i].byte_location >= 0) {
                                    sprintf(bookmark_labels[i],"%d:第%d页 %s",i+1,cfg.book_records[index_cfg].bookmarks[i].page_location+1,cfg.book_records[index_cfg].bookmarks[i].preview);
                                    bookmarks[i].label = bookmark_labels[i];
                                }
                            }
                            drawDialog(50,66,329,197);
rect(50,66,329,197,cfg.color_scheme[CI_MENU_BG]);
                            int choice = flexibleMenu(50,66-24,cfg.color_scheme[CI_MENU_BG],1,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,329-50+1-6,1,8,bookmarks,7,0,1,1);
                            if (choice >= 0) {cfg.book_records[index_cfg].bookmarks[choice].byte_location = current_page_data.pages[current_page];
                            cfg.book_records[index_cfg].bookmarks[choice].page_location = current_page;
                            Bfile_ReadFile_OS(fhBookFileHandle,cfg.book_records[index_cfg].bookmarks[choice].preview,15,current_page_data.pages[current_page]); }
                            stat_read(15);
                    }
                    break;
                    case 3:
                    {
                        int available_bms = 0, first_available = -1;
                        for (int i=0; i<8; i++) {
                            if (cfg.book_records[index_cfg].bookmarks[i].byte_location >= 0)
                                {available_bms++; first_available = i;}
                        }
                        if (!available_bms) {
                            msgbox("没有可用的书签！","错误",38,1,COLOR_RED);
                        } else {
                            MenuItem bookmarks[] = {0,"1:[无记录]",0,"2:[无记录]",0,"3:[无记录]",0,"4:[无记录]",
                                                    0,"5:[无记录]",0,"6:[无记录]",0,"7:[无记录]",0,"8:[无记录]"};
                            char bookmark_labels[8][64];
                            for (int i=0; i<8; i++) {
                                if (cfg.book_records[index_cfg].bookmarks[i].byte_location >= 0) {
                                    bookmarks[i].enabled = 1;
                                    sprintf(bookmark_labels[i],"%d:第%d页 %s",i+1,cfg.book_records[index_cfg].bookmarks[i].page_location+1,cfg.book_records[index_cfg].bookmarks[i].preview);
                                    bookmarks[i].label = bookmark_labels[i];
                                }
                            }
                            drawDialog(50,66,329,197);
rect(50,66,329,197,cfg.color_scheme[CI_MENU_BG]);
                            int choice = flexibleMenu(50,66-24,cfg.color_scheme[CI_MENU_BG],1,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,329-50+1-6,1,8,bookmarks,7,first_available,1,1);
                            if (choice >= 0) {cfg.book_records[index_cfg].bookmarks[choice].byte_location = -1;
                            cfg.book_records[index_cfg].bookmarks[choice].page_location = -1;
                            }
                        }
                    }
                    break;
                    case -1:
                    break;
                    default:
                    fatal_error("程序运行到了一个不该到达的地点。\n请检查你的运行环境，并与开发者联系。",72,1);
                    break;
                }
                break;
            case KEY_CTRL_EXE: case KEY_CTRL_RIGHT:
                page_needs_refreshing = 1;
                current_page++;
                if (current_page >= current_page_data.hdr.n_pages_avail) 
                    current_page = current_page_data.hdr.n_pages_avail - 1;
                break;
            case KEY_CTRL_LEFT:
                page_needs_refreshing = 1;
                current_page--;
                if (current_page < 0)
                    current_page = 0;
                break;
            case KEY_CTRL_UP:
                page_needs_refreshing = 1;
                current_page-=10;
                if (current_page < 0)
                    current_page = 0;
                break;
            case KEY_CTRL_DOWN:
                page_needs_refreshing = 1;
                current_page+=10;
                if (current_page >= current_page_data.hdr.n_pages_avail) 
                    current_page = current_page_data.hdr.n_pages_avail - 1;
                break;
            default:
                page_needs_refreshing = 0;
                break;
        }
    }
    if (index_cfg != -1)
    {
        modified_cfg = 1;
        cfg.book_records[index_cfg].last_location = current_page;
        cfg.book_records[index_cfg].last_byte_location = current_page_data.pages[current_page];
        cfg.last_book = &cfg.book_records[index_cfg];
        cfg.has_last_book = 1;
    }
    Bfile_CloseFile_OS(fhBookFileHandle);
    Bfile_CloseFile_OS(bgImageFileFh);
    DefineStatusMessage("",0,TEXT_COLOR_BLACK,0);
    DefineStatusAreaFlags(1,0,0,0);
    EnableStatusArea(0);
    EnableGetkeyToMainFunctionReturn();
    if (quit)
        return R_READER_STRAIGHT_EXIT;
    else
        return R_SUCCESS;
    fatal_error("程序运行到了一个不该到达的地点。\n请检查你的运行环境，并与开发者联系。",72,1);
}

/* Draws one page based on the file handle and the offset of the file, treating '\n' and
   "\r\n" as newline.
   If arg. dryrun is true, it won't be actually drawn; this is for the paging feature.
   If arg. process_back_slashes is true, the backslashes will be duplicated before printing;
   This is because in GB18030 mode, the system treats backslashes as part of escape sequences,
   and "\\" is printed as "\".
   Arg. font determines the font size. 0 for large font, 1 for small font.
   Returns the offset of next character after this page. If this is the last page, returns -1.*/
int draw_one_page(int filehandle, int offset, int font, int dryrun, int process_black_slashes) {
    unsigned char linebuf[257] = {0}; /*Buffer for drawing a single line.*/
    int maxline = font?9:7;
    int filesize = Bfile_GetFileSize_OS(filehandle);
    int size_to_read = 128;
    int current_line_offset = 0;
    int new_line_offset = 0;
    int this_is_the_last_line = 0;
    /*
    if (dryrun) {
        // Does pure pagination algorithm that is faster than actually
        // dry-running the pagination feature.  Thanks ExAcler!
        int lines_drawn = 0, sx = 0;
        unsigned char screenbuf[2050] = {0};
        memset(screenbuf,0,font?1025:451);
        Bfile_ReadFile_OS(filehandle, screenbuf, font?1024:450, offset);
        screenbuf[1024] = 0;
        if (cfg.extra_settings&BS_USE_STRICT_RENDERER) {
            for (int i=0; screenbuf[i]; i++) {
                unsigned char current_ch = linebuf[i];
                if (current_ch == 0x00 || current_ch == 0x09) linebuf[i]=0x20;
                else if (current_ch < 0x20 && current_ch != 0x0D && current_ch != 0x0A) linebuf[i] ='?';
                else if (current_ch >= 0x80 && current_ch <= 0x1F || current_ch == 0xFF) linebuf[i] = '_';
            }
        }
        if (process_black_slashes) duplicateBackSlashes(screenbuf);
        int flg = 0;
        int cur_char_width; int cur_char_dbyte;
        for (;;) {
            if (screenbuf[flg] == 0) return -1;
            if (screenbuf[flg] >= 0x81 && screenbuf[flg] <= 0xF7) {
                // GB Char
                if (screenbuf[flg+1]) {
                    cur_char_dbyte = 2;
                    cur_char_width = font?16:24;
                    goto width_calc;
                } else {
                    return -1; // EOF
                }
            } else {
                if (screenbuf[flg] == '\r') {
                    if (screenbuf[flg+1] == '\n') {
                        cur_char_width = 0;
                        cur_char_dbyte = 2;
                        goto new_line;
                    } else if (screenbuf[flg+1]) {
                        cur_char_width = 0;
                        cur_char_dbyte = 1;
                        goto new_line;
                    } else {
                        return -1;
                    }
              　  } else if (screenbuf[flg] == '\n') {
                    if (screenbuf[flg+1]) {
                        cur_char_width = 0;
                        cur_char_dbyte = 1;
                        goto new_line;
                    } else return -1;
                } else {
                    cur_char_dbyte = 1;
                    if (font) {
                        GetMiniGlyphPtr(screenbuf[flg], &cur_char_width);
                    } else {
                        cur_char_width = 18;
                    }
                    goto width_calc;
                }
            }

            width_calc: // Ordinary chars
            sx += cur_char_width;
            if (sx > 384) {
                sx = 0;
                lines_drawn++;
                if (lines_drawn > maxline) {
                    // Current char is not rendered
                    return offset+flg;
                } else {
                    flg+=cur_char_dbyte;
                }
            } else {
                flg+=cur_char_dbyte;
            }
            if (screenbuf[flg] == 0) return -1;
            continue;

            new_line: // Manual new lines
            sx = 0;
            lines_drawn++;
            if (lines_drawn > maxline) {
                return offset+flg+cur_char_dbyte;
            } else {
                flg+=cur_char_dbyte;
            }
            if (screenbuf[flg] == 0) return -1;
            continue;
        }
        return -1;
    }
    */

    for (int l=0; l<maxline && !this_is_the_last_line; l++) {
        if (offset + current_line_offset+ size_to_read >= filesize) {size_to_read = filesize-offset-current_line_offset;}
        Bfile_ReadFile_OS(filehandle,linebuf,size_to_read,offset+current_line_offset);
        stat_read(size_to_read);
        if (cfg.extra_settings&BS_USE_STRICT_RENDERER) {
            for (int i=0; i<size_to_read; i++) {
                unsigned char current_ch = linebuf[i];
                if (current_ch == 0x00 || current_ch == 0x09) linebuf[i]=0x20;
                else if (current_ch < 0x20 && current_ch != 0x0D && current_ch != 0x0A) linebuf[i] ='?';
                else if (current_ch >= 0x80 && current_ch <= 0x1F || current_ch == 0xFF) linebuf[i] = '_';
            }
        }
        linebuf[size_to_read] = 0;
        int extra_offset = 0;
        if (process_black_slashes)
            duplicateBackSlashes(linebuf);
        int i;
        int newline_char_offset = 127;
        for (i=0; i<=127; i++) {
            if (linebuf[i] == '\r') {
                linebuf[i] = 0;
                newline_char_offset = i;
                if (linebuf[i+1] == '\n') i++;
                i++; break;
            } else if (linebuf[i] == '\n') {
                newline_char_offset = i;
                linebuf[i] = 0; i++; break;
            }
        }
        /* After this, i will be the offset of first char of the next line, and the linebuf is truncated
           into the first line.*/
        int printable_bytes = bytesYouCanPrintInALineGB2312(linebuf,LCD_WIDTH_PX,font);

        if (printable_bytes < newline_char_offset) current_line_offset += printable_bytes;
        else current_line_offset += i;
        linebuf[printable_bytes] = 0;

        if (process_black_slashes)
            for (int f = 0; linebuf[f]; f++) {
                if (linebuf[f] == '\\') {
                    extra_offset++;
                    f++;
                }
            }

        if (font) {
            int x, y;
            x = 0; y = 18 * l;
            if (!dryrun) {
            ProcessPrintChars(936);
            PrintMini(&x,&y,linebuf,0x02,-1,0,0,fgcolor,bgcolor,1,0);
            ProcessPrintChars(0);
            }
        } else {
            if (!dryrun) {
            ProcessPrintChars(936);
            PrintCXY(0,24*l,linebuf,0x20,-1,fgcolor,bgcolor,1,0);
            ProcessPrintChars(0);
            }
        }

        current_line_offset -= extra_offset; // Thanks xiyihan

        if (offset + current_line_offset >= filesize) {this_is_the_last_line = 1;}
    }
    if (!this_is_the_last_line) return offset+current_line_offset; else return -1;
}

void get_file_basename(const char *pathname, char *filename, char *suffix, char *parentdir) {
    int i = strlen(pathname);
    int j = strlen(pathname);
    do {
        i--;
    } while (pathname[i] != '\\');
    do {
        j--;
    } while (pathname[j] != '.');
    i++;
    strncpy(filename,&pathname[i],j-i);
    filename[j-i] = 0;
    strcpy(suffix,&pathname[j+1]);
    strncpy(parentdir,pathname,i);
    parentdir[i] = 0;
}

void start_paging(int filehandle, PDCache *pagedata, SessionConfig *config) {
    int offset = 0;
    int i=0;
    int filesize = Bfile_GetFileSize_OS(filehandle);
    int dec = 0;
    ProcessPrintChars(936);
    ProgressBar2("正在分页……",0,filesize);
    ProcessPrintChars(0);
    while (i<8192) {
        pagedata->pages[i] = offset;
        offset = draw_one_page(filehandle,offset,cfg.font_size,1,cfg.process_backslashes);
        if (dec != offset * 10 / filesize)
        {
            dec = offset * 10 / filesize;
            ProcessPrintChars(936); ProgressBar2("正在分页……",dec,10); ProcessPrintChars(0);
        }
        i++;
        if (offset < 0) break;
    }
    MsgBoxPop();
    pagedata->hdr.n_pages_avail = i;
    pagedata->hdr.font = cfg.font_size;
    return;
}

int check_magic_paging(char *str) {
    if (str[0] == 'P' && str[1] == 'A' && str[2] == 'G' && str[3] == 'D' && str[4] == 'T' && str[5] == 'A') return 1;
    return 0;
}

int page_immediately(int handle, char *fpath, SessionConfig *config) {
    char fname[64],suffix[8],pdir[48],pagefname[64];
    unsigned short w_pagefname[65];
    get_file_basename(fpath,fname,suffix,pdir);
    strcpy(pagefname,"\\\\fls0\\");
    strcat(pagefname,fname);
    strcat(pagefname,".");
    if (config->font_size) strcat(pagefname,"spd");
    else strcat(pagefname,"lpd");
    Bfile_StrToName_ncpy(w_pagefname,pagefname,64);
    int fhImmediatePageFileHandle = Bfile_OpenFile_OS(w_pagefname,READ,0);
    if (fhImmediatePageFileHandle > 0) {
        Bfile_CloseFile_OS(fhImmediatePageFileHandle);
        int choice = msgbox("这将删除原分页文件。\n继续？\n[EXIT] 取消 [EXE] 确定","警告",82,1,COLOR_GOLD);
        if (choice == KEY_CTRL_EXE) {
            int ret = Bfile_DeleteEntry(w_pagefname);
            if (ret < 0) {
                msgbox("无法删除原分页文件！","错误",40,1,COLOR_RED);
                return -1; // Unable to delete
            }
        } else {
            return -2; // User cancelled the operation
        }
    }
    PDCache pagedata = {{{'P','A','G','D','T','A'},-1,-1,config->font_size}};
    start_paging(handle,&pagedata,config);
    Bdisp_AllClr_VRAM();
    PrintXY(1,1,"\x3\xa8检查分页数据……",0,0);
    Bdisp_PutDisp_DD();
    int filesize = sizeof(PagingDataHeader)+4*pagedata.hdr.n_pages_avail;
    Bdisp_AllClr_VRAM();
    PrintXY(1,1,"\x3\xa8创建分页文件……",0,0);
    Bdisp_PutDisp_DD();
    int ret = Bfile_CreateEntry_OS(w_pagefname,CREATEMODE_FILE,&zero);
    if (ret < 0) {
        msgbox("无法创建新的分页文件！\n下次阅读此书时，您必须重建分页文件。","错误",68,1,COLOR_RED);
        return -8; //Failed on file creation
    }
    Bdisp_AllClr_VRAM();
    PrintXY(1,1,"\x3\xa8打开分页文件……",0,0);
    Bdisp_PutDisp_DD();
    fhImmediatePageFileHandle = Bfile_OpenFile_OS(w_pagefname,WRITE,0);
    if (fhImmediatePageFileHandle < 0) {
        msgbox("无法写入新的分页文件！\n下次阅读此书时，您必须重建分页文件。","错误",68,1,COLOR_RED);
    }
    Bdisp_AllClr_VRAM();
    PrintXY(1,1,"\x3\xa8写入分页文件……",0,0);
    Bdisp_PutDisp_DD();
    Bfile_WriteFile_OS(fhImmediatePageFileHandle,&pagedata,filesize);
    stat_write(filesize);
    Bfile_CloseFile_OS(fhImmediatePageFileHandle);
    stat_paginate();
    infobox("重建成功。\n请重新进入以查看效果。",56,1);
    return 0;
}

void rect_progressbar(int x1, int y1, int x2, color_t texture[]) {
    static short *vram = NULL;
    if (vram == NULL) vram = GetVRAMAddress();
    if (x1<0 || x2<0) return R_INVALID;
    if (x1>=384 || x2>=384) return R_INVALID;
    if (y1<0) return R_INVALID;
    if (y1+8 >= 216) return R_INVALID;
    if (x1>x2) return R_INVALID;
    int dx = x2 - x1 + 1;
    for (int cy=0; cy<9; cy++) {
        for (int cx=0; cx<dx; cx++) {
            vram[(y1+cy)*384+x1+cx] = texture[(cx%2)*9+cy];
        }
    }
    return R_SUCCESS;
}
