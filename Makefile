CC ?= gcc
CFLAGS += -O3 -std=c99 -Wall -Wextra -Werror -pedantic

randomfeh: randomfeh.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f randomfeh
