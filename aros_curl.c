#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define CLOSESOCK close
#define SOCK_ERR (-1)

static char *str_dup(const char *s)
{
    if (!s)
        return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p)
        memcpy(p, s, n);
    return p;
}

static int parse_url(const char *url, char **host_out, char **path_out, int *port_out)
{
    *host_out = NULL;
    *path_out = NULL;
    *port_out = 80;

    if (!url || !*url)
        return -1;

    const char *p = url;
    const char *http = "http://";
    size_t http_len = strlen(http);

    if (strncmp(p, http, http_len) == 0)
    {
        p += http_len;
    }

    const char *slash = strchr(p, '/');
    const char *host_end = slash ? slash : (p + strlen(p));

    char *hostport = (char *)malloc((size_t)(host_end - p) + 1);
    if (!hostport)
        return -1;
    memcpy(hostport, p, (size_t)(host_end - p));
    hostport[host_end - p] = '\0';

    const char *path = slash ? slash : "/";

    char *colon = strchr(hostport, ':');
    if (colon)
    {
        *colon = '\0';
        colon++;
        if (*colon)
        {
            int port = atoi(colon);
            if (port <= 0 || port > 65535)
            {
                free(hostport);
                return -1;
            }
            *port_out = port;
        }
    }

    *host_out = str_dup(hostport);
    *path_out = str_dup(path);
    free(hostport);

    if (!*host_out || !*path_out)
    {
        free(*host_out);
        free(*path_out);
        return -1;
    }
    return 0;
}

static int connect_tcp(const char *host, int port, socket_t *sock_out)
{
    *sock_out = INVALID_SOCKET;

    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *res = NULL;
    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0 || !res)
    {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(rc));
        return -1;
    }

    socket_t s = INVALID_SOCKET;
    for (struct addrinfo *ai = res; ai != NULL; ai = ai->ai_next)
    {
        s = (socket_t)socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (s < 0)
            continue;
        if (connect(s, ai->ai_addr, ai->ai_addrlen) == 0)
        {
            *sock_out = s;
            break;
        }
        CLOSESOCK(s);
        s = INVALID_SOCKET;
    }
    freeaddrinfo(res);

    if (*sock_out == INVALID_SOCKET)
    {
        fprintf(stderr, "Failed to connect to %s:%d\n", host, port);
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s http://host[:port]/path\n", argv[0]);
        return 1;
    }

    char *host = NULL;
    char *path = NULL;
    int port = 80;

    if (parse_url(argv[1], &host, &path, &port) != 0)
    {
        fprintf(stderr, "Invalid URL (only http:// supported).\n");
        return 1;
    }

    socket_t sock;
    if (connect_tcp(host, port, &sock) != 0)
    {
        free(host);
        free(path);
        return 1;
    }

    char request[4096];
    int n = snprintf(request, sizeof(request),
                     "GET %s HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "User-Agent: mini-curl/1.0\r\n"
                     "Connection: close\r\n"
                     "\r\n",
                     path, host);
    if (n <= 0 || n >= (int)sizeof(request))
    {
        fprintf(stderr, "Request too long.\n");
        CLOSESOCK(sock);
        free(host);
        free(path);

    return 0;
}

    ssize_t sent = send(sock, request, (size_t)n, 0);
    if (sent == -1)
    {
        perror("send");
        CLOSESOCK(sock);
        free(host);
        free(path);
        return 1;
    }

    char buf[8192];
    for (;;)
    {
        ssize_t r = recv(sock, buf, sizeof(buf), 0);
        if (r == 0)
            break;
        if (r < 0)
        {
            perror("recv");
            break;
        }
        fwrite(buf, 1, (size_t)r, stdout);
    }

    CLOSESOCK(sock);
    free(host);
    free(path);
    return 0;
}
