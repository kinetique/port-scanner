#include "output.h"
#include <stdio.h>

/* Convert status enum to text */
static const char *status_to_text(port_status_t st) {
    switch (st) {
        case PORT_OPEN:     return "OPEN";
        case PORT_CLOSED:   return "CLOSED";
        case PORT_FILTERED: return "FILTERED";
        default:            return "UNKNOWN";
    }
}

/* Print full scan report */
void print_scan_output(const port_result_t *results,
                       int count,
                       const scan_config_t *cfg)
{
    if (cfg == NULL || results == NULL || count < 0) {
        return; /* simple safety check */
    }

    int start = cfg->port;
    int end = cfg->port + (cfg->range > 0 ? cfg->range - 1 : 0);

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

    /* One line per port */
    for (int i = 0; i < count; i++) {
        printf("%5d/tcp  %s\n",
               results[i].port,
               status_to_text(results[i].status));
    }
}
