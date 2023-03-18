// Include file for textinput.c
// Created by Christopher Mitchell / Kerm Martian
// [http://www.cemetech.net]
#include <keyboard.hpp>
#include <fxcg/display.h>
#define bool unsigned char

#define INPUT_MODE_TEXT 0
#define INPUT_MODE_FLOAT 1
#define INPUT_MODE_INT 2
#define INPUT_MODE_POSINT 3

#define CURSOR_FLASH_RATE 32

#define true 1
#define false 0

int getTextLine(char* buf, int maxstrlen, int x, int y, int maxdisplen, unsigned short inmode);
void DrawCursor(int x, int y, int shiftmode, int alphamode, int cursorstate, char curchar);