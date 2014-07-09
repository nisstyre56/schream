schreammake: tokenize.c
	gcc -DNDEBUG -c -fPIC -O$1 -Wall -Wextra -pedantic -Wpointer-arith -Werror --std=c99 ./tokenize.c;
	gcc -lmaa -shared -o tokenize.so tokenize.o;
