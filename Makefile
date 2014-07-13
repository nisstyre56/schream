$(CC)=clang

default: tokenize.c closures.c
	$(CC) -DNDEBUG -c -fPIC -Wall -Wextra -pedantic -Wpointer-arith -Werror --std=c99 -O3 ./tokenize.c
	$(CC) -lmaa -shared -o tokenize.so tokenize.o;

	$(CC) -DNDEBUG -c -fPIC -Wall -Wextra -pedantic -Wpointer-arith -Werror --std=c99 -O3 ./closures.c;
	$(CC) -shared -o closures.so closures.o;

debug: tokenize.c closures.c
	$(CC) -g -c -fPIC -Wall -Wextra -pedantic -Wpointer-arith -Werror --std=c99 ./tokenize.c;
	$(CC) -lmaa -shared -o tokenize.so tokenize.o;

	$(CC) -g -c -fPIC -Wall -Wextra -pedantic -Wpointer-arith -Werror --std=c99 ./closures.c;
	$(CC) -shared -o closures.so closures.o;
