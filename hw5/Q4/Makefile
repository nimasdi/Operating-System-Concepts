# Compiler
CC = gcc
# Targets
TARGETS = server client

# Compile the programs
compile: $(TARGETS)

server: server.c
	$(CC) -lpthread -lrt -o server server.c
client: client.c
	$(CC) -lpthread -lrt -o client client.c

# Create a chat room
room:
	mkdir -p fifo_files
	@./server $(name)

# Create a chat member
member:
	@./client $(name) $(room)

clean:
	rm -f $(TARGETS)

help:
	@echo "Usage:"
	@echo " make compile Compile the programs"
	@echo " make room name=<room_name> Create a chat room"
	@echo " make member name=<member_name> room=<room_name> Create a chat member"
	@echo " make clean Clean up the compiled programs"


.PHONY: compile room member clean help