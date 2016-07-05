#ifndef KEYBOARD_H
#define KEYBOARD_H

// void printKeyboard(u8 selected, bool shift);
// char getKeyboardChar(u8 selected, bool shift);
bool getKeyboardInput(char* buffer, size_t bufsize, std::string htext, bool multiline);
// std::string getKeyboardInputLegacy(u32 length, std::string str = "");

#endif /* KEYBOARD_H */
