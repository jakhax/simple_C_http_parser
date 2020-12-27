# Simple C HTTP Parser

A simple wrapper of [http_parser](https://github.com/nodejs/http-parser) that makes it very easy to build http requests and parse responses in C.

## Features

- [URL parser](./src/http_parse_url.c)
- [Request builder](./src/request_builder.c)
- [HTTP Response](./src/http_response.c)
- [Response parser](./src/response_parser.c)


## Running Tests

```bash
make tests
```

## Usage

See [examples](./examples)

## Building Examples

```bash
make examples
```

## Todo

- [-] Add error code and error messages to struct fields
- [-] logging, with threading support
- [-] support for very low memory devices

## Contributing

Fork, make changes and make PR

Try to follow this [coding style](./.astylerc)

```bash
astyle -n --options=.astylerc --recursive "*.h"
astyle -n --options=.astylerc --recursive "*.c"
```
