
#include "http_parser.h"
#include "http_response.h"
#include "response_parser.h"

#define DEBUG_CALLBACKS 0

#if DEBUG_CALLBACKS
#define dbg_cb(x, ...) printf("[PARSER][CB] " x "\r\n", ##__VA_ARGS__)
#else
#define dbg_cb(x, ...)
#endif

static int rp_on_message_begin_callback(http_parser* parser) {
    dbg_cb("rp_on_message_begin_callback");
    response_parser_t* rp = ((response_parser_t*)parser->data);
    if (!rp) {
        return -1;
    }

    return 0;
}

static int rp_on_url_callback(http_parser* parser, const char* at, uint32_t length) {
    dbg_cb("rp_on_url_callback");
    response_parser_t* rp = ((response_parser_t*)parser->data);

    return http_response_set_url(rp->res, at, (size_t)length);

}

static int rp_on_status_callback(http_parser* parser, const char* at, uint32_t length) {
    dbg_cb("rp_on_status_callback(len=%lu)", (unsigned long)length);
    response_parser_t* rp = ((response_parser_t*)parser->data);

    return http_response_set_status(rp->res, parser->status_code, (char*)at, (size_t)length);
}

static int rp_on_header_field_callback(http_parser* parser, const char* at, uint32_t length) {
    dbg_cb("rp_on_header_field_callback(len=%lu)", (unsigned long)length);
    response_parser_t* rp = ((response_parser_t*)parser->data);
    return http_response_set_header_field(rp->res, at, (size_t)length);
}

static int rp_on_header_value_callback(http_parser* parser, const char* at, uint32_t length) {
    dbg_cb("rp_on_header_value_callback(len=%lu)", (unsigned long)length);
    response_parser_t* rp = ((response_parser_t*)parser->data);

    return http_response_set_header_value(rp->res, at, (size_t)length);
}

static int rp_on_headers_complete_callback(http_parser* parser) {
    dbg_cb("rp_on_headers_complete_callback");

    response_parser_t* rp = ((response_parser_t*)parser->data);

    int ret = http_response_set_headers_complete(rp->res);

    if (ret == 0) {
        http_response_set_method(rp->res, parser->method);
    }
    return  ret;
}

static int rp_on_body_callback(http_parser* parser, const char* at, uint32_t length) {
    dbg_cb("rp_on_body_callback(len=%lu)", (unsigned long)length);

    response_parser_t* rp = ((response_parser_t*)parser->data);

    http_response_increase_body_length(rp->res, length);

    if (rp->body_cb) {
        rp->body_cb(at, length);
        return 0;
    }

    return http_response_set_body(rp->res, at, (size_t)length);
}

static int rp_on_message_complete_callback(http_parser* parser) {
    dbg_cb("rp_on_message_complete_callback");

    response_parser_t* rp = ((response_parser_t*)parser->data);

    http_response_set_message_complete(rp->res);

    return 0;
}

static int rp_on_chunk_header_callback(http_parser* parser) {
    dbg_cb("rp_on_chunk_header_callback");

    response_parser_t* rp = ((response_parser_t*)parser->data);

    http_response_set_chunked(rp->res);

    return 0;
}

static int rp_on_chunk_complete_callback(http_parser* parser) {
    dbg_cb("rp_on_chunk_complete_callback");

    return 0;
}

int response_parser_init(response_parser_t* rp, http_response_t* res, res_data_cb_t body_cb) {

    if (!rp) {
        log_error("response_parser cannot be null");
        return HTTPS_CLIENT_INVALID_PARAMETER;
    }

    if (!res) {
        log_error("http_response cannot be null");
        return HTTPS_CLIENT_INVALID_PARAMETER;
    }

    http_parser_settings* settings = (http_parser_settings*)malloc(sizeof(http_parser_settings));

    if (!settings) {
        log_error("unable to allocate http_parser_settings");
        return HTTPS_CLIENT_NO_MEMORY;
    }

    settings->on_message_begin = rp_on_message_begin_callback;
    settings->on_url = rp_on_url_callback;
    settings->on_status = rp_on_status_callback;
    settings->on_header_field = rp_on_header_field_callback;
    settings->on_header_value = rp_on_header_value_callback;
    settings->on_headers_complete = rp_on_headers_complete_callback;
    settings->on_chunk_header = rp_on_chunk_header_callback;
    settings->on_chunk_complete = rp_on_chunk_complete_callback;
    settings->on_body = rp_on_body_callback;
    settings->on_message_complete = rp_on_message_complete_callback;

    http_parser* parser = (http_parser*)malloc(sizeof(http_parser));

    if (!settings) {
        log_error("unable to allocate http_parser_settings");
        return HTTPS_CLIENT_NO_MEMORY;
    }


    http_parser_init(parser, HTTP_RESPONSE);
    parser->data = (void*)rp;

    rp->parser = parser;
    rp->settings = settings;
    rp->res = res;

    return HTTPS_CLIENT_OK;
}

void response_parser_free(response_parser_t* rp) {
    if (rp->parser) {
        free(rp->parser);
        rp->parser = NULL;
    }

    if (rp->settings) {
        free(rp->settings);
        rp->settings = NULL;
    }
}


uint32_t response_parser_execute(response_parser_t* rp, const char* buffer, uint32_t buffer_size) {
    return http_parser_execute(rp->parser, rp->settings, buffer, buffer_size);
}

void response_parser_finish(response_parser_t* rp) {
    http_parser_execute(rp->parser, rp->settings, NULL, 0);
}

int response_parser_get_err(response_parser_t* rp) {
    return (int)rp->parser->http_errno;
}