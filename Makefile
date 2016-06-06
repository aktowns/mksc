.PHONY: clean
.DEFAULT_GOAL := mksc

CC=clang
C_SOURCES := $(wildcard src/*.c) src/gen/parser.c src/gen/lexer.c
OBJECTS := $(addprefix obj/,$(notdir $(C_SOURCES:.c=.o))) obj/lexer.o obj/parser.o
CFLAGS := -O1 -g -Werror -Wall -std=gnu11 -fsanitize=address -fblocks -I./deps/blocksruntime/BlocksRuntime/ \
	-fno-omit-frame-pointer -D_GNU_SOURCE=1 -I./include
LDFLAGS := -lfl
BLOCKS := -L./deps/blocksruntime/ -lBlocksRuntime

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: src/gen/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/gen/lexer.c: src/mks.l
	mkdir -p $(dir $@)
	flex $<

src/gen/parser.c: src/mks.y src/gen/lexer.c
	mkdir -p $(dir $@)
	bison --graph=$(basename $@).dot $<

mksc: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(BLOCKS)

make_vim_happy:
	make clean
	bear make
	cat compile_commands.json | jq -r '.[] | .["command"]' | cut -d' ' -f2- | tr ' ' '\n' | grep -v "\.c\|\.o\|-o" | sort | uniq > .clang

clean:
	rm -f obj/*.o
	rm -f src/gen/*
	rm -f mksc
