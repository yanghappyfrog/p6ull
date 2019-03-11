
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "./pool.h"
#include "../include/type.h"

static ThreadPool *pool = NULL;
static int exit_bool = FALSE;

int pool_add_worker(WorkHandle process, void *arg)
{

    Worker *new_worker = (Worker*)malloc(sizeof(Worker));

    new_worker->process = process;
    new_worker->arg = arg;
    new_worker->next = NULL;

    pthread_mutex_lock(&(pool->queue_lock));

    Worker *member = pool->queue_head;
    
    if (member != NULL){
       while (member->next != NULL) 
           member = member->next;
       member->next = new_worker;
    }else{
        pool->queue_head = new_worker;
    }

    if (pool->queue_head == NULL)
        return FALSE;

    pool->cur_queue_num++;
    pthread_mutex_unlock(&(pool->queue_lock));
    if (pool->cur_run_num != pool->thread_pool_sum)
        pthread_cond_signal(&(pool->queue_ready));

    return TRUE;
}

void *thread_routine(void *args)
{

    while (TRUE){
        pthread_mutex_lock(&(pool->queue_lock));
        pthread_cond_wait(&(pool)->queue_ready, &(pool->queue_lock));

        while (0 == pool->cur_queue_num){
            pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
        }

        pool->cur_queue_num--;
        pool->cur_run_num++;
        Worker *worker = pool->queue_head;
        pool->queue_head = worker->next;
        pthread_mutex_unlock(&(pool->queue_lock));

        (*(worker->process))(worker->arg);	
        free(worker);						
        worker = NULL;
        pool->cur_run_num--;

        if (pool->cur_run_num != pool->thread_pool_sum && pool->cur_queue_num != 0)
            pthread_cond_signal(&(pool->queue_ready));
        if (!exit_bool)
            break;
    }
    
    pthread_exit(NULL);
}

void pool_init(int thread_pool_sum)
{
    pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    pthread_mutex_init(&(pool->queue_lock), NULL);
    pthread_cond_init(&(pool->queue_ready), NULL);

    pool->queue_head = NULL;
    pool->thread_pool_sum = thread_pool_sum;
    pool->cur_queue_num = 0;
    pool->cur_run_num = 0;

    pool->threadid = (pthread_t*)malloc(thread_pool_sum * sizeof(pthread_t));

    int i = 0;

    for (i = 0; i < thread_pool_sum; i++)
    {
        exit_bool = 0;
        pthread_create(&(pool->threadid[i]), NULL, thread_routine, NULL); 
    }

    sleep(1); 
}

void pool_destroy()
{
    pthread_cond_broadcast(&(pool->queue_ready));
    int pool_num;

    for (pool_num = 0; pool_num < pool->thread_pool_sum; pool_num++){
        pthread_join(pool->threadid[pool_num], NULL);
        //pthread_exit((void*)pool->threadid[pool_num]);
    }

    free(pool->threadid);
    Worker *head = NULL;

    while (pool->queue_head != NULL)
    {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free(head);
    }

    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));

    free(pool);
    pool = NULL;
    return ;
}

