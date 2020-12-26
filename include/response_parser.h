
#ifndef _C_HTTPS_CLIENT_RESPONSE_PARSER_H_
#define _C_HTTPS_CLIENT_RESPONSE_PARSER_H_

#include "http_parser.h"
#include "http_response.h"

typedef void (*res_data_cb_t)(const char* at, uint32_t length);

typedef struct response_parser {
    http_response_t* res;
    res_data_cb_t body_cb;
    http_parser_settings* settings;
    http_parser* parser;

} response_parser_t;


/**
 * @brief allocates response_parser reources
 * @param response_parser_t* rp
 * @param res http_response_t to store parse response
 * @param body_cb can be set NULL
 */
int response_parser_init(response_parser_t* rp, http_response_t* res, res_data_cb_t body_cb);

/**
 * @brief frees response_parser_t resources
 */
void response_parser_free(response_parser_t* rp);

/**
 * @brief executes underlying http_parser
 *
 * @return the number of parsed bytes, parser error is set incase of an error
 */
uint32_t response_parser_execute(response_parser_t* rp, const char* buffer, uint32_t buffer_size);

/**
 * @brief finish parsing response
 */
void response_parser_finish(response_parser_t* rp);

/**
 * @brief get underlying parser error
 * @return int parser->http_errno
 */
int response_parser_get_err(response_parser_t* rp);


#endif // _HTTP_RESPONSE_PARSER_H_
