#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>

extern "C" {
#include "output.h"
}

/* Helper: capture stdout */
static std::string capture_output(void (*func)(void*), void *arg) {
    // Save original stdout
    fflush(stdout);
    int original_stdout = dup(fileno(stdout));

    // Create temporary file
    FILE *tmp = tmpfile();
    dup2(fileno(tmp), fileno(stdout));

    // Call function
    func(arg);

    // Flush and rewind
    fflush(stdout);
    fseek(tmp, 0, SEEK_SET);

    // Read output
    char buffer[2048];
    size_t len = fread(buffer, 1, sizeof(buffer) - 1, tmp);
    buffer[len] = '\0';

    // Restore stdout
    dup2(original_stdout, fileno(stdout));
    close(original_stdout);

    fclose(tmp);

    return std::string(buffer);
}

struct output_args {
    const port_result_t *results;
    int count;
    const scan_config_t *cfg;
};

static void output_wrapper(void *arg_void) {
    output_args *a = (output_args*)arg_void;
    print_scan_output(a->results, a->count, a->cfg);
}

TEST(OutputTest, BasicPrintSingleOpenPort) {

    port_result_t r[1];
    r[0].port = 80;
    r[0].status = PORT_OPEN;

    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 80;
    cfg.range = 1;
    cfg.verbose = 0;

    output_args args{r, 1, &cfg};

    std::string out = capture_output(output_wrapper, &args);

    EXPECT_NE(out.find("80/tcp"), std::string::npos);
    EXPECT_NE(out.find("OPEN"), std::string::npos);
}

TEST(OutputTest, PrintMultiplePorts) {

    port_result_t r[3];
    r[0].port = 1000; r[0].status = PORT_OPEN;
    r[1].port = 1001; r[1].status = PORT_CLOSED;
    r[2].port = 1002; r[2].status = PORT_FILTERED;

    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 1000;
    cfg.range = 3;
    cfg.verbose = 0;

    output_args args{r, 3, &cfg};

    std::string out = capture_output(output_wrapper, &args);

    EXPECT_NE(out.find("1000/tcp"), std::string::npos);
    EXPECT_NE(out.find("OPEN"), std::string::npos);

    EXPECT_NE(out.find("1001/tcp"), std::string::npos);
    EXPECT_NE(out.find("CLOSED"), std::string::npos);

    EXPECT_NE(out.find("1002/tcp"), std::string::npos);
    EXPECT_NE(out.find("FILTERED"), std::string::npos);
}

TEST(OutputTest, VerboseHeaderPrints) {

    port_result_t r[1];
    r[0].port = 22;
    r[0].status = PORT_CLOSED;

    scan_config_t cfg{};
    cfg.ip = "example.com";
    cfg.port = 22;
    cfg.range = 1;
    cfg.timeout_ms = 500;
    cfg.verbose = 1;

    output_args args{r, 1, &cfg};

    std::string out = capture_output(output_wrapper, &args);

    EXPECT_NE(out.find("Scanning example.com"), std::string::npos);
    EXPECT_NE(out.find("ports 22-22"), std::string::npos);
    EXPECT_NE(out.find("timeout 500 ms"), std::string::npos);
    EXPECT_NE(out.find("-----------------------------"), std::string::npos);
    EXPECT_NE(out.find("22/tcp"), std::string::npos);
}

TEST(OutputTest, NullConfigDoesNothing) {

    port_result_t r[1];
    r[0].port = 22;
    r[0].status = PORT_OPEN;

    output_args args{r, 1, nullptr};

    std::string out = capture_output(output_wrapper, &args);

    EXPECT_EQ(out, "");
}

TEST(OutputTest, NullResultsDoesNothing) {

    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 22;
    cfg.range = 1;

    output_args args{nullptr, 1, &cfg};

    std::string out = capture_output(output_wrapper, &args);

    EXPECT_EQ(out, "");
}

TEST(OutputTest, NegativeCountDoesNothing) {

    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 22;
    cfg.range = 1;

    port_result_t dummy;

    output_args args{&dummy, -10, &cfg};

    std::string out = capture_output(output_wrapper, &args);

    EXPECT_EQ(out, "");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
