TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

TEST_SRC = $(wildcard tests/*.c)
TEST_BIN = $(patsubst tests/%.c, bin/tests/%, $(TEST_SRC))
CFLAGS += -Iinclude
LDFLAGS += -lcmocka

run: clean default
	./$(TARGET) -f ./mynewdb.db -n 
	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f $(TARGET)
	rm -f *.db
	rm -f bin/tests/*

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c -g $< -o $@ -Iinclude

bin/tests/%: tests/%.c
	gcc $(CFLAGS) -o $@ $< $(filter-out src/main.c, $(SRC)) $(LDFLAGS)

tests: $(TEST_BIN)

test: tests
	@for test_bin in $(TEST_BIN); do \
		echo "Running $$test_bin"; \
		$$test_bin || exit 1; \
		done
