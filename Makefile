default: tokenize.c RTS.c tokenize.h RTS.h
	$(CC) -DNDEBUG -Wall -Wextra -pedantic -Wpointer-arith -Wmissing-prototypes -Werror -std=c99 -O3 ./tokenize.c -lmaa -o tokenize_test;
	$(CC) -DNDEBUG -Wall -Wextra -pedantic -Wpointer-arith -Wmissing-prototypes -Werror -std=c99 -O3 ./RTS.c -o rts_test;

unsafe: tokenize.c RTS.c tokenize.h RTS.h
	$(CC) -DNDEBUG -std=c99 -O3 ./tokenize.c -lmaa;
	$(CC) -DNDEBUG -std=c99 -O3 ./RTS.c;

unsafelib: tokenize.c RTS.c tokenize.h RTS.h
	$(CC) -DLIB -DNDEBUG -c -fpic -std=c99 -O3 ./tokenize.c;
	$(CC) -shared -o tokenize.so tokenize.o -lmaa;

	$(CC) -DLIB -DNDEBUG -c -fpic -std=c99 -O3 ./RTS.c;
	$(CC) -shared -o RTS.so RTS.o;

lib: tokenize.c RTS.c tokenize.h RTS.h
	$(CC) -DLIB -DNDEBUG -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./tokenize.c
	$(CC) -shared -o tokenize.so tokenize.o -lmaa;

	$(CC) -DLIB -DNDEBUG -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Werror -std=c99 -O3 ./RTS.c;
	$(CC) -shared -o RTS.so RTS.o;

debug: tokenize.c RTS.c tokenize.h RTS.h
	$(CC) -Wall -Wextra -pedantic -Wpointer-arith -Wmissing-prototypes -Werror -std=c99 ./tokenize.c -lmaa -o tokenize_debug;
	$(CC) -Wall -Wextra -pedantic -Wpointer-arith -Wmissing-prototypes -Werror -std=c99 ./RTS.c;

debuglib: tokenize.c RTS.c tokenize.h RTS.h
	$(CC) -g -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Wmissing-prototypes -Werror -std=c99 ./tokenize.c;
	$(CC) -shared -o tokenize.so tokenize.o -lmaa;

	$(CC) -g -c -fpic -Wall -Wextra -pedantic -Wpointer-arith -Wmissing-prototypes -Werror -std=c99 ./RTS.c;
	$(CC) -shared -o RTS.so RTS.o;


