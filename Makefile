
crunch.dylib: src/crunch.c src/crunch_set.c
	$(GCC) -Wall -Werror -o crunch.dylib -dynamiclib src/crunch.c

.PHONY: clean
clean:
	rm -fr crunch.dylib crunch_out test/a.out
