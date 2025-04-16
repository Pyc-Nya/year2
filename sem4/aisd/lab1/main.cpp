#include <iostream>
#include <vector>
#include <cstdlib>
#include <exception>
#include <string>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>

constexpr int WIDTH = 100;
constexpr int HEIGHT = 30;

#define _USE_MATH_DEFINES

// Интерфейс для фигур, поддерживающих поворот (метод изменяет состояние фигуры)
class IRotatable {
public:
    // Поворот фигуры на угол angle вокруг центра фигуры (накопительный поворот)
    virtual void rotate(double angle) = 0;
    virtual ~IRotatable() {}
};

struct BoundingRect {
    int x1, y1, x2, y2;
};

// Класс исключения для фигур
class ShapeException : public std::exception {
private:
    std::string message;
public:
    explicit ShapeException(const std::string &msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Функция поворота точки вокруг опорной точки (pivotX, pivotY)
std::pair<int, int> rotatePoint(int x, int y, int pivotX, int pivotY, double angle) {
    double rad = angle * M_PI / 180.0;
    double cosA = std::cos(rad);
    double sinA = std::sin(rad);
    int newX = pivotX + std::round((x - pivotX) * cosA - (y - pivotY) * sinA);
    int newY = pivotY + std::round((x - pivotX) * sinA + (y - pivotY) * cosA);
    return {newX, newY};
}

// Предварительное объявление класса Canvas
class Canvas;

// Базовый класс для фигур с уникальным именем и операциями поворота и трансляции
class Shape {
protected:
    std::string name;
public:
    Shape(const std::string& name) : name(name) {}
    virtual void draw(Canvas &canvas) const = 0;
    virtual void translate(int dx, int dy) = 0;
    std::string getName() const { return name; }
    virtual std::string getInfo() const = 0;
    virtual BoundingRect getBoundingRect() const = 0;
    virtual ~Shape() = default;
};

// Класс холста для текстовой графики и хранения фигур
class Canvas {
private:
    std::vector<std::vector<char>> buffer;
    std::map<std::string, Shape*> shapes;
public:
    Canvas() : buffer(HEIGHT, std::vector<char>(WIDTH, ' ')) {}
    
    ~Canvas() {
        clearShapes();
    }
    
    void clearBuffer() {
        for (auto &row : buffer)
            std::fill(row.begin(), row.end(), ' ');
    }
    
    void setPixel(int x, int y, char ch) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            buffer[y][x] = ch;
    }
    
    void display() const {
        std::cout << "   ";
        for (int x = 0; x < WIDTH; ++x)
            std::cout << (x % 10);
        std::cout << "\n  +" << std::string(WIDTH, '-') << "+\n";
        for (int y = 0; y < HEIGHT; ++y) {
            std::cout << (y % 10) << " |";
            for (char ch : buffer[y])
                std::cout << ch;
            std::cout << "|\n";
        }
        std::cout << "  +" << std::string(WIDTH, '-') << "+\n";
    }

    void listShapes() const {
        for (const auto &pair : shapes) {
             std::cout << pair.first << ": " << pair.second->getInfo() << "\n";
        }
    }
    
    // Отрисовка всех фигур: сначала очищается буфер, затем каждая фигура отрисовывается по своим координатам
    void render() {
        clearBuffer();
        for (auto &pair : shapes) {
            pair.second->draw(*this);
        }
        display();
    }
    
    // Добавление фигуры (если фигура с таким именем уже существует, генерируется исключение)
    void addShape(Shape* shape) {
        if (shapes.find(shape->getName()) != shapes.end())
            throw ShapeException("Фигура с таким именем уже существует.");
        shapes[shape->getName()] = shape;
    }
    
    // Удаление всех фигур с холста
    void clearShapes() {
        for (auto &pair : shapes)
            delete pair.second;
        shapes.clear();
    }
    
    // Удаление фигуры по имени
    void removeShape(const std::string &name) {
        auto it = shapes.find(name);
        if (it == shapes.end())
            throw ShapeException("Фигура с таким именем не найдена.");
        delete it->second;
        shapes.erase(it);
    }
    
    // Получение фигуры по имени
    Shape* getShape(const std::string &name) const {
        auto it = shapes.find(name);
        if (it == shapes.end())
            throw ShapeException("Фигура с таким именем не найдена.");
        return it->second;
    }
};

// ---------------------------
// Реализация фигуры "Линия"
// ---------------------------
class Line : public Shape, public IRotatable {
public:
    int x1, y1, x2, y2;
    double angle; // накопленный угол поворота

    // Конструктор принимает имя, координаты и опциональный угол поворота
    Line(const std::string& name, int x1, int y1, int x2, int y2, double angle = 0)
        : Shape(name), x1(x1), y1(y1), x2(x2), y2(y2), angle(0) // инициализируем angle нулём
    {
        if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT ||
            x2 < 0 || x2 >= WIDTH || y2 < 0 || y2 >= HEIGHT)
            throw ShapeException("Line: точки вне границ холста.");
        if (angle != 0) {
            // Поворот по центру линии
            int pivotX = (x1 + x2) / 2, pivotY = (y1 + y2) / 2;
            auto np1 = rotatePoint(x1, y1, pivotX, pivotY, angle);
            auto np2 = rotatePoint(x2, y2, pivotX, pivotY, angle);
            x1 = np1.first; y1 = np1.second;
            x2 = np2.first; y2 = np2.second;
            this->angle = angle; // сохраняем поворот
        }
    }

    std::string getInfo() const override {
        std::ostringstream oss;
        oss << "Line: (" << x1 << "," << y1 << ") - (" 
            << x2 << "," << y2 << "), угол: " << angle;
        return oss.str();
    }
    
    void draw(Canvas &canvas) const override {
        int dx = std::abs(x2 - x1), dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy, e2, x = x1, y = y1;
        while (true) {
            canvas.setPixel(x, y, '*');
            if (x == x2 && y == y2)
                break;
            e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x += sx; }
            if (e2 < dx) { err += dx; y += sy; }
        }
    }

    BoundingRect getBoundingRect() const override {
        int minX = std::min(x1, x2);
        int minY = std::min(y1, y2);
        int maxX = std::max(x1, x2);
        int maxY = std::max(y1, y2);
        return { minX, minY, maxX, maxY };
    }
    
    // Вращение линии вокруг её центра (in-place)
    void rotate(double deltaAngle) override {
        int pivotX = (x1 + x2) / 2;
        int pivotY = (y1 + y2) / 2;
        auto np1 = rotatePoint(x1, y1, pivotX, pivotY, deltaAngle);
        auto np2 = rotatePoint(x2, y2, pivotX, pivotY, deltaAngle);
        if (np1.first < 0 || np1.first >= WIDTH || np1.second < 0 || np1.second >= HEIGHT ||
            np2.first < 0 || np2.first >= WIDTH || np2.second < 0 || np2.second >= HEIGHT)
            throw ShapeException("Line: поворот приводит к выходу за границы холста.");
        x1 = np1.first;
        y1 = np1.second;
        x2 = np2.first;
        y2 = np2.second;
        angle += deltaAngle;
    }
    
    // Перемещение линии на dx, dy (in-place)
    void translate(int dx, int dy) override {
        int newX1 = x1 + dx, newY1 = y1 + dy, newX2 = x2 + dx, newY2 = y2 + dy;
        if (newX1 < 0 || newX1 >= WIDTH || newY1 < 0 || newY1 >= HEIGHT ||
            newX2 < 0 || newX2 >= WIDTH || newY2 < 0 || newY2 >= HEIGHT)
            throw ShapeException("Line: перевод приводит к выходу за границы холста.");
        x1 = newX1; y1 = newY1; x2 = newX2; y2 = newY2;
    }
};

// ------------------------------
// Реализация фигуры "Прямоугольник"
// ------------------------------
class Rectangle : public Shape, public IRotatable {
private:
    int orig_x1, orig_y1, orig_x2, orig_y2;
    double angle; // накопленный угол поворота
public:
    Rectangle(const std::string& name, int x1, int y1, int x2, int y2, double angle = 0)
        : Shape(name), orig_x1(x1), orig_y1(y1), orig_x2(x2), orig_y2(y2), angle(angle)
    {
        if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT ||
            x2 < 0 || x2 >= WIDTH || y2 < 0 || y2 >= HEIGHT)
            throw ShapeException("Rectangle: вершины вне холста.");
        if (x1 >= x2 || y1 >= y2)
            throw ShapeException("Rectangle: некорректные параметры.");
    }

    std::string getInfo() const override {
        std::ostringstream oss;
        oss << "Rectangle: (" << orig_x1 << "," << orig_y1 << ") - (" 
            << orig_x2 << "," << orig_y2 << "), угол: " << angle;
        return oss.str();
    }    
    
    void draw(Canvas &canvas) const override {
        std::vector<std::pair<int, int>> verts = { {orig_x1, orig_y1}, {orig_x2, orig_y1}, {orig_x2, orig_y2}, {orig_x1, orig_y2} };
        int centerX = (orig_x1 + orig_x2) / 2;
        int centerY = (orig_y1 + orig_y2) / 2;
        if (angle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, angle);
        }
        for (size_t i = 0; i < verts.size(); ++i) {
            auto [x1, y1] = verts[i];
            auto [x2, y2] = verts[(i + 1) % verts.size()];
            Line("temp", x1, y1, x2, y2).draw(canvas);
        }
    }
    
    // Поворот прямоугольника (in-place)
    void rotate(double deltaAngle) override {
        double newAngle = angle + deltaAngle;
        std::vector<std::pair<int, int>> verts = { {orig_x1, orig_y1}, {orig_x2, orig_y1}, {orig_x2, orig_y2}, {orig_x1, orig_y2} };
        int centerX = (orig_x1 + orig_x2) / 2;
        int centerY = (orig_y1 + orig_y2) / 2;
        if (newAngle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, newAngle);
        }
        int minX = verts[0].first, maxX = verts[0].first, minY = verts[0].second, maxY = verts[0].second;
        for (const auto &p : verts) {
            minX = std::min(minX, p.first);
            maxX = std::max(maxX, p.first);
            minY = std::min(minY, p.second);
            maxY = std::max(maxY, p.second);
        }
        if(minX < 0 || minY < 0 || maxX >= WIDTH || maxY >= HEIGHT)
            throw ShapeException("Rectangle: поворот приводит к выходу за границы холста.");
        angle = newAngle;
    }
    
    // Перемещение прямоугольника (in-place)
    void translate(int dx, int dy) override {
        int new_x1 = orig_x1 + dx, new_y1 = orig_y1 + dy, new_x2 = orig_x2 + dx, new_y2 = orig_y2 + dy;
        std::vector<std::pair<int, int>> verts = { {new_x1, new_y1}, {new_x2, new_y1}, {new_x2, new_y2}, {new_x1, new_y2} };
        int centerX = (new_x1 + new_x2) / 2;
        int centerY = (new_y1 + new_y2) / 2;
        if (angle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, angle);
        }
        int minX = verts[0].first, maxX = verts[0].first, minY = verts[0].second, maxY = verts[0].second;
        for (const auto &p : verts) {
            minX = std::min(minX, p.first);
            maxX = std::max(maxX, p.first);
            minY = std::min(minY, p.second);
            maxY = std::max(maxY, p.second);
        }
        if(minX < 0 || minY < 0 || maxX >= WIDTH || maxY >= HEIGHT)
            throw ShapeException("Rectangle: перевод приводит к выходу за границы холста.");
        orig_x1 = new_x1; orig_y1 = new_y1; orig_x2 = new_x2; orig_y2 = new_y2;
    }
    
    BoundingRect getBoundingRect() const override {
        std::vector<std::pair<int, int>> verts = { {orig_x1, orig_y1}, {orig_x2, orig_y1},
                                                   {orig_x2, orig_y2}, {orig_x1, orig_y2} };
        int centerX = (orig_x1 + orig_x2) / 2;
        int centerY = (orig_y1 + orig_y2) / 2;
        if (angle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, angle);
        }
        int minX = verts[0].first, minY = verts[0].second;
        int maxX = verts[0].first, maxY = verts[0].second;
        for (const auto &p : verts) {
            minX = std::min(minX, p.first);
            minY = std::min(minY, p.second);
            maxX = std::max(maxX, p.first);
            maxY = std::max(maxY, p.second);
        }
        return { minX, minY, maxX, maxY };
    }
};

// -----------------------------
// Реализация фигуры "Диагональный крест"
// -----------------------------
class DiagonalCross : public Shape, public IRotatable {
private:
    int orig_x1, orig_y1, orig_x2, orig_y2;
    double angle;
public:
    DiagonalCross(const std::string& name, int x1, int y1, int x2, int y2, double angle = 0)
        : Shape(name), orig_x1(x1), orig_y1(y1), orig_x2(x2), orig_y2(y2), angle(angle)
    {
        if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT ||
            x2 < 0 || x2 >= WIDTH || y2 < 0 || y2 >= HEIGHT)
            throw ShapeException("DiagonalCross: точки вне холста.");
    }

    std::string getInfo() const override {
        std::ostringstream oss;
        oss << "DiagonalCross: (" << orig_x1 << "," << orig_y1 << ") - (" 
            << orig_x2 << "," << orig_y2 << "), угол: " << angle;
        return oss.str();
    }    
    
    void draw(Canvas &canvas) const override {
        std::vector<std::pair<int, int>> verts = { {orig_x1, orig_y1}, {orig_x2, orig_y1}, {orig_x2, orig_y2}, {orig_x1, orig_y2} };
        int centerX = (orig_x1 + orig_x2) / 2;
        int centerY = (orig_y1 + orig_y2) / 2;
        if (angle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, angle);
        }
        Line("temp", verts[0].first, verts[0].second, verts[2].first, verts[2].second).draw(canvas);
        Line("temp", verts[1].first, verts[1].second, verts[3].first, verts[3].second).draw(canvas);
    }
    
    // Вращение креста (in-place)
    void rotate(double deltaAngle) override {
        double newAngle = angle + deltaAngle;
        std::vector<std::pair<int, int>> verts = { {orig_x1, orig_y1}, {orig_x2, orig_y1}, {orig_x2, orig_y2}, {orig_x1, orig_y2} };
        int centerX = (orig_x1 + orig_x2) / 2;
        int centerY = (orig_y1 + orig_y2) / 2;
        if (newAngle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, newAngle);
        }
        int minX = verts[0].first, maxX = verts[0].first, minY = verts[0].second, maxY = verts[0].second;
        for (const auto &p : verts) {
            minX = std::min(minX, p.first);
            maxX = std::max(maxX, p.first);
            minY = std::min(minY, p.second);
            maxY = std::max(maxY, p.second);
        }
        if(minX < 0 || minY < 0 || maxX >= WIDTH || maxY >= HEIGHT)
            throw ShapeException("DiagonalCross: поворот приводит к выходу за границы холста.");
        angle = newAngle;
    }
    
    // Перемещение креста (in-place)
    void translate(int dx, int dy) override {
        int new_x1 = orig_x1 + dx, new_y1 = orig_y1 + dy, new_x2 = orig_x2 + dx, new_y2 = orig_y2 + dy;
        std::vector<std::pair<int, int>> verts = { {new_x1, new_y1}, {new_x2, new_y1}, {new_x2, new_y2}, {new_x1, new_y2} };
        int centerX = (new_x1 + new_x2) / 2;
        int centerY = (new_y1 + new_y2) / 2;
        if (angle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, angle);
        }
        int minX = verts[0].first, maxX = verts[0].first, minY = verts[0].second, maxY = verts[0].second;
        for (const auto &p : verts) {
            minX = std::min(minX, p.first);
            maxX = std::max(maxX, p.first);
            minY = std::min(minY, p.second);
            maxY = std::max(maxY, p.second);
        }
        if(minX < 0 || minY < 0 || maxX >= WIDTH || maxY >= HEIGHT)
            throw ShapeException("DiagonalCross: перевод приводит к выходу за границы холста.");
        orig_x1 = new_x1; orig_y1 = new_y1; orig_x2 = new_x2; orig_y2 = new_y2;
    }
    
    BoundingRect getBoundingRect() const override {
        std::vector<std::pair<int, int>> verts = { {orig_x1, orig_y1}, {orig_x2, orig_y1},
                                                   {orig_x2, orig_y2}, {orig_x1, orig_y2} };
        int centerX = (orig_x1 + orig_x2) / 2;
        int centerY = (orig_y1 + orig_y2) / 2;
        if (angle != 0) {
            for (auto &p : verts)
                p = rotatePoint(p.first, p.second, centerX, centerY, angle);
        }
        int minX = verts[0].first, minY = verts[0].second;
        int maxX = verts[0].first, maxY = verts[0].second;
        for (const auto &p : verts) {
            minX = std::min(minX, p.first);
            minY = std::min(minY, p.second);
            maxX = std::max(maxX, p.first);
            maxY = std::max(maxY, p.second);
        }
        return { minX, minY, maxX, maxY };
    }
};

// -----------------------------
// Реализация фигуры "Голова"
// -----------------------------
class Head : public Shape {
private:
    int orig_x1, orig_y1, orig_x2, orig_y2;
public:
    Head(const std::string& name, int x1, int y1, int x2, int y2)
        : Shape(name), orig_x1(x1), orig_y1(y1), orig_x2(x2), orig_y2(y2)
    {
        if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT ||
            x2 < 0 || x2 >= WIDTH || y2 < 0 || y2 >= HEIGHT)
            throw ShapeException("Head: координаты головы вне холста.");
        if (x1 >= x2 || y1 >= y2)
            throw ShapeException("Head: некорректные параметры.");
    }

    std::string getInfo() const override {
        std::ostringstream oss;
        oss << "Head: (" << orig_x1 << "," << orig_y1 << ") - (" 
            << orig_x2 << "," << orig_y2 << "), угол: 0";
        return oss.str();
    }
    
    void draw(Canvas &canvas) const override {
        int x1 = orig_x1, y1 = orig_y1, x2 = orig_x2, y2 = orig_y2;
        int total_height = y2 - y1;
        if (total_height < 6)
            throw ShapeException("Head: недостаточная высота.");
        int head_width = x2 - x1;
        int hat_height = total_height / 3;
        int head_height = total_height - hat_height;
        if (hat_height < 1 || head_height < 1)
            throw ShapeException("Head: слишком мала высота.");
        int hat_width = head_width / 2;
        int hat_x1 = x1 + hat_width / 2;
        int hat_y1 = y1;
        int hat_x2 = x2 - hat_width / 2;
        int hat_y2 = y1 + hat_height - 1;
        int head_x1 = x1;
        int head_y1 = hat_y2 + 1;
        int head_x2 = x2;
        int head_y2 = y2;
        int eye_y = head_y1 + (head_height / 3);
        int eye_width = static_cast<int>(head_width * 0.2);
        if (eye_width < 1) eye_width = 1;
        int eye_x1 = x1 + eye_width + 1;
        int eye_x2 = eye_x1 + 2 * eye_width;
        if (eye_x2 > x2)
            throw ShapeException("Head: глаза некорректно расположены.");
        int nose_x = (x1 + x2) / 2;
        int nose_y1 = eye_y + head_height / 6;
        int nose_y2 = nose_y1 + head_height / 6;
        int mouth_y = nose_y2 + head_height / 6;
        int mouth_width = head_width * 2 / 3;
        int mouth_x1 = x1 + (head_width - mouth_width) / 2;
        int mouth_x2 = mouth_x1 + mouth_width;
        if (mouth_x2 > x2 || mouth_y > y2)
            throw ShapeException("Head: рот некорректно расположен.");
        
        Rectangle("temp", hat_x1, hat_y1, hat_x2, hat_y2).draw(canvas);
        Line("temp", hat_x1 - 1, hat_y2, hat_x2 + 1, hat_y2).draw(canvas);
        Rectangle("temp", head_x1, head_y1, head_x2, head_y2).draw(canvas);
        for (int i = 0; i < (eye_x2 - eye_x1) / 2; i++) {
            canvas.setPixel(eye_x1 + i, eye_y, '-');
            canvas.setPixel(eye_x2 + i, eye_y, '-');
        }
        Line("temp", nose_x, nose_y1, nose_x, nose_y2).draw(canvas);
        Line("temp", mouth_x1, mouth_y, mouth_x2, mouth_y).draw(canvas);
    }
    
    // Перемещение головы (in-place)
    void translate(int dx, int dy) override {
        int new_x1 = orig_x1 + dx, new_y1 = orig_y1 + dy, new_x2 = orig_x2 + dx, new_y2 = orig_y2 + dy;
        if(new_x1 < 0 || new_x1 >= WIDTH || new_y1 < 0 || new_y1 >= HEIGHT ||
           new_x2 < 0 || new_x2 >= WIDTH || new_y2 < 0 || new_y2 >= HEIGHT)
            throw ShapeException("Head: перевод приводит к выходу за границы холста.");
        orig_x1 = new_x1; orig_y1 = new_y1; orig_x2 = new_x2; orig_y2 = new_y2;
    }

    BoundingRect getBoundingRect() const override {
        return { orig_x1, orig_y1, orig_x2, orig_y2 };
    }
};

// -----------------------------
// Главная функция с меню
// -----------------------------
int main() {
    Canvas canvas;
    int choice;
    
    while (true) {
        system("clear");
        canvas.render();
        canvas.listShapes();
        std::cout << "\n\n=== Меню ===\n";
        std::cout << "0. Выход\n";
        std::cout << "1. Очистить холст (удалить все фигуры)\n";
        std::cout << "2. Добавить линию (имя x1 y1 x2 y2 [angle])\n";
        std::cout << "3. Добавить прямоугольник (имя x1 y1 x2 y2 [angle])\n";
        std::cout << "4. Добавить крест (имя x1 y1 x2 y2 [angle])\n";
        std::cout << "5. Добавить голову (имя x1 y1 x2 y2)\n";
        std::cout << "6. Удалить фигуру (имя)\n";
        std::cout << "7. Переместить фигуру (имя dx dy)\n";
        std::cout << "8. Повернуть фигуру (имя angle)\n";
        std::cout << "9. Вывести все фигуры и их координаты\n";
        std::cout << "Выберите опцию: ";
        std::cin >> choice;
        std::cin.ignore();
        
        try {
            if (choice == 0) {
                std::cout << "Выход...\n";
                break;
            }
            else if (choice == 1) {
                canvas.clearShapes();
            }
            else if (choice == 2) {
                std::cout << "Введите параметры для линии (имя x1 y1 x2 y2 [angle]): ";
                std::string input;
                std::getline(std::cin, input);
                std::istringstream iss(input);
                std::string name;
                int x1, y1, x2, y2;
                double angle = 0;
                iss >> name >> x1 >> y1 >> x2 >> y2;
                if (!(iss >> angle))
                    angle = 0;
                Line* line = new Line(name, x1, y1, x2, y2, angle);
                canvas.addShape(line);
            }
            else if (choice == 3) {
                std::cout << "Введите параметры для прямоугольника (имя x1 y1 x2 y2 [angle]): ";
                std::string input;
                std::getline(std::cin, input);
                std::istringstream iss(input);
                std::string name;
                int x1, y1, x2, y2;
                double angle = 0;
                iss >> name >> x1 >> y1 >> x2 >> y2;
                if (!(iss >> angle))
                    angle = 0;
                Rectangle* rect = new Rectangle(name, x1, y1, x2, y2, angle);
                canvas.addShape(rect);
            }
            else if (choice == 4) {
                std::cout << "Введите параметры для креста (имя x1 y1 x2 y2 [angle]): ";
                std::string input;
                std::getline(std::cin, input);
                std::istringstream iss(input);
                std::string name;
                int x1, y1, x2, y2;
                double angle = 0;
                iss >> name >> x1 >> y1 >> x2 >> y2;
                if (!(iss >> angle))
                    angle = 0;
                DiagonalCross* cross = new DiagonalCross(name, x1, y1, x2, y2, angle);
                canvas.addShape(cross);
            }
            else if (choice == 5) {
                std::cout << "Введите параметры для головы (имя x1 y1 x2 y2): ";
                std::string input;
                std::getline(std::cin, input);
                std::istringstream iss(input);
                std::string name;
                int x1, y1, x2, y2;
                iss >> name >> x1 >> y1 >> x2 >> y2;
                Head* head = new Head(name, x1, y1, x2, y2);
                canvas.addShape(head);
            }
            else if (choice == 6) {
                std::cout << "Введите имя фигуры для удаления: ";
                std::string name;
                std::cin >> name;
                canvas.removeShape(name);
            }
            else if (choice == 7) {
                std::cout << "Введите параметры для перемещения (имя dx dy): ";
                std::string name;
                int dx, dy;
                std::cin >> name >> dx >> dy;
                Shape* shape = canvas.getShape(name);
                shape->translate(dx, dy);
            }
            else if (choice == 8) {
                std::cout << "Введите параметры для поворота (имя angle): ";
                std::string name;
                double angle;
                std::cin >> name >> angle;
                Shape* shape = canvas.getShape(name);
                IRotatable* rotatable = dynamic_cast<IRotatable*>(shape);
                if (rotatable) {
                    rotatable->rotate(angle);
                } else {
                    throw ShapeException("Данная фигура не поддерживает вращение");
                }
            }
            else if (choice == 9) {
                canvas.listShapes();
                std::cout << "Нажмите Enter для продолжения...";
                std::cin.ignore();
                std::getchar();
            }
            else {
                std::cout << "Неправильный выбор!\n";
            }
        }
        catch (const ShapeException &e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
            std::cout << "Нажмите Enter для продолжения...";
            std::cin.ignore();
            std::getchar();
        }
    }
    
    std::cout << "Программа завершена.\n";
    return 0;
}
