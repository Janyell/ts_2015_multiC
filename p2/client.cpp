#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/poll.h>
#include <vector>
#include <set>
#include <string.h>

#define PORT 3100
#define POLL_SIZE 2
#define FAIL -1
#define STDIN 0
#define BUF_SIZE 1025


int main() {
    int msfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (msfd == -1) {
        std::cerr << strerror(errno) << std::endl;
        return FAIL;
    }

//    int socketopt = 1;
//    setsockopt(msfd, SOL_SOCKET, SO_REUSEADDR, &socketopt, sizeof(socketopt));

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(PORT);
    sock_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(msfd, (const sockaddr *) (const void*) &sock_addr, sizeof 	(sock_addr)) == -1) {
        std::cerr << strerror(errno) << std::endl;
        return FAIL;
    }

    struct pollfd set[POLL_SIZE];
    set[0].fd = msfd;
    set[0].events = POLLIN;

    set[1].fd = STDIN;
    set[1].events = POLLIN;

    size_t set_size = 2;

    while (true) {
        poll(set, set_size, -1);
        if (set[0].revents & POLLIN) { // msfd
            char buf[BUF_SIZE] = {0};
            buf[BUF_SIZE - 1] = '\0';
            int recv_size = (int) recv(msfd, buf, sizeof(buf) - 1, 0);
            if (recv_size > 0) {
                std::cout << buf << std::flush;
            }
        }
        if (set[1].revents & POLLIN) { // stdin
            std::string message;
            std::cin >> message;
            message += '\n';
            send (msfd, message.c_str(), message.size(), 0);

        }
    }
    shutdown(msfd, SHUT_RDWR);
    close(msfd);
    return 0;
}
