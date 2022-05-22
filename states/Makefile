build: clean
	g++ -std=c++17 -c ./src/player.cpp ./src/player_states.cpp ./src/main.cpp
	g++ player.o player_states.o main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
	rm -f *.o
clean:
	rm -f *.o
	rm -f sfml-app
run: build
	./sfml-app
