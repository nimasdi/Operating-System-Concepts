#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define NAME_LEN 256

typedef struct
{
    char srcPath[NAME_LEN];
    char destPath[NAME_LEN];
} CopyProps;

typedef struct Node
{
    CopyProps props;
    struct Node *next;
} Node;

Node *fileListHead = NULL;
Node *fileListTail = NULL;
Node *dirListHead = NULL;
Node *dirListTail = NULL;
int fileCount = 0;
int dirCount = 0;

void addFileToList(const char *srcPath, const char *destPath)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    strncpy(newNode->props.srcPath, srcPath, NAME_LEN);
    strncpy(newNode->props.destPath, destPath, NAME_LEN);
    newNode->next = NULL;
    if (fileListTail)
    {
        fileListTail->next = newNode;
    }
    else
    {
        fileListHead = newNode;
    }
    fileListTail = newNode;
    fileCount++;
}

void addDirToList(const char *srcPath, const char *destPath)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    strncpy(newNode->props.srcPath, srcPath, NAME_LEN);
    strncpy(newNode->props.destPath, destPath, NAME_LEN);
    newNode->next = NULL;
    if (dirListTail)
    {
        dirListTail->next = newNode;
    }
    else
    {
        dirListHead = newNode;
    }
    dirListTail = newNode;
    dirCount++;
}

void *copyFileThread(void *arg)
{
    CopyProps *task = (CopyProps *)arg;
    int srcFd = open(task->srcPath, O_RDONLY);
    if (srcFd == -1)
    {
        perror("Error opening source file");
        return NULL;
    }

    int destFd = open(task->destPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destFd == -1)
    {
        perror("Error creating destination file");
        close(srcFd);
        return NULL;
    }

    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(srcFd, buffer, sizeof(buffer))) > 0)
    {
        write(destFd, buffer, bytesRead);
    }

    close(srcFd);
    close(destFd);
    return NULL;
}

void *createDirThread(void *arg)
{
    CopyProps *task = (CopyProps *)arg;
    mkdir(task->destPath, 0755);
    return NULL;
}

void traverseDirectory(const char *srcDir, const char *destDir)
{
    DIR *dir = opendir(srcDir);
    if (dir == NULL)
    {
        perror("Error opening source directory");
        return;
    }

    addDirToList(srcDir, destDir);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char srcPath[NAME_LEN], destPath[NAME_LEN];
        snprintf(srcPath, NAME_LEN, "%s/%s", srcDir, entry->d_name);
        snprintf(destPath, NAME_LEN, "%s/%s", destDir, entry->d_name);

        if (entry->d_type == DT_DIR)
        {

            traverseDirectory(srcPath, destPath);
        }
        else
        {

            addFileToList(srcPath, destPath);
        }
    }
    closedir(dir);
}

void createAllDirectories()
{
    pthread_t *threads = malloc(sizeof(pthread_t) * dirCount);
    if (!threads)
    {
        perror("Failed to allocate memory for directory threads");
        return;
    }

    Node *current = dirListHead;
    int i = 0;
    while (current != NULL)
    {
        pthread_create(&threads[i], NULL, createDirThread, &current->props);
        current = current->next;
        i++;
    }

    for (i = 0; i < dirCount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
}

void copyAllFiles()
{
    pthread_t *threads = malloc(sizeof(pthread_t) * fileCount);
    if (!threads)
    {
        perror("Failed to allocate memory for file threads");
        return;
    }

    Node *current = fileListHead;
    int i = 0;
    while (current != NULL)
    {
        pthread_create(&threads[i], NULL, copyFileThread, &current->props);
        current = current->next;
        i++;
    }

    for (i = 0; i < fileCount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Not enough arguments!\n");
        return 1;
    }

    traverseDirectory(argv[1], argv[2]);

    createAllDirectories();
    copyAllFiles();

    printf("Copy completed.\n");
    return 0;
}
