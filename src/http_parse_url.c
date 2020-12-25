#include "http_parse_url.h"
#include "http_parser/http_parser.h"

static struct http_parser_url* h_parse_url;

int parse_url_init(parse_url_t* parse_url) {

    h_parse_url = malloc(sizeof(struct http_parser_url));

    if (!parse_url) {
        log_error("unable to allocate parse_url");
        return HTTPS_CLIENT_NO_MEMORY;
    }

    return HTTPS_CLIENT_OK;
}

int parse_url_string(parse_url_t* parse_url, const char* url) {
    if (!parse_url) {
        log_error("parse_url cannot be null");
        return -1;
    }

    http_parser_parse_url(url, strlen(url), 0, h_parse_url);

    for (size_t ix = 0; ix < UF_MAX; ix++) {
        char* value;
        if (h_parse_url->field_set & (1 << ix)) {
            value = calloc(h_parse_url->field_data[ix].len + 1, 1);
            memcpy(value, url + h_parse_url->field_data[ix].off,
                   h_parse_url->field_data[ix].len);
        } else {
            value = calloc(1, 1);
        }

        switch (ix) {
            case UF_SCHEMA:
                parse_url->schema   = value;
                break;
            case UF_HOST:
                parse_url->host     = value;
                break;
            case UF_PATH:
                parse_url->path     = value;
                break;
            case UF_QUERY:
                parse_url->query    = value;
                break;
            case UF_USERINFO:
                parse_url->userinfo = value;
                break;
            default:
                // PORT is already parsed, FRAGMENT is not relevant for HTTP requests
                free(value);
                break;
        }
    }

    parse_url->port = h_parse_url->port;
    if (!parse_url->port) {
        if (strcmp(parse_url->schema, "https") == 0 || strcmp(parse_url->schema, "wss") == 0) {
            parse_url->port = 443;
        } else {
            parse_url->port = 80;
        }
    }

    if (strcmp(parse_url->path, "") == 0) {
        free(parse_url->path);
        parse_url->path = calloc(2, 1);
        parse_url->path[0] = '/';
    }

    return HTTPS_CLIENT_OK;
}


void parse_url_free(parse_url_t* parse_url) {
    if (parse_url->schema) free(parse_url->schema);
    if (parse_url->host) free(parse_url->host);
    if (parse_url->path) free(parse_url->path);
    if (parse_url->query) free(parse_url->query);
    if (parse_url->userinfo) free(parse_url->userinfo);

    if (h_parse_url) {
        free(h_parse_url);
        h_parse_url = NULL;
    }

}
