
#ifndef _C_HTTPS_CLIENT_HTTP_RESPONSE_H_
#define _C_HTTPS_CLIENT_HTTP_RESPONSE_H_

#include "common.h"


typedef struct http_response {

    int status_code;
    char* status_message;
    char* url;
    int method;

    char** header_fields;
    char** header_values;
    uint16_t headers_count;

    bool concat_header_field;
    bool concat_header_value;

    uint32_t expected_content_length;

    bool is_chunked;

    bool is_message_completed;

    char* body;
    uint32_t body_length;
    uint32_t body_offset;

} http_response_t;

/**
 * @brief allocates and sets defaults on
 * @return 0 on success and < 0 on error
 */
int http_response_init(http_response_t* r);

/**
 * @brief de-allocates http_response_t items
 */
void http_response_free(http_response_t* r);

/**
 * @brief set response status code and message
 * @return 0 on success and < 0 on error
 */
int http_response_set_status(http_response_t* r, int a_status_code, char* a_status_message, size_t len);

/**
 * @brief set response url
 * @return 0 on success and < 0 on error
 */
int http_response_set_url(http_response_t* r, const char* url, size_t len);

/**
 * @brief set response method
 * @return 0 on success and < 0 on error
 */
void http_response_set_method(http_response_t* r, int method);

/**
 * @brief set response body
 * @return 0 on success and < 0 on error
 */
int http_response_set_body(http_response_t* r, const char* at, uint32_t length);

/**
 * @brief set response header field
 * supports chunked
 * @return 0 on success and < 0 on error
 */
int http_response_set_header_field(http_response_t* r, const char* field, size_t field_len);

/**
 * @brief set response header value
 * supports chunked header
 * @return 0 on success and < 0 on error
 */
int http_response_set_header_value(http_response_t* r, const char* value, size_t value_len);


/**
 * @brief find header by key
 * @return header's key, a non negative int on success and -1 if header is not found
 */
int http_response_find_header(http_response_t* r, const char* key);


/**
 * @brief set response status code and message
 * @return 0 on success and < 0 on error
 */
int http_response_set_headers_complete(http_response_t* r);

/**
 * @brief increments bdy_length
 */
void http_response_increase_body_length(http_response_t* r, uint32_t length);

/**
 * @brief set chunked to true
 */
void http_response_set_chunked(http_response_t* r);

/**
 * @brief set message as complete
 */
void http_response_set_message_complete(http_response_t* r);


#endif