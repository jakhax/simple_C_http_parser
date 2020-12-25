#include "common.h"
#include <ctype.h>
#include "http_response.h"

static int strcicmp(char const* a, char const* b);

int http_response_init(http_response_t* r) {
    if (!r) {
        log_error("http_response cannot be null");
        return HTTPS_CLIENT_INVALID_PARAMETER;
    }

    r->method = 0;
    r->status_code = 0;
    r->headers_count = 0;
    r->concat_header_field = false;
    r->concat_header_value = false;
    r->expected_content_length = 0;
    r->is_chunked = false;
    r->is_message_completed = false;
    r->body_length = 0;
    r->body_offset = 0;
    r->body = NULL;
    r->status_message = NULL;
    r->url = NULL;

    return HTTPS_CLIENT_OK;
}

void http_response_free(http_response_t* r) {

    if (r->body != NULL) {
        free(r->body);
    }

    if (r->url) {
        free(r->url);
    }

    if (r->status_message) {
        free(r->status_message);
    }

    for (uint16_t ix = 0; ix < r->headers_count; ix++) {

        if (r->header_fields[ix]) {
            free(r->header_fields[ix]);
        }
        if (r->header_values[ix]) {
            free(r->header_fields[ix]);
        }
    }
}

int http_response_set_status(http_response_t* r, int a_status_code, char* a_status_message, size_t len) {
    r->status_code = a_status_code;

    r->status_message = (char*)malloc(len + 1);
    if (r->status_message == NULL) {
        return HTTPS_CLIENT_NO_MEMORY;
    }

    memcpy(r->status_message, a_status_message, len);
    r->status_message[len] = '\0';

    return HTTPS_CLIENT_OK;
}

int http_response_set_url(http_response_t* r, const char* url, size_t len) {

    r->url = (char*)malloc(len);

    if (!r->url) {
        return HTTPS_CLIENT_NO_MEMORY;
    }

    return HTTPS_CLIENT_OK;
}

void http_response_set_method(http_response_t* r, int method) {
    r->method = method;
}

int http_response_set_body(http_response_t* r, const char* at, uint32_t length) {
    // Connection: close, could not specify Content-Length, nor chunked... So do it like this:
    if (r->expected_content_length == 0 && length > 0) {
        r->is_chunked = true;
    }

    // only malloc when this fn is called, so we don't alloc when body callback's are enabled
    if (r->body == NULL && !r->is_chunked) {
        r->body = (char*)malloc(r->expected_content_length);
    }

    if (r->is_chunked) {
        if (r->body == NULL) {
            r->body = (char*)malloc(length);
        } else {
            char* original_body = r->body;
            r->body = (char*)realloc(r->body, r->body_offset + length);
            if (r->body == NULL) {
                free(original_body);
                return HTTPS_CLIENT_NO_MEMORY;
            }
        }
    }

    memcpy(r->body + r->body_offset, at, length);

    r->body_offset += length;

    return HTTPS_CLIENT_OK;
}


int http_response_set_header_field(http_response_t* r, const char* field, size_t field_len) {
    r->concat_header_value = false;

    uint16_t count = r->headers_count;

    // headers can be chunked
    if (r->concat_header_field) {
        if (!r->header_fields[count]) {
            log_error("cant concat field to null!");
            return HTTPS_CLIENT_INVALID_PARAMETER;
        }

        char* prev =  r->header_fields[count];
        size_t prev_size = strlen(r->header_fields[count]);
        r->header_fields[count] = (char*)realloc((char*)r->header_fields[count], prev_size + field_len + 1);
        if (r->header_fields[count]) {
            free(prev);
            log_error("unable to realloc header field");
            return HTTPS_CLIENT_NO_MEMORY;
        }
        memcpy((r->header_fields[count] + prev_size), field, field_len);
        r->header_fields[count][prev_size + field_len] = '\0';

    } else {
        if (r->headers_count >= MAX_HEADERS_COUNT - 1) {
            log_error("max headers count reached");
            return HTTPS_CLIENT_NO_MEMORY;
        }

        r->header_fields[count] = (char*)malloc(field_len + 1);
        if (r->header_fields[count]) {
            log_error("unable to allocate header field");
            return HTTPS_CLIENT_NO_MEMORY;
        }
        r->header_fields[count][field_len] = '\0';

        memcpy(r->header_fields[count], field, field_len);
    }

    r->concat_header_field = true;

    return HTTPS_CLIENT_OK;
}


int http_response_set_header_value(http_response_t* r, const char* value,  size_t value_len) {
    r->concat_header_field = false;

    uint16_t count = r->headers_count;

    // headers can be chunked
    if (r->concat_header_value) {
        if (!r->header_values[count]) {
            log_error("cant concat value to null!");
            return HTTPS_CLIENT_INVALID_PARAMETER;
        }

        char* prev =  r->header_values[count - 1];
        size_t prev_size = strlen(r->header_values[count - 1]);
        r->header_values[count - 1] = (char*)realloc(r->header_values[count - 1], value_len + 1);
        if (r->header_values[count - 1]) {
            free(prev);
            log_error("unable to realloc header value");
            return HTTPS_CLIENT_NO_MEMORY;
        }

        memcpy((r->header_values[count - 1] + prev_size), value, value_len);
        r->header_values[count - 1][prev_size + value_len] = '\0';

    } else {
        if (r->headers_count >= MAX_HEADERS_COUNT - 1) {
            log_error("max headers count reached");
            return HTTPS_CLIENT_NO_MEMORY;
        }

        r->header_values[count] = (char*)malloc(value_len + 1);
        if (r->header_values[count]) {
            log_error("unable to allocate header value");
            return HTTPS_CLIENT_NO_MEMORY;
        }
        r->header_values[count][value_len] = '\0';

        memcpy(r->header_values[count], value, value_len);
        r->headers_count++;
    }

    r->concat_header_value = true;

    return HTTPS_CLIENT_OK;
}


int http_response_set_headers_complete(http_response_t* r) {
    for (uint16_t ix = 0; ix < r->headers_count; ix++) {
        if (strcicmp(r->header_fields[ix], "content-length") == 0) {
            int len = 0;
            if (!sscanf(r->header_values[ix], "%d", &len)) {
                log_error("unable to parse content-length value");
                return HTTPS_CLIENT_INVALID_PARAMETER;
            }
            r->expected_content_length = (uint32_t)len;
            break;
        }
    }

    return HTTPS_CLIENT_OK;
}


void http_response_increase_body_length(http_response_t* r, uint32_t length) {
    r->body_length += length;
}

void http_response_set_chunked(http_response_t* r) {
    r->is_chunked = true;
}

void http_response_set_message_complete(http_response_t* r) {
    r->is_message_completed = true;
}


static int strcicmp(char const* a, char const* b) {
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a) {
            return d;
        }
    }
}

