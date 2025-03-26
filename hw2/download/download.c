#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/wait.h>

#define MAX_URLS 100

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

void download_file(const char *url, const char *output_filename)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        FILE *file = fopen(output_filename, "wb");
        if (file)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            CURLcode res = curl_easy_perform(curl);
            if(res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
                
            fclose(file);
        }
        curl_easy_cleanup(curl);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("invalid arguments!\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        perror("Unable to open file");
        return 1;
    }

    char url[1024];
    int num_urls = 0;
    
    while (fgets(url, sizeof(url), file) && num_urls < MAX_URLS)
    {
        url[strcspn(url, "\n")] = 0;

        char output_filename[50];
        snprintf(output_filename, sizeof(output_filename), "output_%d.dat", num_urls);

        pid_t pid = fork();
        if (pid == 0)
        {
            curl_global_init(CURL_GLOBAL_DEFAULT);
            download_file(url, output_filename);
            curl_global_cleanup();
            exit(0);
        }
        else if (pid < 0)
        {
            perror("Failed to fork");
            fclose(file);
            return 1;
        }

        num_urls++;
    }

    fclose(file);

    while (wait(NULL) > 0)
        ;

    return 0;
}
