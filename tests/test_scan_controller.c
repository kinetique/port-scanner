#include "../include/scan_controller.h"
#include "../include/network.h"

#include <stdio.h>
#include <stdlib.h>

int mock_scan_calls = 0;
int mock_last_start = 0;
int mock_last_end = 0;

int scan_port_range(const char *ip,
                    int start_port,
                    int end_port,
                    int timeout_ms,
                    port_result_t *out_results)
{
    mock_scan_calls++;
    mock_last_start = start_port;
    mock_last_end = end_port;

    int count = end_port - start_port + 1;

    for (int i = 0; i < count; i++) {
        out_results[i].port = start_port + i;
        out_results[i].status = PORT_OPEN;
    }

    return count;
}

/* Helper to reset mock state */
static void reset_mock() {
    mock_scan_calls = 0;
    mock_last_start = 0;
    mock_last_end = 0;
}

/* ----- TESTS ----- */

static void test_null_config() {
    int r = run_scan(NULL);
    printf("test_null_config: %s\n", r == SCAN_ERR_INVALID_ARGS ? "OK" : "FAIL");
}

static void test_invalid_ip() {
    scan_config_t cfg = {
        .ip = "",
        .start_port = 10,
        .end_port = 20,
        .timeout_ms = 100
    };

    int r = run_scan(&cfg);
    printf("test_invalid_ip: %s\n", r == SCAN_ERR_INVALID_ARGS ? "OK" : "FAIL");
}

static void test_invalid_port_range() {
    scan_config_t cfg = {
        .ip = "127.0.0.1",
        .start_port = 2000,
        .end_port = 1000,
        .timeout_ms = 100
    };

    int r = run_scan(&cfg);
    printf("test_invalid_port_range: %s\n", r == SCAN_ERR_INVALID_ARGS ? "OK" : "FAIL");
}

static void test_no_ports() {
    scan_config_t cfg = {
        .ip = "127.0.0.1",
        .start_port = 5,
        .end_port = 4,
        .timeout_ms = 100
    };

    int r = run_scan(&cfg);
    printf("test_no_ports: %s\n", r == SCAN_ERR_INVALID_ARGS ? "OK" : "FAIL");
}

static void test_valid_scan_triggers_network() {
    reset_mock();

    scan_config_t cfg = {
        .ip = "127.0.0.1",
        .start_port = 10,
        .end_port = 12,
        .timeout_ms = 100,
        .verbose = 0
    };

    int r = run_scan(&cfg);

    int ok =
        r == SCAN_OK &&
        mock_scan_calls == 1 &&
        mock_last_start == 10 &&
        mock_last_end == 12;

    printf("test_valid_scan_triggers_network: %s\n", ok ? "OK" : "FAIL");
}

int main() {
    test_null_config();
    test_invalid_ip();
    test_invalid_port_range();
    test_no_ports();
    test_valid_scan_triggers_network();

    return 0;
}
