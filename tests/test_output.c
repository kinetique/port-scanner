#include "../include/output.h"
#include <stdio.h>

/* Helper: make simple config */
static scan_config_t make_cfg(int verbose) {
    scan_config_t cfg;

    cfg.ip = "127.0.0.1";
    cfg.start_port = 10;
    cfg.end_port = 12;
    cfg.timeout_ms = 100;
    cfg.verbose = verbose;

    return cfg;
}

/* Helper: fill demo results */
static void fill_results(port_result_t *res, int count, port_status_t st) {
    for (int i = 0; i < count; i++) {
        res[i].port = 10 + i;
        res[i].status = st;
    }
}

/* No results: should not crash and not print ports */
static void test_output_empty() {
    scan_config_t cfg = make_cfg(0);
    port_result_t dummy;

    /* count = 0 */
    print_scan_output(&dummy, 0, &cfg);

    printf("test_output_empty: OK\n");
}

/* Non-verbose output */
static void test_output_non_verbose() {
    scan_config_t cfg = make_cfg(0);

    port_result_t res[3];
    fill_results(res, 3, PORT_OPEN);

    print_scan_output(res, 3, &cfg);

    printf("test_output_non_verbose: OK\n");
}

/* Verbose output */
static void test_output_verbose() {
    scan_config_t cfg = make_cfg(1);

    port_result_t res[3];
    fill_results(res, 3, PORT_CLOSED);

    print_scan_output(res, 3, &cfg);

    printf("test_output_verbose: OK\n");
}

int main(void) {
    test_output_empty();
    test_output_non_verbose();
    test_output_verbose();

    return 0;
}
