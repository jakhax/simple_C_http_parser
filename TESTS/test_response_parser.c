#include "test_common.h"
#include "http_response.h"
#include "response_parser.h"

#define TEST_NAME "RESPONSE_PARSER"


int test_response_parser_init(void){
    info("+++ test_init() +++");
    
    http_response_t res;
    response_parser_t rp;

    int ret = 0;
    ret = http_response_init(&res);
    if(ret != 0){
        error("http_response_init ERR %d", ret);
        return ret;
    }

    ret = response_parser_init(&rp, &res, NULL);
    if(ret != 0){
        error("response_parser_init ERR %d", ret);
        return ret;
    }

    if(rp.parser == NULL){
        error("init: parser is NULL");
        ret = -1;
        goto exit;
    }

    if(rp.settings == NULL){
        error("init: settings is NULL");
        ret = -1;
        goto exit;
    }

    if(rp.res != &res){
        error("init: parser<response> addr %p !=  response addr %p", &rp.res, &res);
        ret = -1;
        goto exit;
    }


    //@todo check that all required cbs have been set

exit:
    http_response_free(&res);
    response_parser_free(&rp);
 

    return ret;

}

int test_response_parser_free(void){
    info("+++ test_free() +++");
    
    http_response_t res;
    response_parser_t rp;

    int ret = 0;
    ret = http_response_init(&res);
    if(ret != 0){
        error("http_response_init ERR %d", ret);
        return ret;
    }

    ret = response_parser_init(&rp, &res, NULL);
    if(ret != 0){
        error("response_parser_init ERR %d", ret);
        return ret;
    }

    response_parser_free(&rp);

    if(rp.parser != NULL){
        error("free: parser != NULL");
        ret = -1;
        goto exit;
    }

    if(rp.settings != NULL){
        error("free: settings != NULL");
        ret = -1;
        goto exit;
    }


exit:
    http_response_free(&res);

    return ret;
}

int test_response_parser_execute(void){
    info("+++ test_response_parser_execute() +++");

    uint16_t headers_count = 3;
    int status_code = HTTP_STATUS_BAD_REQUEST;
    const char* res_tmpl = "HTTP/1.1 400 Bad Request\r\n"
        "Date: Sat, 26 Dec 2020 13:47:09 GMT\r\n"
        "Content-Length: %d\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "\r\n"
        "%s\r\n";

    const char* res_body = "{\"msg\":\"ERR_D\"}";
    char response[512];

    const char* response_2 = "HTTP/1.1 400 Bad Request\\r\\nDate: Sat, 26 Dec 2020 13:47:09 GMT\\r\\nContent-Length: 15\\r\\nContent-Type: text/plain; charset=utf-8\\r\\n\\r\\n{\"msg\":\"ERR_D\"}";

    sprintf(response, res_tmpl, (int)strlen(res_body), res_body);

    http_response_t res;
    response_parser_t rp;


    int ret = 0;
    ret = http_response_init(&res);
    if(ret != 0){
        error("http_response_init ERR %d", ret);
        return ret;
    }

    ret = response_parser_init(&rp, &res, NULL);
    if(ret != 0){
        error("response_parser_init ERR %d", ret);
        return ret;
    }

    uint32_t size;
    size = response_parser_execute(&rp, (const char*)response, (uint32_t)strlen(response));

    //size
    if(size <= 0){
        error("parsed bytes: got %lu expected %lu", (unsigned long)size, (unsigned long)strlen(response));
        ret = -1;
        goto exit;
    }

    //is message complete 
    if(res.is_message_completed != true){
        error("response.is_message_completed != true");
        ret = -1;
        goto exit;
    }

    response_parser_finish(&rp);

    //headers count
    if(res.headers_count != headers_count){
        error("headers_count: expected %u got %u", headers_count, res.headers_count);
        ret = -1;
        goto exit;
    }

    //find header
    int k = http_response_find_header(&res, "content-length");
    if(k < 0){
        error("header: content-length not found");
        ret = -1;
        goto exit;
    }

    //content length
    int clen = 0;
    sscanf((const char*)res.header_values[k], "%d", &clen);
    if(clen != strlen(res_body)){
        error("content-length: got %s expected %d", res.header_values[k], (int)strlen(res_body));
        ret = -1;
        goto exit;
    }

    //status code
    if(status_code != res.status_code){
        error("status_code: got %d, expected %d", res.status_code, status_code);
        ret = -1;
        goto exit;
    }

    //body
   if (memcmp((const void*)res.body, res_body, strlen(res_body)) != 0) {
        error("expected:\r\n<%s>\r\ngot:\r\n<%.*s>\r\n", res_body, (int)res.body_length, res.body);
        ret = -1;
        goto exit;
    }

exit:
    http_response_free(&res);
    response_parser_free(&rp);
 
    return ret;
}


int main() {
    int tests_count = 3;

    int (*tests[tests_count])(void);
    tests[0] = test_response_parser_init;
    tests[1] = test_response_parser_free;
    tests[2] = test_response_parser_execute;

    return run_tests((const char*)TEST_NAME, tests, tests_count);
}