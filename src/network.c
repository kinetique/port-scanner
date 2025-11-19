#include "network.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>


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
                    port_result_t *out_results)
{
    int total = 0;

    for (int p = start_port; p <= end_port; p++) {
        out_results[total] = scan_port(ip, p, timeout_ms);
        total++;
    }

    return total;
}
