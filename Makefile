
.PHONY: all examples  no_test  tests

all: examples tests
	$(MAKE) post_build

no_test: examples

all: examples
	
tests: 
	$(MAKE) -C TESTS

examples: 
	$(MAKE) -C examples
