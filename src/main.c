#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "arg_parse.h"
#include "scan_controller.h"

int main(int argc, const char **argv) {
    const char *ip = NULL;
    int port = 0;
    int range = 0;
    int timeout_ms = 1000;
    int verbose = 0;
    int num_threads = 50;
    int show_help = 0;

    const char *usages[] = {
        "portscan --ip <address> --port <n> [options]",
        "portscan --ip <address> --port <n> --range <n> [options]",
        NULL
    };

     struct argparse_option options[] = {
        OPT_BOOLEAN('h', "help", &show_help, "show this help message"),
        OPT_STRING( 'i', "ip",    &ip,    "target ip (IPv4)"),
        OPT_INTEGER('p', "port",  &port,  "starting port (required)"),
        OPT_INTEGER('r', "range", &range, "number of ports starting from --port (optional)"),
        OPT_INTEGER('t', "threads", &num_threads, "number of scanner threads"),
        OPT_INTEGER('T', "timeout", &timeout_ms, "connection timeout in ms"),
        OPT_BOOLEAN('v', "verbose", &verbose, "verbose output"),
        OPT_END()
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    
    const char *outv[32];
    argparse.out = outv;

    argparse_describe(&argparse,
                      "Simple TCP port scanner",
                      "Example:\n  ./portscan --ip 192.168.1.1 --port 1 1000");
    

    argparse_parse(&argparse, argc, argv);

    // Validate required args
    if (show_help) {
        argparse_usage(&argparse);
        return 0;
    }
    
    if (!ip) {
        fprintf(stderr, "Error: --ip is required\n");
        return 1;
    }
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: --port must be in 1..65535\n");
        return 1;
    }
    if (range < 0) {
        fprintf(stderr, "Error: --range must be >= 0\n");
        return 1;
    }

    if (num_threads <= 0 || num_threads > 2000) {
    fprintf(stderr, "Error: --threads must be between 1 and 2000\n");
    return 1;
    }

    scan_config_t cfg;
    cfg.ip = ip;
    cfg.port = port;
    cfg.range = range;
    cfg.timeout_ms = timeout_ms;
    cfg.verbose = verbose;
    cfg.num_threads = num_threads;

    int status = run_scan(&cfg);

    if (status != SCAN_OK) {
        fprintf(stderr, "Scan failed (error code %d)\n", status);
        return 1;
    }

    return 0;
}
