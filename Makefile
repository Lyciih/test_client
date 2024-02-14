CFLAG = -Wall -g -I include -MD

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, ${SRCS})
DEPS = $(OBJS:.o=.d)

all: test_client

-include $(DEPS) #這行如果放在 all 之前，修改.h檔後，不會偵測到改變

test_client: $(OBJS)
	gcc $(CFLAG) -o $@ $^ -pthread 

obj/%.o: src/%.c
	gcc $(CFLAG) -c $< -o $@


.PHONY: clean run re

clean: 
	rm -f test_client obj/*.o obj/*.d

run: 
	./test_client 1 192.168.2.2 1230
	./test_client 1 192.168.2.2 1231
	./test_client 1 192.168.2.2 1232

run0: 
	./test_client 1 192.168.2.2 1230

run1: 
	./test_client 1 192.168.2.2 1231

run2: 
	./test_client 1 192.168.2.2 1232

re:
	make clean
	make
	make run
