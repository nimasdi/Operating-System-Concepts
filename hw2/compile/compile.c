#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>

int compile_and_run(const char *source_file)
{
    char *source_path = strdup(source_file);
    char *dir_path = dirname(strdup(source_file));

    if (chdir(dir_path) != 0)
    {
        perror("chdir failed");
        free(source_path);
        return 1;
    }

    char output_file[256];
    snprintf(output_file, sizeof(output_file), "./%s", basename(source_path));

    char *dot = strrchr(output_file, '.');
    if (dot)
        *dot = '\0';

    char compile_cmd[512];
    snprintf(compile_cmd, sizeof(compile_cmd), "gcc -o %s %s", output_file, source_file);

    int compile_status = system(compile_cmd);
    if (compile_status != 0)
    {
        printf("Compilation failed\n");
        free(source_path);
        return 1;
    }

    printf("Compilation successful. Running the program...\n\n");

    pid_t pid = fork();
    if (pid == 0)
    {
        execlp(output_file, output_file, (char *)NULL);
        perror("execlp failed");
        exit(1);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        perror("fork failed");
        free(source_path);
        return 1;
    }

    free(source_path);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("invalid arguments!\n");
        return 1;
    }

    return compile_and_run(argv[1]);
}

