$(CC)=clang

default: tokenize.c closures.c tokenize.h RTS.h
	$(CC) -DNDEBUG -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./tokenize.c -lmaa;
	$(CC) -DNDEBUG -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./closures.c;

unsafe: tokenize.c closures.c tokenize.h RTS.h
	$(CC) -DNDEBUG -std=c99 -O3 ./tokenize.c -lmaa;
	$(CC) -DNDEBUG -std=c99 -O3 ./closures.c;

unsafelib: tokenize.c closures.c tokenize.h RTS.h
	$(CC) -DLIB -DNDEBUG -c -fpic -std=c99 -O3 ./tokenize.c;
	$(CC) -shared -o tokenize.so tokenize.o -lmaa;

	$(CC) -DLIB -DNDEBUG -c -fpic -std=c99 -O3 ./closures.c;
	$(CC) -shared -o closures.so closures.o;

lib: tokenize.c closures.c tokenize.h RTS.h
	$(CC) -DLIB -DNDEBUG -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./tokenize.c
	$(CC) -shared -o tokenize.so tokenize.o -lmaa;

	$(CC) -DLIB -DNDEBUG -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./closures.c;
	$(CC) -shared -o closures.so closures.o;

debug: tokenize.c closures.c tokenize.h RTS.h
	$(CC) -g -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 ./tokenize.c;
	$(CC) -shared -o tokenize.so tokenize.o -lmaa;

	$(CC) -g -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 ./closures.c;
	$(CC) -shared -o closures.so closures.o;
