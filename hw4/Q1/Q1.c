#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PRODUCTS 4
#define NUM_ASSEMBLY_MACHINES 4
#define NUM_QUALITY_CHECK_MACHINES 3
#define NUM_PACKAGING_MACHINES 2

#define BUFFER_SIZE_1 5
#define BUFFER_SIZE_2 3

/*
  use additional mutex if you need.
  explain what is the reason, which part bocame thread safe after
*/

sem_t assembly_machines;
sem_t quality_machines;
sem_t packaging_machines;

sem_t buffer1;
sem_t buffer2;

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

void assembly(int product_id)
{
    /*
        simulate the process of assembling a product :
        1- wait till one of machines in assembling stage get freed.product_id
        2- acquire machine, print suitable message shows which product is assembling .
        3- wait for acquire one place in the queue between stage 1 and 2 .
    */
    sem_wait(&assembly_machines); // Wait for an available assembly machine
    printf("Product %d: Assembling...\n", product_id);
    sleep(1);
    printf("Product %d: Assembled.\n", product_id);
    sem_wait(&buffer1); // Wait for space in buffer1
    printf("Product %d: Waiting in buffer 1.\n", product_id);
    sem_post(&assembly_machines); // Release assembly machine
}

void quality_check(int product_id)
{
    /*
        simulate the process of quality checking for a product :
        1- wait till one of machines in quality checking stage get freed.product_id
        2- dequeue from buffer1, acquire one machine, print suitable message shows which product is under quality checking.
        3- wait for acquire one place in the queue between stage 2 and 3.
    */

    sem_wait(&quality_machines); // Wait for an available quality check machine
    pthread_mutex_lock(&m1);     // Ensure thread-safe access to buffer1
    sem_post(&buffer1);          // Dequeue product from buffer1
    pthread_mutex_unlock(&m1);
    printf("Product %d: Quality checking...\n", product_id);
    sleep(2);
    printf("Product %d: Quality checked.\n", product_id);
    sem_wait(&buffer2); // Wait for space in buffer2
    printf("Product %d: Waiting in buffer 2.\n", product_id);
    sem_post(&quality_machines); // Release quality check machine
}

void packaging(int product_id)
{
    /*
        simulate the process of packaging a product :
        1- wait till one of machines in packaging stage get freed.product_id
        2- dequeue from buffer2, acquire one machine, print suitable message shows which product is packaging.
        3- exit the pipeline.
    */
   
    sem_wait(&packaging_machines); // Wait for an available packaging machine
    pthread_mutex_lock(&m2);       // Ensure thread-safe access to buffer2
    sem_post(&buffer2);            // Dequeue product from buffer2
    pthread_mutex_unlock(&m2);
    printf("Product %d: Packaging...\n", product_id);
    sleep(1); // Simulate packaging process
    printf("Product %d: Packaged.\n", product_id);
    sem_post(&packaging_machines); // Release packaging machine
}

void *product(void *arg)
{
    int product_id = *(int *)arg;
    assembly(product_id);
    quality_check(product_id);
    packaging(product_id);
    printf("End of pipeline for Product %d\n", product_id);
    return NULL;
}

int main()
{
    pthread_t products[NUM_PRODUCTS];
    int product_ids[NUM_PRODUCTS];

    sem_init(&assembly_machines, 0, NUM_ASSEMBLY_MACHINES);
    sem_init(&quality_machines, 0, NUM_QUALITY_CHECK_MACHINES);
    sem_init(&packaging_machines, 0, NUM_PACKAGING_MACHINES);
    sem_init(&buffer1, 0, BUFFER_SIZE_1);
    sem_init(&buffer2, 0, BUFFER_SIZE_2);

    for (int i = 0; i < NUM_PRODUCTS; i++)
    {
        product_ids[i] = i + 1;
        pthread_create(&products[i], NULL, product, &product_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCTS; i++)
    {
        pthread_join(products[i], NULL);
    }

    sem_destroy(&assembly_machines);
    sem_destroy(&quality_machines);
    sem_destroy(&packaging_machines);
    sem_destroy(&buffer1);
    sem_destroy(&buffer2);

    printf("All products have been processed.\n");
    return 0;
}
