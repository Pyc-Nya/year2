#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

/**
 * @class Node
 * @brief Класс, представляющий узел троичного дерева.
 */
class Node {
public:
    char value; ///< Значение узла.
    Node* left; ///< Указатель на левого потомка.
    Node* middle; ///< Указатель на среднего потомка.
    Node* right; ///< Указатель на правого потомка.
    Node* parent; ///< Указатель на родительский узел.

    /**
     * @brief Конструктор узла дерева.
     * @param val Значение узла.
     * @param parentNode Указатель на родительский узел.
     */
    Node(char val, Node* parentNode = nullptr)
        : value(val), left(nullptr), middle(nullptr), right(nullptr), parent(parentNode) {}
};

/**
 * @class TernaryTree
 * @brief Класс, представляющий троичное дерево.
 */
class TernaryTree {
public:
    Node* root; ///< Указатель на корневой узел дерева.
    size_t nodeCount; ///< Общее количество узлов в дереве.

    /**
     * @brief Конструктор троичного дерева.
     */
    TernaryTree() : root(nullptr), nodeCount(0) {}

    /**
     * @brief Добавляет новый узел в дерево.
     * @param parent Указатель на родительский узел.
     * @param val Значение нового узла.
     * @param position Позиция нового узла относительно родителя ("left", "middle", "right").
     */
    Node* addNode(Node* parent, char val, const string& position) {
        Node* newNode = new Node(val, parent); // Передаем указатель на родителя в конструктор узла.
        if (position == "left") {
            parent->left = newNode;
        } else if (position == "middle") {
            parent->middle = newNode;
        } else if (position == "right") {
            parent->right = newNode;
        }
        nodeCount++; // Увеличиваем счетчик узлов.

        return newNode;
    }

    /**
     * @brief Выполняет обход дерева в ширину.
     * @return Вектор символов, представляющий последовательность узлов при обходе в ширину.
     */
    vector<char> breadthFirstTraversal() {
        vector<char> result;
        result.reserve(nodeCount); // Резервируем память под количество узлов.

        if (!root) return result;

        queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
            result.push_back(current->value);

            if (current->left) q.push(current->left);
            if (current->middle) q.push(current->middle);
            if (current->right) q.push(current->right);
        }

        return result;
    }

    /**
     * @brief Рекурсивно выводит дерево в консоль в графическом виде с использованием стрелочек.
     * @param node Указатель на текущий узел.
     * @param prefix Префикс для текущей строки, используется для создания структуры дерева.
     * @param isLast Флаг, указывающий, является ли узел последним потомком.
     */
    void printTree(Node* node, const string& prefix = "", bool isLast = true) {
        if (!node) return;

        cout << prefix;

        // Выводим соединение: +-- для последнего ребенка, |-- для других
        cout << (isLast ? "+-- " : "|-- ");

        // Выводим значение узла
        cout << node->value << endl;

        // Формируем префикс для следующего уровня
        string newPrefix = prefix + (isLast ? "    " : "|   ");

        // Рекурсивно выводим потомков
        if (node->left || node->middle || node->right) {
            printTree(node->right, newPrefix, node->middle == nullptr && node->right == nullptr);
            printTree(node->middle, newPrefix, node->right == nullptr);
            printTree(node->left, newPrefix, true);
        }
    }

    /**
     * @brief Рекурсивно вычисляет высоту самого мощного поддерева для заданного узла.
     * @param node Указатель на текущий узел.
     * @return Высота поддерева.
     */
    int calculateSubtreeHeight(Node* node) {
        if (!node) return 0;

        int leftHeight = calculateSubtreeHeight(node->left);
        int middleHeight = calculateSubtreeHeight(node->middle);
        int rightHeight = calculateSubtreeHeight(node->right);

        return 1 + max({leftHeight, middleHeight, rightHeight});
    }

    /**
     * @brief Возвращает высоту самого мощного поддерева для корня.
     * @return Высота поддерева.
     */
    int getMaxSubtreeHeight() {
        return calculateSubtreeHeight(root);
    }

    /**
     * @brief Загружает дерево из файла.
     * @param filename Имя файла, содержащего описание дерева.
     */
    void loadTreeFromFile(const string& filename) {
        ifstream file(filename);

        unordered_map<string, Node*> nodes; // Для быстрого поиска узлов по имени.
        string line;

        getline(file, line); // Пропускаем заголовок

        while (getline(file, line)) {
            stringstream ss(line);
            string value, parentValue, position;
            getline(ss, value, ',');
            getline(ss, parentValue, ',');
            getline(ss, position, ',');

            Node* parentNode = (parentValue == "null") ? nullptr : nodes[parentValue];
            Node* newNode = (parentNode == nullptr)
                                ? (root = new Node(value[0])) // Устанавливаем корень
                                : addNode(parentNode, value[0], position);

            nodes[value] = newNode; // Добавляем узел в карту
        }

        file.close();
    }

    /**
     * @brief Добавляет узлы в дерево через ввод с клавиатуры.
     */
    void addNodesFromInput() {
        string value, parentValue, position;
        Node* parentNode = nullptr;

        while (true) {
            cout << "Enter node value (or 'exit' to stop): ";
            cin >> value;
            if (value == "exit") break;

            cout << "Enter parent value (or 'null' for root): ";
            cin >> parentValue;

            if (parentValue != "null") {
                // Если родительский узел не равен "null", то ищем его
                parentNode = findNodeByValue(parentValue);
                if (!parentNode) {
                    cout << "Parent node not found!" << endl;
                    continue;
                }
            } else {
                // Если родитель "null", это означает, что это корень дерева
                if (root == nullptr) {
                    root = new Node(value[0]); // Создаем корень дерева
                    cout << "Root node created: " << value << endl;
                    continue; // Переходим к следующему узлу
                } else {
                    cout << "Root already exists!" << endl;
                    continue; // Корень уже существует, продолжаем добавление других узлов
                }
            }

            cout << "Enter position ('left', 'middle', 'right'): ";
            cin >> position;
            if (position == "left" || position == "middle" || position == "right") {
                addNode(parentNode, value[0], position);
            } else {
                cout << "Invalid position!" << endl;
            }
        }
    }


    /**
     * @brief Рекурсивно ищет узел по значению.
     * @param value Значение узла.
     * @return Указатель на узел, если найден, иначе nullptr.
     */
    Node* findNodeByValue(const string& value) {
        if (!root) return nullptr;
        
        queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();

            // Сравниваем полное значение, а не только первый символ
            if (current->value == value[0]) return current;

            if (current->left) q.push(current->left);
            if (current->middle) q.push(current->middle);
            if (current->right) q.push(current->right);
        }

        return nullptr; // Если узел не найден
    }
};

/**
 * @brief Точка входа в программу. Создает дерево, добавляет узлы, выводит дерево
 *        и выполняет обход в ширину.
 * @return Код завершения программы.
 */
int main() {
    TernaryTree tree;

    // Выбираем способ ввода дерева
    string choice;
    cout << "Choose input method (file/input): ";
    cin >> choice;

    if (choice == "file") {
        string filename = "tree.csv";
        tree.loadTreeFromFile(filename);
    } else if (choice == "input") {
        tree.addNodesFromInput();
    } else {
        cout << "Invalid choice!" << endl;
        return 1;
    }

    // Выводим дерево
    cout << "Tree structure (graphical):" << endl;
    tree.printTree(tree.root);

    // Высота самого мощного поддерева
    int maxHeight = tree.getMaxSubtreeHeight();
    cout << "\nMaximum subtree height: " << maxHeight << endl;

    // Выполняем обход в ширину
    vector<char> traversal = tree.breadthFirstTraversal();
    cout << "\nBreadth-first traversal sequence:" << endl;
    for (char c : traversal) {
        cout << c << " ";
    }
    cout << endl;

    return 0;
}
