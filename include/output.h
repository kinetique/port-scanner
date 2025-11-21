#ifndef OUTPUT_H
#define OUTPUT_H

#include "network.h"
#include "scan_controller.h"

/* Print scan results to stdout */
void print_scan_output(const port_result_t *results,int count,const scan_config_t *cfg);

#endif // OUTPUT_H
