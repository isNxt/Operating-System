#include<iostream>
#include"scheduling.h"
using namespace std;
int main()
{
	ScheduliAlgor *scheduliAlgor = new ScheduliAlgor;
	scheduliAlgor->InputProc();
	scheduliAlgor->FCFS();
	//scheduliAlgor->RR();
	//scheduliAlgor->SPN();
	//scheduliAlgor->SRT();
	//scheduliAlgor->HRRN();
	getchar();
	getchar();
	return 0;
}
