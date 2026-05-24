#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "graph.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { \
    printf("  %-50s ", #fn); \
    tests_run++; \
    if (fn()) { tests_passed++; puts("[PASS]"); } \
    else      { puts("[FAIL]"); } \
} while (0)

static int test_create_destroy(void) {
    graph_error_t err;
    graph_t* g = graph_create(4, &err);
    if (!g || err != GRAPH_OK) return 0;
    graph_destroy(g);
    graph_destroy(NULL); /* must not crash */
    return 1;
}

static int test_create_invalid(void) {
    graph_error_t err;
    graph_t* g = graph_create(0, &err);
    return (g == NULL && err == GRAPH_ERR_INVALID_SIZE);
}

static int test_add_vertex(void) {
    graph_error_t err;
    graph_t* g = graph_create(2, &err);
    assert(graph_add_vertex(g, 10) == GRAPH_OK);
    assert(graph_add_vertex(g, 20) == GRAPH_OK);
    assert(graph_add_vertex(g, 10) == GRAPH_ERR_VERTEX_EXISTS);
    assert(graph_has_vertex(g, 10) == 1);
    assert(graph_has_vertex(g, 99) == 0);
    assert(graph_vertex_count(g) == 2);
    graph_destroy(g);
    return 1;
}

static int test_add_remove_edge(void) {
    graph_error_t err;
    graph_t* g = graph_create(4, &err);
    graph_add_vertex(g, 1); graph_add_vertex(g, 2); graph_add_vertex(g, 3);

    assert(graph_add_edge(g, 1, 2) == GRAPH_OK);
    assert(graph_add_edge(g, 1, 2) == GRAPH_ERR_EDGE_EXISTS);
    assert(graph_add_edge(g, 1, 1) == GRAPH_ERR_SELF_LOOP);
    assert(graph_add_edge(g, 1, 9) == GRAPH_ERR_VERTEX_NOT_FOUND);
    assert(graph_are_adjacent(g, 1, 2) == 1);
    assert(graph_are_adjacent(g, 2, 1) == 1);  /* undirected */
    assert(graph_edge_count(g) == 1);

    assert(graph_remove_edge(g, 1, 2) == GRAPH_OK);
    assert(graph_remove_edge(g, 1, 2) == GRAPH_ERR_EDGE_NOT_FOUND);
    assert(graph_are_adjacent(g, 1, 2) == 0);
    assert(graph_edge_count(g) == 0);

    graph_destroy(g);
    return 1;
}

static int test_null_safety(void) {
    assert(graph_add_vertex(NULL, 1) == GRAPH_ERR_NULL_PTR);
    assert(graph_has_vertex(NULL, 1) == -1);
    assert(graph_add_edge(NULL, 1, 2) == GRAPH_ERR_NULL_PTR);
    assert(graph_are_adjacent(NULL, 1, 2) == -1);
    assert(graph_vertex_count(NULL) == 0);
    assert(graph_edge_count(NULL) == 0);

    int* out = NULL; size_t n = 0;
    assert(graph_dfs(NULL, 1, &out, &n) == GRAPH_ERR_NULL_PTR);
    assert(graph_bfs(NULL, 1, &out, &n) == GRAPH_ERR_NULL_PTR);
    return 1;
}

static int test_dfs(void) {
    graph_error_t err;
    graph_t* g = graph_create(8, &err);
    for (int v = 1; v <= 4; ++v) graph_add_vertex(g, v);
    graph_add_edge(g, 1, 2);
    graph_add_edge(g, 1, 3);
    graph_add_edge(g, 2, 4);

    int* path = NULL; size_t n = 0;
    assert(graph_dfs(g, 1, &path, &n) == GRAPH_OK);
    assert(n == 4);
    assert(path[0] == 1);
    graph_free_traversal(path);

    /* несуществующий старт */
    assert(graph_dfs(g, 99, &path, &n) == GRAPH_ERR_VERTEX_NOT_FOUND);

    graph_destroy(g);
    return 1;
}

static int test_bfs(void) {
    graph_error_t err;
    graph_t* g = graph_create(8, &err);
    for (int v = 1; v <= 5; ++v) graph_add_vertex(g, v);
    graph_add_edge(g, 1, 2);
    graph_add_edge(g, 1, 3);
    graph_add_edge(g, 2, 4);
    graph_add_edge(g, 3, 5);

    int* path = NULL; size_t n = 0;
    assert(graph_bfs(g, 1, &path, &n) == GRAPH_OK);
    assert(n == 5);
    assert(path[0] == 1);
    /* BFS: уровень 0 = {1}, уровень 1 = {2,3}, уровень 2 = {4,5} */
    graph_free_traversal(path);

    graph_destroy(g);
    return 1;
}

static int test_disconnected(void) {
    graph_error_t err;
    graph_t* g = graph_create(4, &err);
    graph_add_vertex(g, 1);
    graph_add_vertex(g, 2);
    graph_add_vertex(g, 3);
    graph_add_edge(g, 1, 2);

    int* path = NULL; size_t n = 0;
    assert(graph_dfs(g, 1, &path, &n) == GRAPH_OK);
    assert(n == 2); /* 3 не достижима из 1 */
    graph_free_traversal(path);
    graph_destroy(g);
    return 1;
}

static int test_capacity_growth(void) {
    graph_error_t err;
    graph_t* g = graph_create(1, &err);
    for (int i = 0; i < 100; ++i) {
        assert(graph_add_vertex(g, i) == GRAPH_OK);
    }
    assert(graph_vertex_count(g) == 100);
    graph_destroy(g);
    return 1;
}

int main(void) {
    puts("=== Graph library tests ===");
    RUN_TEST(test_create_destroy);
    RUN_TEST(test_create_invalid);
    RUN_TEST(test_add_vertex);
    RUN_TEST(test_add_remove_edge);
    RUN_TEST(test_null_safety);
    RUN_TEST(test_dfs);
    RUN_TEST(test_bfs);
    RUN_TEST(test_disconnected);
    RUN_TEST(test_capacity_growth);
    printf("\nPassed: %d / %d\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}