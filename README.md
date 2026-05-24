# Graph Library

Simple undirected graph library in C (adjacency lists).
Laboratory work No. 10, variant 11.

## Features
- Add/remove vertices and edges
- Adjacency check
- DFS and BFS traversals
- Error codes, NULL-safe API

## Build
make all
make test
make py-test
make sanitize
make analyze
make docs-html

## Python usage
See tests/test_graph.py — the library is loaded via ctypes.CDLL.