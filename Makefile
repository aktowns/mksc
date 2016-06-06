.PHONY: clean
CC=clang
C_SOURCES := $(wildcard src/*.c)
OBJECTS := $(addprefix obj/,$(notdir $(C_SOURCES:.c=.o)))
CFLAGS := -O1 -g -Werror -Wall -std=gnu11 -fsanitize=address -fblocks -I./deps/blocksruntime/BlocksRuntime/ \
	-fno-omit-frame-pointer -D_GNU_SOURCE=1 -I./include
LDFLAGS :=
BLOCKS := -L./deps/blocksruntime/ -lBlocksRuntime

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

mksc: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(BLOCKS)

make_vim_happy:
	make clean
	bear make
	cat compile_commands.json | jq -r '.[] | .["command"]' | cut -d' ' -f2- | tr ' ' '\n' | grep -v "\.c\|\.o\|-o" | sort | uniq > .clang


clean:
	rm obj/*.o
	rm mksc
