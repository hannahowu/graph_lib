/**
 * @file graph.h
 * @brief Публичный интерфейс библиотеки простого неориентированного графа
 *        на основе списков смежности.
 *
 * Библиотека предоставляет тип graph_t (непрозрачная структура) и набор
 * функций для работы с неориентированным графом: добавление/удаление
 * вершин и рёбер, проверка смежности, обходы DFS и BFS.
 *
 * @note Все функции, принимающие graph_t*, требуют ненулевой указатель.
 * @note Память, выделенная функциями обхода (graph_dfs, graph_bfs), должна
 *       быть освобождена вызовом graph_free_traversal().
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Коды ошибок, возвращаемые функциями библиотеки.
 */
typedef enum {
    GRAPH_OK                 = 0,  /**< Успешное выполнение */
    GRAPH_ERR_NULL_PTR       = 1,  /**< Передан NULL-указатель */
    GRAPH_ERR_INVALID_SIZE   = 2,  /**< Некорректный размер (0 или слишком большой) */
    GRAPH_ERR_ALLOC_FAILED   = 3,  /**< Ошибка выделения памяти */
    GRAPH_ERR_VERTEX_EXISTS  = 4,  /**< Вершина уже существует */
    GRAPH_ERR_VERTEX_NOT_FOUND = 5,/**< Вершина не найдена */
    GRAPH_ERR_EDGE_EXISTS    = 6,  /**< Ребро уже существует */
    GRAPH_ERR_EDGE_NOT_FOUND = 7,  /**< Ребро не найдено */
    GRAPH_ERR_SELF_LOOP      = 8   /**< Петли (u == v) запрещены */
} graph_error_t;

/**
 * @brief Непрозрачный тип графа.
 */
typedef struct graph graph_t;

/**
 * @brief Создаёт пустой граф с начальной вместимостью.
 *
 * @param[in]  capacity  Начальная вместимость (максимальное число вершин до
 *                       первого расширения). Должна быть > 0.
 * @param[out] err       Указатель для кода ошибки. Может быть NULL.
 * @return Указатель на созданный граф или NULL при ошибке.
 */
graph_t* graph_create(size_t capacity, graph_error_t* err);

/**
 * @brief Освобождает все ресурсы, занятые графом.
 * @param[in] g Граф (может быть NULL — тогда ничего не делается).
 */
void graph_destroy(graph_t* g);

/**
 * @brief Добавляет вершину в граф.
 * @param g      Граф.
 * @param vertex Идентификатор вершины (целое число).
 * @return GRAPH_OK или код ошибки.
 */
graph_error_t graph_add_vertex(graph_t* g, int vertex);

/**
 * @brief Проверяет, содержит ли граф указанную вершину.
 * @param g      Граф.
 * @param vertex Идентификатор вершины.
 * @return 1 — вершина есть, 0 — нет, -1 — ошибка (NULL).
 */
int graph_has_vertex(const graph_t* g, int vertex);

/**
 * @brief Добавляет неориентированное ребро между двумя вершинами.
 *        Обе вершины должны уже существовать.
 * @param g Граф.
 * @param u Первая вершина.
 * @param v Вторая вершина.
 * @return GRAPH_OK или код ошибки.
 */
graph_error_t graph_add_edge(graph_t* g, int u, int v);

/**
 * @brief Удаляет ребро между двумя вершинами.
 * @param g Граф.
 * @param u Первая вершина.
 * @param v Вторая вершина.
 * @return GRAPH_OK или GRAPH_ERR_EDGE_NOT_FOUND.
 */
graph_error_t graph_remove_edge(graph_t* g, int u, int v);

/**
 * @brief Проверяет, смежны ли две вершины.
 * @param g Граф.
 * @param u Первая вершина.
 * @param v Вторая вершина.
 * @return 1 — смежны, 0 — не смежны (или вершины не существуют), -1 — ошибка.
 */
int graph_are_adjacent(const graph_t* g, int u, int v);

/**
 * @brief Обход графа в глубину (DFS) из заданной стартовой вершины.
 *
 * Выделяет массив идентификаторов посещённых вершин в порядке обхода.
 * Память должна быть освобождена graph_free_traversal().
 *
 * @param[in]  g           Граф.
 * @param[in]  start       Стартовая вершина.
 * @param[out] out_vertices Указатель на массив вершин (заполняется функцией).
 * @param[out] out_count   Количество посещённых вершин.
 * @return GRAPH_OK или код ошибки.
 */
graph_error_t graph_dfs(const graph_t* g, int start,
                        int** out_vertices, size_t* out_count);

/**
 * @brief Обход графа в ширину (BFS) из заданной стартовой вершины.
 *
 * Выделяет массив идентификаторов посещённых вершин в порядке обхода.
 * Память должна быть освобождена graph_free_traversal().
 *
 * @param[in]  g           Граф.
 * @param[in]  start       Стартовая вершина.
 * @param[out] out_vertices Указатель на массив вершин (заполняется функцией).
 * @param[out] out_count   Количество посещённых вершин.
 * @return GRAPH_OK или код ошибки.
 */
graph_error_t graph_bfs(const graph_t* g, int start,
                        int** out_vertices, size_t* out_count);

/**
 * @brief Освобождает массив, выделенный graph_dfs() / graph_bfs().
 * @param arr Массив (может быть NULL).
 */
void graph_free_traversal(int* arr);

/**
 * @brief Возвращает число вершин в графе.
 * @param g Граф.
 * @return Число вершин или 0 при g == NULL.
 */
size_t graph_vertex_count(const graph_t* g);

/**
 * @brief Возвращает число рёбер в графе.
 * @param g Граф.
 * @return Число рёбер или 0 при g == NULL.
 */
size_t graph_edge_count(const graph_t* g);

/**
 * @brief Возвращает строковое представление кода ошибки.
 * @param err Код ошибки.
 * @return Строковая константа (статическая память, освобождать не нужно).
 */
const char* graph_error_string(graph_error_t err);

#ifdef __cplusplus
}
#endif

#endif /* GRAPH_H */