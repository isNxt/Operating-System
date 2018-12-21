#include<iostream>
#include<stdio.h>
#include<Windows.h>
#include<time.h>
using namespace std;

#define BUF_SIZE 256
HANDLE hMapFile =NULL;     //内存映射文件对象的句柄
HANDLE CurrentProcess;     //当前进程的句柄
int *pBuf,*input,*output =NULL;		   //共享内存的指针

HANDLE SEM_FULL;           //记录已使用的缓冲区数目的信号量
HANDLE SEM_EMPTY;		   //记录空的缓冲区数目的信号量
HANDLE SEM_MUTEX;		   //确保生产者和消费者互斥访问缓冲区的信号量

int CreateShareMemory();   //建立共享内存，即缓冲区
PROCESS_INFORMATION StartClone(int nCloneID);     //创建一个克隆的进程，返回进程和线程信息
void Producer(int nClone); //生产者
void Consumer(int nClone); //消费者

int main(int argc, char* argv[])
{
	int i,nClone=0;
	//PROCESS_INFORMATION hp[5];         
	HANDLE np[5];					   //保存5个进程的句柄

	//从第二个参数中提取克隆ID
	if(argc >1)
	{
		sscanf(argv[1],"%d",&nClone);
	}

	if( nClone == 0)                   //ID==0是主进程,执行以下代码
	{
		//创建信号量
		SEM_FULL = CreateSemaphore(
				NULL,                  //信号量的安全属性,NULL即使用默认安全属性
				0,					   //初始化的信号量
				3,					   //许信号量增加到最大值
				"FULL");			   //信号量的名称
		SEM_EMPTY = CreateSemaphore(NULL,3,3,"EMPTY");
		SEM_MUTEX = CreateSemaphore(NULL,1,1,"MUTEX");

		
		if( CreateShareMemory()== -1 )     //创建共享内存
			return 0;

		//创建5个进程，先创建两个生产者，再创建两个消费者
		for(i=0;i<5;i++)			   
		{
			np[i] = StartClone(++nClone).hProcess;
		}
		
		//等待子进程结束
		WaitForMultipleObjects(5,np,true,INFINITE);
		//for(i=0;i<5;i++)
		//	WaitForSingleObject(hp[i].hProcess,INFINITE);

		//运行结束后关闭子进程的句柄
		for (i=0;i<5;i++)
		{
			CloseHandle(np[i]);
		}

		//取消文件映射
		UnmapViewOfFile(pBuf);

		//关闭信号量及共享内存的句柄
		CloseHandle(SEM_MUTEX);
        CloseHandle(hMapFile);
        CloseHandle(SEM_EMPTY);
       	CloseHandle(SEM_FULL);

		printf("按任意键结束....\n");
		getchar();

	}
	else if(nClone>0 && nClone<3)	   //创建的进程以nClone区分，nClone=1或2的代表生产者
	{
		Producer(nClone);
	}
	else if(nClone>2 && nClone<6)	   //nClone=3、4或5的代表消费者
	{
		Consumer(nClone);
	}
	return 0;
}

//建立共享内存，即缓冲区
int CreateShareMemory()
{
	CurrentProcess = GetCurrentProcess();

	hMapFile = CreateFileMapping(
					INVALID_HANDLE_VALUE,     //使用分页文件,创建一个使用来自页文件而非指定磁盘文件存储器的文件映射对象
					NULL,					  //使用默认安全属性
					PAGE_READWRITE,			  //允许读写操作
					0,						  //在DWORD的高位中存放大小       
					BUF_SIZE,                 //在DWORD的低位中存放大小
					"buffer");                //缓冲区的名字

	if (hMapFile == NULL) 
	{ 
		printf("无法创建内存映射文件对象(File Mapping Object)\n");
		return -1;
	}

	pBuf = (int*)MapViewOfFile(
				hMapFile,                     //内存映射文件对象的句柄
				FILE_MAP_ALL_ACCESS,		  //允许读写操作
				0,							 
				0,							 
				BUF_SIZE);					  

	if (pBuf == NULL) 
	{ 
		printf("无法映射文件视图\n");
		CloseHandle(hMapFile);
		return -1;
	}
	else
	{
		ZeroMemory(pBuf,BUF_SIZE);            //内存清零
	}

	input = pBuf;
	return 1;
}

//创建一个克隆的进程，返回进程和线程信息
PROCESS_INFORMATION StartClone(int nCloneID)
{
	//获得用于当前可执行文件的文件名
	TCHAR szFilename[MAX_PATH];
	GetModuleFileName(NULL,szFilename,MAX_PATH);

	//创建子进程命令行的格式化，获得应用程序的EXE文件名
	//和克隆进程的ID值
	TCHAR szCmdLine[MAX_PATH];
	sprintf(szCmdLine,"\"%s\" %d",szFilename,nCloneID);

	STARTUPINFO si;											//用于子进程的STARTUPINFO结构
	ZeroMemory(reinterpret_cast<void*>(&si),sizeof(si));	//reinterpret_cast是C++里的强制类型转换符，只修改了
															//操作数的类型，但仅是重新解释了给出的对象的比特模型
															//而没有进行二进制转换
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;                                 //说明一个用于记录子进程的相关信息的结构变量

	//利用同样的可执行文件和命令行创建进程
	BOOL bCreateOK = CreateProcess(
		szFilename,                           //可执行的应用程序的名称
		szCmdLine,		     				  //指定创建一个子进程的符号标识
		NULL,								  //缺省了进程安全性
		NULL,								  //缺省了线程安全性
		FALSE,                                //不继承打开文件的句柄
		NULL,               				  //不使用新的控制台
		NULL,								  //新的环境
		NULL,								  //当前目录
		&si,								  //启动信息
		&pi);								  //返回进程和线程的信息

	return pi;
}

//生产者
void Producer(int nClone)
{
	int i,j,time;          //等待的时间

	for(i=0; i<6; i++)
	{
		//打开信号量
		SEM_FULL=OpenSemaphore(
			SEMAPHORE_ALL_ACCESS,                   //安全属性，
			NULL,									//不继承
			"FULL");								//打开的信号量名称
		SEM_EMPTY=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"EMPTY");
		SEM_MUTEX=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"MUTEX");

		hMapFile = OpenFileMapping(
			FILE_MAP_WRITE,							//安全属性
			FALSE,									//不继承
			"buffer");								//缓冲区的名称
		pBuf = (int*)MapViewOfFile(hMapFile,FILE_MAP_WRITE,0,0,BUF_SIZE);
	
		input = pBuf;

		//等待信号量，相对于P(SEM_MUTEX),P(SEM_EMPTY)
		WaitForSingleObject(SEM_EMPTY,INFINITE);
		WaitForSingleObject(SEM_MUTEX,INFINITE);

		//向缓冲区添加产品,将0置为1
		while(*input==1)
		{
			input++;
		}
		*input=1;

		//获得系统时间
		SYSTEMTIME curtime;
		GetSystemTime(&curtime);

		printf("生产者%d往缓冲区中添加数据，添加时间为：%02d:%02d:%02d:%03d\n",
			nClone,curtime.wHour+8,curtime.wMinute,curtime.wSecond,curtime.wMilliseconds);
		printf("此时，缓冲区的状态为:\n");
		printf("┏━┓┏━┓┏━┓\n");
		printf("┃%2d┃┃%2d┃┃%2d┃\n",*(pBuf),*(pBuf+1),*(pBuf+2));
		printf("┗━┛┗━┛┗━┛\n");

		//释放信号量,相当于V(SEM_MUTEX),V(SEM_FULL)
		ReleaseSemaphore(SEM_MUTEX,1,NULL);
		ReleaseSemaphore(SEM_FULL,1,NULL);	

		//关闭句柄
		CloseHandle(SEM_MUTEX);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
		CloseHandle(hMapFile);

		//随机等待一段时间
		time = rand()%1000*5;
		Sleep(time);
	}

	//取消文件映射
	UnmapViewOfFile(pBuf);

	return;
}

//消费者
void Consumer(int nClone)
{
	int i,time;          //等待的时间

	for(i=0; i<4; i++)
	{
		//打开信号量
		SEM_FULL=OpenSemaphore(
			SEMAPHORE_ALL_ACCESS,                   //安全属性，
			NULL,									//不继承
			"FULL");								//打开的信号量名称
		SEM_EMPTY=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"EMPTY");
		SEM_MUTEX=OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,"MUTEX");

		hMapFile = OpenFileMapping(
			FILE_MAP_WRITE,							//安全属性
			FALSE,									//不继承
			"buffer");								//缓冲区的名称
		pBuf = (int*)MapViewOfFile(hMapFile,FILE_MAP_WRITE,0,0,BUF_SIZE);
	
		output = pBuf;

		//等待信号量，相对于P(SEM_MUTEX),P(SEM_EMPTY)
		WaitForSingleObject(SEM_FULL,INFINITE);
		WaitForSingleObject(SEM_MUTEX,INFINITE);
	
		//在缓冲区中取出产品,将1置为0
		while(*output==0)
		{
			output++;
		}
		*output=0;

		//获得系统时间
		SYSTEMTIME curtime;
		GetSystemTime(&curtime);

		printf("消费者%d往缓冲区中读取数据，读取时间为：%02d:%02d:%02d:%03d\n",
			nClone-2,curtime.wHour+8,curtime.wMinute,curtime.wSecond,curtime.wMilliseconds);
		printf("此时，缓冲区的状态为:\n");
		printf("┏━┓┏━┓┏━┓\n");
		printf("┃%2d┃┃%2d┃┃%2d┃\n",*(pBuf),*(pBuf+1),*(pBuf+2));
		printf("┗━┛┗━┛┗━┛\n");

		//释放信号量,相当于V(SEM_MUTEX),V(SEM_FULL)
		ReleaseSemaphore(SEM_MUTEX,1,NULL);
		ReleaseSemaphore(SEM_EMPTY,1,NULL);	

		//关闭句柄
		CloseHandle(SEM_MUTEX);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
		CloseHandle(hMapFile);

		//随机等待一段时间
		time = rand()%1000*5;
		Sleep(time);
	}
	//取消文件映射
	UnmapViewOfFile(pBuf);

	return;
}

