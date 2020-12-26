
#ifndef _C_HTTPS_CLIENT_REQUEST_BUILDER_H_
#define _C_HTTPS_CLIENT_REQUEST_BUILDER_H_

#include "common.h"
#include "http_parse_url.h"

#define DEBUG_REQUEST 1

typedef struct request_builder {
    int method;
    parse_url_t* parsed_url;
    char* header_values[MAX_HEADERS_COUNT];
    char* header_fields[MAX_HEADERS_COUNT];
    uint16_t headers_count;
} request_builder_t;


/**
 * @brief allocates request_builder_t items
 * @return 0 on success < 0 on error
 */
int request_builder_init(request_builder_t* rb);


/**
 * @brief frees allocated request_builder_t members
 */
void request_builder_free(request_builder_t* rb);


/**
 * @brief set request builder http method
 */
void request_builder_set_method(request_builder_t* request_builder, int method);

/**
 * @brief set parse_url_t
 * adds Host header
 * @return 0 on success < 0 on error
 */
int request_builder_set_parsed_url(request_builder_t* rb, parse_url_t* parsed_url);

/**
 * @brief set header
 * adds Host header
 * @return 0 on success < 0 on error
 */
int request_builder_set_header(request_builder_t* rb,  char* hfield, size_t hf_len, char* hvalue, size_t hv_len);

/**
 * @bried check if request builder has header
 * @param value can be NULL
 * @returns 1 on success and 0 if it  fails to find it
 */
bool request_builder_has_header(request_builder_t* rb, const char* key, const char* value);

/**
 * @brief creates a http request buffer from request_builder_t
 */
char* request_builder_build(request_builder_t* rb, const void* body, uint32_t body_size, uint32_t* size, bool skip_content_length);


#endif