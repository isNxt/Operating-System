#include<iostream>  
#include<string>  
#include<cstdlib>  
#include <cctype>  
#include<time.h>  
using namespace std;

#define MEMORYSIZE 500   

//�ļ������ڵ�  
struct FATableNode
{
	string name;
	unsigned int indexBlockLoction;
	FATableNode *next;
};

//��Ϊ�����Ŀ�Ľṹ��  
struct indexedBlockNode
{
	int *location;//��̬����  
	int size;
};

class Solution
{
public:
	Solution();
	~Solution();
	void run();

private:
	bool bitTable[MEMORYSIZE];//λ���ж���һ���Ƿ��Ѿ���ռ��,false��ʾδ��ռ��  
	FATableNode *headFATable;
	FATableNode *tailFATable;
	indexedBlockNode *indexedBlockNodeTable;

	int getRandLocation(int size);
	int getRandSize();
	void setFirstFiftyBlock();
	void setFATableNode(int location, string name);
	void setBlockNodeTable(int location, double size);
	void deleteOddFATableNode();
	void setFiveAddedFile();
	void findLastFiveFile();
	bool isLastFiveFile(string name);
	void putOutFileInformation(FATableNode *node);
	string getName(int i);
	bool ifFileNameIsOdd(string name);
	void Solution::deleteIndexNode(FATableNode *deleteNode);
	void Solution::testOut();

};

Solution::Solution()
{

	headFATable = NULL;
	tailFATable = NULL;
	indexedBlockNodeTable = new indexedBlockNode[MEMORYSIZE];
	if (indexedBlockNodeTable == NULL)
		return;

	for (int i = 0; i < MEMORYSIZE; i++) {
		bitTable[i] = false;
		indexedBlockNodeTable[i].location = NULL;
		indexedBlockNodeTable[i].size = 0;
	}
}

Solution::~Solution()
{
	for (int i = 0; i < MEMORYSIZE; i++)
	{
		if (bitTable[i])
		{
			int *deleteArray = indexedBlockNodeTable[i].location;
			delete deleteArray;
			deleteArray = NULL;
		}
	}
	delete indexedBlockNodeTable;
	indexedBlockNodeTable = NULL;

	while (headFATable != NULL)
	{
		FATableNode *head = headFATable;
		headFATable = headFATable->next;
		delete head;
		head = NULL;
	}
}


//ɾ�������ļ����������ж��ļ����Ƿ���������ɾ���ļ�  
void Solution::deleteOddFATableNode()
{
	FATableNode *phead = headFATable;
	FATableNode *pafter = headFATable->next;

	while (phead)
	{
		if (ifFileNameIsOdd(phead->name))
		{
			if (phead == headFATable)
			{
				headFATable = headFATable->next;
				pafter = headFATable;
			}

			FATableNode *deleteNode = phead;
			phead = phead->next;

			//�������������������ع���һ��������get  
			deleteIndexNode(deleteNode);
		}
		else {
			if (phead == headFATable)
				pafter = headFATable;
			else {
				pafter->next = phead;
				pafter = pafter->next;
			}
			phead = phead->next;
		}
	}
	tailFATable = pafter;
}

//���������  
void Solution::deleteIndexNode(FATableNode *deleteNode)
{
	//�������������  
	int location = deleteNode->indexBlockLoction;
	indexedBlockNode temp = indexedBlockNodeTable[location];
	int i = 0;
	while (i<temp.size)
	{
		bitTable[temp.location[i]] = false;
		++i;
	}
	delete temp.location;
	temp.location = NULL;
	//����������ɾ�����  

	delete deleteNode;
	deleteNode = NULL;
}

//�ж��ļ����Ƿ���������������򷵻�true  
bool Solution::ifFileNameIsOdd(string name)
{
	//������ܻ��鷳һ�㣬�����ȵð��ļ���ȡ������Ȼ��ת��������  
	string sub = name.substr(0, name.size() - 4);//".txt".size()==4  
	int number = atoi(name.c_str());
	if (number & 1)
		return true;
	return false;
}

//���������--λ��  
int Solution::getRandLocation(int size)
{
	srand(time(NULL));
	int temp = rand() % size;
	while (bitTable[temp])
	{
		temp = rand() % size;
	}
	return temp;
}

//������������ļ���С������������Ķ���������  
//2--10Ӧ�����а����������ģ����ˣ��������ɣ�˼·���ͺ�  
int Solution::getRandSize()
{
	srand(time(NULL));
	return rand() % 9 + 2;
}

//����ͷ��ʮ�����ݣ��ļ���С�����������С2-10��  
//Ϊģ����ʵЧ��������ʮ��������ã���������˳���ŷţ�  
//��Ȼ��Щ�ڴ�һ��ʼ��ʼ����ʱ����˳���ŷ�  
void Solution::setFirstFiftyBlock()
{
	for (int i = 0; i < 50; ++i)
	{
		int location = getRandLocation(MEMORYSIZE);
		bitTable[location] = true;

		string name = getName(i + 1);
		setFATableNode(location, name);

		int size = getRandSize();
		setBlockNodeTable(location, size);
	}
}

//ר��Ϊǰ50�����ķ������֣���Ϊ����Ƚ��鷳  
string Solution::getName(int i)
{
	char str[5];
	_itoa_s(i, str, 10);
	return str;
}

//����������  
void Solution::setBlockNodeTable(int location, double size)
{
	int blockNum = size / 2;
	if ((blockNum * 2)<size)
		++blockNum;

	indexedBlockNodeTable[location].location = new int[blockNum];
	if (indexedBlockNodeTable[location].location == NULL)
		return;

	for (int i = 0; i < blockNum; i++)
	{
		int loca = getRandLocation(MEMORYSIZE);
		indexedBlockNodeTable[location].location[i] = loca;
		bitTable[loca] = true;
	}
	indexedBlockNodeTable[location].size = blockNum;

}

//�����ļ������  
void Solution::setFATableNode(int location, string name)
{
	FATableNode *fatTableNode = new FATableNode;
	fatTableNode->name = name;
	fatTableNode->indexBlockLoction = location;
	fatTableNode->next = NULL;
	if (fatTableNode == NULL)
		return;

	if (headFATable == NULL)
	{
		headFATable = fatTableNode;
		tailFATable = headFATable;
	}
	else
	{
		tailFATable->next = fatTableNode;
		tailFATable = fatTableNode;
	}
}


//�����¼��������ļ�  
void Solution::setFiveAddedFile()
{
	string name[5] = { "A.txt","B.txt","C.txt","D.txt","E.txt" };
	double size[5] = { 7,5,2,9,3.5 };
	for (int i = 0; i < 5; i++)
	{
		int location = getRandLocation(MEMORYSIZE);
		bitTable[location] = true;
		setFATableNode(location, name[i]);
		setBlockNodeTable(location, size[i]);

	}
}

//���Һ�����������ļ�  
void Solution::findLastFiveFile()
{
	FATableNode *head = headFATable;
	int count = 0;
	while (head != NULL)
	{
		if (isLastFiveFile(head->name))
		{
			++count;
			putOutFileInformation(head);
		}
		if (count == 5)
			return;
		head = head->next;
	}
}

void Solution::putOutFileInformation(FATableNode *node)
{
	int location = node->indexBlockLoction;
	cout << "�ļ���:" << node->name << " �ļ�������λ��:" << location << "  " << "�ļ���ֲ�λ��:";

	int length = indexedBlockNodeTable[location].size;
	for (int i = 0; i < length; i++)
	{
		cout << indexedBlockNodeTable[location].location[i] << " ";
	}
	cout << endl;
}

//�ж��Ƿ��Ǻ�����������ļ�֮һ  
bool Solution::isLastFiveFile(string Sname)
{
	string name[5] = { "A.txt","B.txt","C.txt","D.txt","E.txt" };
	for (int i = 0; i < 5; i++)
	{
		if (Sname.compare(name[i]) == 0)
			return true;
	}
	return false;
}

void Solution::run()
{
	setFirstFiftyBlock();
	//testOut();  
	deleteOddFATableNode();
	//testOut();  
	setFiveAddedFile();
	//testOut();  


	cout << "=====500���ļ���ʹ�����,0��ʾδʹ�ã�1��ʾ�Ѿ�ʹ��=====" << endl;
	for (int i = 0; i < MEMORYSIZE; i++)
	{
		if (bitTable[i])
			cout << "1";
		else
			cout << "0";
		if ((i + 1) % 50 == 0)
			cout << endl;
	}
	cout << endl;
	cout << "===�����ӵ�����ļ��ֲ����===" << endl;
	findLastFiveFile();
}

void Solution::testOut()
{
	FATableNode *head = headFATable;
	while (head)
	{
		cout << head->name << " " << head->indexBlockLoction << endl;
		head = head->next;
	}
	cout << endl << endl;
}

int main()
{
	Solution *solu = new Solution;
	solu->run();
	getchar();
	getchar();
	return 0;
}