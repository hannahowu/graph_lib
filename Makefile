CC       ?= gcc
CFLAGS   := -Wall -Wextra -Wpedantic -std=c11 -fPIC -Iinclude -O2 -g
SANFLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer -O1 -g

SRC_LIB  := src/graph.c
SRC_APP  := src/app.c
SRC_TEST := tests/test_graph.c

BUILD    := build
LIB      := $(BUILD)/libgraph.dll
APP      := $(BUILD)/app.exe
TEST_BIN := $(BUILD)/test_graph.exe
SAN_BIN  := $(BUILD)/test_graph_san.exe

.PHONY: all clean shared app run test py-test \
        syntax analyze sanitize docs-html docs-pdf

all: shared app test

shared: $(LIB)

app: $(APP)

$(BUILD):
	@mkdir -p $(BUILD)

$(LIB): $(SRC_LIB) include/graph.h | $(BUILD)
	$(CC) $(CFLAGS) -shared -o $@ $(SRC_LIB)

$(APP): $(SRC_APP) $(LIB) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(SRC_APP) -L$(BUILD) -lgraph

$(TEST_BIN): $(SRC_TEST) $(LIB) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(SRC_TEST) -L$(BUILD) -lgraph

run: app
	./$(APP)

test: $(TEST_BIN)
	./$(TEST_BIN)

py-test: shared
	python3 tests/test_graph.py

syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(SRC_LIB) $(SRC_APP) $(SRC_TEST)

analyze: | reports
	$(CC) $(CFLAGS) -fanalyzer $(SRC_LIB) $(SRC_APP) 2> reports/gcc-analyzer.log || true
	-cppcheck --enable=all --inconclusive --force -Iinclude \
	    $(SRC_LIB) $(SRC_APP) 2> reports/cppcheck.log
	@echo "=== GCC analyzer (head) ==="
	@head -n 40 reports/gcc-analyzer.log || true
	@echo "=== cppcheck (head) ==="
	@head -n 40 reports/cppcheck.log || true

reports:
	@mkdir -p reports

sanitize: | $(BUILD)
	@echo "=== Запуск тестов в режиме строгой проверки (Windows: ASan/UBSan недоступны в MinGW) ==="
	$(CC) $(CFLAGS) -Werror -D_FORTIFY_SOURCE=2 -o $(SAN_BIN) $(SRC_TEST) $(SRC_LIB)
	./$(SAN_BIN)

docs-html: Doxyfile
	doxygen Doxyfile

docs-pdf: docs-html
	$(MAKE) -C docs/latex

clean:
	rm -rf $(BUILD) docs reports
	find . -type d -name __pycache__ -exec rm -rf {} + 2>/dev/null || true