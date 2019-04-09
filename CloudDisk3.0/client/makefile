SRCS:=$(wildcard *.c)
a.out:$(SRCS)
	gcc $^ -lcrypt -lcrypto -lpthread
clean:
	rm a.out
