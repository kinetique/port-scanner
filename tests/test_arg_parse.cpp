#include <gtest/gtest.h>
#include "arg_parse.h"

extern "C" {
#include "arg_parse.h"
}

static void run_parse(int argc, const char **argv,
                     const char **out_ip,
                     int *out_port,
                     int *out_range,
                     int *out_verbose,
                     int *out_remaining)
{
    *out_ip = NULL;
    *out_port = 0;
    *out_range = 0;
    *out_verbose = 0;

    struct argparse_option options[] = {
        OPT_STRING( 'i', "ip",      out_ip,      "target IP"),
        OPT_INTEGER('p', "port",    out_port,    "port number"),
        OPT_INTEGER('r', "range",   out_range,   "port range"),
        OPT_BOOLEAN('v', "verbose", out_verbose, "verbose output"),
        OPT_END()
    };

    const char *usages[] = {"prog [options]", NULL};
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);

    const char *outv[32];
    argparse.out = outv;

    *out_remaining = argparse_parse(&argparse, argc - 1, argv + 1);
}

TEST(ArgParse, ParseIPLong) {
    const char *argv[] = {"prog", "--ip", "127.0.0.1"};
    const char *ip;
    int port, range, verbose, rem;

    run_parse(3, argv, &ip, &port, &range, &verbose, &rem);

    ASSERT_STREQ(ip, "127.0.0.1");
    ASSERT_EQ(port, 0);
    ASSERT_EQ(rem, 0);
}

TEST(ArgParse, ParseIPShort) {
    const char *argv[] = {"prog", "-i", "192.168.1.1"};
    const char *ip;
    int port, range, verbose, rem;

    run_parse(3, argv, &ip, &port, &range, &verbose, &rem);

    ASSERT_STREQ(ip, "192.168.1.1");
}

TEST(ArgParse, ParsePort) {
    const char *argv[] = {"prog", "--port", "80"};
    const char *ip;
    int port, range, verbose, rem;

    run_parse(3, argv, &ip, &port, &range, &verbose, &rem);

    ASSERT_EQ(port, 80);
}

TEST(ArgParse, ParseVerboseFlag) {
    const char *argv[] = {"prog", "-v"};
    const char *ip;
    int port, range, verbose, rem;

    run_parse(2, argv, &ip, &port, &range, &verbose, &rem);

    ASSERT_EQ(verbose, 1);
}

TEST(ArgParse, ParseRange) {
    const char *argv[] = {"prog", "--range", "10"};
    const char *ip;
    int port, range, verbose, rem;

    run_parse(3, argv, &ip, &port, &range, &verbose, &rem);

    ASSERT_EQ(range, 10);
}

TEST(ArgParse, RemainingArgs) {
    const char *argv[] = {"prog", "extra1", "extra2"};
    const char *ip;
    int port, range, verbose, rem;

    run_parse(3, argv, &ip, &port, &range, &verbose, &rem);

    ASSERT_EQ(rem, 2);
}

TEST(ArgParse, MixedOptions) {
    const char *argv[] =
        {"prog", "--ip", "8.8.8.8", "-p", "443", "-v", "--range", "5"};

    const char *ip;
    int port, range, verbose, rem;

    run_parse(8, argv, &ip, &port, &range, &verbose, &rem);

    ASSERT_STREQ(ip, "8.8.8.8");
    ASSERT_EQ(port, 443);
    ASSERT_EQ(range, 5);
    ASSERT_EQ(verbose, 1);
    ASSERT_EQ(rem, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

