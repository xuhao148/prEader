// textinput.c for the Casio Prizm and Cemetech's PrizmSDK
// Created by Christopher Mitchell / Kerm Martian
// [http://www.cemetech.net]
#include <preader/textinput.h>
#include <fxcg/keyboard.h>
#include <fxcg/display.h>

#define KEY_PRGM_DEL 44

int getTextLine(char* buf, int maxstrlen, int x, int y, int maxdisplen, unsigned short inmode) {
    //state variables
    int dispoffset = 0;         //the offset of the starting char displayed
    int stroffset = 0;          //the offset of the cursor into the string
    int curlen = 0;             //current string length (<= maxstrlen)
    int cursorstate = 0;            // 0 = char shown, 1 = cursor shown
    int shiftmode = 0, alphamode = 0;
    int cursorchangetime = RTC_GetTicks();  //last cursor change time
    int kcol, krow;
    bool finished = false;      //if routine should end
    unsigned short key = 0;
    char dispcharbuf[21+2+1];   //max possible width is 21, 2 chars for the XX, 1 for null term
    dispcharbuf[0] = dispcharbuf[1] = 'X';
    
    char* row0[18] = { "", "^2", "^", "", "", "",   "", "sqrt(", "xrt(", "", "", "",  "", "r", "Theta", "", "", ""};
    char* row1[18] = { "X", "log(", "ln(", "sin(", "cos(", "tan(",  "angle", "10x", "ex", "asin(", "acos(", "atan(", "A", "B", "C", "D", "E", "F"};
    char* row2[18] = { "", "", "(", ")", ", ", "", "", "", "cuberoot(", "^-1", "", "", "G", "H", "I", "J", "K", "L"};
    char* row3[18] = { "7", "8", "9", "", "", "", "", "", "", "", "", "", "M", "N", "O", "", "", ""};
    char* row4[18] = { "4", "5", "6", "*", "/", "", "", "", "", "{", "}", "", "P", "Q", "R", "S", "T", ""};
    char* row5[18] = { "1", "2", "3", "+", "-", "", "", "", "", "[", "]", "", "U", "V", "W", "X", "Y", ""};
    char* row6[18] = { "0", ".", "E", "-", "", "", "i", "=", "pi", "Ans", "", "", "Z", " ", "\"", "", "", ""};
    char** rows[7] = { row0, row1, row2, row3, row4, row5, row6 };
    //First, figure out starting position
    while(buf[curlen] != '\0') {
        curlen++;
        if (curlen > maxstrlen)
            return -1;
    }
    stroffset = curlen;
    if (curlen >= maxdisplen)
        dispoffset = curlen-maxdisplen+1;
    
    //Main loop
    do {
    
        // display initial buffer contents
        int i=0;    //copy chars into the buffer
        while(i<maxdisplen && buf[dispoffset+i] != '\0') {
            dispcharbuf[2+i] = buf[dispoffset+i];
            i++;
        }
        while(i<maxdisplen) {
            dispcharbuf[2+i++] = ' ';
        }
        dispcharbuf[2+i] = '\0';            //null terminate it
        PrintXY(x,y,dispcharbuf,0,TEXT_COLOR_BLACK);
        cursorstate = 1;
        DrawCursor(x+(stroffset-dispoffset),y,shiftmode,alphamode,cursorstate,dispcharbuf[2+stroffset-dispoffset]);

        bool redraw = false;
        do {
            key = kcol = krow = 0;
            int retval = GetKeyWait_OS(&kcol,&krow,KEYWAIT_HALTON_TIMERON,1,0,&key);
            Bdisp_EnableColor(1);
            key = (10*kcol)+(krow - 1);
            if (retval == 1) {
                
                //erase the cursor and switch it back to the letter here
                cursorstate = 0;
                cursorchangetime = RTC_GetTicks();
                DrawCursor(x+(stroffset-dispoffset),y,shiftmode,alphamode,0,dispcharbuf[2+stroffset-dispoffset]);

                if (krow == 10 || key == KEY_PRGM_UP || key == KEY_PRGM_DOWN || key == KEY_PRGM_RETURN)
                    return key;
                if (key == KEY_PRGM_SHIFT) {
                    shiftmode = 1-shiftmode;
                } else if (key == KEY_PRGM_ALPHA) {
                    if (shiftmode == 1) alphamode = 2;
                    else if (alphamode != 2) alphamode = 1-alphamode;
                    else alphamode = 0;
                    shiftmode = 0;
                } else if (key == KEY_PRGM_LEFT && stroffset != 0) {
                    redraw = true;  //cursor                    
                    if (stroffset == dispoffset) {
                        dispoffset--;
                    }
                    stroffset--;
                } else if (key == KEY_PRGM_RIGHT && stroffset < curlen) {
                    redraw = true;  //cursor                    
                    if (stroffset == dispoffset+maxdisplen-1) {
                        dispoffset++;
                    }
                    stroffset++;
                } else if (key == KEY_PRGM_ACON) {
                    stroffset = dispoffset = 0;
                    buf[0] = '\0';
                    curlen = 0;
                    redraw = true;
                } else if (key == KEY_PRGM_DEL && stroffset < curlen) {
                    int i = stroffset;
                    do {
                        buf[i] = buf[i+1];
                        i++;
                    } while (buf[i] != '\0');
                    curlen--;
                    redraw = true;
                } else if (key == KEY_PRGM_DEL && stroffset == curlen && stroffset > 0) {
                    curlen--;
                    stroffset--;
                    buf[stroffset] = '\0';
                    redraw = true;
                    if (stroffset <= dispoffset && dispoffset > 0)
                        dispoffset--;
                } else {
                    if (krow <= 8 && krow >= 2) {
                        char** rowdef = rows[8-krow];
                        char* thisstr = rowdef[6*(shiftmode)+12*(alphamode!=0 && shiftmode == 0)+(7-kcol)];
                        if (*thisstr != '\0') {
                            int len = strlen(thisstr);
                            signed char valid = (len+curlen < maxstrlen);
                            switch (inmode) {
                                case INPUT_MODE_FLOAT:
                                    if (len > 1) valid = false;
                                    else if (*thisstr == '-') {
                                        if (stroffset != 0 || buf[stroffset] == '-')
                                            valid = false;
                                    } else if (*thisstr == '.') {
                                        for(int i=0; i<curlen; i++) {
                                            if (buf[i] == '.') {
                                                valid = false;
                                                break;
                                            }
                                        }
                                    } else if (*thisstr >= '0' && *thisstr <= '9') {
                                        //this is fine
                                    } else valid = false;
                                    break;
                                case INPUT_MODE_INT:
                                    if (len > 1) valid = false;
                                    else if (*thisstr == '-') {
                                        if (stroffset != 0 || buf[stroffset] == '-')
                                            valid = false;
                                    } else if (*thisstr >= '0' && *thisstr <= '9') {
                                        //this is fine
                                    } else valid = false;
                                    break;
                                case INPUT_MODE_POSINT:
                                    if (len > 1) valid = false;
                                    else if (*thisstr >= '0' && *thisstr <= '9') {
                                        //this is fine
                                    } else valid = false;
                                    break;
                                case INPUT_MODE_TEXT:
                                default:
                                    valid = true;
                                    break;
                            }

                            if (valid) {
                                for(int i=curlen+len;i>=stroffset+len;i--) {    //make space to insert this
                                    buf[i] = buf[i-len];
                                }
                                for(int i=0; i<len; i++) {                      //drop it into the space
                                    buf[stroffset++] = thisstr[i];
                                }
                                curlen+=len;
                                while (stroffset >= dispoffset+maxdisplen-1) {  //adjust the cursor to be on-screen
                                    redraw = true;
                                    dispoffset++;
                                }
                            }
                            redraw = 1;
                            if (shiftmode)
                                shiftmode = 0;
                            else if (alphamode == 1)
                                alphamode = 0;
                        }
                    }
                }
            } else if (cursorchangetime + CURSOR_FLASH_RATE < RTC_GetTicks()) { //and !key
                cursorstate = 1-cursorstate;
                cursorchangetime = RTC_GetTicks();
                DrawCursor(x+(stroffset-dispoffset),y,shiftmode,alphamode,cursorstate,dispcharbuf[2+stroffset-dispoffset]);
            }
        } while (!redraw && !finished);
    
    } while(!finished);
    
    return KEY_PRGM_RETURN; 
}
            
void DrawCursor(int x, int y, int shiftmode, int alphamode, int cursorstate, char curchar) {
    char buf2[5] = {'X','X',' ','\0','\0'};
    buf2[2] = (curchar?curchar:' ');
    if (cursorstate) {
        if (shiftmode == 0 && alphamode == 0) {
            PrintXY(x,y,buf2,1,TEXT_COLOR_BLACK);
            Bdisp_PutDisp_DD();
            return;
        } else if (shiftmode) {
            buf2[2] = 0xE5;
            buf2[3] = 0xEA;
        } else if (alphamode == 1) {
            buf2[2] = 0xE5;
            buf2[3] = 0x9F;
        } else {
            buf2[2] = 0xE7;
            buf2[3] = 0xAE;
        }
    }
    PrintXY(x,y,buf2,0,TEXT_COLOR_BLACK);
    Bdisp_PutDisp_DD();
}