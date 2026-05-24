#include "graph.h"
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Внутренние структуры (скрыты от пользователя)                      */
/* ------------------------------------------------------------------ */

typedef struct {
    int     vertex;
    int*    neighbors;
    size_t  neighbor_count;
    size_t  neighbor_capacity;
} vertex_entry_t;

struct graph {
    vertex_entry_t* vertices;
    size_t          vertex_count;
    size_t          vertex_capacity;
    size_t          edge_count;
};

/* ------------------------------------------------------------------ */
/* Вспомогательные функции (внутренние, static)                       */
/* ------------------------------------------------------------------ */

static int find_vertex_index(const graph_t* g, int vertex) {
    for (size_t i = 0; i < g->vertex_count; ++i) {
        if (g->vertices[i].vertex == vertex) {
            return (int)i;
        }
    }
    return -1;
}

static int add_neighbor(vertex_entry_t* v, int neighbor) {
    if (v->neighbor_count == v->neighbor_capacity) {
        size_t new_cap = v->neighbor_capacity == 0 ? 4 : v->neighbor_capacity * 2;
        int* tmp = (int*)realloc(v->neighbors, new_cap * sizeof(int));
        if (!tmp) return 0;
        v->neighbors = tmp;
        v->neighbor_capacity = new_cap;
    }
    v->neighbors[v->neighbor_count++] = neighbor;
    return 1;
}

static int remove_neighbor(vertex_entry_t* v, int neighbor) {
    for (size_t i = 0; i < v->neighbor_count; ++i) {
        if (v->neighbors[i] == neighbor) {
            v->neighbors[i] = v->neighbors[v->neighbor_count - 1];
            v->neighbor_count--;
            return 1;
        }
    }
    return 0;
}

static int has_neighbor(const vertex_entry_t* v, int neighbor) {
    for (size_t i = 0; i < v->neighbor_count; ++i) {
        if (v->neighbors[i] == neighbor) return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Публичный API                                                      */
/* ------------------------------------------------------------------ */

graph_t* graph_create(size_t capacity, graph_error_t* err) {
    if (capacity == 0) {
        if (err) *err = GRAPH_ERR_INVALID_SIZE;
        return NULL;
    }
    graph_t* g = (graph_t*)calloc(1, sizeof(graph_t));
    if (!g) {
        if (err) *err = GRAPH_ERR_ALLOC_FAILED;
        return NULL;
    }
    g->vertices = (vertex_entry_t*)calloc(capacity, sizeof(vertex_entry_t));
    if (!g->vertices) {
        free(g);
        if (err) *err = GRAPH_ERR_ALLOC_FAILED;
        return NULL;
    }
    g->vertex_capacity = capacity;
    if (err) *err = GRAPH_OK;
    return g;
}

void graph_destroy(graph_t* g) {
    if (!g) return;
    for (size_t i = 0; i < g->vertex_count; ++i) {
        free(g->vertices[i].neighbors);
    }
    free(g->vertices);
    free(g);
}

graph_error_t graph_add_vertex(graph_t* g, int vertex) {
    if (!g) return GRAPH_ERR_NULL_PTR;
    if (find_vertex_index(g, vertex) >= 0) return GRAPH_ERR_VERTEX_EXISTS;

    if (g->vertex_count == g->vertex_capacity) {
        size_t new_cap = g->vertex_capacity * 2;
        vertex_entry_t* tmp = (vertex_entry_t*)realloc(
            g->vertices, new_cap * sizeof(vertex_entry_t));
        if (!tmp) return GRAPH_ERR_ALLOC_FAILED;
        memset(tmp + g->vertex_capacity, 0,
               (new_cap - g->vertex_capacity) * sizeof(vertex_entry_t));
        g->vertices = tmp;
        g->vertex_capacity = new_cap;
    }

    vertex_entry_t* v = &g->vertices[g->vertex_count++];
    v->vertex = vertex;
    v->neighbors = NULL;
    v->neighbor_count = 0;
    v->neighbor_capacity = 0;
    return GRAPH_OK;
}

int graph_has_vertex(const graph_t* g, int vertex) {
    if (!g) return -1;
    return find_vertex_index(g, vertex) >= 0 ? 1 : 0;
}

graph_error_t graph_add_edge(graph_t* g, int u, int v) {
    if (!g) return GRAPH_ERR_NULL_PTR;
    if (u == v) return GRAPH_ERR_SELF_LOOP;

    int ui = find_vertex_index(g, u);
    int vi = find_vertex_index(g, v);
    if (ui < 0 || vi < 0) return GRAPH_ERR_VERTEX_NOT_FOUND;

    vertex_entry_t* eu = &g->vertices[ui];
    vertex_entry_t* ev = &g->vertices[vi];

    if (has_neighbor(eu, v)) return GRAPH_ERR_EDGE_EXISTS;

    if (!add_neighbor(eu, v)) return GRAPH_ERR_ALLOC_FAILED;
    if (!add_neighbor(ev, u)) {
        remove_neighbor(eu, v);
        return GRAPH_ERR_ALLOC_FAILED;
    }
    g->edge_count++;
    return GRAPH_OK;
}

graph_error_t graph_remove_edge(graph_t* g, int u, int v) {
    if (!g) return GRAPH_ERR_NULL_PTR;
    int ui = find_vertex_index(g, u);
    int vi = find_vertex_index(g, v);
    if (ui < 0 || vi < 0) return GRAPH_ERR_VERTEX_NOT_FOUND;

    vertex_entry_t* eu = &g->vertices[ui];
    vertex_entry_t* ev = &g->vertices[vi];

    if (!remove_neighbor(eu, v) || !remove_neighbor(ev, u)) {
        return GRAPH_ERR_EDGE_NOT_FOUND;
    }
    g->edge_count--;
    return GRAPH_OK;
}

int graph_are_adjacent(const graph_t* g, int u, int v) {
    if (!g) return -1;
    int ui = find_vertex_index(g, u);
    if (ui < 0) return 0;
    return has_neighbor(&g->vertices[ui], v) ? 1 : 0;
}

/* Рекурсивный DFS (внутренний) */
static void dfs_visit(const graph_t* g, int idx, int* visited,
                      int* out, size_t* out_count) {
    visited[idx] = 1;
    out[(*out_count)++] = g->vertices[idx].vertex;
    const vertex_entry_t* v = &g->vertices[idx];
    for (size_t i = 0; i < v->neighbor_count; ++i) {
        int ni = find_vertex_index(g, v->neighbors[i]);
        if (ni >= 0 && !visited[ni]) {
            dfs_visit(g, ni, visited, out, out_count);
        }
    }
}

graph_error_t graph_dfs(const graph_t* g, int start,
                        int** out_vertices, size_t* out_count) {
    if (!g || !out_vertices || !out_count) return GRAPH_ERR_NULL_PTR;
    int si = find_vertex_index(g, start);
    if (si < 0) return GRAPH_ERR_VERTEX_NOT_FOUND;

    int* visited = (int*)calloc(g->vertex_count, sizeof(int));
    int* result  = (int*)malloc(g->vertex_count * sizeof(int));
    if (!visited || !result) {
        free(visited); free(result);
        return GRAPH_ERR_ALLOC_FAILED;
    }

    *out_count = 0;
    dfs_visit(g, si, visited, result, out_count);
    free(visited);
    *out_vertices = result;
    return GRAPH_OK;
}

graph_error_t graph_bfs(const graph_t* g, int start,
                        int** out_vertices, size_t* out_count) {
    if (!g || !out_vertices || !out_count) return GRAPH_ERR_NULL_PTR;
    int si = find_vertex_index(g, start);
    if (si < 0) return GRAPH_ERR_VERTEX_NOT_FOUND;

    int* visited = (int*)calloc(g->vertex_count, sizeof(int));
    int* result  = (int*)malloc(g->vertex_count * sizeof(int));
    size_t* queue = (size_t*)malloc(g->vertex_count * sizeof(size_t));
    if (!visited || !result || !queue) {
        free(visited); free(result); free(queue);
        return GRAPH_ERR_ALLOC_FAILED;
    }

    size_t head = 0, tail = 0;
    visited[si] = 1;
    queue[tail++] = (size_t)si;

    *out_count = 0;
    while (head < tail) {
        size_t cur = queue[head++];
        result[(*out_count)++] = g->vertices[cur].vertex;
        const vertex_entry_t* v = &g->vertices[cur];
        for (size_t i = 0; i < v->neighbor_count; ++i) {
            int ni = find_vertex_index(g, v->neighbors[i]);
            if (ni >= 0 && !visited[ni]) {
                visited[ni] = 1;
                queue[tail++] = (size_t)ni;
            }
        }
    }

    free(visited);
    free(queue);
    *out_vertices = result;
    return GRAPH_OK;
}

void graph_free_traversal(int* arr) {
    free(arr);
}

size_t graph_vertex_count(const graph_t* g) {
    return g ? g->vertex_count : 0;
}

size_t graph_edge_count(const graph_t* g) {
    return g ? g->edge_count : 0;
}

const char* graph_error_string(graph_error_t err) {
    switch (err) {
        case GRAPH_OK:                   return "OK";
        case GRAPH_ERR_NULL_PTR:         return "NULL pointer";
        case GRAPH_ERR_INVALID_SIZE:     return "Invalid size";
        case GRAPH_ERR_ALLOC_FAILED:     return "Allocation failed";
        case GRAPH_ERR_VERTEX_EXISTS:    return "Vertex already exists";
        case GRAPH_ERR_VERTEX_NOT_FOUND: return "Vertex not found";
        case GRAPH_ERR_EDGE_EXISTS:      return "Edge already exists";
        case GRAPH_ERR_EDGE_NOT_FOUND:   return "Edge not found";
        case GRAPH_ERR_SELF_LOOP:        return "Self-loop not allowed";
    }
    return "Unknown error";
}