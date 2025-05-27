#include "ipc.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define TOBACCO 0
#define PAPER   1
#define GLUE    2

int agent_sem;
int smoker_sems[3];
int mutex;

void *agent(void *arg) {
    while(1) {
        down(mutex);
        int r = rand() % 3;
        printf("Agent provides: ");
        switch(r) {
            case 0: printf("Paper and Glue\n"); break;
            case 1: printf("Tobacco and Glue\n"); break;
            case 2: printf("Tobacco and Paper\n"); break;
        }
        up(smoker_sems[r]);
        up(mutex);
        down(agent_sem);
    }
}

void *smoker(void *arg) {
    int id = *(int *)arg;
    while(1) {
        down(smoker_sems[id]);
        down(mutex);
        printf("Smoker %d is making and smoking...\n", id);
        sleep(1);
        up(agent_sem);
        up(mutex);
    }
}

int main() {
    // 初始化信号量
    agent_sem = set_sem(401, 0, IPC_CREAT | 0644);
    mutex = set_sem(402, 1, IPC_CREAT | 0644);
    
    for(int i=0; i<3; i++) {
        smoker_sems[i] = set_sem(403+i, 0, IPC_CREAT | 0644);
    }
    
    pthread_t agent_thread, smoker_threads[3];
    int smoker_ids[3] = {TOBACCO, PAPER, GLUE};
    
    pthread_create(&agent_thread, NULL, agent, NULL);
    for(int i=0; i<3; i++) {
        pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]);
    }
    
    pthread_join(agent_thread, NULL);
    for(int i=0; i<3; i++) {
        pthread_join(smoker_threads[i], NULL);
    }
    
    return 0;
}
