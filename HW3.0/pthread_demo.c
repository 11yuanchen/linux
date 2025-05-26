#include <stdio.h>
#include <pthread.h>

void *thread_func(void *arg) {
    printf("Hello from thread! Thread ID: %lu\n", (unsigned long)pthread_self());
    return NULL;
}

int main() {
    pthread_t tid;
    printf("Main thread starts.\n");
    pthread_create(&tid, NULL, thread_func, NULL);
    pthread_join(tid, NULL); // 等待线程结束
    printf("Main thread exits.\n");
    return 0;
}
