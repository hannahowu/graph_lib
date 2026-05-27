/**
 * @file graph.h
 * @brief Public interface for a simple undirected graph library
 *        based on adjacency lists.
 *
 * This library provides an opaque graph_t type and a set of functions
 * to manage an undirected graph: adding/removing vertices and edges,
 * checking adjacency, and performing DFS/BFS traversals.
 *
 * @note All functions accepting graph_t* require a non-NULL pointer.
 * @note Memory allocated by graph_dfs() and graph_bfs() must be
 *       freed by the caller using graph_free_traversal().
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by library functions.
 */
typedef enum {
    GRAPH_OK                 = 0,  /**< Successful operation */
    GRAPH_ERR_NULL_PTR       = 1,  /**< NULL pointer argument */
    GRAPH_ERR_INVALID_SIZE   = 2,  /**< Invalid size (zero or too large) */
    GRAPH_ERR_ALLOC_FAILED   = 3,  /**< Memory allocation failure */
    GRAPH_ERR_VERTEX_EXISTS  = 4,  /**< Vertex already exists in the graph */
    GRAPH_ERR_VERTEX_NOT_FOUND = 5,/**< Vertex not found in the graph */
    GRAPH_ERR_EDGE_EXISTS    = 6,  /**< Edge already exists */
    GRAPH_ERR_EDGE_NOT_FOUND = 7,  /**< Edge not found */
    GRAPH_ERR_SELF_LOOP      = 8   /**< Self-loops (u == v) are not allowed */
} graph_error_t;

/**
 * @brief Opaque handle for the graph structure.
 */
typedef struct graph graph_t;

/**
 * @brief Creates an empty graph with initial capacity.
 * @param[in]  capacity Initial vertex capacity (must be > 0).
 * @param[out] err      Pointer to store error code. Can be NULL.
 * @return Pointer to the created graph, or NULL on error.
 */
graph_t* graph_create(size_t capacity, graph_error_t* err);

/**
 * @brief Destroys the graph and frees all associated memory.
 * @param[in] g Graph handle (safe to pass NULL).
 */
void graph_destroy(graph_t* g);

/**
 * @brief Adds a vertex to the graph.
 * @param g      Graph handle.
 * @param vertex Vertex identifier (integer).
 * @return GRAPH_OK or an error code.
 */
graph_error_t graph_add_vertex(graph_t* g, int vertex);

/**
 * @brief Checks if the graph contains a specific vertex.
 * @param g      Graph handle.
 * @param vertex Vertex identifier.
 * @return 1 if present, 0 if absent, -1 if g is NULL.
 */
int graph_has_vertex(const graph_t* g, int vertex);

/**
 * @brief Adds an undirected edge between two existing vertices.
 * @param g Graph handle.
 * @param u First vertex identifier.
 * @param v Second vertex identifier.
 * @return GRAPH_OK or an error code.
 */
graph_error_t graph_add_edge(graph_t* g, int u, int v);

/**
 * @brief Removes an edge between two vertices.
 * @param g Graph handle.
 * @param u First vertex identifier.
 * @param v Second vertex identifier.
 * @return GRAPH_OK or GRAPH_ERR_EDGE_NOT_FOUND.
 */
graph_error_t graph_remove_edge(graph_t* g, int u, int v);

/**
 * @brief Checks if two vertices are adjacent.
 * @param g Graph handle.
 * @param u First vertex identifier.
 * @param v Second vertex identifier.
 * @return 1 if adjacent, 0 if not (or vertices missing), -1 if g is NULL.
 */
int graph_are_adjacent(const graph_t* g, int u, int v);

/**
 * @brief Performs Depth-First Search (DFS) from a start vertex.
 *
 * Allocates an array of visited vertex identifiers.
 * The caller must free the array using graph_free_traversal().
 *
 * @param[in]  g           Graph handle.
 * @param[in]  start       Starting vertex identifier.
 * @param[out] out_vertices Pointer to store the allocated array.
 * @param[out] out_count   Pointer to store the number of visited vertices.
 * @return GRAPH_OK or an error code.
 */
graph_error_t graph_dfs(const graph_t* g, int start,
                        int** out_vertices, size_t* out_count);

/**
 * @brief Performs Breadth-First Search (BFS) from a start vertex.
 *
 * Allocates an array of visited vertex identifiers.
 * The caller must free the array using graph_free_traversal().
 *
 * @param[in]  g           Graph handle.
 * @param[in]  start       Starting vertex identifier.
 * @param[out] out_vertices Pointer to store the allocated array.
 * @param[out] out_count   Pointer to store the number of visited vertices.
 * @return GRAPH_OK or an error code.
 */
graph_error_t graph_bfs(const graph_t* g, int start,
                        int** out_vertices, size_t* out_count);

/**
 * @brief Frees the array allocated by graph_dfs() or graph_bfs().
 * @param arr Array pointer (safe to pass NULL).
 */
void graph_free_traversal(int* arr);

/**
 * @brief Returns the number of vertices in the graph.
 * @param g Graph handle.
 * @return Vertex count, or 0 if g is NULL.
 */
size_t graph_vertex_count(const graph_t* g);

/**
 * @brief Returns the number of edges in the graph.
 * @param g Graph handle.
 * @return Edge count, or 0 if g is NULL.
 */
size_t graph_edge_count(const graph_t* g);

/**
 * @brief Returns a human-readable string for an error code.
 * @param err Error code.
 * @return Static string describing the error.
 */
const char* graph_error_string(graph_error_t err);

#ifdef __cplusplus
}
#endif

#endif /* GRAPH_H */