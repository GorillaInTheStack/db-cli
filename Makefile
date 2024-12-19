CLI_TARGET = bin/db_cli
SERVER_TARGET = bin/db_server
CLIENT_TARGET = bin/db_client
DIRS = bin obj obj/common obj/network obj/cli obj/server obj/client bin/tests

COMMON = $(wildcard src/common/*.c)
COMMON_OBJS = $(patsubst src/common/%.c, obj/common/%.o, $(COMMON))

NETWORK = $(wildcard src/network/*.c)
NETWORK_OBJS = $(patsubst src/network/%.c, obj/network/%.o, $(NETWORK))

CLI = $(wildcard src/cli/*.c)
CLI_OBJS = $(patsubst src/cli/%.c, obj/cli/%.o, $(CLI)) $(COMMON_OBJS)

SERVER = $(wildcard src/server/*.c)
SERVER_OBJS = $(patsubst src/server/%.c, obj/server/%.o, $(SERVER)) $(COMMON_OBJS) $(NETWORK_OBJS)

CLIENT = $(wildcard src/client/*.c)
CLIENT_OBJS = $(patsubst src/client/%.c, obj/client/%.o, $(CLIENT)) $(COMMON_OBJS) $(NETWORK_OBJS)

TESTS = $(wildcard tests/*.c)
TEST_BINS = $(patsubst tests/%.c, bin/tests/%, $(TESTS))

CFLAGS += -Iinclude
LDFLAGS += -lcmocka

.PHONY: dirs clean

all: $(CLI_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET)
server: $(SERVER_TARGET)
client: $(CLIENT_TARGET)
cli: $(CLI_TARGET)

dirs:
	mkdir -p $(DIRS)

$(CLI_TARGET): $(CLI_OBJS) | dirs
	gcc -o $@ $^

$(SERVER_TARGET): $(SERVER_OBJS) | dirs
	gcc -o $@ $^

$(CLIENT_TARGET): $(CLIENT_OBJS) | dirs
	gcc -o $@ $^

obj/common/%.o: src/common/%.c
	gcc -c -g $< -o $@ $(CFLAGS)

obj/network/%.o: src/network/%.c
	gcc -c -g $< -o $@ $(CFLAGS)

obj/cli/%.o: src/cli/%.c
	gcc -c -g $< -o $@ $(CFLAGS)

obj/server/%.o: src/server/%.c
	gcc -c -g $< -o $@ $(CFLAGS)

obj/client/%.o: src/client/%.c
	gcc -c -g $< -o $@ $(CFLAGS)

bin/tests/%: tests/%.c | dirs
	gcc $(CFLAGS) -o $@ $< obj/cli/main_cli_logic.o $(COMMON_OBJS) $(LDFLAGS)

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
	rm -rf $(CLI_OBJS) $(SERVER_OBJS) $(CLIENT_OBJS) $(TEST_BINS) $(CLI_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET) *.db
