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

    //�򿪹����ڴ�
    shmid = shmget(KEY, sizeof(int)*4, 0);
    pBuf = (int*)shmat(shmid, 0, 0);
    output = pBuf;

    if(pBuf==NULL)
        printf("�����ڴ渽��ʧ��!\n");

    //�����ź���
    semid = semget(SEM_KEY,3, IPC_CREAT|0660);

    for(i=0; i<4; i++)
    {
        P(semid,SEM_FULL);     //�൱��P(SEM_FULL),�����ȡ������������
        P(semid,SEM_MUTEX);    //�൱��P(SEM_MUTEX),������ʻ�����

        //�ڻ������ж�ȡ����,��1��Ϊ0
        output = pBuf;
		while((*output)==0)
		{
			output++;
		}
		(*output)=0;

		//��ȡ��ǰϵͳʱ��
        struct timeval curtime;
		gettimeofday(&curtime,NULL);

        printf("������%s�ڻ������ж�ȡ���ݣ���ȡʱ��Ϊ��%ld:%ld\n",
			argv[0],curtime.tv_sec,curtime.tv_usec);
		printf("��ʱ����������״̬Ϊ:\n");
		printf("[ %d ][ %d ][ %d ]\n",*(pBuf),*(pBuf+1),*(pBuf+2));


		V(semid,SEM_MUTEX);     //�൱��V(SEM_MUTEX),�ͷ��ź���SEM_MUTEX
		V(semid,SEM_EMPTY);     //�൱��V(SEM_EMPTY),�ͷ��ź���SEM_EMPTY

		//����ȴ�һ��ʱ��
		int time = random()%5;
		sleep(time);
    }
    return 0;
}

//P����
void P(int semid, int sem_num)
{
    struct sembuf sops;
    sops.sem_num=sem_num;
    sops.sem_op=-1;
	sops.sem_flg=0;
    semop(semid, &sops, 1);
}

//V����
void V(int semid, int sem_num)
{
    struct sembuf sops;
    sops.sem_num=sem_num;
    sops.sem_op=1;
	sops.sem_flg=0;
    semop(semid, &sops, 1);
}
