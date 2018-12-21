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
    int *pBuf = NULL,*input;

    //�򿪹����ڴ�
    shmid = shmget(KEY, sizeof(int)*4, 0);
    pBuf = (int*)shmat(shmid, 0, 0);
    input = pBuf;

    if(pBuf==NULL)
        printf("�����ڴ渽��ʧ��!\n");

    //�����ź���
    semid = semget(SEM_KEY,3, IPC_CREAT|0660);

    for(i=0; i<6; i++)
    {
        P(semid,SEM_EMPTY);     //�൱��P(SEM_EMPTY),������л�����
        P(semid,SEM_MUTEX);     //�൱��P(SEM_MUTEX),������ʻ�����

		//�򻺳�����Ӳ�Ʒ,��0��Ϊ1
		input = pBuf;
		while((*input)==1)
		{
			input++;
		}
		(*input)=1;

        //��ȡ��ǰϵͳʱ��
		struct timeval curtime;
		gettimeofday(&curtime,NULL);

		printf("������%s����������������ݣ����ʱ��Ϊ��%02d:%02d\n",
			argv[0],curtime.tv_sec,curtime.tv_usec);
        printf("��ʱ����������״̬Ϊ:\n");
		printf("[ %d ][ %d ][ %d ]\n",*(pBuf),*(pBuf+1),*(pBuf+2));

		V(semid,SEM_MUTEX);     //�൱��V(SEM_MUTEX)
		V(semid,SEM_FULL);      //�൱��V(SEM_FULL)

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
