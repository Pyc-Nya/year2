// main.cpp
#include <iostream>
#include <string>
#include <map>
#include "HashTable.hpp"

// Вывод всех множеств в контейнере sets
void printAll(const std::map<std::string, HashTable<int>>& sets) {
    std::cout << "=== Текущие множества ===\n";
    for (auto& [name, ht] : sets) {
        std::cout << name << ": ";
        ht.print();
    }
    std::cout << "=========================\n\n";
}

void clearStdIn() {
    std::cin.ignore();
}

// Вспомогательная: выбрать существующее множество по имени
HashTable<int>& chooseSet(std::map<std::string, HashTable<int>>& sets) {
    std::string name;
    std::cout << "Введите имя множества: ";
    std::cin >> name;
    return sets[name];  // если не было — создастся пустое
}

int main() {
    std::map<std::string, HashTable<int>> sets;
    // Инициализация пяти множеств A–E
    for (char c = 'A'; c <= 'E'; ++c) {
        sets[std::string(1, c)] = HashTable<int>(8);
    }
    // Пример начальных данных
    for (int i = 0; i < 10; i++)
    {
        sets["A"].insert(i + 1);
    }
    
    sets["B"].insert(2); sets["B"].insert(3); sets["B"].insert(4);
    sets["C"].insert(5); sets["C"].insert(6);
    sets["D"].insert(6); sets["D"].insert(7);
    sets["E"].insert(3);

    while (true) {
        system("clear");
        printAll(sets);
        std::cout << "Меню:\n"
                  << " 0) Выход\n"
                  << " 1) Создать новое множество\n"
                  << " 2) Удалить множество\n"
                  << " 3) Вставить элемент\n"
                  << " 4) Удалить элемент\n"
                  << " 5) Очистить множество\n"
                  << " 6) Пересечение (A ∩ B)\n"
                  << " 7) Объединение (A ∪ B)\n"
                  << " 8) Разность (A \\ B)\n"
                  << " 9) Симметрическая разность (A ⊕ B)\n"
                  << "10) MERGE (последовательность)\n"
                  << "11) CONCAT (последовательность)\n"
                  << "12) ERASE диапазона (последовательность)\n"
                  << "Выберите операцию: ";
        int op; std::cin >> op;
        if (!std::cin || op == 0) break;

        std::string name1, name2, nameR;
        int x, p1, p2;

        switch (op) {
        case 1:
            std::cout << "Имя нового множества: ";
            std::cin >> name1;
            sets[name1] = HashTable<int>(8);
            break;
        case 2:
            std::cout << "Имя множества для удаления: ";
            std::cin >> name1;
            sets.erase(name1);
            break;
        case 3:
            {
                auto& s = chooseSet(sets);
                std::cout << "Значение для вставки: "; std::cin >> x;
                s.insert(x);
            }
            break;
        case 4:
            {
                auto& s = chooseSet(sets);
                std::cout << "Значение для удаления: "; std::cin >> x;
                s.erase(x);
            }
            break;
        case 5:
            {
                auto& s = chooseSet(sets);
                s.clear();
            }
            break;
        case 6: // A ∩ B
            std::cout << "Первое множество: "; std::cin >> name1;
            std::cout << "Второе множество: "; std::cin >> name2;
            std::cout << "Имя результата: "; std::cin >> nameR;
            sets[nameR] = sets[name1].intersection(sets[name2]);
            break;
        case 7: // A ∪ B
            std::cout << "Первое множество: "; std::cin >> name1;
            std::cout << "Второе множество: "; std::cin >> name2;
            std::cout << "Имя результата: "; std::cin >> nameR;
            sets[nameR] = sets[name1].unionSet(sets[name2]);
            break;
        case 8: // A \ B
            std::cout << "Минuсуемое: "; std::cin >> name1;
            std::cout << "Вычитаемое:   "; std::cin >> name2;
            std::cout << "Имя результата: "; std::cin >> nameR;
            sets[nameR] = sets[name1].difference(sets[name2]);
            break;
        case 9: // A ⊕ B
            std::cout << "Первое: "; std::cin >> name1;
            std::cout << "Второе: "; std::cin >> name2;
            std::cout << "Имя результата: "; std::cin >> nameR;
            sets[nameR] = sets[name1].symDifference(sets[name2]);
            break;
        case 10: // MERGE
            std::cout << "Первое (последовательность): "; std::cin >> name1;
            std::cout << "Второе (последовательность): "; std::cin >> name2;
            std::cout << "Имя результата: "; std::cin >> nameR;
            sets[nameR] = sets[name1].MERGE(sets[name2]);
            break;
        case 11: // CONCAT
            std::cout << "Первое (последовательность): "; std::cin >> name1;
            std::cout << "Второе (последовательность): "; std::cin >> name2;
            std::cout << "Имя результата: "; std::cin >> nameR;
            sets[nameR] = sets[name1].CONCAT(sets[name2]);
            break;
        case 12: // ERASE диапазона
            std::cout << "Множество: "; std::cin >> name1;
            std::cout << "p1 (включительно): "; std::cin >> p1;
            std::cout << "p2 (исключительно): "; std::cin >> p2;
            std::cout << "Имя результата: ";   std::cin >> nameR;
            sets[nameR] = sets[name1].ERASE(p1, p2);
            break;
        default:
            std::cout << "Неправильный выбор\n";
        }

        clearStdIn();
        getchar();
    }

    std::cout << "Программа завершена.\n";
    return 0;
}
