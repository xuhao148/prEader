#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/file.h>
#include <string.h>
#include <preader/filedialog.h>
#include <extra_calls.h>
#include <preader/ui.h>
#include <preader/i18n.h>
#include <preader/reader.h>

SessionConfig cfg;
int modified_cfg;


void main(void) {
    Bdisp_EnableColor(1);
    int key;

    /* Config reading & processing*/
    const char *szConfigfilePath = "\\\\fls0\\prconf.cfg";
    short wszConfigfilePath[64] = {0};
    Bfile_StrToName_ncpy(wszConfigfilePath,szConfigfilePath,32);
    int fhConfigHandle = Bfile_OpenFile_OS(wszConfigfilePath,READ,0);
    if (fhConfigHandle < 0) {
        int cfret = Bfile_CreateEntry_OS(wszConfigfilePath,CREATEMODE_FILE,sizeof(SessionConfig));
        if (cfret < 0) {
            fatal_error("�޷���ȡ�򴴽������ļ�prconf.cfg��\n��������ļ�ϵͳ��������Add-in��",72,1);
        }
        fhConfigHandle = Bfile_OpenFile_OS(wszConfigfilePath,WRITE,0);
        if (fhConfigHandle < 0) {
            fatal_error("���ܴ�����prconf.cfg�����ڴ������޷��򿪲�д�롣\n��������ļ�ϵͳ��������Add-in��",72,1);
        }
        cfg.magic[0] = 'P';
        cfg.magic[1] = 'R';
        cfg.magic[2] = 'E';
        cfg.magic[3] = 'A';
        cfg.magic[4] = 'D';
        cfg.magic[5] = 'E';
        cfg.magic[6] = 'R';
        cfg.magic[7] = '0';
        Bfile_WriteFile_OS(fhConfigHandle,&cfg,sizeof(cfg));
        Bfile_CloseFile_OS(fhConfigHandle);
        infobox("�Ѵ���prconfig.cfg��\n[EXE] ����",60,1);
    } else {
        int filesize = Bfile_GetFileSize_OS(fhConfigHandle);
        if (filesize != sizeof(SessionConfig)) {
            Bfile_CloseFile_OS(fhConfigHandle);
            fatal_error("prconf.cfg��С����ȷ��\n��ɾ����ǰ��prconf.cfg�����ٴ����д�Add-in���ؽ������ļ���",72,1);
        } else {
            Bfile_ReadFile_OS(fhConfigHandle,&cfg,sizeof(cfg),0);
            Bfile_CloseFile_OS(fhConfigHandle);
            if (!detect_magic(cfg.magic)) {
                fatal_error("prconf.cfg��ʽ����\n��ɾ����ǰ��prconf.cfg�����ٴ����д�Add-in���ؽ������ļ���",72,1);
            }
        }
    }

    /* Draws the main menu. */
    modified_cfg = 0;
    while (1) {
    Bdisp_AllClr_VRAM();
    DisplayStatusArea();
    MenuItem mainmenu[] = {1,"�����ļ�",1,"��������ļ�",1,"����",1,"���ڴ˳���"};
    mainmenu[1].enabled = cfg.has_last_book;
    drawDialog(45,67,325,180);
    int option = flexibleMenu(45,67-24,COLOR_WHITE,0,COLOR_BLACK,COLOR_RED,COLOR_GRAY,COLOR_CYAN,0,325-45,2,4,mainmenu,4,0,1,0);
    char *filepath[64];
    switch (option) {
        case 0:
            browseAndOpenFileI("\\\\fls0\\","*.*",filepath);
            read_book(filepath);
            break;
        case 1:
            read_book(cfg.last_book->book_path);
            break;
        case 2:
        {
            modified_cfg = 1;
            const char *fonts_setting_str[] = {"�л������С����ǰΪ�����壩","�л������С����ǰΪС���壩"};
            const char *slashes_setting_str[] = {"�л���б�ߴ�����ǰΪת�����","�л���б�ߴ�����ǰΪ�����֣�"};
            MenuItem settingsmenu[] = {1,"�л������С",1,"�л���б�ߴ���",1,"������м�¼",1,"�������˵�"};
            int exit_menu = 0;
            int choice = 0;
            while (!exit_menu) {
                settingsmenu[0].label = fonts_setting_str[cfg.font_size==1];
                settingsmenu[1].label = slashes_setting_str[cfg.process_backslashes==1];
                Bdisp_AllClr_VRAM();
                DisplayStatusArea();
                drawDialog(5,29,376,199);
                choice = flexibleMenu(5,29-24,COLOR_WHITE,0,COLOR_BLACK,COLOR_RED,COLOR_GRAY,COLOR_CYAN,0,376-5+1,2,4,settingsmenu,4,choice,1,0);
                int chs;
                switch (choice) {
                    case 0:
                        chs = msgbox("�⽫���������ǩ��\nȷ����\n[EXE] ȷ��  [EXIT] ȡ��","����",89,1,COLOR_GOLD);
                        if (chs == KEY_CTRL_EXE) {
                            cfg.font_size = 1 - cfg.font_size;
                            for (int i=0; i<32; i++) {
                                cfg.book_records[i].last_location = 0;
                                for (int j=0; j<8; j++) {
                                    cfg.book_records[i].bookmarks[j].byte_location = -1;
                                }
                            }
                            infobox("�������л���",54,1);
                        }
                        break;
                    case 1:
                        cfg.process_backslashes = 1 - cfg.process_backslashes;break;
                    case 2:
                        chs = msgbox("�����Ķ���¼������������޷��ָ���\nȷ����\n[EXE] ȷ��  [EXIT] ȡ��","����",100,1,COLOR_RED);
                        if (chs == KEY_CTRL_EXE) {
                            cfg.font_size = 0;
                            cfg.process_backslashes = 0;
                            cfg.has_last_book = 0;
                            cfg.n_book_records = 0;
                            memset(&cfg.last_book,0,sizeof(BookRecord));
                            for (int i=0; i<32; i++) {
                                cfg.book_records[i].book_path[0] = 0;
                                for (int j=0; j<8; j++)
                                    {cfg.book_records[i].bookmarks[j].byte_location = -1;
                                    cfg.book_records[i].bookmarks[j].preview[0] = 0;}
                                cfg.book_records[i].last_location = 0;
                            }
                            infobox("�������¼��",54,1);
                        }
                        break;
                    case 3:
                    exit_menu = 1;break;
                    case -1:
                    choice = 0; exit_menu = 1; break;
                    default:
                    info_wip();break;
                }
            }
        }
        break;
        case 3:
            infobox("Preader Alpha 0.1.0\nBy Ayachu\n�����ʹ��!����������",120,1);
            break;
    }
    if (modified_cfg) {
    fhConfigHandle = Bfile_OpenFile_OS(wszConfigfilePath,WRITE,0);
    if (fhConfigHandle < 0) {
        fatal_error("�޷�д�������ļ�prconf.cfg��\n�����ļ�ϵͳ��������һ�Ρ�",72,1);
    } else {
        Bfile_SeekFile_OS(fhConfigHandle,0);
        Bfile_WriteFile_OS(fhConfigHandle,&cfg,sizeof(cfg));
        Bfile_CloseFile_OS(fhConfigHandle);
    }
    }
    }
    fatal_error("�������е���һ�����õ���ĵص㡣\n����������л��������뿪������ϵ��",72,1);
}
int detect_magic(char *magic) {
    if (magic[0] == 'P' && magic[1] == 'R' && magic[2] == 'E' &&
        magic[3] == 'A' && magic[4] == 'D' && magic[5] == 'E' &&
        magic[6] == 'R' && magic[7] == '0') return 1;
    return 0;
}

