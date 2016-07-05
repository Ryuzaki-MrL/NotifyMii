#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <hbkb.h>

#include "keyboard.h"
#include "notification.h"
#include "utils.h"
#include "ui.h"
/*
void printKeyboard(u8 selected, bool shift) {
    consoleSelect(&bot);
    consoleClear();
    if (shift) {
        printf("\x1b[11;3H!  @  #  $  %%     &  *  (  )  _  +");
        printf("\x1b[13;3HQ  W  E  R  T  Y  U  I  O  P  \"  {");
        printf("\x1b[15;3HA  S  D  F  G  H  J  K  L     ^  }");
        printf("\x1b[17;3HZ  X  C  V  B  N  M  <  >  :  ?  \\n");
    }
    else {
        printf("\x1b[11;3H1  2  3  4  5  6  7  8  9  0  -  =");
        printf("\x1b[13;3Hq  w  e  r  t  y  u  i  o  p  '  [");
        printf("\x1b[15;3Ha  s  d  f  g  h  j  k  l  \xE9  ~  ]");
        printf("\x1b[17;3Hz  x  c  v  b  n  m  ,  .  ;  /   ");
    }
    printf("\x1b[%u;%uH>", 11 + ( (selected / 12) * 2 ), 2 + ( (selected % 12) * 3 ) );
    consoleSelect(&top);
}

char getKeyboardChar(u8 selected, bool shift) {
    char key[96] = {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\'', '[',
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\xE9', '~', ']',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', ';', '/', ' ',
        '!', '@', '#', '$', '%', ' ', '&', '*', '(', ')', '_', '+',
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '"', '{',
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ' ', '^', '}',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', ':', '?', '\n'
    };
    return key[selected + (shift * 48)];
}
*/
bool getKeyboardInput(char* buffer, size_t bufsize, std::string htext, bool multiline) {
    SwkbdState kb;
    swkbdInit(&kb, SWKBD_TYPE_NORMAL, 2, -1);
    swkbdSetInitialText(&kb, buffer);
    swkbdSetHintText(&kb, htext.c_str());
    swkbdSetButton(&kb, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&kb, SWKBD_BUTTON_RIGHT, "Confirm", true);
    if (multiline) swkbdSetFeatures(&kb, SWKBD_DARKEN_TOP_SCREEN | SWKBD_MULTILINE);
    swkbdSetValidation(&kb, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    swkbdInputText(&kb, buffer, bufsize);
    SwkbdResult result = swkbdGetResult(&kb);
    if (result==SWKBD_D1_CLICK1) return true;
    else return false;
}
/*
std::string getKeyboardInputLegacy(u32 length, std::string str) {
    bool shift = false;
    u8 selected = 0;
    u32 offset = str.length();
    printInfo(MODE_KEYBOARD_LEGACY);
    printf("\x1b[2;0H%s ", str.c_str());
    printKeyboard(selected, shift);
    while ( aptMainLoop() )
    {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_R) {
            shift=(!shift);
        }
        if (kDown & KEY_LEFT) {
            if ( (selected % 12) > 0 ) selected--;
        }
        if (kDown & KEY_RIGHT) {
            if ( (selected % 12) < 11 ) selected++;
        }
        if (kDown & KEY_UP) {
            if (selected > 11) selected-=12;
        }
        if (kDown & KEY_DOWN) {
            if (selected < 36) selected+=12;
        }
        if (kDown & KEY_A) {
            if (offset < length) {
                char c = getKeyboardChar(selected, shift);
                str.push_back(c);
                offset++;
                printf("\x1b[2;0H%s ", str.c_str());
            }
        }
        if (kDown & KEY_L) {
            if (offset > 0) offset--;
            str.pop_back();
            printf("\x1b[2;0H%s ", str.c_str());
        }
        if (kDown) printKeyboard(selected, shift);
        if (kDown & KEY_START) break;
        if (kDown & KEY_B) return "";
        gfxEndFrame();
    }
    if (aptMainLoop()) return str;
    else return "";
}
*/
