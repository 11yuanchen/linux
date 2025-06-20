#ifndef IPC_H
#define IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define BUFSZ 256

// 函数声明
int get_ipc_id(char *proc_file, key_t key);
char *set_shm(key_t shm_key, int shm_num, int shm_flag);
int set_sem(key_t sem_key, int sem_val, int sem_flag);
int down(int sem_id);
int up(int sem_id);

// 信号量共用体
typedef union semuns {
    int val;
} Sem_uns;

// 消息结构体
typedef struct msgbuf {
    long mtype;
    char mtext[1];
} Msg_buf;

// 全局变量声明
extern key_t buff_key;
extern int buff_num;
extern char *buff_ptr;
extern key_t pput_key;
extern int pput_num;
extern int *pput_ptr;
extern key_t cget_key;
extern int cget_num;
extern int *cget_ptr;
extern key_t prod_key;
extern key_t pmtx_key;
extern int prod_sem;
extern int pmtx_sem;
extern key_t cons_key;
extern key_t cmtx_key;
extern int cons_sem;
extern int cmtx_sem;
extern int sem_val;
extern int sem_flg;
extern int shm_flg;

#endif
