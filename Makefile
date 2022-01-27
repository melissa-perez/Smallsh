setup:
	gcc -std=gnu99 -g -Wall -o smallsh smallsh.c smallsh_api.c smallsh_api.h

clean:
	rm -f smallsh
debug:
	valgrind -s --leak-check=yes --track-origins=yes --show-reachable=yes ./smallsh
