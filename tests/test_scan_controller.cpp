#include <gtest/gtest.h>
#include "scan_controller.h"
#include "network.h"

#include <cstring>

extern "C" int scan_port_range(const char *ip,
                               int start_port,
                               int end_port,
                               int timeout_ms,
                               int max_threads,
                               port_result_t *results)
{

    int count = end_port - start_port + 1;
    for (int i = 0; i < count; i++) {
        results[i].port = start_port + i;
        results[i].status = PORT_OPEN; 
    }
    return count;
}

TEST(ScanControllerTest, ValidConfigSinglePort) {
    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 80;
    cfg.range = 1;
    cfg.timeout_ms = 500;
    cfg.num_threads = 2;
    cfg.verbose = 0;

    int ret = run_scan(&cfg);
    EXPECT_EQ(ret, SCAN_OK);
}

TEST(ScanControllerTest, ValidConfigPortRange) {
    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 1000;
    cfg.range = 5;
    cfg.timeout_ms = 200;
    cfg.num_threads = 1;
    cfg.verbose = 1;

    int ret = run_scan(&cfg);
    EXPECT_EQ(ret, SCAN_OK);
}

TEST(ScanControllerTest, InvalidConfigNull) {
    int ret = run_scan(nullptr);
    EXPECT_EQ(ret, SCAN_ERR_INVALID_ARGS);
}

TEST(ScanControllerTest, InvalidConfigIP) {
    scan_config_t cfg{};
    cfg.ip = nullptr;
    cfg.port = 22;
    cfg.range = 1;
    cfg.timeout_ms = 100;

    int ret = run_scan(&cfg);
    EXPECT_EQ(ret, SCAN_ERR_INVALID_ARGS);
}

TEST(ScanControllerTest, InvalidPort) {
    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 70000; // invalid
    cfg.range = 1;

    int ret = run_scan(&cfg);
    EXPECT_EQ(ret, SCAN_ERR_INVALID_ARGS);
}

TEST(ScanControllerTest, InvalidRange) {
    scan_config_t cfg{};
    cfg.ip = "127.0.0.1";
    cfg.port = 80;
    cfg.range = -5; // invalid

    int ret = run_scan(&cfg);
    EXPECT_EQ(ret, SCAN_ERR_INVALID_ARGS);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}