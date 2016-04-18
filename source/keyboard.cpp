#include <stdio.h>
#include <3ds.h>

#include "keyboard.h"
#include "utils.h"

void printKeyboard() {
    consoleSelect(&bot);
    consoleClear();
    if (shifted) {
        printf("\x1b[11;3H!  @  #  $  %%     &  *  (  )  _  +");
        printf("\x1b[13;3HQ  W  E  R  T  Y  U  I  O  P     {");
        printf("\x1b[15;3HA  S  D  F  G  H  J  K  L     ^  }");
        printf("\x1b[17;3HZ  X  C  V  B  N  M  <  >  :  ?  \\n");
    }
    else {
        printf("\x1b[11;3H1  2  3  4  5  6  7  8  9  0  -  =");
        printf("\x1b[13;3Hq  w  e  r  t  y  u  i  o  p     [");
        printf("\x1b[15;3Ha  s  d  f  g  h  j  k  l     ~  ]");
        printf("\x1b[17;3Hz  x  c  v  b  n  m  ,  .  ;  /   ");
    }
    printf("\x1b[%lu;%luH>", 11 + ( (selected / 12) * 2 ), 2 + ( (selected % 12) * 3 ) );
    consoleSelect(&top);
}

char getKeyboardChar(u32 index) {
    char key[96] = {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', ' ', '[',
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ' ', '~', ']',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', ';', '/', ' ',
        '!', '@', '#', '$', '%', ' ', '&', '*', '(', ')', '_', '+',
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', ' ', '{',
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ' ', '^', '}',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', ':', '?', '\n'
    };
    return key[index + (shifted * 48)];
}