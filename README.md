# c_https_client
A C http client using mbed tls 

## Running Tests

```bash
make tests
```

## Running Examples

```bash
make examples
```

you can find the built binaries in [examples](./examples) folder.

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
