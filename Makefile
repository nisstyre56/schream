$(CC)=clang

default: tokenize.c closures.c tokenize.h closures.h
	$(CC) -lmaa -DNDEBUG -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./tokenize.c;
	$(CC) -DNDEBUG -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./closures.c;

unsafe: tokenize.c closures.c tokenize.h closures.h
	$(CC) -lmaa -DNDEBUG -std=c99 -O3 ./tokenize.c;
	$(CC) -DNDEBUG -std=c99 -O3 ./closures.c;

unsafelib: tokenize.c closures.c tokenize.h closures.h
	$(CC) -DLIB -DNDEBUG -c -fpic -std=c99 -O3 ./tokenize.c;
	$(CC) -lmaa -shared -o tokenize.so tokenize.o;

	$(CC) -DLIB -DNDEBUG -c -fpic -std=c99 -O3 ./closures.c;
	$(CC) -shared -o closures.so closures.o;

lib: tokenize.c closures.c tokenize.h closures.h
	$(CC) -DLIB -DNDEBUG -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./tokenize.c
	$(CC) -lmaa -shared -o tokenize.so tokenize.o;

	$(CC) -DLIB -DNDEBUG -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./closures.c;
	$(CC) -shared -o closures.so closures.o;

debug: tokenize.c closures.c tokenize.h closures.h
	$(CC) -g -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 ./tokenize.c;
	$(CC) -lmaa -shared -o tokenize.so tokenize.o;

	$(CC) -g -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 ./closures.c;
	$(CC) -shared -o closures.so closures.o;
