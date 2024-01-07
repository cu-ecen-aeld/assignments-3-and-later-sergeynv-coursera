#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

bool __run(const thread_data_t * params)
{
    if (usleep(params->wait_to_obtain_ms * 1000) != 0) {
        perror("usleep() before locking the mutex failed");
        return false;
    }

    if (pthread_mutex_lock(params->mutex) != 0) {
        perror("pthread_mutex_LOCK_() failed");
        return false;
    }

    if (usleep(params->wait_to_release_ms * 1000) != 0) {
        perror("usleep() while holding the mutex failed");
        return false;
    }

    if (pthread_mutex_unlock(params->mutex) != 0) {
        perror("pthread_mutex_UN.LOCK_() failed");
        return false;
    }

    return true;
}

void* threadfunc(void* thread_param)
{
    thread_data_t * params = (thread_data_t *) thread_param;
    params->thread_complete_success = __run(params);
    return thread_param;
}

bool start_thread_obtaining_mutex(
    pthread_t * thread,
    pthread_mutex_t * mutex,
    int wait_to_obtain_ms,
    int wait_to_release_ms)
{
    thread_data_t * td = malloc(sizeof(thread_data_t));
    if (td == NULL) {
        perror("malloc() failed");
        return false;
    }
    td->mutex = mutex;
    td->wait_to_obtain_ms = wait_to_obtain_ms;
    td->wait_to_release_ms = wait_to_release_ms;

    if (pthread_create(thread, /* attr */ NULL, threadfunc, td) != 0) {
        perror("pthread_create() failed");
        return false;
    }

    return true;
}

