#include "http_parse_url.h"

int main() {
    printf("hello world\r\n");
    const char* url = "https://api.staging.sandbox.com";
    parse_url_t parse_url;
    parse_url_init(&parse_url);
    parse_url_string(&parse_url, url);
    printf("HOST=%s\r\nPORT=%u\r\nSCHEMA=%s\r\nPATH=%s\r\n",
           parse_url.host, parse_url.port,
           parse_url.schema, parse_url.path);
    parse_url_free(&parse_url);
    return -1;
}