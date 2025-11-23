#include "scan_controller.h"
#include "network.h"

#include <stdio.h>
#include <stdlib.h>

/* Validate scan settings */
static int validate_config(const scan_config_t *cfg) {
    if (!cfg) {
        fprintf(stderr, "Error: NULL config\n");
        return 0;
    }

    if (!cfg->ip || cfg->ip[0] == '\0') {
        fprintf(stderr, "Error: IP address not set\n");
        return 0;
    }

    if (cfg->port < 1 || cfg->port > 65535) {
        fprintf(stderr, "Error: invalid port: %d\n", cfg->port);
        return 0;
    }

    if (cfg->range < 0) {
        fprintf(stderr, "Error: invalid range: %d (must be >=1)\n", cfg->range);
        return 0;
    }

    long end_port = (long)cfg->port + (cfg->range > 0 ? (cfg->range - 1) : 0);

    if (end_port > 65535) {
        fprintf(stderr, "Error: port + range exceeds 65535\n");
        return 0;
    }

    if (cfg->timeout_ms < 0) {
        fprintf(stderr, "Error: timeout must be >=0\n");
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
    int start = cfg->port;
    int end = cfg->port + cfg->range - 1;
    
    if (cfg->verbose) {
        int ports_count = end - start + 1;
        int timeout_ms = cfg->timeout_ms > 0 ? cfg->timeout_ms : 1000;

        printf("Scanning %s, ports %d-%d (%d ports), timeout %d ms\n",
                cfg->ip,
                start,
                end,
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

    int start = cfg->port;
    int end = cfg->port + (cfg->range > 0 ? (cfg->range - 1) : 0);
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
