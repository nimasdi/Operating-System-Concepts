CC = gcc

CFLAGS = -lpthread -lrt

TARGETS = merge test_performance

merge: merge.c
	$(CC) $(CFLAGS) -o merge merge.c

test_performance: test_performance.c
	$(CC) $(CFLAGS) -o test_performance test_performance.c

clean:
	rm -f $(TARGETS)

.PHONY: clean
