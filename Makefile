CFLAG = -c -std=c++11 -O2 -Wall -Wextra

bin/solver: obj/main.o obj/anneal.o | bin
	g++ -o $@ $^

obj/main.o: src/main.cpp src/anneal.h | obj
	g++ $(CFLAG) -o $@ $<

obj/anneal.o: src/anneal.cpp src/anneal.h | obj
	g++ $(CFLAG) -o $@ $<

bin:
	mkdir $@

obj:
	mkdir $@

clean:
	-rm obj/*.o