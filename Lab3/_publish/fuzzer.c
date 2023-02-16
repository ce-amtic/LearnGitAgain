#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fuzzer.h"    //实现了Init、Execute、Save函数

LinkQueue queue;  //测试用例队列的队头元素
LinkNode *target;  //每次选取的目标测试用例
LinkNode *newone;  //每次变异后的测试用例

LinkNode * Select(LinkQueue *);  //待实现的筛选函数
LinkNode * Mutate(LinkNode *);  //待实现的变异函数
int Observe(void);  		//待实现的观测函数 

/*声明测试用例信息自定义类*/
typedef struct NodeInfo{
  char fstate[256];
  char fcover[256];
  int pos;
  struct NodeInfo *next;
} NodeInfo;
typedef struct{
  NodeInfo *front, *rear;
} QueueInfo;

QueueInfo info; // 名为 info 的队列
NodeInfo *tarinfo;
NodeInfo *newinfo;

void QI_Save(QueueInfo *info, NodeInfo *newinfo){
  sprintf(newinfo->fcover, "queue/coverage-0");
  sprintf(newinfo->fstate, "queue/state-0");

  if(queue.cnt == 1){
	char cmd[256];
    sprintf(cmd, "cp coverage %s", newinfo->fcover);
    system(cmd);
    sprintf(cmd, "cp state %s", newinfo->fstate);
    system(cmd);
  }
  
  newinfo->next = info->rear->next;
  info->rear->next = newinfo;
  info->rear = newinfo;
}
void QI_Init(QueueInfo *info){
  info->front = info->rear = (NodeInfo *)(malloc(sizeof(NodeInfo)));

  NodeInfo *root; 
  root = (NodeInfo *)(malloc(sizeof(NodeInfo)));
  root->pos = -1, root->next = NULL;

  Execute(queue.front->next);
  QI_Save(info, root);
}

int CaseCount = 0, CurMode = 0;
// CurMode = 0 : 0x00000001 模式
// CurMode = 1 : 0xffff0000 模式
// CurMode = 2 : cover 模式
// CurMode = 3 : crash45 模式

int main(){
  srand(19260817);

  int result = sqrt(1);
  Init(&queue);  	//初始化测试用例队列
  QI_Init(&info);

  while (1) {
    if(CurMode < 3) CaseCount++;
    if(CurMode < 2 && CaseCount >= 56) CurMode++, CaseCount = 0;
	  if(CurMode == 2 && CaseCount >= 2048) CurMode++, CaseCount = 0;

    target = Select(&queue);	//筛选目标测试用例
    newone = Mutate(target);	//变异目标测试用例
    Execute(newone);		//执行变异后的测试用例
	
    if(CurMode < 2) continue;
    result = Observe();                 //观测执行结果
    if (result){	//新覆盖或新状态
      Save(&queue, newone);	//加入测试用例队列
      QI_Save(&info, newinfo);
    }
  }
}

/* Select */
LinkNode * Select(LinkQueue *queue){
  LinkNode *target = queue->front->next;
  tarinfo = info.front->next;
  int n = queue->cnt, pos = 0;
  pos = rand() % n;
  for(int i = 0; i < pos; i++){
    target = target->next;
    tarinfo = tarinfo->next;
  }
  return target;
}

/* Mutate */
void Adjust(unsigned char *buffer, int index){ // 有指向性的随机数
  switch(rand() % 2){
    case 0:
      buffer[index] = (buffer[index] + 1) % 256;
      break;
    case 1:
      buffer[index] = (buffer[index] + 16) % 256;
      break;
  }
}
int KeyBit[11] = {3, 4, 5, 7, 9, 10, 26, 38, 49, 56, 61};
int VitalBit[5] = {14, 18, 22, 28, 30};
LinkNode * Mutate(LinkNode *target){
  FILE *fin, *fout;
  unsigned int size, index, tot;
  unsigned char *buffer; 
  LinkNode *newone;

  fin = fopen(target->fname, "rb");
  fseek(fin, 0, SEEK_END);  //读写头移到文件末尾
  size = ftell(fin);  //获得文件大小
  buffer = (unsigned char*)(malloc(size));  //动态分配内存空间
  rewind(fin);  //读写头移到文件开头
  fread(buffer, 1, size, fin);   //文件内容读入buffer
  fclose(fin);
  
  newinfo = (NodeInfo *)(malloc(sizeof(NodeInfo)));

  if(CurMode == 0){
    index = CaseCount + 3;
    newinfo->pos = index;
    buffer[index] = 1;
    buffer[index + 1] = 0;
    buffer[index + 2] = 0;
    buffer[index + 3] = 0;
  }
  else if(CurMode == 1){
    index = CaseCount + 3;
    newinfo->pos = index;
    buffer[index] = 0;
    buffer[index + 1] = 0;
    buffer[index + 2] = 0xff;
    buffer[index + 3] = 0xff;    
  }
  else if(CurMode == 3){
	for(int i = 0; i < 11; i++){
		buffer[KeyBit[i]] = rand() % 2;
	}
	for(int i = 0; i < 5; i++){
		if(!(rand() % 10)) buffer[VitalBit[i]] = 0;
		if(rand() % 2) Adjust(buffer, VitalBit[i]);
	}
  }
  else{
    // 对指定字段进行等概率的微调
    if(tarinfo->pos < 0 || !(rand() % 2)) index = rand() % size;
    else index = tarinfo->pos;
    newinfo->pos = index;
    Adjust(buffer, index);
  }

  newone = (LinkNode *)(malloc(sizeof(LinkNode)));
  sprintf(newone->fname, "newone");

  fout = fopen(newone->fname, "wb+");
  fwrite(buffer, 1, size, fout);
  fclose(fout);
  free(buffer);

  return newone;
}

/* Observe */
int Observe(void){
  FILE *fcoverage = fopen("coverage", "r+");
  FILE *fstate = fopen("state", "r+");
  FILE *tarcov = fopen(tarinfo->fcover, "r+");
  FILE *tarsta = fopen(tarinfo->fstate, "r+");
  int result = 0, vis[65536] = {0};
  unsigned char newbuf[65536 * 19 + 1], tarbuf[65536 * 19 + 1]; 
  unsigned int size1, size2;
  
  fread(newbuf, 1, 65536, fcoverage);
  fread(tarbuf, 1, 65536, tarcov);
  for(int i = 0; i < 65536; i++){
    if(newbuf[i] > tarbuf[i]) result |= 1;
    tarbuf[i] |= newbuf[i];
  }
  rewind(tarcov);
  fwrite(tarbuf, 1, 65536, tarcov);

  fseek(fstate, 0, SEEK_END);
  size1 = ftell(fstate);
  rewind(fstate);
  fread(newbuf, 1, size1, fstate);
  fseek(tarsta, 0, SEEK_END);
  size2 = ftell(tarsta);
  rewind(tarsta);
  fread(tarbuf, 1, size2, tarsta);
  for(int i = 0; i < size2; i += 19){
    int id = tarbuf[i] | (tarbuf[i + 1] << 8);
    vis[id] = 1;
  }
  for(int i = 0; i < size1; i += 19){
    int id = newbuf[i] | (newbuf[i + 1] << 8);
    if(!vis[id]){
      result |= 2;
      fwrite(newbuf + i, 1, 19, tarsta);
    }
  }

  fclose(tarcov);
  fclose(tarsta);
  fclose(fcoverage);
  fclose(fstate);
  return result;
}