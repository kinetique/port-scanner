#include "arg_parse.h"
#include <stdio.h>

int main(int argc, const char **argv) {
    int port = 0;
    const char *ip = NULL;
    int range = 0;
    int verbose = 0;

    struct argparse_option options[] = {
    { 'i', "ip",    &ip,    ARGPARSE_OPT_STRING,  0, "target ip", 0, NULL },
    { 'p', "port",  &port,  ARGPARSE_OPT_INTEGER, 0, "port",      0, NULL },
    { 'r', "range", &range, ARGPARSE_OPT_INTEGER, 0, "range",     0, NULL },
    { 'v', "verbose", &verbose, ARGPARSE_OPT_BOOLEAN, 0, "verbose", 0, NULL },
    OPT_END()
};

    const char *usages[] = {"prog [options]", NULL};

    struct argparse a;
    argparse_init(&a, options, usages, 0);

    const char *out[32];
    a.out = out;

    int n = argparse_parse(&a, argc, argv);

    printf("IP: %s\n", ip);
    printf("Port: %d\n", port);
    printf("Range: %d\n", range);
    printf("Verbose: %d\n", verbose);

    printf("Remaining args:\n");
    for (int i = 0; i < n; i++)
        printf("  %s\n", out[i]);
}
