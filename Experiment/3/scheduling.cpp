#include"scheduling.h"
ScheduliAlgor::ScheduliAlgor()
{
	proc = NULL;
	proInfor = NULL;
	length = 0;
}
ScheduliAlgor::~ScheduliAlgor()
{
	delete [] proc;
	proc = NULL;
	delete [] proInfor;
	proInfor = NULL;
}
void ScheduliAlgor::InputProc()
{
	cout<<"Input the numbers of proc:";
	cin>>length;
	if(length<1)
	{
		cout<<"Error"<<endl;
		return ;
	}
	proc = new struct Proc[length];
	if(proc == NULL)
	{
		cout<<"Error"<<endl;
		return ;
	}
	proInfor = new struct ProcInfor[length];
	if (proInfor == NULL)
	{
		cout<<"Error"<<endl;
		return ;
	}
	ClearProcInfor();
	cout<<"Input arrive time and sever time:"<<endl;
	for (int i = 0; i < length; i++)
	{
		cin>>proc[i].arrivedTime>>proc[i].serverTime;
		proc[i].ID = 'A'+i;
		if (proc[i].serverTime == 0)
		{
			cout<<"Error"<<endl;
			return ;
		}
	}
 
}
void ScheduliAlgor::FCFS()
{
	int runTime = 0;
	for (int i = 0; i < length; i++)
	{
		if (proc[i].arrivedTime>runTime)
		{
			runTime = proc[i].arrivedTime;
		}
		runTime = runTime + proc[i].serverTime;
		SetProInfor(i,runTime);
	}
	OutputInfor("FCFS");
	ClearProcInfor();
}
void ScheduliAlgor::RR()
{
	int runTime = 0;
	int excutedProcNumbers = 0;
	int currentProc = 0;
	for (int i = 0; i < length; i++)
	{
		proInfor[i].remainTime = proc[i].serverTime;
	}
	while (excutedProcNumbers < length)
	{
		currentProc = FindNextExcuteProc(currentProc, runTime);
		if (currentProc < 0)
		{
			++runTime;
		}
		else
		{
			--proInfor[currentProc].remainTime;
			++runTime;
			if(proInfor[currentProc].remainTime ==0 )
			{
				SetProInfor(currentProc,runTime);
				excutedProcNumbers++;
			}
		}
	}
	OutputInfor("RR(q=1)");
	ClearProcInfor();
}
int ScheduliAlgor::FindNextExcuteProc( int currentProc, int runTime)
{
	if (proc[0].arrivedTime > runTime)
	{
		return -1;
	}
	int tMark = currentProc;
	int mark = --currentProc;
	if(mark<0)
		mark+= length;
	int excuteTime = 0;
	bool isFind = false;
	bool isFirstOne = true;
	while (excuteTime < length)
	{
		if(proInfor[mark].isExcuted)
		{  }
		else
		{
			if (proc[mark].arrivedTime<=runTime)
			{
				if (isFirstOne)
				{
					isFirstOne = !isFirstOne;
					isFind = true;
					currentProc = mark;
				}
				else
				{
					if (tMark == mark)
					{   }
					else if(( proc[mark].arrivedTime<runTime)&&
						(proc[currentProc].arrivedTime==runTime))
					{
						currentProc = mark;
					}
				}
			}
		}
		--mark;
		if(mark<0)
			mark+=length;
		++excuteTime;
	}
	if(isFind)
		return currentProc;
	return -1;
}
void ScheduliAlgor::SPN()
{
	int runTime = 0;
	int excutedProcNumbers = 0;
	int currentProc = 0;
	while(excutedProcNumbers<length)
	{
		currentProc = FindTheShortestProc( runTime);
		if(currentProc<0)
		{
			runTime++;
		}
		else
		{
			runTime = runTime + proc[currentProc].serverTime;
			SetProInfor(currentProc,runTime);
			++excutedProcNumbers;
		}
	}
	OutputInfor("SPN");
	ClearProcInfor();
}
int ScheduliAlgor::FindTheShortestProc( int runTime)
{
	int mark = -1;
	bool isFirstOne = true;
	for (int i = 0; i < length; i++)
	{
		if (!proInfor[i].isExcuted)
		{
			if (proc[i].arrivedTime<=runTime)
			{
				if(isFirstOne)
				{
					mark = i;
					isFirstOne = !isFirstOne;
				}
				else
				{
					if(proc[i].serverTime<proc[mark].serverTime)
						mark = i;
				}
			}
		}
	}
	return mark;
}
void ScheduliAlgor::SRT()
{
	int runTime = 0;
	int excutedProcNumbers = 0;
	int currentProc = 0;
	for (int i = 0; i < length; i++)
	{
		proInfor[i].remainTime = proc[i].serverTime;
	}
	while(excutedProcNumbers < length)
	{
		currentProc = FindShortestRemain(runTime);
		if(currentProc < 0)
		{
			++runTime;
		}
		else
		{
			++runTime;
			--proInfor[currentProc].remainTime;
			if(proInfor[currentProc].remainTime == 0)
			{
				SetProInfor(currentProc,runTime);
				excutedProcNumbers++;
			}
		}
	}
	OutputInfor("SRT");
	ClearProcInfor();
}
int ScheduliAlgor::FindShortestRemain(int runTime)
{
	bool isFirstOne = true;
	int shortestRemainTime = 0;
	int mark = -1;
	for (int i = 0; i < length; i++)
	{
		if (!proInfor[i].isExcuted)
		{
			if(proc[i].arrivedTime<=runTime)
			{
				if(isFirstOne)
				{
					shortestRemainTime = proInfor[i].remainTime;
					mark = i;
					isFirstOne = !isFirstOne;
				}
				else
				{
					int tempTime = proInfor[i].remainTime;
					if(shortestRemainTime > tempTime)
					{
						shortestRemainTime = tempTime;
						mark = i;
					}
				}
			}
		}
	}
	return mark;
}
void ScheduliAlgor::HRRN( )
{
	int currentProc = 0;
	int runTime = 0;
	int excutedProcNumbers = 0;
	while (excutedProcNumbers < length)
	{
		currentProc = FindHightestRate(runTime);
		if(currentProc < 0)
		{
			++runTime;
		}
		else
		{
			runTime = runTime + proc[currentProc].serverTime;
			SetProInfor(currentProc, runTime);
			++excutedProcNumbers;
		}
	}
	OutputInfor("HRRN");
	ClearProcInfor();
}
int ScheduliAlgor::FindHightestRate(int runTime)
{
	int mark = -1;
	bool isFirstOne = true;
	double rate=0;
	for (int i = 0; i < length; i++)
	{
		if(!proInfor[i].isExcuted)
		{
			if(proc[i].arrivedTime <= runTime)
			{
				if(isFirstOne)
				{
					mark = i;
					rate = (double)(runTime + proc[i].serverTime - proc[i].arrivedTime)/(double)proc[i].serverTime;
					isFirstOne = !isFirstOne;
				}
				else
				{
					double tempRate = (double)(runTime + proc[i].serverTime - proc[i].arrivedTime)/(double)proc[i].serverTime;
					if (tempRate > rate)
					{
						rate = tempRate;
						mark = i;
					}
				}
			}
		}
	}
	return mark;
}
 
void ScheduliAlgor::ClearProcInfor()
{
	for (int i = 0; i < length; i++)
	{
		proInfor[i].finishTime = 0;
		proInfor[i].turnaroundTime = 0;
		proInfor[i].rate = 0;
		proInfor[i].turnaroundTime = 0;
		proInfor[i].remainTime = 0;
		proInfor[i].isExcuted = false;
		proInfor[i].isFirst = true;
	}
}
void ScheduliAlgor::SetProInfor(int currentProc , int runTime)
{
	proInfor[currentProc].finishTime = runTime;
	proInfor[currentProc].isExcuted = true;
	proInfor[currentProc].turnaroundTime = proInfor[currentProc].finishTime - proc[currentProc].arrivedTime;
	proInfor[currentProc].rate = (double)proInfor[currentProc].turnaroundTime/(double)proc[currentProc].serverTime;
}
void ScheduliAlgor::OutputInfor(char *name)
{
	double totalTurnaroundTime = 0;
	double totalRate = 0;
	cout<<"Scheduling discipline is "<<name<<""<<endl;
	cout<<"     finish   "<<"turnaround   "<<"rate"<<endl;
	for (int i = 0; i < length; i++)
	{
	cout<<proc[i].ID<<"\t"<<proInfor[i].finishTime<<"\t"<<proInfor[i].turnaroundTime<<"\t"<<proInfor[i].rate<<endl;
	}
}
