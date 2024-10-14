#include <conio.h>
#include <dos.h>

int x1 = 20, y1 = 10, x2 = 60, y2 = 20;

void scrollWindow(int lines)
{
    union REGS regs;

    regs.h.ah = 0x07;   // Scroll function (down)
    regs.h.al = lines;  // Number of lines to scroll
    regs.h.bh = 0x00;   // Attribute of blank lines
    regs.h.ch = y1 - 1; // Top-left row (adjusted for zero-based index)
    regs.h.cl = x1 - 1; // Top-left column (adjusted for zero-based index)
    regs.h.dh = y2 - 1; // Bottom-right row (adjusted for zero-based index)
    regs.h.dl = x2 - 1; // Bottom-right column (adjusted for zero-based index)

    int86(0x10, &regs, &regs); // BIOS interrupt 0x10 to perform the scroll
}

int main()
{
    int text_color = 0, background_color = 0, count = y2 - y1 + 1;

    // Clear the entire screen
    clrscr();

    // Set the window with coordinates (x1, y1) to (x2, y2)
    textbackground(0);
    textcolor(0);
    window(x1, y1, x2, y2);
    gotoxy(1, count);
    clrscr(); // Clear the newly defined window area

    count = y2 - y1 + 1; // Reset count to the height of the window

    for (background_color = 0; background_color < 16; background_color++)
    {
        for (text_color = 0; text_color < 16; text_color++)
        {
            textcolor(text_color);            // Set text color
            textbackground(background_color); // Set background color

            // Print the line with the current color combination
            gotoxy(1, count);
            cprintf("Text: %d, Background: %d", text_color, background_color);
            count--;

            if (count < 1)
            {
                scrollWindow(2);
                count = 2;
            }

            delay(1500);
        }
    }
    return 0;
}