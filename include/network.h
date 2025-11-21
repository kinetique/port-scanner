#ifndef NETWORK_SCANNER_H
#define NETWORK_SCANNER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Port status
typedef enum {
    PORT_CLOSED = 0, // TCP connection failed
    PORT_OPEN   = 1, // TCP connection succeeded
    PORT_FILTERED = 2 // Timeout or unreachable (filtered by firewall)
} port_status_t;

// Result of scanning a single port
typedef struct {
    int port;           
    port_status_t status; 
} port_result_t;

/**
 * @brief Scan a single TCP port on a target IP.
 *
 * @param ip Target IP address (IPv4 string)
 * @param port Port number
 * @param timeout_ms Connection timeout in milliseconds
 *
 * @return port_result_t Result structure with port and status
 */
port_result_t scan_port(const char *ip, int port, int timeout_ms);

/**
 * @brief Scan a range of TCP ports on a target IP.
 *
 * @param ip Target IP address
 * @param start_port Starting port number
 * @param end_port Ending port number
 * @param timeout_ms Connection timeout in milliseconds
 * @param out_results Array of results allocated by caller
 *
 * @return int Number of ports scanned (end_port - start_port + 1)
 */
int scan_port_range(const char *ip,
                    int start_port,
                    int end_port,
                    int timeout_ms,
                    port_result_t *out_results);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_SCANNER_H
