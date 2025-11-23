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

static int parse_port_range(const char *s, struct port_range *out) {
    int a, b;
    if (sscanf(s, "%d-%d", &a, &b) == 2) {
        if (a >= 0 && b >= a && b <= 65535) {
            out->start = a;
            out->end = b;
            return 0;
        }
    }
    return -1;
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

int argparse_init(struct argparse *self, struct argparse_option *options,
                  const char *const *usages, int flags)
{
    self->options = options;
    self->usages = usages;
    self->flags = flags;
    self->description = NULL;
    self->epilog = NULL;
    self->argc = 0;
    self->argv = NULL;
    self->out = NULL;
    self->cpidx = 0;
    self->optvalue = NULL;
    return 0;
}

void argparse_describe(struct argparse *self,
                       const char *description,
                       const char *epilog)
{
    self->description = description;
    self->epilog = epilog;
}

int argparse_parse(struct argparse *self, int argc, const char **argv)
{
    self->argc = argc;
    self->argv = argv;

    while (self->argc > 0) {
        const char *arg = *self->argv;

        /* Non-option argument */
        if (arg[0] != '-') {
            if (self->flags & ARGPARSE_STOP_AT_NONOPTION)
                break;
            self->out[self->cpidx++] = arg;
            self->argv++; self->argc--;
            continue;
        }

        /* "--" indicates the end of option parsing */
        if (strcmp(arg, "--") == 0) {
            self->argv++; self->argc--;
            break;
        }

        /* Long option */
        if (arg[1] == '-') {
            const char *name = arg + 2;
            const char *value = NULL;

            char buf[256];
            const char *eq = strchr(name, '=');
            if (eq) {
                size_t len = eq - name;
                memcpy(buf, name, len);
                buf[len] = '\0';
                name = buf;
                value = eq + 1;
            }

            struct argparse_option *opt = find_option(self, 0, name);
            if (!opt) {
                if (!(self->flags & ARGPARSE_IGNORE_UNKNOWN_ARGS)) {
                    printf("Unknown option: %s\n", arg);
                    exit(1);
                }
                self->argv++; self->argc--;
                continue;
            }

            /* Boolean option */
            if (opt->type == ARGPARSE_OPT_BOOLEAN) {
                *(int *)opt->value = 1;
            }
            /* Integer */
            else if (opt->type == ARGPARSE_OPT_INTEGER) {
                if (!value) {
                    if (self->argc < 2) {
                        printf("Option --%s requires value\n", name);
                        exit(1);
                    }
                    value = self->argv[1];
                    self->argv++; self->argc--;
                }
                if (parse_int(value, (int *)opt->value) != 0) {
                    printf("Invalid integer for --%s\n", name);
                    exit(1);
                }
            }
            /* String */
            else if (opt->type == ARGPARSE_OPT_STRING) {
                if (!value) {
                    if (self->argc < 2) {
                        printf("Option --%s requires value\n", name);
                        exit(1);
                    }
                    value = self->argv[1];
                    self->argv++; self->argc--;
                }
                *(const char **)opt->value = value;
            }
            /* Port range */
            else if (opt->type == ARGPARSE_OPT_PORT_RANGE) {
                if (!value) {
                    if (self->argc < 2) {
                        printf("Option --%s requires value (example: 1-100)\n", name);
                        exit(1);
                    }
                    value = self->argv[1];
                    self->argv++; self->argc--;
                }

                if (parse_port_range(value, (struct port_range *)opt->value) != 0) {
                    printf("Invalid port range for --%s. Use format a-b\n", name);
                    exit(1);
                }
            }

            if (opt->callback)
                opt->callback(self, opt);

            self->argv++; self->argc--;
            continue;
        }

        /* Short option */
        char short_name = arg[1];
        struct argparse_option *opt = find_option(self, short_name, NULL);
        if (!opt) {
            printf("Unknown option: -%c\n", short_name);
            exit(1);
        }

        const char *value = NULL;

        if (opt->type == ARGPARSE_OPT_BOOLEAN) {
            *(int *)opt->value = 1;
        }
        else if (opt->type == ARGPARSE_OPT_INTEGER) {
            if (self->argc < 2) {
                printf("Option -%c requires value\n", short_name);
                exit(1);
            }
            value = self->argv[1];

            if (parse_int(value, (int *)opt->value) != 0) {
                printf("Invalid integer for -%c\n", short_name);
                exit(1);
            }
            self->argv++; self->argc--;
        }
        else if (opt->type == ARGPARSE_OPT_STRING) {
            if (self->argc < 2) {
                printf("Option -%c requires value\n", short_name);
                exit(1);
            }
            value = self->argv[1];
            *(const char **)opt->value = value;
            self->argv++; self->argc--;
        }

        else if (opt->type == ARGPARSE_OPT_PORT_RANGE) {

            if (self->argc < 2) {
                printf("Option -%c requires value like 1-100\n", short_name);
                exit(1);
            }

            value = self->argv[1];
            if (parse_port_range(value, (struct port_range *)opt->value) != 0) {
                printf("Invalid port range for -%c (expected a-b)\n", short_name);
                exit(1);
            }

            self->argv++; self->argc--;
        }

        if (opt->callback)
            opt->callback(self, opt);

        self->argv++; self->argc--;
    }

    /* Copy remaining non-option args */
    while (self->argc > 0) {
        self->out[self->cpidx++] = *self->argv;
        self->argv++; self->argc--;
    }

    return self->cpidx;
}

