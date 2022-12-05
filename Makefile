CC=gcc
CFLAGS=-Wall -Werror -Wvla -std=c11 -fsanitize=address
PFLAGS=-fprofile-arcs -ftest-coverage
DFLAGS=-g
HEADERS=server.h msg_utilities.h
SRC=server.c msg_utilities.c

procchat: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o $@

test:
	$(CC) $(CFLAGS) $(PFLAGS) $(SRC) -o $@

run_tests:
	$(CC) -DTEST $(CFLAGS) $(SRC) -o procchat
	$(CC) client.c -o client 
	bash test.sh


clean:
	rm -f procchat
