#include <iostream>
#include <vector>
#include <cstdlib>

constexpr int WIDTH = 100;
constexpr int HEIGHT = 30;

// Класс холста для текстовой графики
class Canvas {
private:
    std::vector<std::vector<char>> buffer;

public:
    Canvas() : buffer(HEIGHT, std::vector<char>(WIDTH, ' ')) {}

    void clear() {
        for (auto &row : buffer) {
            std::fill(row.begin(), row.end(), ' ');
        }
    }

    void setPixel(int x, int y, char ch) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            buffer[y][x] = ch;
        }
    }

    void render() const {
        std::cout << "   ";
        for (int x = 0; x < WIDTH; ++x) {
            std::cout << (x % 10);
        }
        std::cout << std::endl;

        std::cout << "  +" << std::string(WIDTH, '-') << "+" << std::endl;
        
        for (int y = 0; y < HEIGHT; ++y) {
            std::cout << (y % 10) << " |";
            for (char ch : buffer[y]) {
                std::cout << ch;
            }
            std::cout << "|" << std::endl;
        }

        std::cout << "  +" << std::string(WIDTH, '-') << "+" << std::endl;
    }
};

// Базовый класс для фигур
class Shape {
public:
    virtual void draw(Canvas &canvas) const = 0;
    virtual ~Shape() = default;
};

// Класс линии
class Line : public Shape {
public:
    int x1, y1, x2, y2;
    Line(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

    void draw(Canvas &canvas) const override {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        int e2;

        int x = x1, y = y1;
        while (true) {
            canvas.setPixel(x, y, '*');
            if (x == x2 && y == y2) break;
            e2 = 2 * err;
            if (e2 > -dy) { 
                err -= dy; 
                x += sx; 
            }
            if (e2 < dx) { 
                err += dx; 
                y += sy; 
            }
        }
    }
};

class Rectangle : public Shape {
public:
    int x1, y1, x2, y2;
    Rectangle(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

    void draw(Canvas &canvas) const override {
        Line(x1, y1, x2, y1).draw(canvas);
        Line(x1, y2, x2, y2).draw(canvas);
        Line(x1, y1, x1, y2).draw(canvas);
        Line(x2, y1, x2, y2).draw(canvas);
    }
};

class Head : public Shape {
public:
    int hat_x1, hat_y1, hat_x2, hat_y2;
    int head_x1, head_y1, head_x2, head_y2;
    int eye_x1, eye_x2, eye_y;
    int nose_x, nose_y1, nose_y2;
    int mouth_x1, mouth_x2, mouth_y;

    Head(int x1, int y1, int x2, int y2) {
        int total_height = y2 - y1;
        int head_width = x2 - x1;

        int hat_height = total_height / 3;
        int head_height = total_height - hat_height;

        int hat_width = head_width / 2;
        hat_x1 = x1 + hat_width / 2;
        hat_y1 = y1;
        hat_x2 = x2 - hat_width / 2;
        hat_y2 = y1 + hat_height - 1;

        head_x1 = x1;
        head_y1 = hat_y2 + 1;
        head_x2 = x2;
        head_y2 = y2;

        eye_y = head_y1 + (head_height / 3);
        int eye_width = head_width * 0.2;
        eye_x1 = x1 + eye_width + 1;
        eye_x2 = eye_x1 + eye_width + eye_width;

        nose_x = (x1 + x2) / 2;
        nose_y1 = eye_y + head_height / 6;
        nose_y2 = nose_y1 + head_height / 6;

        mouth_y = nose_y2 + head_height / 6;
        int mouth_width = head_width * 2 / 3;
        mouth_x1 = x1 + (head_width - mouth_width) / 2;
        mouth_x2 = mouth_x1 + mouth_width;
    }

    void draw(Canvas &canvas) const override {
        Rectangle(hat_x1, hat_y1, hat_x2, hat_y2).draw(canvas);
        Line(hat_x1 - 1, hat_y2, hat_x2 + 1, hat_y2).draw(canvas);
        Rectangle(head_x1, head_y1, head_x2, head_y2).draw(canvas);
        for (int i = 0; i < (eye_x2 - eye_x1) / 2; i++) {
            canvas.setPixel(eye_x1 + i, eye_y, '-');
            canvas.setPixel(eye_x2 + i, eye_y, '-');
        }
        Line(nose_x, nose_y1, nose_x, nose_y2).draw(canvas);
        Line(mouth_x1, mouth_y, mouth_x2, mouth_y).draw(canvas);
    }
};

class DiagonalCross : public Shape {
private:
    int x1, y1, x2, y2;

public:
    DiagonalCross(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

    void draw(Canvas &canvas) const override {
        // Первая диагональ: из (x1, y1) в (x2, y2)
        Line diag1(x1, y1, x2, y2);
        // Вторая диагональ: из (x2, y1) в (x1, y2)
        Line diag2(x2, y1, x1, y2);

        // Рисуем диагонали
        diag1.draw(canvas);
        diag2.draw(canvas);
    }
};

int main() {
    Canvas canvas;
    bool clear_needed = false;

    while (true) {
        // Очистка экрана перед обновлением
        system("clear");

        // Отображение холста
        canvas.render();

        // Меню
        std::cout << "\n\n=== Меню ===\n";
        std::cout << "0. Выход\n";
        std::cout << "1. Очистить холст\n";
        std::cout << "2. Отрисовать линию (x1, y1, x2, y2)\n";
        std::cout << "3. Отрисовать прямоугольник (x1, y1, x2, y2)\n";
        std::cout << "4. Отрисовать крест (диагонали прямоугольника)\n";
        std::cout << "5. Отрисовать голову (x1, y1, x2, y2)\n";
        std::cout << "6. Отрисовать голову + кресты\n";
        std::cout << "Выберите опцию: ";

        int choice;
        std::cin >> choice;

        // Выход
        if (choice == 0) {
            std::cout << "Выход...\n";
            break;
        }

        // Очистка холста
        if (choice == 1) {
            canvas.clear();
            clear_needed = true;
            continue;
        }

        // Обработка выбора
        int x1, y1, x2, y2;
        if (choice >= 2 && choice <= 6) {
            std::cout << "Введите координаты (x1 y1 x2 y2): ";
            std::cin >> x1 >> y1 >> x2 >> y2;
        }

        if (choice == 2) {
            Line line(x1, y1, x2, y2);
            line.draw(canvas);
        } else if (choice == 3) {
            Rectangle rect(x1, y1, x2, y2);
            rect.draw(canvas);
        } else if (choice == 4) {
            DiagonalCross cross(x1, y1, x2, y2);
            cross.draw(canvas);
        } else if (choice == 5) {
            Head head(x1, y1, x2, y2);
            head.draw(canvas);
        } else if (choice == 6) {
            int head_width = x2 - x1;
            int head_height = y2 - y1;
            
            Head head(x1, y1, x2, y2);
            head.draw(canvas);
            
            int cross_size = 2;
            int cross_size2 = 3;
            
            int hat_x1 = (x1 + x2) / 2 - cross_size2 / 2; 
            int hat_y1 = (y1 + head.hat_y2 - 1) / 2 - cross_size2 / 2; 
            int hat_x2 = hat_x1 + cross_size2;
            int hat_y2 = hat_y1 + cross_size2;
            DiagonalCross crossHat(hat_x1, hat_y1, hat_x2, hat_y2);
            crossHat.draw(canvas);
            
            int cheek_left_x1 = x1 - cross_size - 2;
            int cheek_left_y1 = y1 + (head_height * 3/5) - cross_size / 2;
            int cheek_left_x2 = cheek_left_x1 + cross_size;
            int cheek_left_y2 = cheek_left_y1 + cross_size;
            DiagonalCross crossLeft(cheek_left_x1, cheek_left_y1, cheek_left_x2, cheek_left_y2);
            crossLeft.draw(canvas);
            
            int cheek_right_x1 = x2 + 2;
            int cheek_right_y1 = y1 + (head_height * 3/5) - cross_size / 2;
            int cheek_right_x2 = cheek_right_x1 + cross_size;
            int cheek_right_y2 = cheek_right_y1 + cross_size;
            DiagonalCross crossRight(cheek_right_x1, cheek_right_y1, cheek_right_x2, cheek_right_y2);
            crossRight.draw(canvas);
            
        } else {
            std::cout << "Ошибка: неправильный выбор!\n";
        }
    }

    std::cout << "Программа завершена.\n";
    return 0;
}

