#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>

#define MAX_MSG 1024
#define FIFO_DIR "fifo_files"
#define SERVER_FIFO "fifo_files/chat_server_%s_fifo"
#define CLIENT_FIFO "fifo_files/chat_client_%s_%d_fifo"

typedef struct
{
    char type;
    char display_name[32];
    char internal_id[64];
    char content[MAX_MSG];
    char client_fifo[128];
    char room_name[32];
} Message;

int server_fd = -1;
int client_fd = -1;
char client_fifo[128];
char internal_id[64];
char display_name[32];
char room_name[32];
volatile int running = 1;
pthread_t receive_thread;

void clear_line()
{
    printf("\r\033[K");
    fflush(stdout);
}

void print_message(const char *name, const char *content, int is_you)
{
    clear_line();
    if (is_you)
    {
        printf("%s (you): %s\n", name, content);
    }
    else
    {
        printf("%s: %s\n", name, content);
    }
    printf("You >> ");
    fflush(stdout);
}

void *get_message(void *arg)
{
    Message msg;
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;

    while (running)
    {
        int ret = poll(&pfd, 1, 1000); // 1 second timeout

        if (ret > 0 && (pfd.revents & POLLIN))
        {
            ssize_t bytes = read(client_fd, &msg, sizeof(Message));
            if (bytes <= 0)
            {
                if (errno != EAGAIN)
                {
                    printf("\nServer disconnected.\n");
                    running = 0;
                    exit(0);
                }
                continue;
            }

            clear_line();
            if (msg.type == 'S')
            {
                printf("\n%s\n", msg.content);
                running = 0;
                cleanup();
                exit(0); 
            }
            else if (strstr(msg.content, "Welcome") != NULL)
            {
                printf("%s\nYou >> ", msg.content);
            }
            else if (strcmp(msg.internal_id, internal_id) != 0)
            {
                print_message(msg.display_name, msg.content, 0);
            }
            fflush(stdout);
        }
    }
    return NULL;
}

void cleanup()
{
    running = 0;

    if (server_fd != -1)
    {
        Message msg;
        msg.type = 'L';
        strcpy(msg.display_name, display_name);
        strcpy(msg.internal_id, internal_id);
        strcpy(msg.client_fifo, client_fifo);
        strcpy(msg.room_name, room_name);
        write(server_fd, &msg, sizeof(Message));
        close(server_fd);
        server_fd = -1;
    }

    if (client_fd != -1)
    {
        close(client_fd);
        client_fd = -1;
    }


    unlink(client_fifo);
    printf("\nDisconnected from chat room.\n");

    if (receive_thread)
    {
        pthread_join(receive_thread, NULL);  
    }


    _exit(0);

}

void handle_signal(int sig)
{
    cleanup();
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("you need to give: %s <username> <room_name>\n", argv[0]);
        return 1;
    }

    strcpy(display_name, argv[1]);
    sprintf(internal_id, "%s_%d", argv[1], getpid());
    strcpy(room_name, argv[2]);

    char server_fifo[128];
    sprintf(server_fifo, SERVER_FIFO, room_name);
    sprintf(client_fifo, CLIENT_FIFO, internal_id, getpid());

    mkdir(FIFO_DIR, 0777);

    if (mkfifo(client_fifo, 0666) == -1 && errno != EEXIST)
    {
        printf(" create client FIFO error: %s\n", strerror(errno));
        return 1;
    }

    server_fd = open(server_fifo, O_WRONLY);
    if (server_fd == -1)
    {
        printf("Cannot connect to server (room '%s'): %s\n", room_name, strerror(errno));
        unlink(client_fifo);
        return 1;
    }

    client_fd = open(client_fifo, O_RDONLY | O_NONBLOCK);
    if (client_fd == -1)
    {
        printf("error in opening client FIFO: %s\n", strerror(errno));
        close(server_fd);
        unlink(client_fifo);
        return 1;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (pthread_create(&receive_thread, NULL, get_message, NULL) != 0)
    {
        printf("Failed to create thread\n");
        cleanup();
        return 1;
    }
    Message msg;
    msg.type = 'J';
    strcpy(msg.display_name, display_name);
    strcpy(msg.internal_id, internal_id);
    strcpy(msg.client_fifo, client_fifo);
    strcpy(msg.room_name, room_name);
    write(server_fd, &msg, sizeof(Message));

    printf("Connected to chat room '%s'. Type your messages:\nYou >> ", room_name);
    fflush(stdout);

    char input[MAX_MSG];
    struct pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLOUT;

    while (running)
    {
        clear_line();
        printf("You >> ");
        fflush(stdout);

        if (fgets(input, MAX_MSG, stdin) != NULL && running)
        {
            input[strlen(input) - 1] = '\0';
            if (strlen(input) > 0)
            {
                msg.type = 'M';
                strcpy(msg.content, input);
                strcpy(msg.internal_id, internal_id);
                strcpy(msg.display_name, display_name);

                // check if we can write to server
                int ret = poll(&pfd, 1, 1000);
                if (ret <= 0 || !(pfd.revents & POLLOUT))
                {
                    printf("\nLost connection to server.\n");
                    running = 0;
                    break;
                }

                if (write(server_fd, &msg, sizeof(Message)) <= 0)
                {
                    printf("\nServer disconnected.\n");
                    running = 0;
                    break;
                }
            }
        }
    }

    cleanup();
    return 0;
}