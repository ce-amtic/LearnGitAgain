#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fuzzer.h"    //实现了Init、Execute、Save函数

#define N 10000005 //Hash 表数组大小
#define base 13131 //Hash 基数

LinkQueue queue;  //测试用例队列的队头元素
LinkNode *target;  //每次选取的目标测试用例
LinkNode *newone;  //每次变异后的测试用例

LinkNode * Select(LinkQueue *);  //待实现的筛选函数
LinkNode * Mutate(LinkNode *,int Rd);  //待实现的变异函数
int Observe(void);  		//待实现的观测函数

int tot;

// 预先处理跑出来的 crash = 5
unsigned char crash0[] = { // vul1
	0x42, 0x4d, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00,  0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x03, 0x00, 
	0xff, 0xff, 0x04, 0x00, 0x00, 0x00, 0x4f, 0x00,  0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,  0xff, 0x00, 0xff, 0xff, 0xff, 0x00};
unsigned char crash1[] = {
	0x42, 0x4d, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x36, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00,  0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x4f, 0x00,  0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,  0xff, 0x00, 0xff, 0xff, 0xff, 0x00};
unsigned char crash2[] = {
	0x42, 0x4d, 0x3a, 0x00, 0x00, 0x03, 0x00, 0xff,  0xff, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00,  0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,  0xff, 0x00, 0xff, 0xff, 0xff, 0x00};
unsigned char crash3[] = { // vulner3
	0x42, 0x4d, 0x3a, 0xff, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x35, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00,  0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x03, 0x00, 
	0x00, 0x00, 0x04, 0x00, 0xff, 0x00, 0x4f, 0x00,  0x00, 0x00, 0x4f, 0x00, 0xff, 0x01, 0x00, 0x00, 
	0x00, 0x01, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff,  0xff, 0x00, 0xfe, 0xff, 0xff, 0x00};
unsigned char crash4[] = { 
	0x42, 0x4d, 0x3a, 0x00, 0x01, 0x00, 0x00, 0x00,  0xfe, 0x01, 0x36, 0x00, 0x00, 0x00, 0x38, 0x00, 
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00,  0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x03, 0x00, 
	0x00, 0x00, 0x04, 0x01, 0x00, 0x01, 0x4e, 0x00,  0xff, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,  0xff, 0x00, 0xff, 0xff, 0xff, 0x00};

int main()  
{
	int result;
	Init(&queue);  	//初始化测试用例队列
 	srand(998244353);  //设定随机化种子

	while (1)    //随机化爬山算法
	{
		target = Select(&queue);	//筛选目标测试用例
		for (int i = 1; i <= 10; i++)
		{
			newone = Mutate(target, 0);	//变异目标测试用例
			Execute(newone);		//执行变异后的测试用例
			result = Observe();                 //观测执行结果
			if (result&1 == 1 || result&2 == 2)	//新覆盖或新状态
				Save(&queue, newone);	//加入测试用例队列
		}
		if (queue.front == queue.rear)    // 如果队为空则随机构造一个新用例
			Save(&queue, Mutate(target, 1));
  }
}

LinkNode * Select(LinkQueue *queue)
{ 
	tot = tot + 1; //统计模糊测试次数
	LinkNode *target = queue->front->next; //选择队首
	queue->front = queue->front->next; //队首出队
	queue->cnt = queue->cnt - 1;
	return target;  
}

LinkNode * Mutate(LinkNode *target, int Rd) 
{
	FILE *fin, *fout;
	unsigned int size, index;
	unsigned char *buffer; 
	LinkNode *newone;

	fin = fopen(target->fname, "rb");
	fseek(fin, 0, SEEK_END);  //读写头移到文件末尾
	size = ftell(fin);  //获得文件大小
	buffer = (unsigned char*)(malloc(size));  //动态分配内存空间
	rewind(fin);  //读写头移到文件开头
	fread(buffer, 1, size, fin);   //文件内容读入buffer
	fclose(fin);
	
	int op = rand() % (tot <= 100 ? 3 : 5); // 令前 100 次模糊操作主要解决 3 个显式漏洞
	switch (op)
	{
		case 0:
			index = rand() % (size - 3); //随机选择 4 个字节,改变成 0xffff0003
			buffer[index] = 3;
			buffer[index + 1] = 0;
			buffer[index + 2] = 0xff;
			buffer[index + 3] = 0xff;
			break;

		case 1:
			index = rand() % (size - 3); //随机选择 4 个字节,改变成 0x00000001
			buffer[index] = 1;
			buffer[index + 1] = 0;
			buffer[index + 2] = 0;
			buffer[index + 3] = 0;
			break;

		case 2:
			for(int i = 0; i < size; i++) //随机微调
				if (rand() % 100 < 10)
 	   				buffer[i] = rand() & 1 ? (buffer[i] + 1) % 256 : (buffer[i] + 255) % 256;
			buffer[28] = 0x20, buffer[29] = 0; //钦定 biBitCount = 3, biCompression = 32
			buffer[30] = 3; buffer[31] = buffer[32] = buffer[33] = 0;
			break;

		case 3: 
			for(int i = 54; i < 58; i++) buffer[i] = rand() % 256;  //修改最后两个颜色
			for(int i = 59; i < 62; i++) buffer[i] = rand() % 256;
			break;

		default:
			for(int i = 0; i < size; i++) //随机微调
				if (rand() % 100 < 10)
 	   				buffer[i] = rand() & 1 ? (buffer[i] + 1) % 256 : (buffer[i] + 255) % 256;
			break;
	}

	if (Rd) //重新构造随机数
		for (int i = 0; i < size; i++)
			buffer[i] = rand() % 256;

	if(rand() % 100 >= 5) //有 95% 的概率钦定为格式规范的 bmp 文件
		buffer[0] = 0x42, buffer[1] = 0x4d, buffer[2] = 0x3a;
	if(rand() % 100 >= 30) //有 70% 的概率钦定 bmp 大小为 1x2 或 2x1
	{
		buffer[18] = 1 + (rand() & 1);
		buffer[22] = 3 - buffer[18];
		buffer[19] = buffer[20] = buffer[21] = buffer[23] = buffer[24] = buffer[25] = 0;
	}
	if(rand() % 100 >= 50) //有 50% 的概率钦定 bfOffBits = 54
	{
		buffer[10] = 0x36;
		buffer[11] = buffer[12] = buffer[13] = 0;
	}
	if(rand() % 100 >= 30) //有 70% 的概率钦定 biSize 合法
	{
		static unsigned int biSize[] = {12u, 40u, 52u, 56u, 108u, 124u};
		buffer[14] = biSize[rand() % 6];
		buffer[15] = buffer[16] = buffer[17] = 0;
	}
	if(rand() % 100 >= 30) //有 70% 的概率钦定 biCompression, biBitCount 尽可能合法
	{
		buffer[28] = 4 * (rand() % 8 + 1) , buffer[29] = 0;
		buffer[30] = rand() % 4; buffer[31] = buffer[32] = buffer[33] = 0;
	}
	if(rand() % 100 >= 98) //有 2% 的概率将两个颜色变为 #000000
		for(int i = 54; i < 62; i++) 
			buffer[i] = 0;

	newone = (LinkNode *)(malloc(sizeof(LinkNode))); //将 newone 写入文件
	sprintf(newone->fname, "newone");

	fout = fopen(newone->fname, "wb+");
	fwrite(buffer, 1, size, fout);
	fclose(fout);
	free(buffer);

	return newone;
}

struct hash_map{ //前向星式 Hash 表
	int h[1 << 20], nxt[N], to[N], cnt;
	ull fr[N];
}mp1, mp2, bugs;

void Hash_map_add(struct hash_map *mp, ull x, int y) 
{
	mp->nxt[++mp->cnt] = mp->h[x & 1048575], mp->fr[mp->cnt] = x, 
	mp->to[mp->cnt] = y, mp->h[x & 1048575] = mp->cnt;
}
int Hash_map_ask(struct hash_map *mp, ull x) 
{
	for (int i = mp->h[x & 1048575]; i; i = mp->nxt[i])
	if (mp->fr[i] == x) return mp->to[i];
	return 0;
}

int Observe(void)
{
	FILE *fcoverage = fopen("coverage", "rb");
	FILE *fstate = fopen("state", "rb");
	int result = 0;
	
	char ch;
	ull Hash_num1 = 0, Hash_num2 = 0;

	while (fread(&ch, 1, 1, fcoverage) == 1)
		Hash_num1 = Hash_num1 * base + ch;
	
	if (Hash_map_ask(&mp1, Hash_num1) == 0) result |= 1, Hash_map_add(&mp1, Hash_num1, 1);
	// 对 coverage 进行 Hash 判重

	unsigned short BBidx;
	unsigned char op;
	ull lVal, rVal;
	int Find_iidx = 0;

	while(fread((void *)&BBidx, sizeof(BBidx), 1, fstate) == 1)
	{
  		fread((void *)&op, sizeof(op), 1, fstate);
   		fread((void *)&lVal, sizeof(lVal), 1, fstate);
    	fread((void *)&rVal, sizeof(rVal), 1, fstate);
		ull cur = (ull)BBidx * 396421313 + (ull)op * 978341141 + lVal * 426123113 + rVal;
		Hash_num2 = Hash_num2 * base + cur;
		if (BBidx == 0xd664 || BBidx == 0xdc17) // 对两个隐式漏洞进行捕捉
			if (Hash_map_ask(&bugs, cur) == 0) result |= 2, Hash_map_add(&bugs, cur, 1);
	}

	if (Hash_map_ask(&mp2, Hash_num2) == 0) result |= 2, Hash_map_add(&mp2, Hash_num2, 1);
	// 对 state 进行 Hash 判重

	fclose(fcoverage);
	fclose(fstate);
	return result;
}
