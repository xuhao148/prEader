/*
    Internationalization Components
    Components used to localize some texts.
*/

#include <fxcg/display.h>

/* Get system language.
   0 - 6 represents English, French, Spanish, Portuguese, German, Chinese and Italian.
   If something goes wrong, or you are using a custom language, it returns -1.
*/
int getSystemLanguage() {
    const static char *langcode[] = {"En","Fr","Es","Po","De","\xd6\xd0","It"};
    char lang[16];
    LocalizeMessage1(1,lang);
    for (int i=0; i<7; i++) {
        if (langcode[i][0] == lang[0] && langcode[i][1] == lang[1]) return i;
    }
    return -1;
}