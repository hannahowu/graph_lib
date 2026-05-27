# Graph Library

Simple undirected graph library in C (adjacency lists).
Laboratory Work No. 10, Variant 11.

## Features
- Add/remove vertices and edges
- Adjacency checking
- DFS and BFS traversals
- Error codes and NULL-safe API

## Build & Test
```sh
make all        # Build library, app, and C-tests
make run        # Run demo application
make test       # Run C-tests
make py-test    # Run Python ctypes tests
make sanitize   # Run strict safety checks
make analyze    # Run static analysis
make docs-html  # Generate HTML documentation