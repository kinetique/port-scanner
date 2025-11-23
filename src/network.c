#include "network.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
    const char *ip;
    int timeout_ms;
    int *next_port;
    int last_port;
    port_result_t *results;
    int start_port;
    pthread_mutex_t *lock;
} thread_context_t;

static void *worker_thread(void *arg) {
    thread_context_t *ctx = (thread_context_t *)arg;

    while (1) {
        int port;

        pthread_mutex_lock(ctx->lock);
        if (*ctx->next_port > ctx->last_port) {
            pthread_mutex_unlock(ctx->lock);
            break;
        }
        port = (*ctx->next_port)++;
        pthread_mutex_unlock(ctx->lock);

        port_result_t result = scan_port(ctx->ip, port, ctx->timeout_ms);

        int index = port - ctx->start_port;
        ctx->results[index] = result;
    }

    return NULL;
}

static int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

port_result_t scan_port(const char *ip, int port, int timeout_ms) {
    port_result_t res;
    res.port = port;
    res.status = PORT_FILTERED;  

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        res.status = PORT_CLOSED;
        return res;
    }

    set_nonblocking(sockfd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        close(sockfd);
        res.status = PORT_CLOSED;
        return res;
    }

    // Start connection
    int conn = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (conn < 0) {
        if (errno != EINPROGRESS) {
            close(sockfd);
            res.status = PORT_CLOSED;
            return res;
        }
    }

    // Use select() to wait with timeout
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sockfd, &writefds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int sel = select(sockfd + 1, NULL, &writefds, NULL, &tv);

    if (sel == 0) {
        res.status = PORT_FILTERED;
        close(sockfd);
        return res;
    }
    if (sel < 0) {
        res.status = PORT_CLOSED;
        close(sockfd);
        return res;
    }

    // Check actual connection result
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len);

    if (err == 0)
        res.status = PORT_OPEN;
    else
        res.status = PORT_CLOSED;

    close(sockfd);
    return res;
}

int scan_port_range(const char *ip,
                    int start_port,
                    int end_port,
                    int timeout_ms,
                    int num_threads,
                    port_result_t *out_results)
{
     int total_ports = end_port - start_port + 1;

    if (num_threads <= 1) {
        for (int p = start_port; p <= end_port; p++) {
            out_results[p - start_port] = scan_port(ip, p, timeout_ms);
        }
        return total_ports;
    }

    if (num_threads > total_ports) {
        num_threads = total_ports;
    }
    if (num_threads > 2000) {
        num_threads = 2000;
    }

    int next_port = start_port;
    pthread_mutex_t port_lock = PTHREAD_MUTEX_INITIALIZER;

    thread_context_t ctx = {
        .ip = ip,
        .timeout_ms = timeout_ms,
        .next_port = &next_port,
        .last_port = end_port,
        .results = out_results,
        .start_port = start_port,
        .lock = &port_lock
    };

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    if (!threads) {
        return -1;
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, worker_thread, &ctx) != 0) {
            fprintf(stderr, "Warning: failed to create thread %d\n", i);
            num_threads = i;
            break;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    pthread_mutex_destroy(&port_lock);

    return total_ports;
}
