#ifndef _HTTPS_CLIENT_PARSED_URL_H__
#define _HTTPS_CLIENT_PARSED_URL_H__

#include "common.h"

typedef struct parse_url {

    uint16_t port;
    char* schema;
    char* host;
    char* path;
    char* query;
    char* userinfo;

} parse_url_t;


/**
 * \brief Allocates parse_url_t
 * @param parsed_url Pointer to the url to be allocated
 * @return 0 on success and -1 if it fails to allocate
 */
int parse_url_init(parse_url_t* parsed_url);

/**
 * \brief Parses @param url to @param parsed_url
 * @param parsed_url Pointer to parse_url_t, the parsed url will be written here
 * @param url Pointer to the url string to be parsed, must be null terminated
 * @return 0 on success and -1 if it fails to allocate
 */
int parse_url_string(parse_url_t* parsed_url, const char* url);

/**
 * \brief Frees @param parsed_url
 * @param parsed_url parse_url_t to be freed
 */
void parse_url_free(parse_url_t* parsed_url);


#endif
