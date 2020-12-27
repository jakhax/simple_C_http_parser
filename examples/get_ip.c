/**
 * uses icanhazip.com to get your IP by sending a http get request
 * BUILD: make get_ip
 * USAGE: ./get_ip.o
 */
#include "example_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

#include "http_response.h"
#include "response_parser.h"
#include "request_builder.h"
#include "http_parse_url.h"

#define URL "http://icanhazip.com"

int main(int argc, char* argv[]) {

    int ret = 0;
    struct hostent* server;
    struct sockaddr_in serv_addr;
    int sockfd = 0;
    int bytes, sent, received, total;
    char response[4096];

    //http
    parse_url_t parsed_url;
    request_builder_t request_builder;
    http_response_t http_response;
    response_parser_t response_parser;

    if ((ret = parse_url_init(&parsed_url)) != 0) {
        error("parse_url_init() %d", ret);
        return -1;
    }

    if ((ret = request_builder_init(&request_builder)) != 0) {
        error("request_builder_init() %d", ret);
        return -1;
    }

    if ((ret = http_response_init(&http_response)) != 0) {
        error("http_response_init() %d", ret);
        return -1;
    }

    if ((ret = response_parser_init(&response_parser, &http_response, NULL)) != 0) {
        error("response_parser_init() %d", ret);
        return -1;
    }

    //parse url
    if ((ret = parse_url_string(&parsed_url, URL)) != 0) {
        error("parse_url_string(%s) %d", URL, ret);
        goto exit;
    }

    info("parsed_url, HOST=%s PORT=%u", parsed_url.host, parsed_url.port);

    /* create the socket */
    info("creating socket");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
        ret = -1;
        goto exit;
    }

    /* lookup the ip address */
    info("gethostbyname(%s)", parsed_url.host);
    server = gethostbyname(parsed_url.host);
    if (server == NULL) {
        error("unable to get ip of %s", parsed_url.host);
        ret = -1;
        goto exit;
    }

    /* fill in the structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(parsed_url.port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    /* connect the socket */
    info("connecting socket");
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        error("unable to connect");
        ret = -1;
        goto exit;
    }

    request_builder_set_parsed_url(&request_builder, &parsed_url);
    request_builder_set_method(&request_builder, HTTP_GET);
    uint32_t size = 0;
    char* request = request_builder_build(&request_builder, NULL, 0, &size, false);

    info("sending http request");

    /* send the request */
    total = strlen(request);
    sent = 0;
    do {
        bytes = write(sockfd, request + sent, total - sent);
        if (bytes < 0) {
            error("error writing request to socket");
            ret = -1;
            goto exit;
        }

        if (bytes == 0) {
            break;
        }
        info("sent %d bytes", bytes);

        sent += bytes;
    } while (sent < total);

    /* receive the response */
    info("reading http response");
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
    do {
        bytes = read(sockfd, response + received, total - received);

        if (bytes < 0) {
            error("error reading response from socket");
            ret = -1;
            goto exit;
        }

        if (bytes == 0) {
            error("server closed connection");
            ret = -1;
            goto exit;
            break;
        }

        info("received %d bytes", bytes);

        uint32_t parsed_bytes = 0;
        parsed_bytes = response_parser_execute(&response_parser, (const char*)(response + received), (uint32_t)bytes);
        if ((int)parsed_bytes != bytes) {
            error("recved %d bytes but parsed %d bytes", bytes, (int)parsed_bytes);
            ret = -1;
            goto exit;
        }
        received += bytes;
        if (http_response.is_message_completed) {
            info("done parsing http response");
            response_parser_finish(&response_parser);
            break;
        }


    } while (received < total);

    //raw response
    response[received] = '\0';
    printf("\r\n+++++ BEGIN RESPONSE +++++\r\n");
    printf("\r\n%s\r\n", response);
    printf("----- END RESPONSE -----\r\n\r\n");

    //parsed response
    info("status_code: %d", http_response.status_code);
    info("status_message: %s", http_response.status_message);
    printf("\r\n++++ HEADERS ++++\r\n");
    for (uint16_t i = 0; i < http_response.headers_count; i++) {
        printf("%s: %s\r\n", http_response.header_fields[i], http_response.header_values[i]);
    }
    printf("---- HEADERS ----\r\n\r\n");
    printf("++++ BODY ++++\r\n\r\n");
    printf("%.*s", (int)http_response.body_length, http_response.body);
    printf("\r\n---- BODY ++++\r\n");

exit:
    if (errno != 0) {
        error("%s", strerror(errno));
    }

    if (sockfd) {
        close(sockfd);
    }

    parse_url_free(&parsed_url);
    request_builder_free(&request_builder);
    response_parser_free(&response_parser);
    http_response_free(&http_response);
    return ret;
}