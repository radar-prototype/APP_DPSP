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
 
// 结构体：存放需要交互的数据
typedef struct
{
    pid_t pid;  // 当前在进行数据读取的进程号
    xd_vsip_cmview_f *buf; // 要传输的数据
} SHM;
 
void handler(int signo)
{
    // printf("get signal\n");
    return;
}
 
int main()
{
    // 使用key_t的值来创建或者打开信号量， 共享内存和消息队列
    key_t key;
    // 共享内存表示符，共享内存申请函数shmget的返回结果
    int shmid;
    // 数据结构：存储进程号和要交互的数据
    SHM *p;
    // 进程号
    xd_vsip_cmview_f *temp = xd_vsip_cmcreate_f(10,10,XD_VSIP_ROW,XD_VSIP_MEM_CONST);
    pid_t pid;

    
    // ftok()创建key值， -1表示失败
    if ((key = ftok(".", 'm')) < 0) 
    {
        perror("fail to ftok");
        exit(-1);
    }
 
    //  注冊一个信号处理函数
    signal(SIGUSR1, handler);

    /* int shmget(key_t key, int size, int shmflg): 共享内存申请函数
    参数：
        key: 用来标识共享内存块的键值
        size: 内存块的大小：数据结构体的大小
        shmflg: 操作标识：
            IPC_CREAT: 创建新的
            IPC_EXCL: 与IPC_CREAT同时使用，假设已有一个具有键值的共享内存块存在，则shmget会调用失败
    返回值：
        成功：返回共享内存标识符shmid
        失败：-1
    */     
    if ((shmid = shmget(key, sizeof(SHM), 0666 | IPC_CREAT | IPC_EXCL)) < 0)
    {
        // shmget执行失败时：
        // 存在则直接打开
        if (EEXIST == errno)                
        {
            
            shmid = shmget(key, sizeof(SHM), 0666);
 
            p = (SHM *)shmat(shmid, NULL, 0);
            p->buf = temp;
            
 
            pid = p->pid;
            p->pid = getpid();
            printf("fail pid: %d\n", p->pid);
            kill(pid, SIGUSR1);
        }
        // 打开失败
        else
        {
            perror("fail to shmget");
            exit(-1);
        }
    }
    else
    {
        // shmget执行成功时：
        /* int shmat(int shmid, char *shmaddr, int shmflag) 映射共享内存
        参数：
            shmid: 共享内存的ID， 是shmget()函数返回的标识符
            shmaddr: 共享内存的起始地址，一般为0
            shmflag： 操作模式 SHM_RDONLY仅读模式， 其他是读写模式
        返回值：
            成功： 共享内存的起始地址
            失败： -1
        */
        p = (SHM *)shmat(shmid, NULL, 0);
        p->pid = getpid();                  //  把自己的pid写到共享内存
        p->buf = temp;
        printf("success pid: %d\n", p->pid);
        pause();
        pid = p->pid;                       //  得到读端进程的pid
    }

    while ( 1 )
    {
        printf("writer start!\n");
        printf("write to shm:");
        xd_vsip_cmfill_f(xd_vsip_cmplx_f(1.,1.),p->buf);       //  接收输入
        printf("send to pid: %d\n", pid);
        pid = p->pid;
        p->pid = getpid();
        kill(pid, SIGUSR1);                 //  向读进程发SIGUSR1信号

        printf("wait read\n");
        pause();                            //  堵塞，等待信号
    }
    /*
    int shmdt(char *shmaddr): 解除共享内存映射
    参数：
        shmdt：共享内存起始地址
    返回值：
        成功：0
        失败：-1

    */
    shmdt(p);
    /*
    int shmctl(int shmid, int cmd, struct shmid_ds * buf) 删除共享内存
    参数：
        shmid: 共享内存ID
        cmd: 控制命令
        buf: 共享内存状态的结构体指针 
            IPC_STAT: 得到共享内存的状态
            IPC_SET: 改变共享内存的状态
            IPC_RMID: 删除共享内存 
    返回值：
        成功:0
        失败:1
    */
    shmctl(shmid, IPC_RMID, NULL);          //  删除共享内存
 
    return 0;
}