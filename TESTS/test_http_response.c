#include "test_common.h"
#include "http_parser.h"
#include "http_response.h"

#define TEST_NAME "HTTP_RESPONSE"


int test_set_status(void) {
    info("+++ test_set_status() +++");

    int status_code = 400;
    const char* status_message = "Bad Request";

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    ret = http_response_set_status(&res, status_code, (char*)status_message, strlen(status_message));
    if (ret != HTTPS_CLIENT_OK) {
        error("http_response_set_status() %d", ret);
        return ret;
    }

    if (res.status_code != status_code) {
        error("status code: got %d expected %d", res.status_code, status_code);
        return -1;
    }

    if (strcmp((const char*)res.status_message, status_message) != 0) {
        error("status message: got %s expected %s", res.status_message, status_message);
        return -1;
    }

    http_response_free(&res);

    return 0;
}


int test_set_url(void) {
    info("+++ test_set_url() +++");

    const char* url = "https://google.com";

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    ret = http_response_set_url(&res, url, strlen(url));
    if (ret != HTTPS_CLIENT_OK) {
        error("http_response_set_status() %d", ret);
        return ret;
    }


    if (strcmp((const char*)res.url, url) != 0) {
        error("url: got %s expected %s", res.url, url);
        return -1;
    }

    http_response_free(&res);

    return 0;
}



int test_set_method(void) {
    info("+++ test_set_method() +++");

    http_response_t res;
    int http_method = HTTP_POST;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    http_response_set_method(&res, http_method);

    if (res.method != http_method) {
        error("method: got %d expected %d", res.method, http_method);
        return -1;
    }

    http_response_free(&res);

    return 0;
}

int test_set_chunked(void) {
    info("+++ test_set_chunked() +++");

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    http_response_set_chunked(&res);

    if (res.is_chunked != true) {
        error("is_chunked: got %d expected %d", res.is_chunked, true);
        return -1;
    }

    http_response_free(&res);

    return 0;
}


int test_set_message_complete(void) {
    info("+++ test_set_message_complete() +++");


    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    http_response_set_message_complete(&res);

    if (res.is_message_completed != true) {
        error("is_message_completed: got %d expected %d", res.is_message_completed, true);
        return -1;
    }

    http_response_free(&res);

    return 0;
}

int test_increase_body_length(void) {
    info("+++ test_increase_body_length() +++");

    int body_len = 50;

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    http_response_increase_body_length(&res, body_len);

    if (res.body_length != body_len) {
        error("body_length: got %d expected %d", res.body_length, body_len);
        return -1;
    }

    http_response_free(&res);

    return 0;
}

int test_set_headers_complete(void) {
    info("+++ test_set_headers_complete() +++");

    int content_length = 100;
    const char* h_field = "Content-Length";

    char h_value[10];
    sprintf(h_value, "%d", content_length);

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    http_response_set_header_field(&res, h_field, strlen(h_field));

    http_response_set_header_value(&res, (const char*)h_value, strlen(h_value));

    http_response_set_headers_complete(&res);

    if (res.expected_content_length != content_length) {
        error("expected_content_length: got %d expected %d", res.expected_content_length, content_length);
        return -1;
    }

    http_response_free(&res);

    return 0;
}


int test_set_header(void) {
    info("+++ test_set_header() +++");

    char hf[3][100];
    char hv[3][100];

    strncpy(hf[0], "Content-Type", strlen("Content-Type"));
    strncpy(hv[0], "application/json", strlen("application/json"));

    strncpy(hf[1], "Content-Length", strlen("Content-Length"));
    strncpy(hv[1], "65", strlen("65"));

    strncpy(hf[2], "Content-Length", strlen("Content-Length"));
    strncpy(hv[2], "65", strlen("65"));

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    for (int i = 0; i < 3; i++) {
        http_response_set_header_field(&res, (const char*)hf[i], strlen(hf[i]));
        http_response_set_header_value(&res, (const char*)hv[i], strlen(hv[i]));
    }

    for (int i = 0; i < 3; i++) {
        if (strcmp(res.header_fields[i], hf[i]) != 0) {
            error("header_field: got %s expected %s", res.header_fields[i], hf[i]);
            return -1;
        }

        if (strcmp(res.header_values[i], hv[i]) != 0) {
            error("header_value: got %s expected %s", res.header_values[i], hv[i]);
            return -1;
        }
    }

    http_response_set_headers_complete(&res);
    int content_length = 0;
    sscanf(hv[2], "%d", &content_length);
    if (res.expected_content_length != content_length) {
        error("expected_content_length: got %d expected %d", res.expected_content_length, content_length);
        return -1;
    }

    http_response_free(&res);

    return 0;
}

int test_set_header_chunked(void) {
    info("+++ test_set_header_chunked() +++");

    int content_length = 100;
    char* h_f0 = "Content";
    char* h_f1 = "-Length";

    char h_v[10];
    sprintf(h_v, "%d", content_length);

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }

    http_response_set_header_field(&res, (const char*)h_f0, strlen(h_f0));

    http_response_set_header_field(&res, (const char*)h_f1, strlen(h_f1));

    http_response_set_header_value(&res, (const char*)h_v, strlen(h_v));

    http_response_set_headers_complete(&res);

    if (res.expected_content_length != content_length) {
        error("expected_content_length: got %d expected %d", res.expected_content_length, content_length);
        return -1;
    }

    char* h_f3 = "Content-Type";
    const char* content_type = "application/json";
    char h_v1[11];
    memcpy(h_v1, content_type, 10);
    h_v1[10] = '\0';
    char h_v2[50];
    strncpy(h_v2, content_type + 10, strlen(content_type) - 10);


    http_response_set_header_field(&res, (const char*)h_f3, strlen(h_f3));

    http_response_set_header_value(&res, (const char*)h_v1, strlen(h_v1));

    http_response_set_header_value(&res, (const char*)h_v2, strlen(h_v2));


    if (strcmp(res.header_values[1], content_type) != 0) {
        error("header_value: got %s expected %s", res.header_values[1], content_type);
        return -1;
    }

    http_response_free(&res);

    return 0;
}

int test_set_body(void) {
    info("+++ test_set_body() +++");

    const char* body = "00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    size_t len = strlen(body);

    int content_length = 100;
    const char* h_field = "Content-Length";

    char h_value[10];
    sprintf(h_value, "%d", content_length);

    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }
    http_response_increase_body_length(&res, (uint32_t)len);
    http_response_set_body(&res, body, (uint32_t)len);

    if (res.body_length != (uint32_t)len) {
        error("body_length: got %d expected %ld", res.body_length, len);
        return -1;
    }


    if (memcmp((void*) body, (void*)res.body, len) != 0) {
        error("body: got %s expected %s", res.body, body);
        return -1;
    }

    http_response_free(&res);



    return 0;
}

int test_set_body_chunked(void) {
    info("+++ test_set_body_chunked() +++");

    const char* body_80_bytes = "dhMFURUvf0S1naQrPaqCJp4snfZtPaF6QZpIXA3NHf2PUjo6XjXg5ZzDHjbc0AvxoREhI3VQUhUjNM8w3yXzBoLtZ";
    char body_c1[41];
    char body_c2[100];


    memcpy(body_c1, body_80_bytes, 40);
    body_c1[40] = '\0';

    strncpy(body_c2, body_80_bytes + 40, strlen(body_80_bytes) - 40);

    size_t len = strlen(body_80_bytes);


    http_response_t res;
    int ret = 0;
    if ((ret = http_response_init(&res)) != HTTPS_CLIENT_OK) {
        error("http_response_init() %d", ret);
        return ret;
    }
    http_response_increase_body_length(&res, (uint32_t)(strlen(body_c1)));
    http_response_set_body(&res, (const char*)body_c1, (uint32_t)(strlen(body_c1)));
    http_response_increase_body_length(&res, (uint32_t)(strlen(body_c2)));
    http_response_set_body(&res, (const char*)body_c2, (uint32_t)(strlen(body_c2)));


    if (res.body_length != (uint32_t)len) {
        error("body_length: got %d expected %ld", res.body_length, len);
        return -1;
    }

    if (memcmp((void*) body_80_bytes, (void*)res.body, len) != 0) {
        error("body: got %s expected %s", res.body, body_80_bytes);
        return -1;
    }

    http_response_free(&res);

    return -0;
}


int main() {

    int tests_count = 11;

    int (*tests[tests_count])(void);
    tests[0] = test_set_status;
    tests[1] = test_set_url;
    tests[2] = test_set_method;
    tests[3] = test_set_chunked;
    tests[4] = test_set_message_complete;
    tests[5] = test_increase_body_length;
    tests[6] = test_set_headers_complete;
    tests[7] = test_set_header;
    tests[8] = test_set_header_chunked;
    tests[9] = test_set_body;
    tests[10] = test_set_body_chunked;

    run_tests((const char*)TEST_NAME, tests, tests_count);

    return 0;
}