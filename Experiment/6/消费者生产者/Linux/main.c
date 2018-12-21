#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define SEM_EMPTY 0
#define SEM_FULL 1
#define SEM_MUTEX 2
#define KEY 3
#define SEM_KEY 4

int main()
{
    int i;
    int shmid,id;

    //���������ڴ���
    shmid = shmget(KEY, sizeof(int)*4,IPC_CREAT | IPC_EXCL | 0666);
    if(shmid<0)
    {
        printf("�����ڴ洴��ʧ��!\n");
        return 0;
    }

    //�����ź�������
    id = semget(SEM_KEY,3,IPC_CREAT|0660);
    //��ʼ���ź���
    semctl(id,SEM_EMPTY,SETVAL,3);
    semctl(id,SEM_FULL,SETVAL,0);
    semctl(id,SEM_MUTEX,SETVAL,1);

    //���������߽���
    if(fork()==0)
        execl("producer","1",(char*)0);
    if(fork()==0)
        execl("producer","2",(char*)0);

    //���������߽���
    if(fork()==0)
        execl("consumer","1",(char*)0);
    if(fork()==0)
        execl("consumer","2",(char*)0);
    if(fork()==0)
        execl("consumer","3",(char*)0);

    //�ȴ��ӽ��̽���
    int status;
    for(i=0; i<5; i++)
        wait(&status);

    //ɾ�������ڴ���
    shmctl(shmid, IPC_RMID, 0);

    return 0;
}

