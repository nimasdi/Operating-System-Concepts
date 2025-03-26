#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define CAR_COUNT_PER_SPAWNER 20
#define MAX_SLEEP 3

enum Direction
{
    NORTH,
    SOUTH,
    EAST,
    WEST
};

pthread_mutex_t north_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t south_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t east_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t west_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

void safe_print(const char *message)
{
    pthread_mutex_lock(&print_mutex);
    printf("%s", message);
    pthread_mutex_unlock(&print_mutex);
}

// For handeling right turn logic
void turn_right(int id, int direction)
{
    pthread_mutex_t *dir_mutex = NULL;
    char message[256];

    switch (direction)
    {
    case NORTH:
        dir_mutex = &north_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning right from North to West\n", id, direction);
        break;
    case SOUTH:
        dir_mutex = &south_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning right from South to East\n", id, direction);
        break;
    case EAST:
        dir_mutex = &east_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning right from East to North\n", id, direction);
        break;
    case WEST:
        dir_mutex = &west_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning right from West to South\n", id, direction);
        break;
    default:
        return;
    }

    pthread_mutex_lock(dir_mutex);
    safe_print(message);

    int sleep_time = rand() % (MAX_SLEEP + 1);
    sleep(sleep_time);

    pthread_mutex_unlock(dir_mutex);
}

// For handeling Streight logic
void go_straight(int id, int direction)
{
    pthread_mutex_t *dir_mutex = NULL;
    char message[256];

    switch (direction)
    {
    case NORTH:
        dir_mutex = &north_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is going straight from North to South\n", id, direction);
        break;
    case SOUTH:
        dir_mutex = &south_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is going straight from South to North\n", id, direction);
        break;
    case EAST:
        dir_mutex = &east_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is going straight from East to West\n", id, direction);
        break;
    case WEST:
        dir_mutex = &west_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is going straight from West to East\n", id, direction);
        break;
    default:
        return;
    }

    pthread_mutex_lock(dir_mutex);
    safe_print(message);

    // Simulate straight movement with randomized but controlled sleep
    int sleep_time = rand() % (MAX_SLEEP + 1);
    sleep(sleep_time);

    pthread_mutex_unlock(dir_mutex);
}

// For handeling left turn logic
void turn_left(int id, int direction)
{
    pthread_mutex_t *dir_mutex = NULL;
    char message[256];

    switch (direction)
    {
    case NORTH:
        dir_mutex = &north_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning left from North to East\n", id, direction);
        break;
    case SOUTH:
        dir_mutex = &south_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning left from South to West\n", id, direction);
        break;
    case EAST:
        dir_mutex = &east_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning left from East to South\n", id, direction);
        break;
    case WEST:
        dir_mutex = &west_mutex;
        snprintf(message, sizeof(message),
                 "car with id %d with source %d is turning left from West to North\n", id, direction);
        break;
    default:
        return;
    }

    pthread_mutex_lock(dir_mutex);
    safe_print(message);

    int sleep_time = rand() % (MAX_SLEEP + 1);
    sleep(sleep_time);

    pthread_mutex_unlock(dir_mutex);
}

typedef struct
{
    int source;
    int count;
} SpawnerArgs;

// DO NOT CHANGE - Spawns cars from a given direction in the intersection
void *car_spawner(void *args)
{
    SpawnerArgs *spawner_args = (SpawnerArgs *)args;
    int source = spawner_args->source;
    int count = spawner_args->count;
    for (int i = 0; i < count; i++)
    {
        int move = rand() % 3;

        switch (move)
        {
        case 0:
            turn_right(i, source);
            break;
        case 1:
            go_straight(i, source);
            break;
        case 2:
            turn_left(i, source);
            break;
        default:
            break;
        }
    }
}

// DO NOT CHANGE - Simulation Logic
int main()
{
    pthread_t spawner_tid[4];
    SpawnerArgs args[4];
    clock_t time;
    time = clock();
    for (int i = 0; i < 4; i++)
    {
        args[i].source = i;
        args[i].count = CAR_COUNT_PER_SPAWNER;
        pthread_create(&spawner_tid[i], NULL, car_spawner, (void *)&args[i]);
    }
    for (int i = 0; i < 4; i++)
        pthread_join(spawner_tid[i], NULL);
    time = clock() - time;
    double runtime = ((double)time);
    printf("the simulation took %f seconds\n", runtime);

    pthread_mutex_destroy(&north_mutex);
    pthread_mutex_destroy(&south_mutex);
    pthread_mutex_destroy(&east_mutex);
    pthread_mutex_destroy(&west_mutex);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}