CC = gcc

SRCs = ../log/log.c\
		../http_parser/http_parser.c\
		../src/http_parse_url.c\
		../src/http_response.c\
		./test_common.c

PARSE_URL_TEST=http_parse_url
HTTP_RESPONSE_TEST=http_response

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

tests: $(PARSE_URL_TEST) $(HTTP_RESPONSE_TEST)
	./$(PARSE_URL_TEST).o
	./$(HTTP_RESPONSE_TEST).o





