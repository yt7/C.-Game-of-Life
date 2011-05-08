CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99
LIBS=-lSDL
BIN=/usr/bin/

gameoflife: gameoflife.c
	${CC} gameoflife.c ${FLAGS} ${LIBS} -o gameoflife

clean:
	rm gameoflife

install: gameoflife
	cp -v gameoflife ${BIN}

uninstall:
	rm -v ${BIN}gameoflife

