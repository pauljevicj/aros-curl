#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s http://host[:port]/path\n", argv[0]);
        return 1;
    }	

    return 0;
}
