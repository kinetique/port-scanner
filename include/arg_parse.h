#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include <stddef.h>

#define ARGPARSE_OPT_BOOLEAN 0
#define ARGPARSE_OPT_BIT 1
#define ARGPARSE_OPT_STRING 2
#define ARGPARSE_OPT_INT 3
#define ARGPARSE_OPT_FLOAT 4
#define ARGPARSE_OPT_GROUP 5
#define ARGPARSE_OPT_END 6

#define ARGPARSE_OPT_NONEG (1<<0)

#define ARGPARSE_STOP_AT_NONOPTION (1<<0)

struct argparse_option;
struct argparse;

typedef void (*argparse_callback_t)(struct argparse *self,
                                     const struct argparse_option *option);

struct argparse_option {
    char short_name;
    const char *long_name;
    int type;
    void *value;
    const char *help;
    int flags;
    argparse_callback_t callback;
};

struct argparse {
    struct argparse_option *options;
    const char *const *usages;
    int flags;
    const char *description;
    const char *epilog;
    
    int argc;
    const char **argv;
    const char **out;
    int cpidx;
    const char *optvalue;
};

#endif // ARG_PARSE_H