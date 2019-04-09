SRCS:=$(wildcard *.c)
a.out:$(SRCS)
	gcc $^ -I /usr/include/mysql/ -lmysqlclient -lcrypto -lpthread
clean:
	rm a.out
