#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vsip.h>
#define N 64
 
typedef struct
{
    pid_t pid;
    xd_vsip_cmview_f *buf;
} SHM;

void handler(int signo)
{
    printf("get signal\n");
    return;
}
 
int main()
{
    key_t key;
    int shmid;
    SHM *p;
    xd_vsip_cmview_f *temp = xd_vsip_cmcreate_f(10,10,XD_VSIP_ROW,XD_VSIP_MEM_CONST);
    pid_t pid;
 
    if ((key = ftok(".", 'm')) < 0)
    {
        perror("fail to ftok");
        exit(-1);
    }
 
    signal(SIGUSR1, handler);//注冊一个信号处理函数
    if ((shmid = shmget(key, sizeof(SHM), 0666|IPC_CREAT|IPC_EXCL)) < 0)
    {
        if (EEXIST == errno)//存在则直接打开
        {
            shmid = shmget(key, sizeof(SHM), 0666);
            p = (SHM *)shmat(shmid, NULL, 0);
            p->buf = temp;
            pid = p->pid;
            p->pid = getpid();//把自己的pid写到共享内存
            printf("fail pid: %d\n", p->pid);
            kill(pid, SIGUSR1);
        }
        else//出错
        {
            perror("fail to shmget");
            exit(-1);
        }
    }
    else//成功
    {
        p = (SHM *)shmat(shmid, NULL, 0);
        p->pid = getpid();
        p->buf = temp;
        printf("success pid: %d\n", p->pid);
        pause();
        pid = p->pid;//得到写端进程的pid
    }
 
    while ( 1 )
    {
        printf("wait data!\n");
        pause();//堵塞，等待信号
        printf("reader start!\n");


        printf("read from shm : %f", xd_vsip_cmget_f(p->buf,0,0).r);
        printf("send to pid: %d\n", pid);
        pid = p->pid;
        p->pid = getpid();
        kill(pid, SIGUSR1);//向写进程发SIGUSR1信号
    }

    return 0;
}