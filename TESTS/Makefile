CC = gcc

SRCs = ../log/log.c\
		../http_parser/http_parser.c\
		../src/http_parse_url.c\
		../src/http_response.c\
		../src/request_builder.c\
		./test_common.c

PARSE_URL_TEST=http_parse_url
HTTP_RESPONSE_TEST=http_response
REQUEST_BUILDER_TEST=request_builder

TESTS = $(PARSE_URL_TEST)\
		$(HTTP_RESPONSE_TEST)\
		$(REQUEST_BUILDER_TEST)


RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color



all: tests

$(PARSE_URL_TEST): 
	$(CC) test_$(PARSE_URL_TEST).c $(SRCs)\
		-I ../\
		-I ../include\
		-I ../log\
		-I ../http_parser\
		-o $(PARSE_URL_TEST).o


$(HTTP_RESPONSE_TEST): 
	$(CC) test_$(HTTP_RESPONSE_TEST).c $(SRCs)\
		-I ../\
		-I ../include\
		-I ../log\
		-I ../http_parser\
		-o $(HTTP_RESPONSE_TEST).o

$(REQUEST_BUILDER_TEST): 
	$(CC) test_$(REQUEST_BUILDER_TEST).c $(SRCs)\
		-I ../\
		-I ../include\
		-I ../log\
		-I ../http_parser\
		-o $(REQUEST_BUILDER_TEST).o

tests: $(TESTS)
	for p in $(TESTS); do              \
		./$$p.o ; \
		if [ $$? -eq 0 ] ; \
		then \
			echo $(GREEN) $$p All Tests Passed!$(NC); \
		else \
			echo $(RED) $$p $$? Tests Failed!$(NC);  \
			exit 1 ; \
		fi \
	done 
	exit 0 ;
