CC = g++
CPPFLAGS = -I. -Wall -Wextra -Werror -fsigned-char -O3
DEPS = shell.hpp
OBJ = main.o shell.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

shell: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)

.PHONY: clean

clean:
	rm -f *.o shell
