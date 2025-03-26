CC = gcc

CFLAGS = -pthread -lpthread -lrt

TARGETS = pipe

pipe: pipe.c
	$(CC) $(CFLAGS) -o pipe pipe.c

clean:
	rm -f $(TARGETS)

.PHONY: clean
