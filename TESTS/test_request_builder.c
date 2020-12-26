#include "test_common.h"
#include "http_parser.h"
#include "http_parse_url.h"
#include "request_builder.h"

#define TEST_NAME "REQUEST_BUILDER"

int test_set_method(void) {
    info("+++ test_set_method() +++");

    request_builder_t rb;

    int ret = request_builder_init(&rb);
    if (ret != 0) {
        error("request_builder_init() Err %d", ret);
        return ret;
    }

    request_builder_set_method(&rb, HTTP_POST);

    if (rb.method != HTTP_POST) {
        error("method: got %d expected %d", rb.method, HTTP_POST);
        return -1;
    }

    request_builder_free(&rb);
    return 0;
}

int test_set_header(void) {
    info("+++ test_set_header() +++");
    int ret = 0;
    request_builder_t rb;

    ret = request_builder_init(&rb);
    if (ret != 0) {
        error("request_builder_init() Err %d", ret);
        return ret;
    }
    uint16_t headers_count = 3;

    int content_length = 65;
    char hf[headers_count][100];
    char hv[headers_count][100];

    sprintf(hf[0], "%s", "Content-Type");
    sprintf(hv[0], "%s", "application/json");

    sprintf(hf[1], "%s", "Content-Length");
    sprintf(hv[1], "%d", content_length);

    sprintf(hf[2], "%s", "Transfer-Encoding");
    sprintf(hv[2], "%s", "Chunked");

    for (uint16_t i = 0; i < headers_count; i++) {
        request_builder_set_header(&rb, hf[i], strlen(hf[i]), hv[i], strlen(hv[i]));
    }

    if (rb.headers_count != headers_count) {
        error("header_count: got %u expected %u", rb.headers_count, headers_count);
        ret = -1;
        goto exit;
    }

    for (uint16_t i = 0; i < headers_count; i++) {
        if (strcmp(rb.header_fields[i], hf[i]) != 0) {
            error("header field %u: got %s expected %s", i, rb.header_fields[i], hf[i]);
            ret = -1;
            goto exit;
        }

        if (strcmp(rb.header_values[i], hv[i]) != 0) {
            error("header value %u: got %s expected %s", i, rb.header_values[i], hv[i]);
            ret = -1;
            goto exit;
        }
    }

exit:
    request_builder_free(&rb);

    return ret;
}

int test_has_header(void) {
    info("+++ test_has_header() +++");

    request_builder_t rb;

    int ret = request_builder_init(&rb);
    if (ret != 0) {
        error("request_builder_init() Err %d", ret);
        return ret;
    }

    const char* hf = "Content-Type";
    const char* hv = "application/json";
    request_builder_set_header(&rb, (char*)hf, strlen(hf), (char*)hv, strlen(hv));

    bool r;
    if ((r = request_builder_has_header(&rb, hf, hv)) != true) {
        error("has_header: got %d expected %d", r, true);
        ret = -1;
        goto exit;
    }

exit:
    request_builder_free(&rb);
    return ret;

}

int test_set_parsed_url(void) {
    info("+++ test_set_parsed_url() +++");

    const char* host = "api.google.com";
    char url[100];
    sprintf(url, "https://%s", host);

    parse_url_t parse_url;
    int ret = 0;
    ret = parse_url_init(&parse_url);
    if (ret != 0) {
        error("parse_url_init ERR %d", ret);
        return ret;
    }

    ret = parse_url_string(&parse_url, (const char*)url);
    if (ret != 0) {
        error("parse_url_string ERR %d", ret);
        return ret;
    }

    request_builder_t rb;
    ret = request_builder_init(&rb);
    if (ret != 0) {
        error("request_builder_init() Err %d", ret);
        return ret;
    }
    ret = request_builder_set_parsed_url(&rb, &parse_url);
    if (ret != 0) {
        error("request_builder_set_parsed_url() Err %d", ret);
        goto exit;
    }
    const char* hf = "Host";
    if (request_builder_has_header(&rb, hf, host) != true) {
        error("has_header(%s, %s) returned false", hf, host);
        ret = -1;
        goto exit;
    }

exit:
    request_builder_free(&rb);
    parse_url_free(&parse_url);
    return ret;
}

int test_build(void) {
    info("+++ test_build() +++");
    const char* url = "https://example.com/login/";

    const char* request_tmpl = "POST /login/ HTTP/1.1\r\n"
                               "Host: example.com\r\n"
                               "Connection: keep-alive\r\n"
                               "Content-Type: application/json\r\n"
                               "Content-Length: %d\r\n"
                               "\r\n"
                               "%s";
    const char* payload = "{\"username\":\"user\",\"password\":\"password\"}";
    char request[256];
    sprintf(request, request_tmpl, (int)strlen(payload), payload);


    uint16_t headers_count = 2;
    char hf[headers_count][100];
    char hv[headers_count][100];

    sprintf(hf[0], "%s", "Connection");
    sprintf(hv[0], "%s", "keep-alive");

    sprintf(hf[1], "%s", "Content-Type");
    sprintf(hv[1], "%s", "application/json");

    parse_url_t parse_url;
    int ret = 0;
    ret = parse_url_init(&parse_url);
    if (ret != 0) {
        error("parse_url_init ERR %d", ret);
        return ret;
    }

    ret = parse_url_string(&parse_url, url);
    if (ret != 0) {
        error("parse_url_string ERR %d", ret);
        return ret;
    }

    request_builder_t rb;
    ret = request_builder_init(&rb);
    if (ret != 0) {
        error("request_builder_init() Err %d", ret);
        return ret;
    }
    ret = request_builder_set_parsed_url(&rb, &parse_url);
    if (ret != 0) {
        error("request_builder_set_parsed_url() Err %d", ret);
        goto exit;
    }

    request_builder_set_method(&rb, HTTP_POST);

    for (uint16_t i = 0; i < headers_count; i++) {
        request_builder_set_header(&rb, hf[i], strlen(hf[i]), hv[i], strlen(hv[i]));
    }


    uint32_t size = 0;

    char* body =  request_builder_build(&rb, (const void*)payload, (uint32_t)strlen(payload), &size, false);
    if ((unsigned long)strlen(request) != (unsigned long)size) {
        error("request_size: got %lu expected %lu", (unsigned long)size, (unsigned long)strlen(request));
        ret = -1;
        goto exit;
    }

    if (memcmp((const void*)body, (const void*)request, strlen(request)) != 0) {
        error("expected:\r\n<%s>\r\ngot:\r\n<%.*s>\r\n", request, (int)size, body);
        ret = -1;
        goto exit;
    }

exit:
    request_builder_free(&rb);
    parse_url_free(&parse_url);
    return ret;
}



int main() {
    int tests_count = 5;

    int (*tests[tests_count])(void);
    tests[0] = test_set_method;
    tests[1] = test_set_header;
    tests[2] = test_has_header;
    tests[3] = test_set_parsed_url;
    tests[4] = test_build;

    return run_tests((const char*)TEST_NAME, tests, tests_count);
}