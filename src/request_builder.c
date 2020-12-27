#include "common.h"
#include "http_parser.h"
#include "request_builder.h"

int request_builder_init(request_builder_t* rb) {
    if (!rb) {
        log_error("request_builder cannot be null");
        return HTTPS_CLIENT_INVALID_PARAMETER;
    }

    rb->parsed_url = NULL;
    rb->headers_count = 0;
    rb->method = -1;

    return HTTPS_CLIENT_OK;
}


void request_builder_free(request_builder_t* rb) {

    for (uint16_t ix = 0; ix < rb->headers_count; ix++) {

        if (rb->header_fields[ix]) {
            free(rb->header_fields[ix]);
        }
        if (rb->header_values[ix]) {
            free(rb->header_values[ix]);
        }
    }

    //we did not allocate this
    rb->parsed_url = NULL;
}

void request_builder_set_method(request_builder_t* request_builder, int method) {

    request_builder->method = method;
}

int request_builder_set_parsed_url(request_builder_t* rb, parse_url_t* parsed_url) {
    if (!parsed_url) {
        log_error("parsed_url cannot be null");
        return HTTPS_CLIENT_INVALID_PARAMETER;
    }

    char* host = (char*)malloc(strlen(parsed_url->host) + 10); //max port str len is 5 (2^16)

    if (strcmp(parsed_url->schema, "http") == 0 && parsed_url->port != 80) {
        sprintf(host, "%s:%u", parsed_url->host, parsed_url->port);
    } else if (strcmp(parsed_url->schema, "https") == 0 && parsed_url->port != 443) {
        sprintf(host, "%s:%u", parsed_url->host, parsed_url->port);
    } else if (strcmp(parsed_url->schema, "ws") == 0 && parsed_url->port != 80) {
        sprintf(host, "%s:%u", parsed_url->host, parsed_url->port);
    } else if (strcmp(parsed_url->schema, "wss") == 0 && parsed_url->port != 443) {
        sprintf(host, "%s:%u", parsed_url->host, parsed_url->port);
    } else {
        sprintf(host, "%s", parsed_url->host);
    }

    const char* host_header = "Host";
    int r = request_builder_set_header(rb, (char*)host_header, strlen(host_header), host, strlen(host));
    free(host);

    if (r == HTTPS_CLIENT_OK) {
        //should we do a deep copy ?
        rb->parsed_url = parsed_url;
    }
    return r;
}

int request_builder_set_header(request_builder_t* rb,  char* hfield, size_t hf_len, char* hvalue, size_t hv_len) {

    uint16_t count = rb->headers_count;
    rb->header_fields[count] = (char*)malloc(hf_len + 1);
    if (!rb->header_fields[count]) {
        log_error("unable to allocate header field");
        return HTTPS_CLIENT_NO_MEMORY;
    }
    memcpy(rb->header_fields[count], hfield, hf_len);
    rb->header_fields[count][hf_len] = '\0';

    rb->header_values[count] = (char*)malloc(hv_len + 1);
    if (!rb->header_values[count]) {
        log_error("unable to allocate header value");
        return HTTPS_CLIENT_NO_MEMORY;
    }

    memcpy(rb->header_values[count], hvalue, hv_len);
    rb->header_values[count][hv_len] = '\0';

    rb->headers_count++;
    return HTTPS_CLIENT_OK;
}


bool request_builder_has_header(request_builder_t* rb, const char* key, const char* value) {
    for (uint16_t ix = 0; ix < rb->headers_count; ix++) {
        if (!rb->header_fields[ix]) {
            continue;
        }

        if (strcmp(rb->header_fields[ix], key) == 0) { // key matches
            if (value == NULL || (strcmp(rb->header_values[ix], value) == 0)) { // value is NULL or matches
                return true;
            }
        }
    }

    return false;
}


char* request_builder_build(request_builder_t* rb, const void* body, uint32_t body_size, uint32_t* ssize, bool skip_content_length) {

    const char* method_str = http_method_str(rb->method);

    bool is_chunked = request_builder_has_header(rb, "Transfer-Encoding", "chunked");

    if (!is_chunked && (rb->method == HTTP_POST || rb->method == HTTP_PUT || rb->method == HTTP_DELETE || body_size > 0)) {
        char buffer[10];
        snprintf(buffer, 10, "%lu", (unsigned long)body_size);
        const char* clh = "Content-Length";
        request_builder_set_header(rb, (char*)clh, strlen(clh), buffer, strlen(buffer));
    }

    uint32_t size = 0;
    // first line is METHOD PATH+QUERY HTTP/1.1\r\n
    size += strlen(method_str) + 1 + strlen(rb->parsed_url->path) + (strlen(rb->parsed_url->query) ? strlen(rb->parsed_url->query) + 1 : 0) + 1 + 8 + 2;

    for (uint16_t i = 0; i != rb->headers_count; i++) {
        // line is KEY: VALUE\r\n
        size += strlen(rb->header_fields[i]) + 1 + 1 + strlen(rb->header_values[i]) + 2;
    }

    // then the body, first an extra newline
    size += 2;

    if (!is_chunked) {
        // body
        size += body_size;
    }

    // Now let's print it
    char* req = (char*)calloc((size_t)size + 1, 1);
    char* originalReq = req;

    if (strlen(rb->parsed_url->query)) {
        sprintf(req, "%s %s?%s HTTP/1.1\r\n", method_str, rb->parsed_url->path, rb->parsed_url->query);
    } else {
        sprintf(req, "%s %s%s HTTP/1.1\r\n", method_str, rb->parsed_url->path, rb->parsed_url->query);
    }
    req += strlen(method_str) + 1 + strlen(rb->parsed_url->path) + (strlen(rb->parsed_url->query) ? strlen(rb->parsed_url->query) + 1 : 0) + 1 + 8 + 2;

    for (uint16_t i = 0; i != rb->headers_count; i++) {
        sprintf(req, "%s: %s\r\n", rb->header_fields[i], rb->header_values[i]);
        req += strlen(rb->header_fields[i]) + 1 + 1 + strlen(rb->header_values[i])  + 2;
    }


    sprintf(req, "\r\n");
    req += 2;

    if (body_size > 0) {
        memcpy(req, body, body_size);
    }
    req += body_size;

#if DEBUG_REQUEST
    printf("\r\n+++++ BEGIN REQUEST +++++\r\n");
    printf("\r\n%s\r\n", originalReq);
    printf("----- END REQUEST -----\r\n\r\n");
#endif

    *ssize = size;

    return originalReq;
}