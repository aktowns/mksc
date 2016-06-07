.PHONY: clean
.DEFAULT_GOAL := mksc

CC=clang
C_SOURCES := $(wildcard src/*.c) src/gen/parser.c src/gen/lexer.c
OBJECTS := $(addprefix obj/,$(notdir $(C_SOURCES:.c=.o)))
CFLAGS := -O1 -g -Werror -Wall -std=gnu11 -fsanitize=address -fblocks -I./deps/blocksruntime/BlocksRuntime/ \
	-fno-omit-frame-pointer -D_GNU_SOURCE=1 -I./include
LDFLAGS := -lfl
BLOCKS := -L./deps/blocksruntime/ -lBlocksRuntime

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: src/gen/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

deps/lemon: deps/lemon.c
	$(CC) -o deps/lemon $<

src/gen/lexer.c: src/lexer.rl src/gen/parser.c
	mkdir -p $(dir $@)
	ragel -o $@ $<

src/gen/parser.c: deps/lemon src/parser.y
	mkdir -p $(dir $@)
	deps/lemon -Tdeps/lempar.c.tmpl src/parser.y
	mv -v src/parser.{h,c,out} src/gen/

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
