CFLAGS=-O1 -g -Werror -Wall -std=gnu11 -fsanitize=address -fblocks -fintegrated-as -I./blocksruntime/BlocksRuntime/ -fno-omit-frame-pointer -D_GNU_SOURCE=1
LDFLAGS=
BLOCKS=-L./blocksruntime/ -lBlocksRuntime

%.o: %.c
	clang ${CFLAGS} -c -o $@ $<

mksc: mks_node.o main.o
	clang ${CFLAGS} ${LDFLAGS} -o $@ mks_node.o main.o ${BLOCKS}

clean:
	rm -v *.o
	rm mksc
