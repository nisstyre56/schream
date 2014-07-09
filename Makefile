schreammake: tokenize.c
	gcc -DNDEBUG -c -fPIC -O3 -Wall -Wextra -pedantic -Wpointer-arith -Werror --std=c99 ./tokenize.c;
	gcc -lmaa -shared -o tokenize.so tokenize.o;
