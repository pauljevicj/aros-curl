#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        p += http_len; // preskoči "http://"
    }
    // sada p pokazuje na host[:port]/...

    // Nađi početak path-a
    const char *slash = strchr(p, '/');
    const char *host_end = slash ? slash : (p + strlen(p));

    // Izdvoji host[:port]
    char *hostport = (char *)malloc((size_t)(host_end - p) + 1);
    if (!hostport)
        return -1;
    memcpy(hostport, p, (size_t)(host_end - p));
    hostport[host_end - p] = '\0';

    // Default path
    const char *path = slash ? slash : "/";

    // Proveri port
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

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s http://host[:port]/path\n", argv[0]);
        return 1;
    }

    return 0;
}
