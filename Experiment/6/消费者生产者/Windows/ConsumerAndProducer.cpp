#include<iostream>
#include<stdio.h>
#include<Windows.h>
#include<time.h>
using namespace std;

#define BUF_SIZE 256
HANDLE hMapFile =NULL;     //�ڴ�ӳ���ļ�����ľ��
HANDLE CurrentProcess;     //��ǰ���̵ľ��
int *pBuf,*input,*output =NULL;		   //�����ڴ��ָ��

HANDLE SEM_FULL;           //��¼��ʹ�õĻ�������Ŀ���ź���
HANDLE SEM_EMPTY;		   //��¼�յĻ�������Ŀ���ź���
HANDLE SEM_MUTEX;		   //ȷ�������ߺ������߻�����ʻ��������ź���

int CreateShareMemory();   //���������ڴ棬��������
PROCESS_INFORMATION StartClone(int nCloneID);     //����һ����¡�Ľ��̣����ؽ��̺��߳���Ϣ
void Producer(int nClone); //������
void Consumer(int nClone); //������

int main(int argc, char* argv[])
{
	int i,nClone=0;
	//PROCESS_INFORMATION hp[5];         
	HANDLE np[5];					   //����5�����̵ľ��

	//�ӵڶ�����������ȡ��¡ID
	if(argc >1)
	{
		sscanf(argv[1],"%d",&nClone);
	}

	if( nClone == 0)                   //ID==0��������,ִ�����´���
	{
		//�����ź���
		SEM_FULL = CreateSemaphore(
				NULL,                  //�ź����İ�ȫ����,NULL��ʹ��Ĭ�ϰ�ȫ����
				0,					   //��ʼ�����ź���
				3,					   //���ź������ӵ����ֵ
				"FULL");			   //�ź���������
		SEM_EMPTY = CreateSemaphore(NULL,3,3,"EMPTY");
		SEM_MUTEX = CreateSemaphore(NULL,1,1,"MUTEX");

		
		if( CreateShareMemory()== -1 )     //���������ڴ�
			return 0;

		//����5�����̣��ȴ������������ߣ��ٴ�������������
		for(i=0;i<5;i++)			   
		{
			np[i] = StartClone(++nClone).hProcess;
		}
		
		//�ȴ��ӽ��̽���
		WaitForMultipleObjects(5,np,true,INFINITE);
		//for(i=0;i<5;i++)
		//	WaitForSingleObject(hp[i].hProcess,INFINITE);

		//���н�����ر��ӽ��̵ľ��
		for (i=0;i<5;i++)
		{
			CloseHandle(np[i]);
		}

		//ȡ���ļ�ӳ��
		UnmapViewOfFile(pBuf);

		//�ر��ź����������ڴ�ľ��
		CloseHandle(SEM_MUTEX);
        CloseHandle(hMapFile);
        CloseHandle(SEM_EMPTY);
       	CloseHandle(SEM_FULL);

		printf("�����������....\n");
		getchar();

	}
	else if(nClone>0 && nClone<3)	   //�����Ľ�����nClone���֣�nClone=1��2�Ĵ���������
	{
		Producer(nClone);
	}
	else if(nClone>2 && nClone<6)	   //nClone=3��4��5�Ĵ���������
	{
		Consumer(nClone);
	}
	return 0;
}

//���������ڴ棬��������
int CreateShareMemory()
{
	CurrentProcess = GetCurrentProcess();

	hMapFile = CreateFileMapping(
					INVALID_HANDLE_VALUE,     //ʹ�÷�ҳ�ļ�,����һ��ʹ������ҳ�ļ�����ָ�������ļ��洢�����ļ�ӳ�����
					NULL,					  //ʹ��Ĭ�ϰ�ȫ����
					PAGE_READWRITE,			  //�����д����
					0,						  //��DWORD�ĸ�λ�д�Ŵ�С       
					BUF_SIZE,                 //��DWORD�ĵ�λ�д�Ŵ�С
					"buffer");                //������������

	if (hMapFile == NULL) 
	{ 
		printf("�޷������ڴ�ӳ���ļ�����(File Mapping Object)\n");
		return -1;
	}

	pBuf = (int*)MapViewOfFile(
				hMapFile,                     //�ڴ�ӳ���ļ�����ľ��
				FILE_MAP_ALL_ACCESS,		  //�����д����
				0,							 
				0,							 
				BUF_SIZE);					  

	if (pBuf == NULL) 
	{ 
		printf("�޷�ӳ���ļ���ͼ\n");
		CloseHandle(hMapFile);
		return -1;
	}
	else
	{
		ZeroMemory(pBuf,BUF_SIZE);            //�ڴ�����
	}

	input = pBuf;
	return 1;
}

//����һ����¡�Ľ��̣����ؽ��̺��߳���Ϣ
PROCESS_INFORMATION StartClone(int nCloneID)
{
	//������ڵ�ǰ��ִ���ļ����ļ���
	TCHAR szFilename[MAX_PATH];
	GetModuleFileName(NULL,szFilename,MAX_PATH);

	//�����ӽ��������еĸ�ʽ�������Ӧ�ó����EXE�ļ���
	//�Ϳ�¡���̵�IDֵ
	TCHAR szCmdLine[MAX_PATH];
	sprintf(szCmdLine,"\"%s\" %d",szFilename,nCloneID);

	STARTUPINFO si;											//�����ӽ��̵�STARTUPINFO�ṹ
	ZeroMemory(reinterpret_cast<void*>(&si),sizeof(si));	//reinterpret_cast��C++���ǿ������ת������ֻ�޸���
															//�����������ͣ����������½����˸����Ķ���ı���ģ��
															//��û�н��ж�����ת��
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;                                 //˵��һ�����ڼ�¼�ӽ��̵������Ϣ�Ľṹ����

	//����ͬ���Ŀ�ִ���ļ��������д�������
	BOOL bCreateOK = CreateProcess(
		szFilename,                           //��ִ�е�Ӧ�ó��������
		szCmdLine,		     				  //ָ������һ���ӽ��̵ķ��ű�ʶ
		NULL,								  //ȱʡ�˽��̰�ȫ��
		NULL,								  //ȱʡ���̰߳�ȫ��
		FALSE,                                //���̳д��ļ��ľ��
		NULL,               				  //��ʹ���µĿ���̨
		NULL,								  //�µĻ���
		NULL,								  //��ǰĿ¼
		&si,								  //������Ϣ
		&pi);								  //���ؽ��̺��̵߳���Ϣ

	return pi;
}

//������
void Producer(int nClone)
{
	int i,j,time;          //�ȴ���ʱ��

	for(i=0; i<6; i++)
	{
		//���ź���
		SEM_FULL=OpenSemaphore(
			SEMAPHORE_ALL_ACCESS,                   //��ȫ���ԣ�
			NULL,									//���̳�
			"FULL");								//�򿪵��ź�������
		SEM_EMPTY=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"EMPTY");
		SEM_MUTEX=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"MUTEX");

		hMapFile = OpenFileMapping(
			FILE_MAP_WRITE,							//��ȫ����
			FALSE,									//���̳�
			"buffer");								//������������
		pBuf = (int*)MapViewOfFile(hMapFile,FILE_MAP_WRITE,0,0,BUF_SIZE);
	
		input = pBuf;

		//�ȴ��ź����������P(SEM_MUTEX),P(SEM_EMPTY)
		WaitForSingleObject(SEM_EMPTY,INFINITE);
		WaitForSingleObject(SEM_MUTEX,INFINITE);

		//�򻺳�����Ӳ�Ʒ,��0��Ϊ1
		while(*input==1)
		{
			input++;
		}
		*input=1;

		//���ϵͳʱ��
		SYSTEMTIME curtime;
		GetSystemTime(&curtime);

		printf("������%d����������������ݣ����ʱ��Ϊ��%02d:%02d:%02d:%03d\n",
			nClone,curtime.wHour+8,curtime.wMinute,curtime.wSecond,curtime.wMilliseconds);
		printf("��ʱ����������״̬Ϊ:\n");
		printf("������������������\n");
		printf("��%2d����%2d����%2d��\n",*(pBuf),*(pBuf+1),*(pBuf+2));
		printf("������������������\n");

		//�ͷ��ź���,�൱��V(SEM_MUTEX),V(SEM_FULL)
		ReleaseSemaphore(SEM_MUTEX,1,NULL);
		ReleaseSemaphore(SEM_FULL,1,NULL);	

		//�رվ��
		CloseHandle(SEM_MUTEX);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
		CloseHandle(hMapFile);

		//����ȴ�һ��ʱ��
		time = rand()%1000*5;
		Sleep(time);
	}

	//ȡ���ļ�ӳ��
	UnmapViewOfFile(pBuf);

	return;
}

//������
void Consumer(int nClone)
{
	int i,time;          //�ȴ���ʱ��

	for(i=0; i<4; i++)
	{
		//���ź���
		SEM_FULL=OpenSemaphore(
			SEMAPHORE_ALL_ACCESS,                   //��ȫ���ԣ�
			NULL,									//���̳�
			"FULL");								//�򿪵��ź�������
		SEM_EMPTY=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"EMPTY");
		SEM_MUTEX=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"MUTEX");

		hMapFile = OpenFileMapping(
			FILE_MAP_WRITE,							//��ȫ����
			FALSE,									//���̳�
			"buffer");								//������������
		pBuf = (int*)MapViewOfFile(hMapFile,FILE_MAP_WRITE,0,0,BUF_SIZE);
	
		output = pBuf;

		//�ȴ��ź����������P(SEM_MUTEX),P(SEM_EMPTY)
		WaitForSingleObject(SEM_FULL,INFINITE);
		WaitForSingleObject(SEM_MUTEX,INFINITE);
	
		//�ڻ�������ȡ����Ʒ,��1��Ϊ0
		while(*output==0)
		{
			output++;
		}
		*output=0;

		//���ϵͳʱ��
		SYSTEMTIME curtime;
		GetSystemTime(&curtime);

		printf("������%d���������ж�ȡ���ݣ���ȡʱ��Ϊ��%02d:%02d:%02d:%03d\n",
			nClone-2,curtime.wHour+8,curtime.wMinute,curtime.wSecond,curtime.wMilliseconds);
		printf("��ʱ����������״̬Ϊ:\n");
		printf("������������������\n");
		printf("��%2d����%2d����%2d��\n",*(pBuf),*(pBuf+1),*(pBuf+2));
		printf("������������������\n");

		//�ͷ��ź���,�൱��V(SEM_MUTEX),V(SEM_FULL)
		ReleaseSemaphore(SEM_MUTEX,1,NULL);
		ReleaseSemaphore(SEM_EMPTY,1,NULL);	

		//�رվ��
		CloseHandle(SEM_MUTEX);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
		CloseHandle(hMapFile);

		//����ȴ�һ��ʱ��
		time = rand()%1000*5;
		Sleep(time);
	}
	//ȡ���ļ�ӳ��
	UnmapViewOfFile(pBuf);

	return;
}

