#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <functional>

extern "C" {
    #include "arg_parse.h"
    #include "scan_controller.h"
    int app_main(int argc, const char **argv); 
}

static int mock_run_scan_return = SCAN_OK;

extern "C" int run_scan(const scan_config_t *cfg) {
    (void)cfg;
    return mock_run_scan_return;
}

static std::string capture_fd(int fd, std::function<int()> func) {
    fflush(nullptr);

    int saved = dup(fd);
    FILE *tmp = tmpfile();
    dup2(fileno(tmp), fd);

    int ret = func();

    fflush(nullptr);
    fseek(tmp, 0, SEEK_SET);

    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf)-1, tmp);
    buf[n] = '\0';

    dup2(saved, fd);
    close(saved);
    fclose(tmp);

    return std::string(buf);
}


TEST(MainTest, HelpFlag) {
    const char *argv[] = {"portscan", "--help", nullptr};

    std::string out = capture_fd(STDOUT_FILENO, [&](){
        return app_main(2, argv);  
    });

    EXPECT_NE(out.find("Usage"), std::string::npos);
}

TEST(MainTest, MissingIP) {
    const char *argv[] = {"portscan", "--port", "80", nullptr};

    std::string out = capture_fd(STDERR_FILENO, [&](){
        return app_main(3, argv); 
    });

    EXPECT_NE(out.find("--ip is required"), std::string::npos);
}

TEST(MainTest, InvalidPort) {
    const char *argv[] = {"portscan", "--ip", "127.0.0.1", "--port", "99999", nullptr};

    std::string out = capture_fd(STDERR_FILENO, [&](){
        return app_main(5, argv); 
    });

    EXPECT_NE(out.find("must be in 1..65535"), std::string::npos);
}

TEST(MainTest, InvalidRange) {
    const char *argv[] = {"portscan", "--ip", "127.0.0.1", "--port", "80", "--range", "-5", nullptr};

    std::string out = capture_fd(STDERR_FILENO, [&](){
        return app_main(7, argv); 
    });

    EXPECT_NE(out.find("must be >= 0"), std::string::npos);
}

TEST(MainTest, InvalidThreadCount) {
    const char *argv[] = {"portscan", "--ip", "1.2.3.4", "--port", "80", "--threads", "3000", nullptr};

    std::string out = capture_fd(STDERR_FILENO, [&](){
        return app_main(7, argv);
    });

    EXPECT_NE(out.find("must be between 1 and 2000"), std::string::npos);
}

TEST(MainTest, ScanErrorPropagated) {
    const char *argv[] = {"portscan", "--ip", "1.2.3.4", "--port", "80", nullptr};

    mock_run_scan_return = SCAN_ERR_ALLOC;

    std::string out = capture_fd(STDERR_FILENO, [&](){
        return app_main(5, argv);  
    });

    EXPECT_NE(out.find("Scan failed"), std::string::npos);
}

TEST(MainTest, ValidRun) {
    const char *argv[] = {"portscan", "--ip", "1.2.3.4", "--port", "22", nullptr};

    mock_run_scan_return = SCAN_OK;

    int ret = app_main(5, argv); 

    EXPECT_EQ(ret, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}