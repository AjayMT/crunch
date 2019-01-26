
GCC=gcc-8

crunch.dylib: src/crunch.c src/crunch.h
	$(GCC) -Wall -Werror -o crunch.dylib -dynamiclib src/crunch.c

.PHONY: clean
clean:
	rm -f crunch.dylib
