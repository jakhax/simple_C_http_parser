CC = gcc

SRCs = ../log/log.c\
		../http_parser/http_parser.c\
		../src/http_parse_url.c\
		../src/http_response.c

URL_PARSER=url_parser

all: examples

$(URL_PARSER): 
	$(CC) $(URL_PARSER).c $(SRCs)\
		-I ../\
		-I ../include\
		-I ../log\
		-I ../http_parser\
		-o $(URL_PARSER).o


examples: $(URL_PARSER) 






