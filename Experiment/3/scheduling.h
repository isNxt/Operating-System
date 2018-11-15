#include<iostream>
using namespace std;
class ScheduliAlgor
{
public:
	ScheduliAlgor();
	~ScheduliAlgor();
	void InputProc();
	void FCFS();
	void RR();
	void SPN();
	void SRT();
	void HRRN(); 
private:
	void OutputInfor( char * name);
	int FindTheShortestProc( int runTime);
	int FindHightestRate (int runTime);
	int FindShortestRemain(int runTime);
	int FindNextExcuteProc( int currentProc, int runTime);
	void SetProInfor(int currentProc , int runTime);
	void ClearProcInfor();
	struct Proc
	{
		char ID;
		unsigned int arrivedTime;
		unsigned int serverTime;
	};
	struct ProcInfor
	{
		unsigned int finishTime;
		unsigned int turnaroundTime;
		double rate;
		bool isExcuted;
		bool isFirst;
		int remainTime;
	};
	int length;
	struct Proc *proc;
	struct ProcInfor *proInfor;
	void put();
};
