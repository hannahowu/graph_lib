#include <stdio.h>
#include "graph.h"

int main(void) {
    graph_error_t err = GRAPH_OK;
    graph_t* g = graph_create(8, &err);
    if (!g) {
        fprintf(stderr, "graph_create failed: %s\n", graph_error_string(err));
        return 1;
    }

    for (int v = 1; v <= 6; ++v) {
        err = graph_add_vertex(g, v);
        if (err != GRAPH_OK) {
            fprintf(stderr, "add_vertex(%d): %s\n", v, graph_error_string(err));
        }
    }

    int edges[][2] = {{1,2},{1,3},{2,4},{3,4},{4,5},{5,6}};
    for (size_t i = 0; i < sizeof(edges)/sizeof(edges[0]); ++i) {
        err = graph_add_edge(g, edges[i][0], edges[i][1]);
        if (err != GRAPH_OK) {
            fprintf(stderr, "add_edge(%d,%d): %s\n",
                    edges[i][0], edges[i][1], graph_error_string(err));
        }
    }

    printf("Vertices: %zu, Edges: %zu\n",
           graph_vertex_count(g), graph_edge_count(g));
    printf("1 and 4 adjacent: %d\n", graph_are_adjacent(g, 1, 4));
    printf("1 and 5 adjacent: %d\n", graph_are_adjacent(g, 1, 5));

    int* path = NULL;
    size_t n = 0;

    if (graph_dfs(g, 1, &path, &n) == GRAPH_OK) {
        printf("DFS from 1:");
        for (size_t i = 0; i < n; ++i) printf(" %d", path[i]);
        printf("\n");
        graph_free_traversal(path);
    }

    if (graph_bfs(g, 1, &path, &n) == GRAPH_OK) {
        printf("BFS from 1:");
        for (size_t i = 0; i < n; ++i) printf(" %d", path[i]);
        printf("\n");
        graph_free_traversal(path);
    }

    graph_destroy(g);
    return 0;
}