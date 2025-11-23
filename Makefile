CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC_MAIN = \
    src/arg_parse.c \
    src/network.c \
    src/output.c \
    src/scan_controller.c \
    src/main.c

SRC_TEST = \
    src/arg_parse.c \
    src/output.c \
    src/scan_controller.c

MAIN_BIN = portscan

TEST_BINS = \
    tests/test_argparse \
    tests/test_output \
    tests/test_scan_controller

all: $(MAIN_BIN)

$(MAIN_BIN): $(SRC_MAIN)
	$(CC) $(CFLAGS) $^ -o $@

tests: $(TEST_BINS)

tests/test_argparse: tests/test_arg_parse.c $(SRC_TEST)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_output: tests/test_output.c $(SRC_TEST)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_scan_controller: tests/test_scan_controller.c $(SRC_TEST)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(MAIN_BIN) $(TEST_BINS)
