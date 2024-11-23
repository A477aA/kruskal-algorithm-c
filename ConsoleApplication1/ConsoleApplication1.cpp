#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// Структура для представления графа с матрицей смежности
struct Graph {
    int V;
    std::vector<std::vector<int>> adjMatrix;

    Graph(int V) : V(V) {
        adjMatrix.resize(V, std::vector<int>(V, 0));  // Инициализируем матрицу смежности
    }

    void addEdge(int u, int v, int weight) {
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight;  // Неориентированный граф
    }
};

// Структура для представления ребра
struct Edge {
    int u, v, weight;
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// Функция для нахождения родителя с применением пути сжатия
int find(std::vector<int>& parent, int i) {
    if (parent[i] == i) return i;
    return parent[i] = find(parent, parent[i]);
}

// Функция для объединения двух подмножеств
void Union(std::vector<int>& parent, std::vector<int>& rank, int x, int y) {
    int rootX = find(parent, x);
    int rootY = find(parent, y);
    if (rootX != rootY) {
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        }
        else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        }
        else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
}

// Алгоритм Краскала для поиска минимального остовного дерева
std::vector<Edge> kruskal(Graph& graph) {
    std::vector<Edge> mst;
    std::vector<int> parent(graph.V);
    std::vector<int> rank(graph.V, 0);

    for (int i = 0; i < graph.V; i++) {
        parent[i] = i;
    }

    // Составляем список рёбер из матрицы смежности
    std::vector<Edge> edges;
    for (int i = 0; i < graph.V; i++) {
        for (int j = i + 1; j < graph.V; j++) {
            if (graph.adjMatrix[i][j] != 0) {
                edges.push_back({ i, j, graph.adjMatrix[i][j] });
            }
        }
    }

    // Сортируем рёбра по весу
    std::sort(edges.begin(), edges.end());

    // Применяем алгоритм Краскала
    for (const Edge& e : edges) {
        int uRoot = find(parent, e.u);
        int vRoot = find(parent, e.v);
        if (uRoot != vRoot) {
            mst.push_back(e);
            Union(parent, rank, uRoot, vRoot);
        }
    }

    return mst;
}

// Функция для визуализации графа и минимального остовного дерева
void visualizeGraph(Graph& graph, const std::vector<Edge>& mst) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minimum Spanning Tree");

    // Создаем шрифт
    sf::Font font;
    if (!font.loadFromFile("C:/Users/A477aA/source/repos/ConsoleApplication1/ConsoleApplication1/assets/arial.ttf")) {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
        return;
    }

    // Массив для хранения позиций вершин на экране
    std::vector<sf::Vector2f> positions(graph.V);

    // Для простоты размещаем вершины по кругу
    float radius = 250.f;  // Радиус круга
    sf::Vector2f center(400.f, 300.f);  // Центр круга
    for (int i = 0; i < graph.V; i++) {
        float angle = i * 2 * 3.14159f / graph.V;
        positions[i] = sf::Vector2f(center.x + radius * cos(angle), center.y + radius * sin(angle));
    }

    // Отображаем рёбра минимального остовного дерева
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        // Рисуем рёбра
        for (const Edge& edge : mst) {
            sf::Vertex line[] = {
                sf::Vertex(positions[edge.u], sf::Color::White),
                sf::Vertex(positions[edge.v], sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);  // Рисуем линию между двумя вершинами
        }

        // Рисуем вершины
        for (int i = 0; i < graph.V; i++) {
            sf::CircleShape vertex(20.f);  // Радиус вершины
            vertex.setFillColor(sf::Color::Green);
            vertex.setPosition(positions[i] - sf::Vector2f(20.f, 20.f));  // Центрируем вершину
            window.draw(vertex);

            // Рисуем текст с номером вершины
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(i));
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::Black);
            text.setPosition(positions[i] - sf::Vector2f(10.f, 10.f));  // Центрируем текст
            window.draw(text);
        }

        window.display();
    }
}

int main() {
    int V, E;
    std::cout << "Введите количество вершин: ";
    std::cin >> V;
    Graph graph(V);

    std::cout << "Введите количество рёбер: ";
    std::cin >> E;

    // Ввод рёбер в матрицу смежности
    for (int i = 0; i < E; i++) {
        int u, v, weight;
        std::cout << "Введите ребро " << i + 1 << " (формат: вершина1 вершина2 вес): ";
        std::cin >> u >> v >> weight;
        graph.addEdge(u, v, weight);
    }

    // Получаем минимальный остовной каркас
    std::vector<Edge> mst = kruskal(graph);

    // Вывод минимального остовного каркаса
    std::cout << "Минимальный остовной каркас:" << std::endl;
    for (const Edge& e : mst) {
        std::cout << e.u << " -- " << e.v << " == " << e.weight << std::endl;
    }

    // Визуализируем граф
    visualizeGraph(graph, mst);

    return 0;
}
