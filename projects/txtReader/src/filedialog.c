#include <fxcg/file.h>
#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <extra_calls.h>
#include <string.h>
#include <preader/i18n.h>
#include <preader/ui.h>
#include "prdefinitions.h"


typedef unsigned short TStandardScrollbar[0x12];
int fileMenuSelect(file_info_t files[], int max_n, char *pathname, char *filter) {
    static int langid = -1;
    int key,menuindex = 0,listindex=0, drawableitems;
    TScrollbar scb = {0,0,0,0,0,378,24,144,6};
    scb.indicatormaximum = max_n;
    scb.indicatorheight = 1;
    static int fkeyImgOpen;
    if (!fkeyImgOpen) GetFKeyPtr(945,&fkeyImgOpen);
    char buf[32];
    Bdisp_AllClr_VRAM();
    drawableitems = max_n;
    if (max_n)
        sprintf(buf,"打开%s [%d/%d]",filter,listindex+menuindex+1,max_n);
    else
        sprintf(buf,"打开%s [--/--]",filter);
    ProcessPrintChars(936);
    PrintCXY(0,0,buf,0,-1,COLOR_BLUE,COLOR_WHITE,1,0);
    ProcessPrintChars(0);
    drawFileList(&files[listindex],menuindex,drawableitems);
    if (max_n == 0) {
        ProcessPrintChars(936);
        PrintCXY(LCD_WIDTH_PX/2-60,LCD_HEIGHT_PX/2-24,"（无文件）",0,-1,COLOR_BLACK,COLOR_WHITE,1,0);
        ProcessPrintChars(0);
    }
    DefineStatusAreaFlags(1,0,0,0);
    char escbuf[36];
    if (langid == -1) langid = getSystemLanguage();
    switch (langid) {
        case 0:
        DefineStatusMessage(pathname,1,TEXT_COLOR_BLACK,0);break;
        case 5:
        strcpy(escbuf,pathname);
        duplicateBackSlashes(escbuf);
        DefineStatusMessage(escbuf,1,TEXT_COLOR_BLACK,0);break;
        default:
        sprintf(escbuf,"<UNS.LANG %d>",langid);
        DefineStatusMessage(escbuf,1,TEXT_COLOR_BLACK,0);break;
    }
    DisplayStatusArea();
    FKey_Display(0,(void *)fkeyImgOpen);
    scb.indicatorpos = listindex+menuindex;
    Scrollbar(&scb);
    while (1) {
        GetKey(&key);
        if (key == KEY_CTRL_DOWN) {
            if (max_n >= 6) {
            if (menuindex == 5) {
                listindex++;
                if (listindex + menuindex >= max_n) {
                    listindex = 0;
                    menuindex = 0;
                    drawableitems = max_n;
                } else {
                    drawableitems = max_n-listindex;
                }
            } else {
                menuindex++;
            } } else {
                menuindex++;
                if (menuindex >= max_n) menuindex = 0;
            }
        }
        if (key == KEY_CTRL_UP) {
            if (max_n >= 6) {
            if (menuindex == 0) {
                listindex--;
                if (listindex < 0) {
                    listindex = max_n - 6;
                    menuindex = 5;
                    drawableitems = 6;
                } else {
                    drawableitems = max_n-listindex;
                }
            } else {
                menuindex--;
            }
            } else {
                menuindex--; if (menuindex < 0) {menuindex = max_n-1; if (menuindex < 0) menuindex = 0;}
            }
        }
        if (key == KEY_CTRL_EXE || key == KEY_CTRL_F1) {
            if (max_n)
            {   
                DefineStatusMessage("",0,TEXT_COLOR_BLACK,0);
                return listindex+menuindex;
            }
        }
        if (key == KEY_CTRL_EXIT || key == KEY_CTRL_AC) {
            DefineStatusMessage("",0,TEXT_COLOR_BLACK,0);
            return MENU_DISCARDED;
        }
        Bdisp_AllClr_VRAM();
        ProcessPrintChars(936);
        if (max_n)
            sprintf(buf,"打开%s [%d/%d]",filter,listindex+menuindex+1,max_n);
        else
            sprintf(buf,"打开%s [--/--]",filter);
        PrintCXY(0,0,buf,0,-1,COLOR_BLUE,COLOR_WHITE,1,0);
        ProcessPrintChars(0);
        DisplayStatusArea();
        FKey_Display(0,(void *)fkeyImgOpen);
        drawFileList(files+listindex,menuindex,drawableitems);
        if (max_n == 0) {
            ProcessPrintChars(936);
            PrintCXY(LCD_WIDTH_PX/2-60,LCD_HEIGHT_PX/2-24,"（无文件）",0,-1,COLOR_BLACK,COLOR_WHITE,1,0);
            ProcessPrintChars(0);
        }
        scb.indicatorpos = listindex+menuindex;
        Scrollbar(&scb);
    }
}
void drawFileList(file_info_t files[], int chosen, int n) {
    int i, len,len2;
    char buf[23];
    char lengthbuf[16];
    for (i=0; i<n && i<6; i++) {
        int color, bcolor;
        color = COLOR_BLACK;
        if (i == chosen) {
            bcolor = COLOR_CYAN;
        } else {
            bcolor = COLOR_WHITE;
        }
        int siz = files[i].fProperties.fsize * 10 / 1024;
        if (files[i].fProperties.type)
            if (!strcmp(files[i].pathname,".") || !strcmp(files[i].pathname,".."))
                sprintf(lengthbuf,"<DIR>");
            else sprintf(lengthbuf,"%d.%dK",siz/10,siz%10);
        else
            sprintf(lengthbuf,"<DIR>");
        

        len2 = strlen(lengthbuf);
        int len1 = strlen(files[i].pathname);
        if (len2 + len1 <= 21) {
            sprintf(buf,"%s",files[i].pathname);
        } else {
            int avail = 21-len2-2;
            if (avail >= 0) {
            strncpy(buf,files[i].pathname,avail);
            buf[avail] = '$';
            buf[avail+1] = '\0';
            }
        }
        len = strlen(buf);
        while (len+len2 < 21) {
            buf[len] = ' ';
            len++;
        }
        buf[len] = '\0';
        strcat(buf,lengthbuf);
        PrintCXY(0,24+i*24,buf,0,-1,color,bcolor,1,0);
    }
}

int openFileDialogDisclosure(const char *pathname, const char *filter, char *filename)
{ 
    /* Takes over the screen with a menu where you can choose a file / directory. */
    /* Returns the type of the file, and writes the filename to *filename. */
    char wcharpath[128];
    char buf[64];
    char pathnameHolder[64];
    strcpy(pathnameHolder,pathname);
    strcat(pathnameHolder,filter);

    short wcharbuf[128];
    int find_handle;
    file_type_t ft;
    file_info_t fi[128];
    int i = 0,ret;
    Bfile_StrToName_ncpy(wcharpath,pathnameHolder,48);
    ret = Bfile_FindFirst(wcharpath,&find_handle,wcharbuf,&fi[i].fProperties);
    while (ret != -16 && i < 128) {
        Bfile_NameToStr_ncpy(fi[i].pathname,wcharbuf,48);
        i++;
        ret = Bfile_FindNext(find_handle,wcharbuf,&fi[i].fProperties);
    }
    Bfile_FindClose(find_handle);
    int chosen = fileMenuSelect(fi,i,pathname,filter);
    if (chosen == MENU_DISCARDED) return R_FB_PRESSED_EXIT;
    strcpy(filename,fi[chosen].pathname);
    return fi[chosen].fProperties.type;
}

void path_concat(char *dest, char *src)
{
    if (dest[strlen(dest)-1] == '\\') {
        if (src[0] == '\\') {
            strcat(dest,src+1);
        } else {
            strcat(dest,src);
        }
    } else {
        if (src[0] == '\\') {
            strcat(dest,src);
        } else {
            int len = strlen(dest);
            dest[len] = '\\';
            dest[len+1] = '\0';
            strcat(dest,src);
        }
    }
}

void path_deletelast(char *dest) {
    int last_backslash = strlen(dest);
    if (dest[last_backslash-1] == '\\') last_backslash-=2;
    while (dest[last_backslash] != '\\' && last_backslash) {
        last_backslash--;
    }
    if (last_backslash) {
        dest[last_backslash+1] = '\0';
    }
}

int browseAndOpenFileI(const char *path, const char *filter, char *filename_chosen) {
    char filename[64];
    char path_holder[128];
    strcpy(path_holder,path);
    int filetype = openFileDialogDisclosure(path_holder,filter,filename);
    if (filetype == R_FB_PRESSED_EXIT) return R_FB_PRESSED_EXIT;
    while (1) {
        if (!strcmp(filename,".")) {
            filetype = openFileDialogDisclosure(path_holder,filter,filename);
            if (filetype == R_FB_PRESSED_EXIT) return R_FB_PRESSED_EXIT;
        } else if (!strcmp(filename,"..")) {
            path_deletelast(path_holder);
            filetype = openFileDialogDisclosure(path_holder,filter,filename);
            if (filetype == R_FB_PRESSED_EXIT) return R_FB_PRESSED_EXIT;
        } else if (filetype == 0) {
            path_concat(path_holder,filename);
            int len = strlen(path_holder);
            path_holder[len] = '\\';
            path_holder[len+1] = '\0';
            filetype = openFileDialogDisclosure(path_holder,filter,filename);
            if (filetype == R_FB_PRESSED_EXIT) return R_FB_PRESSED_EXIT;
        } else {
            path_concat(path_holder,filename);
            break;
        }
    }
    strcpy(filename_chosen,path_holder);
    return filetype;
}

void duplicateBackSlashes(char *str) {
    char buf[513] = {0};
    int a=0,  b=0;
    while (str[a]) {
        if (str[a] == '\\')
        {
            buf[b] = '\\';
            b++;
            buf[b] = '\\';
        } else {
            buf[b] = str[a];
        }
        b++;
        a++;
    }
    buf[b] = 0;
    strcpy(str,buf);
    return;
}