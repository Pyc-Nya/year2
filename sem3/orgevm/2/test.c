#include <conio.h>
#include <dos.h>

int x1 = 20, y1 = 10, x2 = 60, y2 = 20;

void scrollWindow(int lines) {
    union REGS regs;

    regs.h.ah = 0x06;            // Scroll function (upwards)
    regs.h.al = lines;           // Number of lines to scroll
    regs.h.bh = 0x00;            // Attribute of blank lines (формирование атрибута из фона и текста)
    regs.h.ch = y1 - 1;          // Top-left row (adjusted for zero-based index)
    regs.h.cl = x1 - 1;          // Top-left column (adjusted for zero-based index)
    regs.h.dh = y2 - 1;          // Bottom-right row (adjusted for zero-based index)
    regs.h.dl = x2 - 1;          // Bottom-right column (adjusted for zero-based index)

    int86(0x10, &regs, &regs);   // BIOS interrupt 0x10 to perform the scroll
}

int main() {
    int text_color = 0, background_color = 0, count = 1;

    clrscr();
    
    textbackground(0);
    textcolor(0);
    window(x1, y1, x2, y2);
    gotoxy(1, 1);
    clrscr(); 

    for (background_color = 0; background_color < 16; background_color++) {  
        for (text_color = 0; text_color < 16; text_color++) {
            textcolor(text_color);            
            textbackground(background_color); 

            if (count == 12) {
                scrollWindow(2);
                gotoxy(1, 10);
                count = 10;
                getch();
            }

            cprintf("Text: %d, Background: %d, count: %d", text_color, background_color, count);
            count++;
            gotoxy(1, count);

            // delay(1500);
            getch();
        }  
    }

    getch(); 
    return 0;
}