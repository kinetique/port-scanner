#ifndef SCAN_CONTROLLER_H
#define SCAN_CONTROLLER_H

#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *ip;      ///< Target IP address (IPv4 string)
    int port;           ///< Single port to scan
    int range;          ///< Number of ports to scan starting from 'port'
    int timeout_ms;      ///< Timeout for each connection attempt (ms)
    int verbose;         ///< Verbose output flag (0 = off, 1 = on)
    int num_threads;        ///< Number of concurrent threads to use
} scan_config_t;

// Status codes returned by run_scan()
#define SCAN_OK                0  ///< Scan completed successfully
#define SCAN_ERR_INVALID_ARGS  1  ///< Invalid or missing arguments in config
#define SCAN_ERR_NO_PORTS      2  ///< start_port > end_port or invalid range
#define SCAN_ERR_ALLOC         3  ///< Memory allocation failure during scan

int run_scan(const scan_config_t *cfg);

#ifdef __cplusplus
}
#endif

#endif // SCAN_CONTROLLER_H
