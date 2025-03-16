#include <dos.h>
#include <stdio.h>

#define X1 10 
#define Y1 15 
#define X2 70  
#define Y2 20 
#define DELAY 150

// KoopDuHaTbI u HaI7paBJIeHue DBu}|{eHu9I
typedef struct {
    int x;
    int y;
    int dx;
    int dy; 
} Point;

int my_getch() {
    union REGS in, out;
    in.h.ah = 0x07;
    int86(0x21, &in, &out);
    return out.h.al;
}

int my_kbhit() {
    union REGS in, out;
    in.h.ah = 0x0B; 
    int86(0x21, &in, &out);
    return out.h.al != 0; 
}

void draw_window() {
    int i;
    for (i = X1; i <= X2; i++) {
        gotoxy(i, Y1);
        printf("-");
        gotoxy(i, Y2);
        printf("-");
    }
    for (i = Y1; i <= Y2; i++) {
        gotoxy(X1, i);
        printf("|");
        gotoxy(X2, i);
        printf("|");
    }
}

void render(Point p) {
    gotoxy(p.x, p.y);
    printf("*");
}

void clear(Point p) {
    gotoxy(p.x, p.y);
    printf(" ");
}

void update_direction(Point *p) {
    int ch = my_getch(); 

    if (ch == 27) { // ESC key to exit
        p->dx = 0;
        p->dy = 0;
        return;
    }

    // Arrow keys => two bytes
    if (ch == 0) {
        ch = my_getch(); 

        if (ch == 72) { // Up arrow
            p->dx = 0;
            p->dy = -1;
        } else if (ch == 80) { // Down arrow
            p->dx = 0;
            p->dy = 1;
        } else if (ch == 75) { // Left arrow
            p->dx = -1;
            p->dy = 0;
        } else if (ch == 77) { // Right arrow
            p->dx = 1;
            p->dy = 0;
        }
    }
}

void update_position(Point *p) {
    clear(*p);

    if (p->dy == -1 && p->y > Y1 + 1) { // Upward
        p->y--;
    } else if (p->dy == 1 && p->y < Y2 - 1) { // Downward
        p->y++;
    } else if (p->dx == -1 && p->x > X1 + 1) { // Left 
        p->x--;
    } else if (p->dx == 1 && p->x < X2 - 1) { // Right 
        p->x++;
    }

    render(*p);
}

int main() {
    Point point = { 
        (X1 + X2) / 2, 
        (Y1 + Y2) / 2,
        0,             
        0              
    };

    clrscr(); 
    draw_window(); 
    render(point); 

    while (1) {
        if (my_kbhit()) { 
            update_direction(&point);

            if (point.dx == 0 && point.dy == 0) {
                break; // Exit if ESC was pressed
            }
        }

        if (point.dx != 0 || point.dy != 0) {
            update_position(&point);
            delay(DELAY);
        }
    }

    clrscr();
    return 0;
}
