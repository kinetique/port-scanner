#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include <stddef.h>

#define ARGPARSE_OPT_BOOLEAN  0
#define ARGPARSE_OPT_BIT      1
#define ARGPARSE_OPT_STRING   2
#define ARGPARSE_OPT_INTEGER  3   
#define ARGPARSE_OPT_GROUP    4
#define ARGPARSE_OPT_END      5

#define ARGPARSE_OPT_NONEG (1 << 0)

#define ARGPARSE_STOP_AT_NONOPTION (1 << 0)
#define ARGPARSE_IGNORE_UNKNOWN_ARGS (1 << 1)

struct argparse;
struct argparse_option;

typedef void (*argparse_callback_t)(struct argparse *self,
                                    const struct argparse_option *option);


struct argparse_option {
    char short_name;                
    const char *long_name;          
    void *value;                    
    int type;                       
    int data;                       
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

#define OPT_BOOLEAN(s,l,ptr,help_text) \
    { (s), (l), (void *)(ptr), ARGPARSE_OPT_BOOLEAN, 0, (help_text), 0, NULL }

#define OPT_BIT(s,l,ptr,bit,help_text) \
    { (s), (l), (void *)(ptr), ARGPARSE_OPT_BIT, (bit), (help_text), 0, NULL }

#define OPT_STRING(s,l,ptr,help_text) \
    { (s), (l), (void *)(ptr), ARGPARSE_OPT_STRING, 0, (help_text), 0, NULL }

#define OPT_INTEGER(s,l,ptr,help_text) \
    { (s), (l), (void *)(ptr), ARGPARSE_OPT_INTEGER, 0, (help_text), 0, NULL }

#define OPT_GROUP(help_text) \
    { 0, NULL, NULL, ARGPARSE_OPT_GROUP, 0, (help_text), 0, NULL }

#define OPT_CUSTOM(s,l,ptr,type_,data_,help_,flags_,cb_) \
    { (s), (l), (void *)(ptr), (type_), (data_), (help_), (flags_), (cb_) }

#define OPT_END() \
    { 0, NULL, NULL, ARGPARSE_OPT_END, 0, NULL, 0, NULL }

int argparse_init(struct argparse *self, struct argparse_option *options,
                  const char *const *usages, int flags);

void argparse_describe(struct argparse *self, const char *description,
                       const char *epilog);

int argparse_parse(struct argparse *self, int argc, const char **argv);

void argparse_usage(struct argparse *self);

int argparse_help_cb_no_exit(struct argparse *self,
                             const struct argparse_option *option);

int argparse_help_cb(struct argparse *self,
                     const struct argparse_option *option);

#endif // ARG_PARSE_H 
