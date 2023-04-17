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
#include "colordialog.h"


SessionConfig cfg = {0};
int modified_cfg = 0;

void SaveAndOpenMainMenu(void);
extern void slot_manager(void);

void main(void) {
    Bdisp_EnableColor(1);
    cfg.color_scheme[CI_MENU_BG] = COLOR_MENU_BG;
    cfg.color_scheme[CI_MENU_BG_CHOSEN] = COLOR_MENU_BG_CHOSEN;
    cfg.color_scheme[CI_MENU_FG] = COLOR_MENU_FG;
    cfg.color_scheme[CI_MENU_FG_CHOSEN] = COLOR_MENU_FG_CHOSEN;
    cfg.color_scheme[CI_MENU_FG_UNAVAIL] = COLOR_MENU_FG_UNAVAIL;
    cfg.color_scheme[CI_DIALOG_BG] = COLOR_DIALOG_BG;
    cfg.color_scheme[CI_DIALOG_BORDER] = COLOR_DIALOG_BORDER;
    cfg.color_scheme[CI_DIALOG_SHADOW] = COLOR_DIALOG_SHADOW;
    cfg.color_scheme[CI_READER_BG] = COLOR_READER_BG;
    cfg.color_scheme[CI_READER_FG] = COLOR_READER_FG;
    int key;
    const char *szDataFileFolderPath = "\\\\fls0\\@PRDR";
    {
    short wszDataFileFolderPath[64];
    Bfile_StrToName_ncpy(wszDataFileFolderPath,szDataFileFolderPath,64);
    unsigned short buf[32];
    file_type_t phinfo;
    int findhandle;
    int r = Bfile_FindFirst(wszDataFileFolderPath,&findhandle,buf,&phinfo);
    Bfile_FindClose(findhandle);
    if (r < 0) {
      int cfret = Bfile_CreateEntry_OS(wszDataFileFolderPath,5,NULL);
      if (cfret != 0) {
        char buf[64];
        sprintf(buf,"无法创建文件夹@PRDR。(errno=%d)\n检查文件系统，并再试一次。",cfret);
        fatal_error(buf,80,1);
      }
    } else {
      if (phinfo.type != 0)
        fatal_error("系统中存在“@PRDR”文件。\n请删除此文件，然后再试一次。",60,1);
    }
    }
    /* Config reading & processing*/
    const char *szConfigfilePath = "\\\\fls0\\@PRDR\\prconf.cfg";
    short wszConfigfilePath[64] = {0};
    Bfile_StrToName_ncpy(wszConfigfilePath,szConfigfilePath,32);
    int fhConfigHandle = Bfile_OpenFile_OS(wszConfigfilePath,READ,0);
    if (fhConfigHandle < 0) {
        int scsize = sizeof(SessionConfig);
        int cfret = Bfile_CreateEntry_OS(wszConfigfilePath,CREATEMODE_FILE,&scsize);
        if (cfret < 0) {
            fatal_error("无法读取或创建配置文件prconf.cfg。\n请检查你的文件系统，并重启Add-in。",90,1);
        }
        fhConfigHandle = Bfile_OpenFile_OS(wszConfigfilePath,WRITE,0);
        if (fhConfigHandle < 0) {
            fatal_error("尽管创建了prconf.cfg，但在创建后无法打开并写入。\n请检查你的文件系统，并重启Add-in。",72,1);
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
        infobox("已创建prconfig.cfg。\n[EXE] 继续",60,1);
    } else {
        int filesize = Bfile_GetFileSize_OS(fhConfigHandle);
        if (filesize != sizeof(SessionConfig)) {
            Bfile_CloseFile_OS(fhConfigHandle);
            fatal_error("prconf.cfg大小不正确。\n请删除当前的prconf.cfg，并再次运行此Add-in来重建配置文件。",72,1);
        } else {
            Bfile_ReadFile_OS(fhConfigHandle,&cfg,sizeof(cfg),0);
            Bfile_CloseFile_OS(fhConfigHandle);
            if (!detect_magic(cfg.magic)) {
                fatal_error("prconf.cfg格式错误。\n请删除当前的prconf.cfg，并再次运行此Add-in来重建配置文件。",72,1);
            }
        }
    }

    /* Draws the main menu. */
    DefineStatusAreaFlags(1,0,0,0);
    while (1) {
    Bdisp_AllClr_VRAM();
    DefineStatusMessage("Preader v0.1.3 Alpha",1,TEXT_COLOR_BLACK,0);
    EnableStatusArea(0);
    DisplayStatusArea();
    if (modified_cfg) {
      printMiniSingleLineCutOffUnprintables(0,LCD_HEIGHT_PX-40,"设置已更新！",LCD_WIDTH_PX,COLOR_BLUE,COLOR_WHITE,0);
    }
    modified_cfg = 0;
    MenuItem mainmenu[N_M_MAIN];
    register_menuitem_normal(&mainmenu[M_MAIN_BROWSE],1,"打开新文件");
    register_menuitem_normal(&mainmenu[M_MAIN_OPEN_RECENT],1,"打开最近的文件");
    register_menuitem_normal(&mainmenu[M_MAIN_SETTINGS],1,"设置");
    register_menuitem_normal(&mainmenu[M_MAIN_ABOUT],1,"关于此程序");
    register_menuitem_normal(&mainmenu[M_MAIN_MANAGE_SLOTS],1,"管理存储槽位");
    mainmenu[M_MAIN_OPEN_RECENT].enabled = cfg.has_last_book;
    drawDialog(45,47,325,184);
    rect(45,47,325,184,cfg.color_scheme[CI_MENU_BG]);
    int option = flexibleMenu(45,47-24,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,325-45,2,5,mainmenu,5,0,1,0);
    char *filepath[64];
    int reader_ret = 0;
    switch (option) {
        case M_MAIN_BROWSE:
            {
            int ft_ret = browseAndOpenFileI("\\\\fls0\\","*.*",filepath);
            if (ft_ret != R_FB_PRESSED_EXIT)
              reader_ret = read_book(filepath);
            }
            break;
        case M_MAIN_OPEN_RECENT:
            reader_ret = read_book(cfg.last_book->book_path);
            break;
        case M_MAIN_SETTINGS:
       {
        complexMenuItem settingsItems[N_M_CONF];
        register_menuitem_complex(&settingsItems[M_CONF_LABEL_FONT_SIZE],0,0,0,"字体大小",0);
        register_menuitem_complex(&settingsItems[M_CONF_CHECK_FONT_LARGE],1,2,0,"- 大字体",0);
        register_menuitem_complex(&settingsItems[M_CONF_CHECK_FONT_SMALL],1,2,0,"- 小字体",0);
        register_menuitem_complex(&settingsItems[M_CONF_CHECK_BACKSLASH],1,1,0,"反斜线按原字符输出",0);
        register_menuitem_complex(&settingsItems[M_CONF_CLEAR_SESSION],1,0,0,"清除所有记录",0);
        register_menuitem_complex(&settingsItems[M_CONF_APPLY_RETURN],1,0,0,"应用并返回主菜单",0);
        register_menuitem_complex(&settingsItems[M_CONF_DISMISS_RETURN],1,0,0,"取消并返回主菜单",0);
        register_menuitem_complex(&settingsItems[M_CONF_BGPICT_SETTINGS],1,3,0,"背景图片设置",0);
        register_menuitem_complex(&settingsItems[M_CONF_HIDE_HUD],1,1,0,"隐藏状态栏与Fn键栏",0);
        register_menuitem_complex(&settingsItems[M_CONF_DRAW_PROGRESSBAR],1,1,0,"在底部绘制进度条",0);
        register_menuitem_complex(&settingsItems[M_CONF_USE_STRICT_RNDR],1,1,0,"使用严格的渲染机制",0);
        register_menuitem_complex(&settingsItems[M_CONF_COLOR_SCHEME],1,3,0,"配色方案设置",0);
        settingsItems[M_CONF_CHECK_FONT_LARGE].value = !cfg.font_size;
        settingsItems[M_CONF_CHECK_FONT_SMALL].value = cfg.font_size;
        settingsItems[M_CONF_CHECK_BACKSLASH].value = cfg.process_backslashes;
        settingsItems[M_CONF_HIDE_HUD].value = cfg.hide_ui;
        settingsItems[M_CONF_DRAW_PROGRESSBAR].value = cfg.draw_progressbar;
        settingsItems[M_CONF_USE_STRICT_RNDR].value = ((cfg.extra_settings&BS_USE_STRICT_RENDERER)!=0);
        int choice;
        char tmp_pictpath[32];
        int tmp_set_pictpath = 0;
        int tmp_pictpath_settings_changed = 0;
        strcpy(tmp_pictpath,cfg.bgpict_path); tmp_set_pictpath = cfg.use_bgpict;
        color_t new_color_scheme[10];
        for (int i=0; i<10; i++) {
          new_color_scheme[i] = cfg.color_scheme[i];
        }
        do {
        Bdisp_AllClr_VRAM();
        DisplayStatusArea();
        drawDialog(5,29,376,193);
        rect(5,29,376,193,cfg.color_scheme[CI_MENU_BG]);
        choice = flexibleMenu_complex(5,29-24,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,376-5+1-6,2,N_M_CONF,settingsItems,6,1,1,1,NULL,NULL);
        if (choice == M_CONF_CLEAR_SESSION) {
          int chs = msgbox("所有阅读记录将永久清除、无法恢复！\n确定？\n[EXE] 确定  [EXIT] 取消","警告",100,1,COLOR_RED);
          if (chs == KEY_CTRL_EXE) {
                            modified_cfg = 1;
                            cfg.font_size = 0;
                            cfg.process_backslashes = 0;
                            cfg.has_last_book = 0;
                            cfg.n_book_records = 0;
                            cfg.hide_ui = 0;
                            cfg.draw_progressbar = 0;
                            cfg.extra_settings = 0;
                            memset(&cfg.last_book,0,sizeof(BookRecord));
                            for (int i=0; i<32; i++) {
                                cfg.book_records[i].book_path[0] = 0;
                                for (int j=0; j<8; j++)
                                    {cfg.book_records[i].bookmarks[j].byte_location = -1;
                                    cfg.book_records[i].bookmarks[j].page_location = -1;
                                    cfg.book_records[i].bookmarks[j].preview[0] = 0;}
                                cfg.book_records[i].last_location = 0;
                                cfg.book_records[i].last_byte_location = 0;
                            }
                            cfg.color_scheme[CI_MENU_BG] = COLOR_MENU_BG;
                            cfg.color_scheme[CI_MENU_BG_CHOSEN] = COLOR_MENU_BG_CHOSEN;
                            cfg.color_scheme[CI_MENU_FG] = COLOR_MENU_FG;
                            cfg.color_scheme[CI_MENU_FG_CHOSEN] = COLOR_MENU_FG_CHOSEN;
                            cfg.color_scheme[CI_MENU_FG_UNAVAIL] = COLOR_MENU_FG_UNAVAIL;
                            cfg.color_scheme[CI_DIALOG_BG] = COLOR_DIALOG_BG;
                            cfg.color_scheme[CI_DIALOG_BORDER] = COLOR_DIALOG_BORDER;
                            cfg.color_scheme[CI_DIALOG_SHADOW] = COLOR_DIALOG_SHADOW;
                            cfg.color_scheme[CI_READER_BG] = COLOR_READER_BG;
                            cfg.color_scheme[CI_READER_FG] = COLOR_READER_FG;
                            infobox("已清除记录。",54,1);
                        }
                        break;
        } else if (choice == M_CONF_APPLY_RETURN) {
          modified_cfg = 1;
          cfg.font_size = settingsItems[M_CONF_CHECK_FONT_SMALL].value;
          cfg.process_backslashes = settingsItems[M_CONF_CHECK_BACKSLASH].value;
          cfg.hide_ui = settingsItems[M_CONF_HIDE_HUD].value;
          cfg.draw_progressbar = settingsItems[M_CONF_DRAW_PROGRESSBAR].value;
          int do_use_strict_rndr = cfg.extra_settings&BS_USE_STRICT_RENDERER;
          for (int i=0; i<10; i++)
            cfg.color_scheme[i] = new_color_scheme[i];
          if (settingsItems[M_CONF_USE_STRICT_RNDR].value) {
            if (!do_use_strict_rndr)
              infobox("使用严格的渲染机制将使得不能渲染的控制字符、制表符等被渲染成“?”或“ ”。\n这在一定程度上能够解决某些分页、渲染时的错误，但会大幅增大分页、渲染时间。\n启用后，请立即对您的书重新分页。",180,1);
            cfg.extra_settings |= BS_USE_STRICT_RENDERER;
          } else {
            unsigned int bitmask = BS_USE_STRICT_RENDERER;
            bitmask = ~bitmask;
            cfg.extra_settings &= bitmask;
          }
          if (tmp_pictpath_settings_changed) {
          strcpy(cfg.bgpict_path,tmp_pictpath);
          cfg.use_bgpict = tmp_set_pictpath;
          }
        } else if (choice == M_CONF_BGPICT_SETTINGS) {
          complexMenuItem bgSettingsItems[N_M_BG];
          register_menuitem_complex(&bgSettingsItems[M_BG_CHECK_ENABLED],1,1,0,"启用",0);
          register_menuitem_complex(&bgSettingsItems[M_BG_CHOOSE_PICTURE],1,3,0,"选择图片",0);
          register_menuitem_complex(&bgSettingsItems[M_BG_APPLY],1,0,0,"确定",0);
          register_menuitem_complex(&bgSettingsItems[M_BG_DISMISS],1,0,0,"取消",0);
          register_menuitem_complex(&bgSettingsItems[M_BG_LABEL_CURRENT_PICT],0,4,0,NULL,0);
          int choice;
          char new_tmp_pictpath[32];
          int new_tmp_set_pictpath;
          strcpy(new_tmp_pictpath,tmp_pictpath);
          new_tmp_set_pictpath = tmp_set_pictpath;
          do {
          Bdisp_AllClr_VRAM();
          DisplayStatusArea();
          drawDialog(5,29,376,193);
          bgSettingsItems[M_BG_CHECK_ENABLED].value = new_tmp_set_pictpath;
          char curr[64];
          sprintf(curr,"当前：%s",new_tmp_pictpath[0]?new_tmp_pictpath:"（无）");
          duplicateBackSlashes(curr);
          bgSettingsItems[M_BG_LABEL_CURRENT_PICT].label = curr;
          rect(5,29,376,193,cfg.color_scheme[CI_MENU_BG]);
          choice = flexibleMenu_complex(5,29-24,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,376-5+1-6,2,5,bgSettingsItems,5,1,1,1,NULL,NULL);
          int fb_ret;
          if (choice == M_BG_CHOOSE_PICTURE) {
            fb_ret = browseAndOpenFileI("\\\\fls0\\","*.*",new_tmp_pictpath);
          }
          if (fb_ret != R_FB_PRESSED_EXIT) new_tmp_set_pictpath = bgSettingsItems[M_BG_CHECK_ENABLED].value;
          } while (choice != MENU_DISCARDED && choice != M_BG_APPLY && choice != M_BG_DISMISS);
          if (choice == M_BG_APPLY) {tmp_pictpath_settings_changed = 1; strcpy(tmp_pictpath,new_tmp_pictpath); tmp_set_pictpath = new_tmp_set_pictpath;}
        } else if (choice == M_CONF_COLOR_SCHEME) {
          complexMenuItem colorSchemeChooser[18];
          register_menuitem_complex(&colorSchemeChooser[0],0,0,0,"文章配色",0);
          register_menuitem_complex(&colorSchemeChooser[1],1,6,0,"文本颜色",0);
          register_menuitem_complex(&colorSchemeChooser[2],1,6,0,"背景颜色",0);
          register_menuitem_complex(&colorSchemeChooser[3],1,3,0,"使用预设配色",0);
          register_menuitem_complex(&colorSchemeChooser[4],0,4,0,"注：背景图片存在时，背景颜色不生效。",0);
          register_menuitem_complex(&colorSchemeChooser[5],0,0,0,"界面配色",0);
          register_menuitem_complex(&colorSchemeChooser[6],1,6,0,"菜单背景色",0);
          register_menuitem_complex(&colorSchemeChooser[7],1,6,0,"菜单高亮项背景色",0);
          register_menuitem_complex(&colorSchemeChooser[8],1,6,0,"文本颜色",0);
          register_menuitem_complex(&colorSchemeChooser[9],1,6,0,"高亮文本颜色",0);
          register_menuitem_complex(&colorSchemeChooser[10],1,6,0,"不可用文本颜色",0);
          register_menuitem_complex(&colorSchemeChooser[11],1,6,0,"对话框背景色",0);
          register_menuitem_complex(&colorSchemeChooser[12],1,6,0,"对话框边框色",0);
          register_menuitem_complex(&colorSchemeChooser[13],1,6,0,"对话框阴影色",0);
          register_menuitem_complex(&colorSchemeChooser[14],1,3,0,"使用预设界面主题",0);
          register_menuitem_complex(&colorSchemeChooser[15],1,0,0,"恢复出厂配色",0);
          register_menuitem_complex(&colorSchemeChooser[16],1,0,0,"确定更改",0);
          register_menuitem_complex(&colorSchemeChooser[17],1,0,0,"取消更改",0);
          int finished = 0;
          for (int i=0; i<8; i++) {
            colorSchemeChooser[i+6].value = new_color_scheme[i];
          }
          colorSchemeChooser[1].value = new_color_scheme[CI_READER_FG];
          colorSchemeChooser[2].value = new_color_scheme[CI_READER_BG];
          int cs_last_choice = 1;
          while (!finished) {
          drawDialog(5,29,376,193);
          rect(5,29,376,193,cfg.color_scheme[CI_MENU_BG]);
          int cs_choice = flexibleMenu_complex(5,29-24,cfg.color_scheme[CI_MENU_BG],0,cfg.color_scheme[CI_MENU_FG],cfg.color_scheme[CI_MENU_FG_CHOSEN],cfg.color_scheme[CI_MENU_FG_UNAVAIL],cfg.color_scheme[CI_MENU_BG_CHOSEN],0,376-5+1-6,2,18,colorSchemeChooser,6,cs_last_choice,1,1,NULL,NULL);
          switch (cs_choice) {
            case 1:case 2:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:
            colorSchemeChooser[cs_choice].value = pick_a_color(colorSchemeChooser[cs_choice].value);
            cs_last_choice = cs_choice;
            break;
            case 3:
            {
            color_t tmpbuf[2];
            int pcret = preset_chooser(palettes_reader,N_PALETTES_READER,tmpbuf);
            if (pcret == 0) {colorSchemeChooser[2].value = tmpbuf[0]; 
                             colorSchemeChooser[1].value = tmpbuf[1];}
            }
            cs_last_choice = cs_choice;
            break;
            case 14:
            {
            color_t tmpbuf[10];
            int pcret = preset_chooser(palettes_ui,N_PALETTES_UI,tmpbuf);
            if (pcret == 0) {
              for (int i=0; i<10; i++)
                colorSchemeChooser[i+6].value = tmpbuf[i];
            }
            }
            cs_last_choice = cs_choice;
            break;
            case 15:
            colorSchemeChooser[1].value = COLOR_READER_FG;
            colorSchemeChooser[2].value = COLOR_READER_BG;
            colorSchemeChooser[6].value = COLOR_MENU_BG;
            colorSchemeChooser[7].value = COLOR_MENU_BG_CHOSEN;
            colorSchemeChooser[8].value = COLOR_MENU_FG;
            colorSchemeChooser[9].value = COLOR_MENU_FG_CHOSEN;
            colorSchemeChooser[10].value = COLOR_MENU_FG_UNAVAIL;
            colorSchemeChooser[11].value = COLOR_DIALOG_BG;
            colorSchemeChooser[12].value = COLOR_DIALOG_BORDER;
            colorSchemeChooser[13].value = COLOR_DIALOG_SHADOW;
            cs_last_choice = 0;
            break;
            case 16:
            new_color_scheme[CI_READER_FG] = colorSchemeChooser[1].value;
            new_color_scheme[CI_READER_BG] = colorSchemeChooser[2].value;
            for (int i=0; i<8; i++) {
              new_color_scheme[i] = colorSchemeChooser[i+6].value;
            }
            finished = 1;
            break;
            case 17:case -1:
            finished = 1;
            break;
          }
          } 
        }
        } while (choice != MENU_DISCARDED && choice != M_CONF_APPLY_RETURN && choice != M_CONF_DISMISS_RETURN);
       }
        break;
        case M_MAIN_ABOUT:
            infobox("Preader Alpha 0.1.3\nBy Ayachu\n请谨慎使用!（认真脸）",120,1);
            break;
        case M_MAIN_MANAGE_SLOTS:
            slot_manager();
            break;
        case MENU_DISCARDED:
            SaveAndOpenMainMenu();
            break;
    }
    if (modified_cfg) {
    fhConfigHandle = Bfile_OpenFile_OS(wszConfigfilePath,WRITE,0);
    if (fhConfigHandle < 0) {
        fatal_error("无法写入配置文件prconf.cfg。\n请检查文件系统，并再试一次。",72,1);
    } else {
        Bfile_SeekFile_OS(fhConfigHandle,0);
        Bfile_WriteFile_OS(fhConfigHandle,&cfg,sizeof(cfg));
        Bfile_CloseFile_OS(fhConfigHandle);
    }
    
    }
    if (reader_ret == R_READER_STRAIGHT_EXIT) {
        SaveAndOpenMainMenu();
        reader_ret = 0;
    }
    }
    fatal_error("程序运行到了一个不该到达的地点。\n请检查你的运行环境，并与开发者联系。",72,1);
}
int detect_magic(char *magic) {
    if (magic[0] == 'P' && magic[1] == 'R' && magic[2] == 'E' &&
        magic[3] == 'A' && magic[4] == 'D' && magic[5] == 'E' &&
        magic[6] == 'R' && magic[7] == '0') return 1;
    return 0;
}

/* Opens the main menu.
    Written by dr-carlos on Cemetech */
void SaveAndOpenMainMenu(void) {
  int addr;

  // get the address of the syscall table in it
  addr = *(unsigned int *)0x8002007C;

  if (addr < (int)0x80020070)
    return;
  if (addr >= (int)0x81000000)
    return;

  // get the pointer to syscall 1E58 - SwitchToMainMenu
  addr += 0x1E58 * 4;
  if (addr < (int)0x80020070)
    return;
  if (addr >= (int)0x81000000)
    return;

  addr = *(unsigned int *)addr;
  if (addr < (int)0x80020070)
    return;
  if (addr >= (int)0x81000000)
    return;

  // Now addr has the address of the first operation in %1e58

  // Run up to 150 times (300/2). OS 3.60's is 59 instructions, so this should
  // be plenty, but will let it stop if nothing is found
  for (unsigned short *currentAddr = (unsigned short *)addr;
       (unsigned int)currentAddr < ((unsigned int)addr + 300); currentAddr++) {
    // MOV.L GetkeyToMainFunctionReturn Flag, r14
    if (*(unsigned char *)currentAddr != 0xDE)
      continue;

    // MOV #3, 2
    if (*(currentAddr + 1) != 0xE203)
      continue;

    // BSR <SaveAndOpenMainMenu>
    if (*(unsigned char *)(currentAddr + 2) != 0xB5)
      continue;

    // MOV.B r2, @r14
    if (*(currentAddr + 3) != 0x2E20)
      continue;

    // BRA <some addr>
    if (*(currentAddr + 4) != 0xAFFB)
      continue;

    // NOP
    if (*(currentAddr + 5) != 0x0009)
      continue;

    unsigned short branchInstruction = *(currentAddr + 2);

    // Clear first 4 bits (BSR identifier)
    branchInstruction <<= 4;
    branchInstruction >>= 4;

    // branchInstruction is now the displacement of BSR

    // Create typedef so we can cast the pointer
    typedef void (*voidFunc)(void);

    // JMP to disp*2 + PC + 4
    ((voidFunc)((unsigned int)branchInstruction * 2 +
                (unsigned int)currentAddr + 4 + 4))();

    return;
  }
}