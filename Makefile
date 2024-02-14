carrace: main.cpp
	g++ -o carrace main.cpp -I/usr/include/SDL2 -D_REENTRANT -lSDL2

clean:
	rm carrace
