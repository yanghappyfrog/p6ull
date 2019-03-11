#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H

typedef struct worker Worker;
typedef void *(*WorkHandle)(void *arg);

struct worker
{
    void *(*process)(void *arg);
    void *arg;
    struct worker *next;
};

typedef struct
{
    pthread_mutex_t queue_lock;
    pthread_cond_t  queue_ready;
    /* 等待任务 */
    Worker *queue_head;
    pthread_t * threadid;
    int thread_pool_sum; /* 当前总的任务队列数 */
    int cur_queue_num;   /* 当前等待队列的任务数目 */
    int cur_run_num;     /* 当前正在运行的线程*/ 
}ThreadPool;

extern void  pool_init();
extern int   pool_add_worker();
extern void  pool_destroy();

#endif

