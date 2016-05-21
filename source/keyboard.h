#ifndef KEYBOARD_H
#define KEYBOARD_H

void printKeyboard(u8 selected, bool shift);
char getKeyboardChar(u8 selected, bool shift);
std::string getKeyboardInput(u32 length, std::string str = "");
std::string getKeyboardInputLegacy(u32 length, std::string str = "");
void flushKeyboard();

#endif /* KEYBOARD_H */
