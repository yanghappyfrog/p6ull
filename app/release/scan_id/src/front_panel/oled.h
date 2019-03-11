
#include <stdio.h>

#include "./front_panel.h"
#include "./led.h"

int flash_char(int x, int y, char c);
int flash_line(char *w_buf, int line);
int flash_oled(char w_buf[4][16]);
void clean_oled();
void oled_init();
