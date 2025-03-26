#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <poll.h>
#include <pthread.h>

#define MAX_MSG 1024
#define FIFO_DIR "fifo_files"
#define SERVER_FIFO "fifo_files/chat_server_%s_fifo"

typedef struct
{
    char type;
    char display_name[32];  
    char internal_id[64];  
    char content[MAX_MSG];
    char client_fifo[128];
    char room_name[32];
} Message;

typedef struct Client
{
    char display_name[32];
    char internal_id[64];
    char fifo_path[128];
    int fd;
    struct Client *next;
} Client;

Client *clients = NULL;
int server_fd;
char room_name[32];
char server_fifo_path[128];

void create_user(const char *display_name, const char *internal_id, const char *fifo_path)
{
    Client *new_client = malloc(sizeof(Client));
    strcpy(new_client->display_name, display_name);
    strcpy(new_client->internal_id, internal_id);
    strcpy(new_client->fifo_path, fifo_path);
    new_client->fd = open(fifo_path, O_WRONLY | O_NONBLOCK);
    new_client->next = clients;
    clients = new_client;

    if (new_client->fd == -1)
    {
        printf("Warning: Could not open client FIFO: %s\n", strerror(errno));
    }
}

void remove_user(const char *internal_id)
{
    Client *current = clients;
    Client *prev = NULL;

    while (current != NULL)
    {
        if (strcmp(current->internal_id, internal_id) == 0)
        {
            if (current->fd != -1)
            {
                close(current->fd);
            }
            if (prev == NULL)
            {
                clients = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void send_message_all(const Message *msg, const char *sender_id, int is_welcome)
{
    Client *current = clients;

    if (is_welcome)
    {
        while (current != NULL)
        {
            if (strcmp(current->internal_id, sender_id) == 0 && current->fd != -1)
            {
                if (write(current->fd, msg, sizeof(Message)) == -1)
                {
                    if (errno == EPIPE)
                    {
                        printf("Client %s disconnected unexpectedly\n", current->display_name);
                        removve_user(current->internal_id);
                    }
                }
                break;
            }
            current = current->next;
        }
        return;
    }

    while (current != NULL)
    {
        if (strcmp(current->internal_id, sender_id) != 0 && current->fd != -1)
        {
            if (write(current->fd, msg, sizeof(Message)) == -1)
            {
                if (errno == EPIPE)
                {
                    printf("Client %s disconnected unexpectedly\n", current->display_name);
                    removve_user(current->internal_id);
                }
            }
        }
        current = current->next;
    }
}


int server_fd_read = -1;
int server_fd_write = -1;

void cleanup()
{
    // notify all clients
    Message shutdown_msg;
    shutdown_msg.type = 'S';
    sprintf(shutdown_msg.content, "Server closed the connection. Exiting...");
    
    Client *current = clients;
    while (current != NULL)
    {
        if (current->fd != -1)
        {
            write(current->fd, &shutdown_msg, sizeof(Message));
            close(current->fd);
        }
        unlink(current->fifo_path);
        Client *next = current->next;
        free(current);
        current = next;
    }
    clients = NULL;

    if (server_fd_read != -1)
    {
        close(server_fd_read);
    }
    if (server_fd_write != -1)
    {
        close(server_fd_write);
    }
    unlink(server_fifo_path);
}


int get_online_members()
{
    int count = 0;
    Client *current = clients;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

int running = 1;

void *console_input_handler(void *arg)
{
    char input[16];
    while (running)
    {
        printf(">> ");
        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            if (strncmp(input, "close", 5) == 0)
            {
                printf("Closing chat room...\n");
                running = 0;
                break;
            }
            else
            {
                printf("Unknown command. Type 'close' to terminate the chat room.\n");
            }
        }
    }
    return NULL;
}

void handle_signal(int sig)
{
    running = 0;
    printf("\nClosing chat room '%s'...\n", room_name);
    cleanup();
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("you need to give: %s <room_name>\n", argv[0]);
        return 1;
    }

    strcpy(room_name, argv[1]);
    sprintf(server_fifo_path, SERVER_FIFO, room_name);

    mkdir(FIFO_DIR, 0777);

    if (mkfifo(server_fifo_path, 0666) == -1 && errno != EEXIST)
    {
        printf("error in creating server FIFO: %s\n", strerror(errno));
        return 1;
    }

    server_fd_read = open(server_fifo_path, O_RDONLY | O_NONBLOCK);
    if (server_fd_read == -1)
    {
        printf("error in opening server FIFO for reading: %s\n", strerror(errno));
        return 1;
    }

    server_fd_write = open(server_fifo_path, O_WRONLY | O_NONBLOCK);
    if (server_fd_write == -1)
    {
        printf("error in opening server FIFO for writing: %s\n", strerror(errno));
        close(server_fd_read);
        return 1;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    printf("Chatroom '%s' started. To close the chatroom, type 'close'.\n", room_name);

    pthread_t input_thread;
    pthread_create(&input_thread, NULL, console_input_handler, NULL);

    struct pollfd fds[1];
    fds[0].fd = server_fd_read;
    fds[0].events = POLLIN;

    Message msg;
    while (running)
    {
        int ret = poll(fds, 1, 1000);

        if (ret > 0 && (fds[0].revents & POLLIN))
        {
            if (read(server_fd_read, &msg, sizeof(Message)) > 0)
            {
                switch (msg.type)
                {
                case 'J':
                {
                    create_user(msg.display_name, msg.internal_id, msg.client_fifo);

                    Message welcome_msg = msg;
                    int online_count = get_online_members();
                    sprintf(welcome_msg.content, "Welcome %s to chat room %s! online members: %d",
                            msg.display_name, room_name, online_count);
                    send_message_all(&welcome_msg, msg.internal_id, 1);

                    sprintf(msg.content, "%s joined the chat", msg.display_name);
                    send_message_all(&msg, msg.internal_id, 0);

                    printf("%s joined the room\n", msg.display_name);
                    break;
                }

                case 'M':
                    printf("%s: %s\n", msg.display_name, msg.content);
                    send_message_all(&msg, msg.internal_id, 0);
                    break;

                case 'L':
                    printf("%s left the room\n", msg.display_name);
                    sprintf(msg.content, "%s left the room", msg.display_name);
                    send_message_all(&msg, msg.internal_id, 0);
                    removve_user(msg.internal_id);
                    break;
                }
            }
        }
    }

    pthread_join(input_thread, NULL);
    cleanup();
    return 0;
}