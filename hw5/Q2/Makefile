CC = gcc

CFLAGS = -lpthread -lrt

TARGETS = max_pooling

max_pooling: max_pooling.c
	$(CC) $(CFLAGS) -o max_pooling max_pooling.c

clean:
	rm -f $(TARGETS)

.PHONY: clean
