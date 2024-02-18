carrace: main.c
	gcc -o carrace main.c -I/usr/include -D_REENTRANT -lSDL2

clean:
	rm carrace
