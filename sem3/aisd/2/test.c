#include <conio.h>
#include <dos.h>
#include <stdio.h>

/* enum COLORS { BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY, DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE };
*/

enum {ENTIRE, UP, DOWN};
const char* colorNames[] = { "BLACK", "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN", "LIGHTGRAY", "DARKGRAY", "LIGHTBLUE", "LIGHTGREEN", "LIGHTCYAN", "LIGHTRED", "LIGHTMAGENTA", "YELLOW", "WHITE" };
void scroll(int direction, int l_row, int l_col, int r_row, int r_col, int attr) {
    union REGS r;
    if (direction == DOWN) {
        r.h.ah = 7; 
    } else {
        r.h.ah = 6; 
    }
    r.h.al = 1;
    r.h.ch = l_row; 
    r.h.cl = l_col; 
    r.h.dh = r_row; 
    r.h.dl = r_col;
    r.h.bh = attr; 
    int86(0x10, &r, &r);
}
void displayColors(int x1, int y1, int x2, int y2) {
    int colorFore, colorBack;
    for (colorBack = 0; colorBack < 8; ++colorBack) {
        for (colorFore = 0; colorFore < 8; ++colorFore) {
            textbackground(colorBack);
            textcolor(colorFore);
            gotoxy(x1, y1);
            clreol();
            cprintf("Background: %s, Foreground: %s", colorNames[colorBack], colorNames[colorFore]);
	    delay(1500);
	    if (colorFore == 7) {
		y1++;
		if (y1 > y2) {
		    scroll(UP, y1, x1, y2, x2, LIGHTGRAY);
		    y1 = 5;
		}
	    }
	}
    }
}

int main() {
    int x1 = 20, y1 = 10, x2 = 60, y2 = 20;
    clrscr(); 

    // window(x1, x2, y1, y2);
    displayColors(x1, y1, x2, y2);

    getch(); 
    return 0;
}