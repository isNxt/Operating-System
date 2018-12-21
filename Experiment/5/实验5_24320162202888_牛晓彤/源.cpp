#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstring>
 
using namespace std;
 
void initializeFiles();
void deleteOddFiles();
void new5Files();
char nth_letter(int n);
void printFreeBlocks();
typedef struct Fat//file allocation table 文件分配表
{
	string name;
	int indexBlock;//索引块号
}Fat;
 
//索引块
class IndexBlock {
public:
	//指向另外5个块存放内容
	int contents[5];
	IndexBlock() {
		for (int i = 0; i < 5; i++) {
			contents[i] = 0;
		}
	}
	IndexBlock(int* arr) {
		for (int i = 0; i < 5; i++) {
			contents[i] = *(arr + i);
		}
	}
};
 
bool bitTable[500];//位表初始为空 false 代表没有占位
vector<Fat> fatTable;//初始化文件分配表
IndexBlock* memory = new IndexBlock[500];
 
int main() {
	//初始化50个文件
	initializeFiles();
	deleteOddFiles();
	new5Files();
	printFreeBlocks();
 
	return 0;
}
 
//生成50个文件，并存放
void initializeFiles() {
	srand((unsigned)time(NULL));
	for (int i = 0; i < 50; i++) {
		Fat temp;
		stringstream ss;
		ss << (i+1);
		string str=ss.str();
		temp.name = str+".txt";
		for (int j = 0; j < 500; j++) {
			//如果块为空，放入索引块
			if (!bitTable[j]) {
				temp.indexBlock = j;
				bitTable[j] = true;
 
				//随机产生1-5个块的内容，并放入
				int blocks = (rand() % 5) + 1;
				for (int k = 1; k <= blocks; k++) {
					memory[temp.indexBlock].contents[k - 1] = k+temp.indexBlock;
					bitTable[k + temp.indexBlock] = true;
				}
				break;
			}
		}
		//插入新的文件表项
		fatTable.push_back(temp);
	}
}
 
//删除奇数文件
void deleteOddFiles() {
	vector<Fat> ::iterator it = fatTable.begin();
	for (int i = 0; i < 50; i += 2,it++) {
		bitTable[it->indexBlock] = false;
		for (int j = 0; j < 5; j++) {
			int t = memory[it->indexBlock].contents[j];
			if (t != 0) {
				bitTable[t] = false;
				memory[it->indexBlock].contents[j] = 0;
			}
			else
				break;
		}
		/*vector<Fat>::iterator temp = it;
		it ++;*/
		it=fatTable.erase(it);
	}
}
 
//生成5个新的文件并放入
void new5Files() {
	int size[5] = { 4,3,1,5,2 };
	for (int i = 0; i < 5; i++) {
		Fat temp;
		char alpha = nth_letter(i);
		stringstream ss;
		ss << alpha;
		string str = ss.str();
		temp.name = str + ".txt";
		for (int j = 0; j < 500; j++) {
			if (!bitTable[j]) {
				temp.indexBlock = j;
				bitTable[j] = true;
 
				for (int k = 1; k <= size[i]; k++) {
					for (int l = j + 1; l < 500; l++) {
						if (!bitTable[l]) {
							bitTable[l] = true;
							memory[temp.indexBlock].contents[k - 1] = l;
							break;
						}
					}
					
				}
				break;
			}
		}
		fatTable.push_back(temp);
	}
}
 
//int转char
char nth_letter(int n)
{
		return "ABCDEFGHIJLMNOPQRSTUVWXYZ"[n];
}
 
//显示所有的文件表项和空闲区块状态。
void printFreeBlocks() {
	vector<Fat> ::iterator it = fatTable.begin();
	for (; it != fatTable.end(); it++) {
		cout << it->name<<endl;
		cout << "索引块内容：";
		for (int i = 0; i < 5; i++) {
			cout << memory[it->indexBlock].contents[i] << ",";
		}
		cout << endl;
	}
	cout << "位表：";
	for (int i = 0; i < 500; i++) {
		cout << bitTable[i];
	}
}
