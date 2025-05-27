#include "ipc.h"
#include <string.h>

// 全局变量定义
key_t buff_key;
int buff_num;
char *buff_ptr;
key_t pput_key;
int pput_num;
int *pput_ptr;
key_t cget_key;
int cget_num;
int *cget_ptr;
key_t prod_key;
key_t pmtx_key;
int prod_sem;
int pmtx_sem;
key_t cons_key;
key_t cmtx_key;
int cons_sem;
int cmtx_sem;
int sem_val;
int sem_flg;
int shm_flg;

int get_ipc_id(char *proc_file, key_t key) {
    FILE *pf;
    int i, j;
    char line[BUFSZ], column[BUFSZ];

    if ((pf = fopen(proc_file, "r")) == NULL) {
        perror("Proc file not open");
        exit(EXIT_FAILURE);
    }

    fgets(line, BUFSZ, pf);
    while (!feof(pf)) {
        i = j = 0;
        fgets(line, BUFSZ, pf);
        while (line[i] == ' ') i++;
        while (line[i] != ' ') column[j++] = line[i++];
        column[j] = '\0';
        if (atoi(column) != key) continue;

        j = 0;
        while (line[i] == ' ') i++;
        while (line[i] != ' ') column[j++] = line[i++];
        column[j] = '\0';
        i = atoi(column);
        fclose(pf);
        return i;
    }
    fclose(pf);
    return -1;
}

int down(int sem_id) {
    struct sembuf buf;
    buf.sem_op = -1;
    buf.sem_num = 0;
    buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &buf, 1) < 0) {
        perror("down error");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int up(int sem_id) {
    struct sembuf buf;
    buf.sem_op = 1;
    buf.sem_num = 0;
    buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &buf, 1) < 0) {
        perror("up error");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int set_sem(key_t sem_key, int sem_val, int sem_flg) {
    int sem_id;
    Sem_uns sem_arg;

    if ((sem_id = get_ipc_id("/proc/sysvipc/sem", sem_key)) < 0) {
        if ((sem_id = semget(sem_key, 1, sem_flg)) < 0) {
            perror("semaphore create error");
            exit(EXIT_FAILURE);
        }
        sem_arg.val = sem_val;
        if (semctl(sem_id, 0, SETVAL, sem_arg) < 0) {
            perror("semaphore set error");
            exit(EXIT_FAILURE);
        }
    }
    return sem_id;
}

char *set_shm(key_t shm_key, int shm_num, int shm_flg) {
    int shm_id;
    char *shm_buf;

    if ((shm_id = get_ipc_id("/proc/sysvipc/shm", shm_key)) < 0) {
        if ((shm_id = shmget(shm_key, shm_num, shm_flg)) < 0) {
            perror("shareMemory set error");
            exit(EXIT_FAILURE);
        }
        if ((shm_buf = shmat(shm_id, 0, 0)) == (char *)-1) {
            perror("get shareMemory error");
            exit(EXIT_FAILURE);
        }
        memset(shm_buf, 0, shm_num);
    } else {
        if ((shm_buf = shmat(shm_id, 0, 0)) == (char *)-1) {
            perror("get shareMemory error");
            exit(EXIT_FAILURE);
        }
    }
    return shm_buf;
}

int set_msg(key_t msg_key, int msg_flag) {
    int msg_id;
    if ((msg_id = get_ipc_id("/proc/sysvipc/msg", msg_key)) < 0) {
        if ((msg_id = msgget(msg_key, msg_flag)) < 0) {
            perror("messageQueue set error");
            exit(EXIT_FAILURE);
        }
    }
    return msg_id;
}
