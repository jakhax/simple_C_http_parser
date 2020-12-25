CC = gcc

all: 
	$(CC) main.c\
		./log/log.c\
		./http_parser/http_parser.c\
		./src/http_parse_url.c\
		-I .\
		-I ./include\
		-I ./log\
		-I ./http_parser\
		-o app


