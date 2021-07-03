// makefile
/*
hw4.out : hw4.o
        gcc -pthread -o hw4.out hw4.o
hw4.o : hw4.c
        gcc -c -o hw4.o hw4.c
clean :
        rm *.o hw4.out
*/
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#define BUFFER_SIZE 10              // buffer size
#define RAND_DIVISOR 100000000      // Rand divisor value
typedef int buffer_item;

/* the semaphores */
sem_t empty;
sem_t full;
sem_t mutex;

/* the buffer */
buffer_item buf[BUFFER_SIZE];

pthread_t tid;	// thread ID
pthread_attr_t attr;	// Set of thread attributes

/* the initialize */
void init();
/* the consumer thread*/
void* consumer(void* param);
/* the producer thread*/
void* producer(void* param);
/* buffer count */
int buf_count;

int insert_item(int item);
int remove_item(int* item);

int main() {

    int num_producer = 10;   // number of producer threads 10
    int num_consumer = 10;   // number of consumer threads 10
    long taskids[] = { 0,1,2,3,4,5,6,7,8,9 };       // For producer and consumer ID  0~9

    /* initialize the app */
    init();

    for (int i = 0; i < num_producer; i++) {
        pthread_create(&tid, &attr, producer, taskids + i);  // create the thread
        printf("Createing producer # %d\n", i);
    }
    for (int i = 0; i < num_consumer; i++) {
        pthread_create(&tid, &attr, consumer, taskids + i);  // create the thread
        printf("Createing consumer # %d\n", i);
    }

    pthread_join(tid, NULL);    // waiting the thread end to free 

    exit(0);
}

void init() {
    /* Create the mutex semaphore lock */
    sem_init(&mutex, 0, 1);
    /* Create the full semaphore and initialize to BUFFER_SIZE */
    sem_init(&full, 0, BUFFER_SIZE);
    /* Create the empty semaphore and initialize to 0 */
    sem_init(&empty, 0, 0);
    /* Get ther default attributes */
    pthread_attr_init(&attr);
    //ptrhead_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
    /* init buffer */
    buf_count = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) buf[i] = 0;
}
void* producer(void* param) {
    buffer_item item;
    int* producer_id = (int*)param;
    while (1) {
        /* sleep for a random period of time  */
        int r_num = rand() / RAND_DIVISOR;
        sleep(r_num);
        /* Create random item number */
        item = rand();

        sem_wait(&full);    /* Lock empty semaphore if not zero */
        sem_wait(&mutex);   /* if semaphore value is greater than 1, descrease it and immediatly exit the func */
        if (insert_item(item)) {
            printf("Producer %d error %d\n", *producer_id, item);
        }
        else {
            printf("Producer %d produced %d\n", *producer_id, item);
        }
        sem_post(&mutex);   /* increase semaphore value and wake up another thread that waiting if semaphore is greater than 1. */
        sem_post(&empty);   /* Increments semaphore for # of full */

    }
}
void* consumer(void* param) {
    buffer_item item;
    int* consumer_id = (int*)param;
    while (1) {
        /* sleep for a random period of time  */
        int r_num = rand() / RAND_DIVISOR;
        sleep(r_num);

        sem_wait(&empty);   /* Lock empty semaphore if not zero */
        sem_wait(&mutex);   /* if semaphore value is greater than 1, descrease it and immediatly exit the func */
        if (remove_item(&item)) {
            printf("Consumer %d error %d\n", *consumer_id, item);
        }
        else {
            printf("Consumer %d consumed %d\n", *consumer_id, item);
        }
        sem_post(&mutex);   /* increase semaphore value and wake up another thread that waiting if semaphore is greater than 1. */
        sem_post(&full);    /* Increments semaphore for # of full */
    }
}
int insert_item(buffer_item item) {
    if (buf_count < BUFFER_SIZE) {      /* buffer have space */
        buf[buf_count] = item;
        buf_count++;
        return 0;
    }
    else {                              /* buffer is full */
        return -1;
    }
}
int remove_item(buffer_item* item) {
    if (buf_count > 0) {                /* buffer have item value */
        *item = buf[(buf_count - 1)];
        buf_count--;
        return 0;
    }
    else {                              /* buffer empty */
        return -1;
    }
}

