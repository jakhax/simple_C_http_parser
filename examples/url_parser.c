#include <stdio.h>
#include <stdlib.h>
#include "http_parse_url.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./url_parser.0 <url>\r\n");
        return EXIT_FAILURE;
    }

    parse_url_t parse_url;

    if (parse_url_init(&parse_url) != 0) {
        printf("parse_url_init() ERR\r\n");
        return EXIT_FAILURE;
    }

    char* url = argv[1];

    if (parse_url_string(&parse_url, (const char*)url) != 0) {
        printf("parse_url_string(%s) ERR\r\n", url);
        return EXIT_FAILURE;
    }

    printf("HOST=%s\r\nPORT=%u\r\nSCHEMA=%s\r\nPATH=%s\r\nQUERY=%s\r\nUSER_INFO=%s\r\n",
           parse_url.host, parse_url.port, parse_url.schema,
           parse_url.path, parse_url.query, parse_url.userinfo);

    parse_url_free(&parse_url);

    return EXIT_SUCCESS;


}