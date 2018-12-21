#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>

#define SEM_EMPTY 0
#define SEM_FULL 1
#define SEM_MUTEX 2
#define KEY 3
#define SEM_KEY 4

void P(int semid, int sem_num);
void V(int semid, int sem_num);

int main(int argc,char * argv[])
{
    int i;
    int shmid,semid;
    int *pBuf = NULL,*output;

    //打开共享内存
    shmid = shmget(KEY, sizeof(int)*4, 0);
    pBuf = (int*)shmat(shmid, 0, 0);
    output = pBuf;

    if(pBuf==NULL)
        printf("共享内存附加失败!\n");

    //创建信号量
    semid = semget(SEM_KEY,3, IPC_CREAT|0660);

    for(i=0; i<4; i++)
    {
        P(semid,SEM_FULL);     //相当于P(SEM_FULL),申请读取缓冲区的数据
        P(semid,SEM_MUTEX);    //相当于P(SEM_MUTEX),申请访问缓冲区

        //在缓冲区中读取数据,将1置为0
        output = pBuf;
		while((*output)==0)
		{
			output++;
		}
		(*output)=0;

		//获取当前系统时间
        struct timeval curtime;
		gettimeofday(&curtime,NULL);

        printf("消费者%s在缓冲区中读取数据，读取时间为：%ld:%ld\n",
			argv[0],curtime.tv_sec,curtime.tv_usec);
		printf("此时，缓冲区的状态为:\n");
		printf("[ %d ][ %d ][ %d ]\n",*(pBuf),*(pBuf+1),*(pBuf+2));


		V(semid,SEM_MUTEX);     //相当于V(SEM_MUTEX),释放信号量SEM_MUTEX
		V(semid,SEM_EMPTY);     //相当于V(SEM_EMPTY),释放信号量SEM_EMPTY

		//随机等待一段时间
		int time = random()%5;
		sleep(time);
    }
    return 0;
}

//P操作
void P(int semid, int sem_num)
{
    struct sembuf sops;
    sops.sem_num=sem_num;
    sops.sem_op=-1;
	sops.sem_flg=0;
    semop(semid, &sops, 1);
}

//V操作
void V(int semid, int sem_num)
{
    struct sembuf sops;
    sops.sem_num=sem_num;
    sops.sem_op=1;
	sops.sem_flg=0;
    semop(semid, &sops, 1);
}
