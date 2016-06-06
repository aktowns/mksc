.PHONY: clean

C_SOURCES := $(wildcard src/*.c)
OBJECTS := $(addprefix obj/,$(notdir $(C_SOURCES:.c=.o)))
CFLAGS := -O1 -g -Werror -Wall -std=gnu11 -fsanitize=address -fblocks -I./deps/blocksruntime/BlocksRuntime/ \
	-fno-omit-frame-pointer -D_GNU_SOURCE=1 -I./include
LDFLAGS :=
BLOCKS := -L./deps/blocksruntime/ -lBlocksRuntime

obj/%.o: src/%.c
	clang ${CFLAGS} -c -o $@ $<

mksc: $(OBJECTS)
	clang ${CFLAGS} ${LDFLAGS} -o $@ $(OBJECTS) ${BLOCKS}

clean:
	rm obj/*.o
	rm mksc
