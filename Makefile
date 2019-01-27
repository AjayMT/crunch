
crunch.dylib: src/crunch.c src/crunch.h src/crunch_set.c
	$(GCC) -Wall -Werror -o crunch.dylib -dynamiclib src/crunch.c

.PHONY: clean
clean:
	rm -f crunch.dylib
