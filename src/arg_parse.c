#include "arg_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_int(const char *s, int *out) {
    char *end;
    long v = strtol(s, &end, 10);
    if (*end != '\0')
        return -1;
    *out = (int)v;
    return 0;
}

static struct argparse_option *find_option(struct argparse *self,
                                           char short_name,
                                           const char *long_name)
{
    for (struct argparse_option *opt = self->options; opt->type != ARGPARSE_OPT_END; opt++) {
        if (short_name && opt->short_name == short_name)
            return opt;
        if (long_name && opt->long_name &&
            strcmp(opt->long_name, long_name) == 0)
            return opt;
    }
    return NULL;
}

void argparse_usage(struct argparse *self) {
    if (self->usages) {
        for (const char *const *u = self->usages; *u; u++)
            printf("Usage: %s\n", *u);
    }

    if (self->description)
        printf("\n%s\n", self->description);

    printf("\nOptions:\n");

    for (struct argparse_option *opt = self->options; opt->type != ARGPARSE_OPT_END; opt++) {
        if (opt->type == ARGPARSE_OPT_GROUP) {
            printf("\n%s:\n", opt->help);
            continue;
        }

        printf("  ");
        if (opt->short_name) printf("-%c", opt->short_name);
        if (opt->short_name && opt->long_name) printf(", ");
        if (opt->long_name) printf("--%s", opt->long_name);

        printf("\t%s\n", opt->help ? opt->help : "");
    }

    if (self->epilog)
        printf("\n%s\n", self->epilog);
}

int argparse_help_cb_no_exit(struct argparse *self,
                             const struct argparse_option *option)
{
    (void)option;
    argparse_usage(self);
    return 0;
}

int argparse_help_cb(struct argparse *self,
                     const struct argparse_option *option)
{
    (void)option;
    argparse_usage(self);
    exit(0);
    return 0;
}

