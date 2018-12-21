#include<iostream>  
#include<string>  
#include<cstdlib>  
#include <cctype>  
#include<time.h>  
using namespace std;

#define MEMORYSIZE 500   

//文件分配表节点  
struct FATableNode
{
	string name;
	unsigned int indexBlockLoction;
	FATableNode *next;
};

//作为索引的块的结构体  
struct indexedBlockNode
{
	int *location;//动态数组  
	int size;
};

class Solution
{
public:
	Solution();
	~Solution();
	void run();

private:
	bool bitTable[MEMORYSIZE];//位表，判断这一块是否已经被占用,false表示未被占用  
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


//删除奇数文件，下面用判断文件名是否是奇数的删除文件  
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

			//把清空索引块这个功能重构成一个函数，get  
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

//清空索引块  
void Solution::deleteIndexNode(FATableNode *deleteNode)
{
	//清空索引快内容  
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
	//索引块内容删除完毕  

	delete deleteNode;
	deleteNode = NULL;
}

//判断文件名是否是奇数，如果是则返回true  
bool Solution::ifFileNameIsOdd(string name)
{
	//这里可能会麻烦一点，我首先得把文件名取出来，然后转化成数字  
	string sub = name.substr(0, name.size() - 4);//".txt".size()==4  
	int number = atoi(name.c_str());
	if (number & 1)
		return true;
	return false;
}

//产生随机数--位置  
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

//产生随机数，文件大小，我这里产生的都是整数，  
//2--10应该是有包含浮点数的，算了，就这样吧，思路懂就好  
int Solution::getRandSize()
{
	srand(time(NULL));
	return rand() % 9 + 2;
}

//设置头五十块内容，文件大小随机产生，大小2-10，  
//为模拟真实效果，这五十块随机放置，并不按照顺序排放，  
//虽然有些内存一开始初始化的时候是顺序排放  
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

//专门为前50块做的返回名字，因为这个比较麻烦  
string Solution::getName(int i)
{
	char str[5];
	_itoa_s(i, str, 10);
	return str;
}

//设置索引块  
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

//设置文件分配表  
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


//设置新加入的五个文件  
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

//查找后面加入的五个文件  
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
	cout << "文件名:" << node->name << " 文件索引块位置:" << location << "  " << "文件块分布位置:";

	int length = indexedBlockNodeTable[location].size;
	for (int i = 0; i < length; i++)
	{
		cout << indexedBlockNodeTable[location].location[i] << " ";
	}
	cout << endl;
}

//判断是否是后面加入的五个文件之一  
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


	cout << "=====500个文件块使用情况,0表示未使用，1表示已经使用=====" << endl;
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
	cout << "===新增加的五个文件分布情况===" << endl;
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