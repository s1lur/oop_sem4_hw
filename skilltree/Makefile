build: clean
	g++ -std=c++17 -c skilltree.cpp
	g++ skilltree.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
clean:
	rm -f *.o
	rm -f sfml-app
run: build
	./sfml-app
