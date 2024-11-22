CC = clang

# The -MMD flag automatically generates Makefile dependencies for source files, tracking
# changes in included headers to enable efficient recompilation.
# The -MP flag tells the compiler to add dummy targets for each dependency,
# thus avoiding make errors if header files are removed.
CFLAGS = -Wall -Wextra -Iinclude -MMD -MP
VALGRINDFLAGS = --leak-check=full --show-leak-kinds=all

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)

# patsubst (pattern substitution) replaces a given pattern in a list.
# $(patsubst <pattern_a>, <pattern_b>, <list>)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPENDS = $(OBJS:.o=.d)

TARGET = $(BIN_DIR)/prog

DEP_FILES = $(DEPENDS)

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

-include $(DEP_FILES)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

format:
	find . -name '*.c' -o -name '*.h' | xargs clang-format -i

debug: CFLAGS += -g -DDEBUG
debug: clean all

leaks: debug
	valgrind $(VALGRINDFLAGS) ./$(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean format debug run leaks