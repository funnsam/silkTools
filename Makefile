silkTools: main.o silkcompile.o
	g++ $? -o $@

%.o: %.cpp
	g++ -o $@ $< -c

clean:
	rm *.o silkTools

.PHONY: clean
