CC = gcc

SRCs = ../log/log.c\
		../http_parser/http_parser.c\
		../src/common.c\
		../src/http_parse_url.c\
		../src/http_response.c\
		../src/request_builder.c\
		../src/response_parser.c

URL_PARSER=url_parser
GET_IP=get_ip
TEMP_CLI=temp_cli

EXAMPLES = $(URL_PARSER) \
			$(GET_IP)\
			$(TEMP_CLI)\
			$(HTTPS_GET_IP)

all: examples

$(URL_PARSER): 
	$(CC) $(URL_PARSER).c $(SRCs)\
		-I ../\
		-I ../include\
		-I ../log\
		-I ../http_parser\
		-o $(URL_PARSER).o

$(GET_IP): 
	$(CC) $(GET_IP).c $(SRCs)\
		-I ../\
		-I ../include\
		-I ../log\
		-I ../http_parser\
		-o $(GET_IP).o

$(TEMP_CLI): 
	$(CC) $(TEMP_CLI).c $(SRCs)\
		-I ../\
		-I ../include\
		-I ../log\
		-I ../http_parser\
		-o $(TEMP_CLI).o

server:
	python3 server.py


examples: $(EXAMPLES) 






