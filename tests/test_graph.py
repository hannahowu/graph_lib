#!/usr/bin/env python3
"""Тесты библиотеки graph через ctypes."""

import ctypes
import os
import sys
import unittest

LIB_NAME = "libgraph.dll"
LIB_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "build", LIB_NAME))

class GraphError:
    OK = 0
    NULL_PTR = 1
    INVALID_SIZE = 2
    ALLOC_FAILED = 3
    VERTEX_EXISTS = 4
    VERTEX_NOT_FOUND = 5
    EDGE_EXISTS = 6
    EDGE_NOT_FOUND = 7
    SELF_LOOP = 8

def load_lib():
    lib = ctypes.CDLL(LIB_PATH)

    lib.graph_create.argtypes = [ctypes.c_size_t, ctypes.POINTER(ctypes.c_int)]
    lib.graph_create.restype = ctypes.c_void_p

    lib.graph_destroy.argtypes = [ctypes.c_void_p]
    lib.graph_destroy.restype = None

    lib.graph_add_vertex.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.graph_add_vertex.restype = ctypes.c_int

    lib.graph_has_vertex.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.graph_has_vertex.restype = ctypes.c_int

    lib.graph_add_edge.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
    lib.graph_add_edge.restype = ctypes.c_int

    lib.graph_remove_edge.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
    lib.graph_remove_edge.restype = ctypes.c_int

    lib.graph_are_adjacent.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
    lib.graph_are_adjacent.restype = ctypes.c_int

    lib.graph_dfs.argtypes = [
        ctypes.c_void_p, ctypes.c_int,
        ctypes.POINTER(ctypes.POINTER(ctypes.c_int)),
        ctypes.POINTER(ctypes.c_size_t),
    ]
    lib.graph_dfs.restype = ctypes.c_int

    lib.graph_bfs.argtypes = lib.graph_dfs.argtypes
    lib.graph_bfs.restype = ctypes.c_int

    lib.graph_free_traversal.argtypes = [ctypes.POINTER(ctypes.c_int)]
    lib.graph_free_traversal.restype = None

    lib.graph_vertex_count.argtypes = [ctypes.c_void_p]
    lib.graph_vertex_count.restype = ctypes.c_size_t

    lib.graph_edge_count.argtypes = [ctypes.c_void_p]
    lib.graph_edge_count.restype = ctypes.c_size_t

    return lib


class TestGraph(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.lib = load_lib()

    def _create(self, cap=8):
        err = ctypes.c_int(0)
        g = self.lib.graph_create(cap, ctypes.byref(err))
        self.assertEqual(err.value, GraphError.OK)
        self.assertIsNotNone(g)
        return g

    def test_create_and_add(self):
        g = self._create()
        self.assertEqual(self.lib.graph_add_vertex(g, 1), GraphError.OK)
        self.assertEqual(self.lib.graph_add_vertex(g, 1), GraphError.VERTEX_EXISTS)
        self.assertEqual(self.lib.graph_has_vertex(g, 1), 1)
        self.assertEqual(self.lib.graph_vertex_count(g), 1)
        self.lib.graph_destroy(g)

    def test_edges_and_adjacency(self):
        g = self._create()
        for v in (1, 2, 3):
            self.lib.graph_add_vertex(g, v)
        self.assertEqual(self.lib.graph_add_edge(g, 1, 2), GraphError.OK)
        self.assertEqual(self.lib.graph_add_edge(g, 1, 1), GraphError.SELF_LOOP)
        self.assertEqual(self.lib.graph_are_adjacent(g, 1, 2), 1)
        self.assertEqual(self.lib.graph_are_adjacent(g, 2, 1), 1)
        self.assertEqual(self.lib.graph_remove_edge(g, 1, 2), GraphError.OK)
        self.assertEqual(self.lib.graph_are_adjacent(g, 1, 2), 0)
        self.lib.graph_destroy(g)

    def test_dfs(self):
        g = self._create()
        for v in range(1, 5):
            self.lib.graph_add_vertex(g, v)
        self.lib.graph_add_edge(g, 1, 2)
        self.lib.graph_add_edge(g, 1, 3)
        self.lib.graph_add_edge(g, 2, 4)

        out = ctypes.POINTER(ctypes.c_int)()
        count = ctypes.c_size_t(0)
        rc = self.lib.graph_dfs(g, 1, ctypes.byref(out), ctypes.byref(count))
        self.assertEqual(rc, GraphError.OK)
        self.assertEqual(count.value, 4)
        visited = [out[i] for i in range(count.value)]
        self.assertEqual(visited[0], 1)
        self.assertEqual(set(visited), {1, 2, 3, 4})
        self.lib.graph_free_traversal(out)
        self.lib.graph_destroy(g)

    def test_bfs(self):
        g = self._create()
        for v in range(1, 5):
            self.lib.graph_add_vertex(g, v)
        self.lib.graph_add_edge(g, 1, 2)
        self.lib.graph_add_edge(g, 1, 3)
        self.lib.graph_add_edge(g, 2, 4)

        out = ctypes.POINTER(ctypes.c_int)()
        count = ctypes.c_size_t(0)
        rc = self.lib.graph_bfs(g, 1, ctypes.byref(out), ctypes.byref(count))
        self.assertEqual(rc, GraphError.OK)
        self.assertEqual(count.value, 4)
        visited = [out[i] for i in range(count.value)]
        self.assertEqual(visited[0], 1)
        self.lib.graph_free_traversal(out)
        self.lib.graph_destroy(g)

    def test_null_safety(self):
        self.assertEqual(self.lib.graph_add_vertex(None, 1), GraphError.NULL_PTR)
        self.assertEqual(self.lib.graph_has_vertex(None, 1), -1)


if __name__ == "__main__":
    unittest.main(verbosity=2)