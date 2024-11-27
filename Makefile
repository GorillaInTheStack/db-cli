CLI_TARGET = bin/db_cli
SERVER_TARGET = bin/db_server
CLIENT_TARGET = bin/db_client

COMMON_OBJS = obj/context.o obj/file.o obj/networking.o obj/parse.o obj/utils.o
CLI_OBJS = obj/main_cli.o obj/main_cli_logic.o $(COMMON_OBJS)
SERVER_OBJS = obj/main_server.o obj/main_server_logic.o $(COMMON_OBJS)
CLIENT_OBJS = obj/main_client.o obj/main_client_logic.o $(COMMON_OBJS)

TESTS = $(wildcard tests/*.c)
TEST_BINS = $(patsubst tests/%.c, bin/tests/%, $(TESTS))

CFLAGS += -Iinclude
LDFLAGS += -lcmocka

all: $(CLI_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET)

$(CLI_TARGET): $(CLI_OBJS)
	mkdir -p bin
	gcc -o $@ $^

$(SERVER_TARGET): $(SERVER_OBJS)
	mkdir -p bin
	gcc -o $@ $^

$(CLIENT_TARGET): $(CLIENT_OBJS)
	mkdir -p bin
	gcc -o $@ $^

obj/%.o: src/%.c
	mkdir -p obj
	gcc -c -g $< -o $@ $(CFLAGS)

bin/tests/%: tests/%.c
	mkdir -p bin/tests
	gcc $(CFLAGS) -o $@ $< obj/main_cli_logic.o $(COMMON_OBJS) $(LDFLAGS)

tests: $(TEST_BINS)

test: tests
	@total=0; \
	passed=0; \
	failed=0; \
	for test_bin in $(TEST_BINS); do \
		echo "############################## Running $$test_bin ##############################"; \
		if $$test_bin; then \
			echo "############################## $$test_bin PASSED ##############################"; \
			passed=$$((passed + 1)); \
		else \
			echo "############################## $$test_bin FAILED ##############################"; \
			failed=$$((failed + 1)); \
		fi; \
		total=$$((total + 1)); \
	done; \
	echo "############################## SUMMARY ##############################"; \
	echo "Total Tests Files: $$total"; \
	echo "Passed: $$passed"; \
	echo "Failed: $$failed"; \
	if [ $$failed -ne 0 ]; then exit 1; fi

clean:
	rm -rf obj/*.o bin/* *.db
