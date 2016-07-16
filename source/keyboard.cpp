#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "keyboard.h"

bool getKeyboardInput(char* buffer, size_t bufsize, std::string htext, bool multiline) {
    SwkbdState kb;
    swkbdInit(&kb, SWKBD_TYPE_NORMAL, 2, -1);
    swkbdSetInitialText(&kb, buffer);
    swkbdSetHintText(&kb, htext.c_str());
    swkbdSetButton(&kb, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&kb, SWKBD_BUTTON_RIGHT, "Confirm", true);
    if (multiline) swkbdSetFeatures(&kb, SWKBD_MULTILINE);
    swkbdSetValidation(&kb, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    swkbdInputText(&kb, buffer, bufsize);
    SwkbdResult result = swkbdGetResult(&kb);
    if (result==SWKBD_D1_CLICK1) return true;
    else return false;
}
