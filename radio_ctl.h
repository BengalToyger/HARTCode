#ifndef radio_H_   /* Include guard */
#define radio_H_

void macDecode();
void initUART1();
void send();
int  receive();
int  escape();
int  unescape();

#endif