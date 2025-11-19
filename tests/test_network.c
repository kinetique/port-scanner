#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "network.h"

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

    /* Keep server alive for test duration */
    sleep(2);

    close(server_fd);
    pthread_exit(NULL);
}----------------- */

static void test_open_port() {
    printf("[TEST] scan_port(): open port\n");

    int test_port = 50505;
    pthread_t th;
    pthread_create(&th, NULL, test_tcp_server, &test_port);
    usleep(200 * 1000); // wait server startup

    port_result_t r = scan_port("127.0.0.1", test_port, 500);

    assert(r.port == test_port);
    assert(r.status == PORT_OPEN);

    pthread_join(th, NULL);
    printf("   OK\n");
}

static void test_closed_port() {
    printf("[TEST] scan_port(): closed port\n");

    int test_port = 50506; // nothing listens

    port_result_t r = scan_port("127.0.0.1", test_port, 300);

    assert(r.port == test_port);
    assert(r.status == PORT_CLOSED || r.status == PORT_FILTERED);

    printf("   OK\n");
}

static void test_range_scan() {
    printf("[TEST] scan_port_range(): basic range test\n");

    port_result_t results[10];
    int count = scan_port_range("127.0.0.1", 50000, 50005, 300, results);

    assert(count == 6);

    for (int i = 0; i < count; i++) {
        assert(results[i].port >= 50000);
        assert(results[i].port <= 50005);

        assert(results[i].status == PORT_OPEN ||
               results[i].status == PORT_CLOSED ||
               results[i].status == PORT_FILTERED);
    }

    printf("   OK\n");
}

int main() {
    printf("Running network tests...\n\n");

    test_open_port();
    test_closed_port();
    test_range_scan();

    printf("\nAll tests passed!\n");
    return 0;
}
