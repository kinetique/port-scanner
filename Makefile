CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Iinclude
CXXFLAGS = -std=c++17 -Iinclude

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
    tests/test_scan_controller \
    tests/test_network \
    tests/test_main


# --- Main program ---
all: $(MAIN_BIN)

$(MAIN_BIN): $(SRC_MAIN)
	$(CC) $(CFLAGS) $^ -o $@

# --- Tests ---
tests: $(TEST_BINS)

# arg_parse test
tests/test_argparse: tests/test_arg_parse.cpp src/arg_parse.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lpthread -o $@

# output test
tests/test_output: tests/test_output.cpp src/arg_parse.o src/output.o 
	$(CXX) $(CXXFLAGS) $^ -lgtest -lpthread -o $@

# scan_controller test
tests/test_scan_controller: tests/test_scan_controller.cpp src/arg_parse.o src/output.o src/scan_controller.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lpthread -o $@

# network test
tests/test_network: tests/test_network.cpp src/network.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lpthread -o $@

# main test
tests/test_main: tests/test_main.cpp src/arg_parse.o src/output.o src/main_test.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lpthread -o $@

# --- Compile object files for tests ---
src/arg_parse.o: src/arg_parse.c
	$(CC) $(CFLAGS) -c $< -o $@

src/output.o: src/output.c
	$(CC) $(CFLAGS) -c $< -o $@

src/scan_controller.o: src/scan_controller.c
	$(CC) $(CFLAGS) -c $< -o $@

src/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

src/main_test.o: src/main.c
	$(CC) $(CFLAGS) -DTESTING -c $< -o $@

clean:
	rm -f $(MAIN_BIN) $(TEST_BINS) src/*.o