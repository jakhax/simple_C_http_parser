#include "test_common.h"
#include "http_parse_url.h"

#define TEST_NAME "HTTP_PARSE_URL"

int test_get_port(void) {
    info("+++ test_get_port() +++");
    uint16_t port = 9991;
    char url[100];
    const char* tmpl = "https://127.0.0.1:%u";
    sprintf(url, tmpl, port);


    int ret = 0;
    parse_url_t parse_url;
    if ((ret = parse_url_init(&parse_url)) != HTTPS_CLIENT_OK) {
        error("parse_url_init err %d", ret);
        return -1;
    }

    if ((ret = parse_url_string(&parse_url, url)) != HTTPS_CLIENT_OK) {
        return ret;
    }

    if (parse_url.port != port) {
        error("expected %u got %u", port, parse_url.port);
        return -1;
    }

    parse_url_free(&parse_url);

    return 0;
}

int test_get_host(void) {
    info("+++ test_get_host() +++");
    char* hosts[5];
    hosts[0] = "google.com";
    hosts[1] = "127.0.0.1";
    hosts[2] = "192.168.0.1";
    hosts[3] = "api.data.something.dev";
    hosts[4] = "a-l0ng.but.st1ll.val1d.h0stn4m3";

    char url[100];
    const char* tmpl = "https://%s/some-resource?q=1";

    for (int i = 0; i < 5; i++) {
        memset(url, 0, 100);
        sprintf(url, tmpl, hosts[i]);

        int ret = 0;
        parse_url_t parse_url;
        if ((ret = parse_url_init(&parse_url)) != HTTPS_CLIENT_OK) {
            error("parse_url_init err %d", ret);
            return -1;
        }

        if ((ret = parse_url_string(&parse_url, url)) != HTTPS_CLIENT_OK) {
            error("parse_url_string err %d", ret);
            return ret;
        }
        if (strcmp(hosts[i], parse_url.host)) {
            error("expected %s got %s", hosts[i], parse_url.host);
            return -1;
        }

        parse_url_free(&parse_url);

    }

    return 0;
}

int test_get_schema(void) {
    info("+++ test_get_schema() +++");

    const char* scheme  = "http";
    const char* tmpl = "%s://google.com?system=ubuntu";
    char url[100];
    sprintf(url, tmpl, scheme);

    int ret = 0;
    parse_url_t parse_url;
    if ((ret = parse_url_init(&parse_url)) != HTTPS_CLIENT_OK) {
        error("parse_url_init err %d", ret);
        return -1;
    }

    if ((ret = parse_url_string(&parse_url, url)) != HTTPS_CLIENT_OK) {
        return ret;
    }

    if (strcmp(scheme, parse_url.schema)) {
        error("expected %s got %s", scheme, parse_url.schema);
        return -1;
    }

    parse_url_free(&parse_url);

    return 0;
}


int main() {
    int tests_count = 3;

    int (*tests[tests_count])(void);
    tests[0] = test_get_port;
    tests[1] = test_get_host;
    tests[2] = test_get_schema;

    return run_tests((const char*)TEST_NAME, tests, tests_count);
}