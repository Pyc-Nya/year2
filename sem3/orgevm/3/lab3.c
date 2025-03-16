#include <GRAPHICS.H>
#include <MATH.H>
#include <CONIO.H>
#include <STDIO.H>

#define PI M_PI

float f(float x){
    // (sin(x))**2 - (cos(x))**2 = -cos(2x)
    return -1 * cos(2*x);
}

void drawAxesLabels(float x_left_border, float x_right_border, float y_bottom_border, float y_top_border, float x_step, float y_step) {
    char label[50];
    float x, y;
    float screen_x, screen_y;
    float count = 2;
    float offsetX, scaleX;
    float offsetY, scaleY;

    setcolor(WHITE);

    for (x = x_left_border; x <= x_right_border; x += x_step) { 
        count += 1;
        offsetX = x - x_left_border;
        scaleX = 550 / (x_right_border - x_left_border);
        screen_x = 50 + offsetX * scaleX;
        line(screen_x, 335, screen_x, 325); 
        sprintf(label, "%.1fpi", count / 2);
        outtextxy(screen_x - 10, 340, label); 
    }

    for (y = y_bottom_border + y_step; y < y_top_border; y += y_step) { 
        offsetY = y - y_bottom_border;
        scaleY = 350 / (y_top_border - y_bottom_border);
        screen_y = 400 - offsetY * scaleY;
        line(42, screen_y, 57, screen_y); 
        sprintf(label, "%.1f", y);
        outtextxy(20, screen_y - 10, label); 
    }
}

int main(void) {
   int gdriver = DETECT, gmode, errorcode; 
   float x_left_border = 3*PI/2, x_right_border = 8*PI; 
   float y_bottom_border = -2, y_top_border = 8; 
   float step = 0.01;
   float x, y;
   float screen_x, screen_y; 
   float y_max_value = -32000; 
   int x_max_coordinate, y_max_coordinate;
   char max_value[50];
   float scaleX, scaleY, offsetX, offsetY;

   initgraph(&gdriver, &gmode, "//tc//bgi");

   setlinestyle(0, 1, 2); 
   setcolor(WHITE);
   line (50, 330, 600, 330); 
   line(595, 325, 600, 330);
   line(595, 335, 600, 330);
   outtextxy(590, 340, "X");
   line(50, 400, 50, 50); 
   line(45, 55, 50, 50);
   line(55, 55, 50, 50);
   outtextxy(30, 50, "Y");

   drawAxesLabels(x_left_border, x_right_border, y_bottom_border, y_top_border, PI / 2, 1);

   for(x = x_left_border; x < x_right_border - PI; x += step) { 
      y = f(x);
      scaleX = 550 / (x_right_border - x_left_border);
      scaleY = 350 / (y_top_border - y_bottom_border);
      offsetX = x - x_left_border;
      offsetY = y - y_bottom_border;
      screen_x = 50 + offsetX * scaleX; 
      screen_y = 400 - offsetY * scaleY; 
      putpixel(screen_x, screen_y, WHITE);

      if(y > y_max_value){
         x_max_coordinate = screen_x;
         y_max_coordinate = screen_y;
         y_max_value = y;
      }
   }

   settextstyle(0, 0, 2);
   sprintf(max_value, "Max Value: %.2f", y_max_value);
   setcolor(RED);
   circle(x_max_coordinate, y_max_coordinate, 10);
   outtextxy(x_max_coordinate + 10, y_max_coordinate-20, "max value");
   setcolor(WHITE);
   settextstyle(0, 0, 1);
   outtextxy(50, 420, "f(x) = (sin(x))**2 - (cos(x))**2");
   outtextxy(50, 450, max_value);
   getch();
   closegraph(); 
   return 0;
}
