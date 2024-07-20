silkTools: main.o silkcompile.o
	g++ *.o -o $@

%.o: %.cpp
	g++ -o $@ $< -c

clean:
	rm *.o silkTools

.PHONY: clean
