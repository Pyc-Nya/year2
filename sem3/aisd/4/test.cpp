#include <iostream>        // Подключаем библиотеку ввода-вывода (используем cout, cin)
#include <unordered_map>   // Подключаем хеш-таблицу (ассоциативный массив с доступом по ключу, как map, но быстрее)
#include <vector>          // Подключаем вектор (динамический массив)
#include <unordered_set>   // Подключаем хеш-множество (уникальные элементы, быстрый поиск)
#include <cstdlib>         // Подключаем для функций rand, srand
#include <ctime>           // Подключаем для функции time(), нужна для инициализации rand
#include <queue>           // Подключаем очередь FIFO, нужна для обхода в ширину (BFS)
#include <string>          // Подключаем для работы со строками std::string
#include <fstream>         // Подключаем для записи в файл (ofstream)

using namespace std;       // Чтобы не писать std:: перед cout, cin и т.д.

class Graph {
private:
    // adjList — это "список смежности". 
    // Это способ хранить граф: для каждой вершины мы храним список всех соседей (вершин, с которыми у неё есть ребро).
    // Ключ (string) — имя вершины, 
    // Значение (vector<string>) — список вершин, соединённых с этой вершиной ребром.
    unordered_map<string, vector<string>> adjList;

    // color — здесь мы храним цвет каждой вершины после проверки двудольности.
    // color[u] = 1, если вершина "u" принадлежит левой части,
    // color[u] = -1, если принадлежит правой части,
    // color[u] = 0, если цвет пока не назначен.
    unordered_map<string,int> color;

    // matchR — хранит для каждой вершины правой доли (color[u] = -1) ту вершину из левой доли, с которой она "спарена" в текущем паросочетании.
    // Если matchR[v] = u, значит есть ребро в паросочетании (u,v).
    // Если matchR[v] = "", значит вершина v справа свободна, не участвует в паросочетании.
    unordered_map<string,string> matchR;

public:
    // addEdge(u, v) — добавляет ребро между вершинами u и v.
    // Так как граф неориентированный, мы добавляем v в список соседей u и u в список соседей v.
    void addEdge(const string& u, const string& v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    // isBipartite() — проверяем, является ли граф двудольным.
    // Двудольный граф — это такой, в котором вершины можно разделить на 2 группы так,
    // что ребра идут только между группами, а внутри группы ребер нет.
    // Мы красим вершины в два цвета: 1 и -1.
    // Если найдём ребро, соединяющее вершины одного цвета, граф не двудольный.
    bool isBipartite() {
        // Сначала для всех вершин ставим цвет 0 (неокрашены)
        for (auto &pair : adjList) {
            color[pair.first] = 0;
        }

        // Обходим все вершины, так как граф может быть не связным (из нескольких частей).
        for (auto &vertex : adjList) {
            // Если вершина ещё не окрашена:
            if (color[vertex.first] == 0) {
                // Начнём обход в ширину (BFS) с этой вершины
                queue<string> q;
                q.push(vertex.first);

                // Красим стартовую вершину в цвет 1
                color[vertex.first] = 1;

                // Пока очередь не пуста, достаем по одной вершине
                while(!q.empty()) {
                    string u = q.front(); 
                    q.pop();

                    // Смотрим всех соседей вершины u
                    for (auto &v : adjList[u]) {
                        if (color[v] == 0) {
                            // Если сосед не окрашен, красим его в противоположный цвет от u
                            color[v] = -color[u];
                            q.push(v);
                        } else {
                            // Если сосед уже окрашен, проверяем, совпадает ли цвет.
                            // Если совпадает — плохой знак, граф не двудольный.
                            if (color[v] == color[u]) {
                                return false; 
                            }
                        }
                    }
                }
            }
        }
        // Если мы не нашли проблем, значит граф двудольный
        return true;
    }

    // inputEdges — ввод рёбер с клавиатуры.
    // Пользователь вводит количество рёбер, затем пары вершин, между которыми есть ребро.
    void inputEdges(int edgeCount) {
        cout << "Enter " << edgeCount << " edges (u v), where u and v are the names of vertices (e.g. A B):\n";
        for (int i = 0; i < edgeCount; ++i) {
            string u, v;
            cin >> u >> v; // Читаем имена двух вершин
            addEdge(u, v); // Добавляем ребро
        }
    }

    // generateRandomBipartiteGraph — случайно генерирует двудольный граф.
    // Идея: сначала генерируем набор вершин, делим их на две части (левую и правую),
    // а затем случайным образом создаём рёбра только между этими двумя частями.
    void generateRandomBipartiteGraph(int vertexCount) {
        vector<string> vertices; 
        unordered_set<string> uniqueVertices;
        srand((unsigned)time(nullptr)); // Инициализируем генератор случайных чисел текущим временем

        // Генерируем уникальные вершины, имена из одной буквы A-Z, пока не наберём vertexCount штук.
        while ((int)uniqueVertices.size() < vertexCount) {
            string vertex;
            vertex += static_cast<char>('A' + rand() % 26);
            uniqueVertices.insert(vertex);
        }

        // Перекладываем вершины из множества в вектор, чтобы удобно было их индексировать
        for (const auto& v : uniqueVertices) {
            vertices.push_back(v);
        }

        // Разбиваем вершины на две части: первые half — левая часть, остальные — правая.
        int half = vertexCount / 2;
        vector<string> leftPart(vertices.begin(), vertices.begin() + half);
        vector<string> rightPart(vertices.begin() + half, vertices.end());

        // Максимальное число рёбер между двумя частями — просто произведение размеров.
        int maxPossibleEdges = (int)leftPart.size() * (int)rightPart.size();

        if (maxPossibleEdges == 0) {
            // Если одна из частей пуста, рёбер быть не может.
            return;
        }

        // Выбираем случайное число рёбер от half до maxPossibleEdges
        int edgeCount = half + rand() % maxPossibleEdges;

        unordered_set<string> uniqueEdges;

        // Добавляем рёбра, пока не наберём edgeCount штук
        while ((int)uniqueEdges.size() < edgeCount) {
            int i = rand() % leftPart.size();
            int j = rand() % rightPart.size();

            string u = leftPart[i];
            string v = rightPart[j];

            // Чтобы не добавить одно и то же ребро дважды (u-v и v-u это одно и то же ребро),
            // создаём ключ edgeKey, отсортировав имена вершин по алфавиту.
            string edgeKey = (u < v) ? (u + "-" + v) : (v + "-" + u);

            if (uniqueEdges.find(edgeKey) == uniqueEdges.end()) {
                addEdge(u, v);
                uniqueEdges.insert(edgeKey);
            }
        }
    }

    // printGraph — печатает список смежности.
    // Показывает для каждой вершины, с кем она соединена.
    void printGraph() const {
        cout << "\nAdjacency list of a graph:\n";
        for (const auto& pair : adjList) {
            cout << "Vertex " << pair.first << " is connected to: ";
            for (const auto& neighbor : pair.second) {
                cout << neighbor << " ";
            }
            cout << endl;
        }
    }

    // bpmDfs — функция для поиска "увеличивающего пути" в контексте алгоритма Куна,
    // который используется для нахождения максимального паросочетания в двудольном графе.
    // u — вершина левой доли, с которой мы пытаемся "увеличить" паросочетание.
    // used — отмечаем вершины правой доли, которые мы уже посетили при поиске пути, чтобы не зациклиться.
    // смысл метода в том, что мы берём вершину из доли U и пробуем поставить ей в соответствие какую-то вершину из доли V,
    // которая не была затронута в нашем паросочетании ранее.
    // выполняя проверку для u раз за разом мы можем вычислить количество рёбер в текущем паросочетании
    bool bpmDfs(const string &u, unordered_map<string,bool> &used) {
        // Перебираем всех соседей вершины u
        for (auto &v : adjList[u]) {
            // Рассматриваем только рёбра из левой доли (color[u] = 1) в правую долю (color[v] = -1).
            // Это часть алгоритма поиска увеличивающего пути.
            // if (color[v] == -1) { // данная проверка закомментирована, поскольку является избыточной, т к adjList[u] содержит вершины только из правой доли
                // Если вершина v правой доли ещё не посещена в этом поиске
                if (!used[v]) {
                    used[v] = true; // Помечаем, что посетили v
                    // Если v сейчас свободна (matchR[v] == "") или
                    // если занята, но мы можем подвинуть u_i = matchR[v] вершину и переназначить ей другую вершину из V,
                    // то присваиваем matchR[v] = u, тем самым увеличивая паросочетание.
                    if (matchR[v] == "" || bpmDfs(matchR[v], used)) {
                        matchR[v] = u; 
                        return true;
                    }
                }
            // }
        }
        return false; // Не смогли найти увеличивающий путь для u
    }

    // maxMatching — находит максимальное паросочетание в двудольном графе, используя алгоритм Куна.
    // смысл метода в том, что мы буквально перебираем все вершины левой доли и применяем для них метод выше
    // первую вершину мы берём как отправную точку и начинаем строить ребра из неё соседей из adjList[v], тем самым, увеличивая result на 1, 
    // если удалось создать ребро

    // теперь о том, почему не важен порядок, т е отправная точка
    // в функции выше мы имеем возможность переставлять вершины (и более того, делаем это рекурсивно!), так что мы имеем право выбрать 
    // в качестве отправной точки любую вершину левой доли, поскольку функция bpmDfs все равно будет перставлять наши вершины в мапе matchR,
    // поскольку именно она играет здесь ключевую роль
    int maxMatching() {
        // Очищаем matchR и изначально все вершины правой доли свободны ("")
        matchR.clear();
        for (auto &pair : adjList) {
            if (color[pair.first] == -1) {
                matchR[pair.first] = ""; 
            }
        }

        int result = 0;
        // Для каждой вершины левой доли пытаемся найти увеличивающий путь
        // Если нашли — увеличиваем счётчик result
        for (auto &pair : adjList) {
            if (color[pair.first] == 1) {
                unordered_map<string,bool> used; 
                if (bpmDfs(pair.first, used)) {
                    result++;
                }
            }
        }
        return result; // Возвращаем размер максимального паросочетания
    }

    void findMinVertexCover() {
        vector<string> leftVertices;
        vector<string> rightVertices;

        // Разделяем вершины на левую (U) и правую (V) доли по их цвету:
        // color[u] = 1 для левой доли, color[u] = -1 для правой.
        for (auto &pair : adjList) {
            if (color[pair.first] == 1) {
                leftVertices.push_back(pair.first);
            } else if (color[pair.first] == -1) {
                rightVertices.push_back(pair.first);
            }
        }

        // coveredLeft — множество вершин левой доли, которые покрыты паросочетанием.
        // Если вершина левой доли u покрыта, значит существует вершина v правой доли такая, что matchR[v] = u.
        // matchR[v] = u означает, что ребро (u,v) входит в паросочетание.
        unordered_set<string> coveredLeft;
        for (auto &m : matchR) {
            // m.first — вершина правой доли, m.second — вершина левой доли, с которой она связана паросочетанием.
            if (m.second != "") {
                // Значит у нас есть ребро паросочетания (m.second, m.first).
                coveredLeft.insert(m.second);
            }
        }

        // freeLeft — вершины левой доли, НЕ покрытые паросочетанием.
        // Это те вершины левой доли, которые не входят ни в одно ребро паросочетания.
        vector<string> freeLeft;
        for (auto &u : leftVertices) {
            // Если u не найдена в coveredLeft, значит она свободна (не покрыта паросочетанием).
            if (coveredLeft.find(u) == coveredLeft.end()) {
                freeLeft.push_back(u);
            }
        }

        // visited — структура, где будем отмечать вершины, которых мы "достигли" при специальном обходе.
        // Мы хотим определить множество достижимых вершин Z.
        // Начинаем обход с freeLeft — свободных вершин левой доли.
        unordered_map<string,bool> visited; 
        queue<string> q;

        // Помечаем все свободные вершины левой доли как посещённые и ставим их в очередь.
        // Эти вершины — стартовые точки нашего специального обхода.
        for (auto &u : freeLeft) {
            visited[u] = true;
            q.push(u);
        }

        // Теперь основной цикл обхода.
        // Идея: 
        // - Из левой доли (U) в правую долю (V) идём по рёбрам, НЕ принадлежащим паросочетанию.
        // - Из правой доли (V) в левую долю (U) идём по рёбрам, принадлежащим паросочетанию.

        // Почему так?
        // Это стандартная процедура, которая позволяет найти множество достижимых вершин Z, 
        // используя структуру уже найденного максимального паросочетания.

        while(!q.empty()) {
            string u = q.front(); 
            q.pop();

            if (color[u] == 1) {
                // Если мы находимся в вершине левой доли:
                // Идём только в вершины правой доли по рёбрам, которые НЕ входят в паросочетание.

                for (auto &v : adjList[u]) {
                    // if (color[v] == -1) {
                        // Проверим, входит ли ребро (u,v) в паросочетание.
                        // Если matchR[v] == u, значит (u,v) — паросочетание.
                        // Нам же нужны рёбра, не входящие в паросочетание, 
                        // следовательно, нам подходит случай, когда matchR[v] != u.

                        if (matchR[v] != u) {
                            // Если вершина v ещё не была посещена:
                            if (!visited[v]) {
                                visited[v] = true;
                                q.push(v); 
                                // Добавляем v в очередь для продолжения обхода.
                            }
                        }
                    // }
                }

            } else {
                // Если мы в вершине правой доли:
                // Отсюда мы можем идти только по рёбрам паросочетания в левую долю.
                // Т.е. находим вершину левой доли, которая связана с u в паросочетании.

                // matchR[u] — это левая вершина, с которой связана правая вершина u, если такая есть.
                // Если matchR[u] != "", значит у нас есть парное ребро (matchR[u], u) в паросочетании.
                if (matchR.find(u) != matchR.end() && matchR[u] != "") {
                    string uPair = matchR[u]; // левая вершина, с которой u соединена паросочетанием

                    // Если эта левая вершина ещё не посещена:
                    if (!visited[uPair]) {
                        visited[uPair] = true;
                        q.push(uPair);
                        // Добавляем её в очередь, теперь будем рассматривать её соседей.
                    }
                }
            }
        }

        // После завершения обхода у нас есть множество достижимых вершин Z: это все вершины, для которых visited[...] = true.

        // По теореме Кёнига:
        // Минимальное вершинное покрытие = (U \ Z) ∪ (V ∩ Z),
        // где U — множество всех вершин левой доли, V — множество всех вершин правой доли,
        // Z — множество достижимых вершин.
        //
        // U \ Z — вершины левой доли, которые НЕ достижимы.
        // V ∩ Z — вершины правой доли, которые достижимы.

        vector<string> minCover;

        // Добавляем в минимальное покрытие все вершины левой доли, которые НЕ достижимы:
        for (auto &u : leftVertices) {
            if (!visited[u]) {
                minCover.push_back(u);
            }
        }

        // Добавляем в минимальное покрытие все вершины правой доли, которые ДОСТИЖИМЫ:
        for (auto &v : rightVertices) {
            if (visited[v]) {
                minCover.push_back(v);
            }
        }

        // Выводим результат:
        cout << "\nMinimum vertex cover (size = " << minCover.size() << "): ";
        for (auto &vert : minCover) {
            cout << vert << " ";
        }
        cout << endl;
    }


    // generateDotFile — создаём файл в формате DOT (Graphviz),
    // чтобы потом визуализировать наш граф. 
    // rankdir=LR; — расположим вершины слева направо.
    // Далее рисуем вершины и ребра.
    // Цвета: синие круги для левой доли, красные квадраты для правой.
    void generateDotFile(const string &filename) {
        ofstream out(filename); // Открываем файл для записи
        out << "graph G {\n";
        out << "rankdir=LR;\n";
        out << "node [fontname=\"Arial\"];\n";

        // Выводим вершины
        for (auto &p : adjList) {
            if (color[p.first] == 1) {
                // Левая доля — синий круг
                out << p.first << " [shape=circle, color=blue];\n";
            } else if (color[p.first] == -1) {
                // Правая доля — красный квадрат
                out << p.first << " [shape=box, color=red];\n";
            } else {
                // Если вдруг есть вершина без цвета (не должно быть), рисуем чёрный овал
                out << p.first << " [shape=ellipse, color=black];\n";
            }
        }

        // Выводим рёбра
        unordered_set<string> printedEdges;
        for (auto &pr : adjList) {
            for (auto &nbr : pr.second) {
                // Формируем ключ ребра e1
                string e1 = pr.first < nbr ? pr.first + "--" + nbr : nbr + "--" + pr.first;
                // Чтобы не напечатать ребро дважды, проверяем через множество printedEdges
                if (printedEdges.find(e1) == printedEdges.end()) {
                    printedEdges.insert(e1);
                    out << pr.first << " -- " << nbr << ";\n";
                }
            }
        }

        out << "}\n";
        out.close(); // Закрываем файл
    }
};

int main() {
    int choice;
    cout << "Choose input type:\n";
    cout << "1. Keyboard\n";
    cout << "2. Random generation\n";
    cout << "Enter your choice (1 or 2): ";
    cin >> choice;

    Graph graph;

    if (choice == 1) {
        // Если пользователь хочет вводить рёбра сам
        int edgeCount;
        cout << "Number of edges: ";
        cin >> edgeCount;
        graph.inputEdges(edgeCount); // Вызываем ввод рёбер

        // Проверяем, двудольный ли граф
        if (!graph.isBipartite()) {
            cout << "The graph is not bipartite. The program terminates." << endl;
            return 1; // Завершаем программу с кодом ошибки
        }

    } else if (choice == 2) {
        // Случайная генерация
        int vertexCount;
        cout << "Number of edges: ";
        cin >> vertexCount;
        graph.generateRandomBipartiteGraph(vertexCount);

        // Проверяем двудольность сгенерированного графа
        if (!graph.isBipartite()) {
            cout << "Error: The generated graph is not bipartite (shouldn't happen)." << endl;
            return 1;
        }
    } else {
        // Если пользователь ввёл не 1 и не 2
        cout << "Incorrect choice.\n";
        return 1; 
    }

    // Печатаем список смежности
    graph.printGraph();

    // Находим максимальное паросочетание
    int matchingSize = graph.maxMatching();
    cout << "\nSize of maximum matching: " << matchingSize << endl;

    // Находим минимальное вершинное покрытие
    graph.findMinVertexCover();

    // Создаём dot-файл для визуализации
    graph.generateDotFile("graph.dot");
    // Запускаем систему с командой "dot -Tpng graph.dot -o graph.png"
    // Эта команда использует Graphviz для преобразования dot в png картинку
    system("dot -Tpng graph.dot -o graph.png");

    return 0; // Программа успешно завершена
}
