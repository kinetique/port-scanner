#include "scan_controller.h"
#include "network.h"

#include <stdio.h>
#include <stdlib.h>

/* Validate scan settings */
static int validate_config(const scan_config_t *cfg) {
    if (cfg == NULL) {
        fprintf(stderr, "Error: NULL config\n");
        return 0;
    }

    if (cfg->ip == NULL || cfg->ip[0] == '\0') {
        fprintf(stderr, "Error: IP address is not set\n");
        return 0;
    }

    /* Port range checks */
    if (cfg->start_port < 1 || cfg->start_port > 65535) {
        fprintf(stderr, "Error: invalid start port: %d\n", cfg->start_port);
        return 0;
    }
    if (cfg->end_port < 1 || cfg->end_port > 65535) {
        fprintf(stderr, "Error: invalid end port: %d\n", cfg->end_port);
        return 0;
    }
    if (cfg->start_port > cfg->end_port) {
        fprintf(stderr, "Error: start port must be <= end port\n");
        return 0;
    }

    /* Timeout check */
    if (cfg->timeout_ms < 0) {
        fprintf(stderr, "Error: timeout must be >= 0\n");
        return 0;
    }

    return 1;
}

/* Convert port status to readable text */
static const char *port_status_to_string(int status) {
    if (status == PORT_OPEN) {
        return "OPEN";
    } else if (status == PORT_CLOSED) {
        return "CLOSED";
    } else if (status == PORT_FILTERED) {
        return "FILTERED";
    }
    return "UNKNOWN";
}

/* Print scan results */
static void print_results(const port_result_t *results, int count, const scan_config_t *cfg) {
    if (cfg->verbose) {
        int ports_count = cfg->end_port - cfg->start_port + 1;
        int timeout_ms = cfg->timeout_ms > 0 ? cfg->timeout_ms : 1000;

        printf("Scanning %s, ports %d-%d (%d ports), timeout %d ms\n",
               cfg->ip,
               cfg->start_port,
               cfg->end_port,
               ports_count,
               timeout_ms);

        printf("---------------------------------------------\n");
    }

    /* Print one line per port */
    for (int i = 0; i < count; i++) {
        printf("%5d/tcp  %s\n",
               results[i].port,
               port_status_to_string(results[i].status));
    }
}

/* Main scan controller */
int run_scan(const scan_config_t *cfg) {
    if (!validate_config(cfg)) {
        return SCAN_ERR_INVALID_ARGS;
    }

    int start = cfg->start_port;
    int end = cfg->end_port;
    int count = end - start + 1;

    if (count <= 0) {
        fprintf(stderr, "Error: no ports to scan\n");
        return SCAN_ERR_NO_PORTS;
    }

    int timeout_ms = cfg->timeout_ms > 0 ? cfg->timeout_ms : 1000;

    /* Allocate result array */
    port_result_t *results = malloc(sizeof(port_result_t) * count);
    if (results == NULL) {
        fprintf(stderr, "Error: cannot allocate memory for results\n");
        return SCAN_ERR_ALLOC;
    }

    /* Perform scan */
    int scanned = scan_port_range(cfg->ip, start, end, timeout_ms, results);
    if (scanned <= 0) {
        fprintf(stderr, "Warning: scan_port_range returned %d\n", scanned);
        free(results);
        return SCAN_ERR_NO_PORTS;
    }

    /* Output */
    print_results(results, scanned, cfg);

    free(results);
    return SCAN_OK;
}
