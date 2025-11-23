#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "network.h"

// TCP test server
static void *test_tcp_server(void *arg) {
    int port = *(int *)arg;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) pthread_exit(NULL);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int enable = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        pthread_exit(NULL);
    }

    if (listen(server_fd, 1) < 0) {
        close(server_fd);
        pthread_exit(NULL);
    }

    sleep(2);

    close(server_fd);
    pthread_exit(NULL);
}

TEST(NetworkTest, OpenPort) {
    int test_port = 50505;
    pthread_t th;
    pthread_create(&th, NULL, test_tcp_server, &test_port);
    usleep(200 * 1000); 

    port_result_t r = scan_port("127.0.0.1", test_port, 500);

    EXPECT_EQ(r.port, test_port);
    EXPECT_EQ(r.status, PORT_OPEN);

    pthread_join(th, NULL);
}

TEST(NetworkTest, ClosedPort) {
    int test_port = 50506; // nothing listens

    port_result_t r = scan_port("127.0.0.1", test_port, 300);

    EXPECT_EQ(r.port, test_port);
    EXPECT_TRUE(r.status == PORT_CLOSED || r.status == PORT_FILTERED);
}

TEST(NetworkTest, RangeScan) {
    port_result_t results[10];
    int max_threads = 4; 
    int count = scan_port_range("127.0.0.1", 50000, 50005, 300, max_threads, results);

    EXPECT_EQ(count, 6);

    for (int i = 0; i < count; i++) {
        EXPECT_GE(results[i].port, 50000);
        EXPECT_LE(results[i].port, 50005);
        EXPECT_TRUE(results[i].status == PORT_OPEN ||
                    results[i].status == PORT_CLOSED ||
                    results[i].status == PORT_FILTERED);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
