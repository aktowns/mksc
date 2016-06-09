.PHONY: clean
.DEFAULT_GOAL := all

CC=clang
C_SOURCES := $(wildcard src/*.c) src/gen/parser.c src/gen/lexer.c
OBJECTS := $(addprefix obj/,$(notdir $(C_SOURCES:.c=.o)))
CFLAGS := -O1 -g -Werror -Wall -std=gnu11 -fsanitize=address -fblocks -I./deps/blocksruntime/BlocksRuntime/ \
	-fno-omit-frame-pointer -D_GNU_SOURCE=1 -I./include
LDFLAGS :=
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
	mv -v src/parser.{c,out} src/gen/
	mv -v src/parser.h include/

all: pre-build src/gen/lexer.c mksc post-build

mksc: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(BLOCKS)

pre-build:
	# grrr lemon.
	rm -f src/gen/*
	rm -f src/parser.{c,out}
	rm -f include/parser.h

post-build:
	# grrr lemon.
	rm -f src/gen/*
	rm -f src/parser.{c,out}
	rm -f include/parser.h

test: all
	@echo -e "\n================================\n"
	./mksc tests/parsing/01-test.syntax
	@echo -e "\n================================\n"
	./mksc tests/parsing/02-test.syntax
	@echo -e "\n================================\n"

make_vim_happy:
	make clean
	bear make
	cat compile_commands.json | jq -r '.[] | .["command"]' | cut -d' ' -f2- | tr ' ' '\n' | grep -v "\.c\|\.o\|-o" | sort | uniq > .clang

clean:
	rm -f obj/*.o
	rm -f src/gen/*
	rm -f mksc
