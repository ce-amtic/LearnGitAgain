/*
关键位调整 dev01

Based on KeyBit static analysis

disable reference system                 [o]
enable slight-adjustment method          [o]
disable SuccessRate system               [o]
improve Observe() efficiency             [o]
improve Select() efficiency              [o]
improve Mode 0/1 efficiency              [o]
enable visited marking system            [o]
confining size of queue                  [o]
efficiency optimize                      [dev]
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "fuzzer.h"    //实现了Init、Execute、Save函数

LinkQueue queue;  //测试用例队列的队头元素
LinkNode *target;  //每次选取的目标测试用例
LinkNode *newone;  //每次变异后的测试用例

LinkNode * Select(LinkQueue *);  //待实现的筛选函数
LinkNode * Mutate(LinkNode *);  //待实现的变异函数
int Observe(void);  		//待实现的观测函数 

/*声明测试用例信息自定义类*/
typedef struct NodeInfo{
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
  system("cp coverage queue/coverage-0");
  system("cp state queue/state-0");
  QI_Save(info, root);
}

int CaseCount = 0, CurMode = 0;
// CurMode = 0 : 0x00000001 模式
// CurMode = 1 : 0xffff0000 模式
// CurMode = 2 : 扩展 coverage
// CurMode = 3 : crash45 模式

int main(){
  srand(19260817);

  int result = sqrt(1);
  Init(&queue);  	//初始化测试用例队列
  QI_Init(&info);

  while (1) {
    CaseCount++;
    if(CurMode < 2 && CaseCount >= 59) CurMode++, CaseCount = 0;
	  if(CurMode == 2 && CaseCount >= 2048) CurMode++, CaseCount = 0;
    // if(CurMode == 3 && CaseCount >= 4096) CurMode--, CaseCount = 0;

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
  int pos = rand() % queue->cnt;
  for(int i = 0; i < pos; i++){
    target = target->next;
    tarinfo = tarinfo->next;
  }
  return target;
}

/* Mutate */
void Adjust(unsigned char *buffer, int index){ // 有指向性的随机数
  assert(index < 62);
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

  assert(size == 62);
  
  newinfo = (NodeInfo *)(malloc(sizeof(NodeInfo)));

  if(CurMode == 0){
    index = CaseCount;
    newinfo->pos = index;
    buffer[index] = 1;
    buffer[index + 1] = 0;
    buffer[index + 2] = 0;
    buffer[index + 3] = 0;
  }
  else if(CurMode == 1){
    index = CaseCount;
    newinfo->pos = index;
    buffer[index] = 0;
    buffer[index + 1] = 0;
    buffer[index + 2] = 0xff;
    buffer[index + 3] = 0xff;    
  }
  else if(CurMode == 2){
    // 对指定字段进行有指向的微调
    if(tarinfo->pos < 0 || !(rand() % 2)) index = rand() % size;
    else index = tarinfo->pos;
    newinfo->pos = index;
    Adjust(buffer, index);
  }
  else{
    newinfo->pos = -1;
	  for(int i = 0; i < 11; i++){
      buffer[KeyBit[i]] = rand() % 2;
	  }
	  for(int i = 0; i < 5; i++){
		  if(!(rand() % 10)) buffer[VitalBit[i]] = 0;
		  if(rand() % 2) Adjust(buffer, VitalBit[i]);
	  }
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
static int vis[65536];
static unsigned char newbuf[65536], tarbuf[65536];
int Observe(void){
  FILE *fcoverage = fopen("coverage", "rb");
  FILE *fstate = fopen("state", "rb");
  FILE *tarcov = fopen("queue/coverage-0", "rb");
  FILE *tarsta = fopen("queue/state-0", "rb");
  int result = 0;
  
  fread(newbuf, 1, 65536, fcoverage);
  fread(tarbuf, 1, 65536, tarcov);
  for(int i = 0; i < 65536; i++){
    if(newbuf[i] > tarbuf[i]) result |= 1;
    tarbuf[i] |= newbuf[i];
  }
  fclose(tarcov);
  tarcov = fopen("queue/coverage-0", "wb");
  fwrite(tarbuf, 1, 65536, tarcov);
  fclose(tarcov);

  unsigned short id;
  unsigned char opt;
  unsigned long long lv, rv;
  memset(vis, 0, sizeof(vis));
  while(fread(&id, 2, 1, tarsta)){
    assert(id < 65536);
    fread(&opt, 1, 1, tarsta);
    fread(&lv, 8, 1, tarsta);
    fread(&rv, 8, 1, tarsta);
    vis[id] = 1;
  }
  fclose(tarsta);
  tarsta = fopen("queue/state-0", "ab");
  while(fread(&id, 2, 1, fstate)){
    assert(id < 65536);
    fread(&opt, 1, 1, fstate);
    fread(&lv, 8, 1, fstate);
    fread(&rv, 8, 1, fstate);
    if(!vis[id]){
      result |= 2;
      fwrite(&id, 2, 1, tarsta);
      fwrite(&opt, 1, 1, tarsta);
      fwrite(&lv, 8, 1, tarsta);
      fwrite(&rv, 8, 1, tarsta);
    }
  }  
  fclose(tarsta);

  fclose(fcoverage);
  fclose(fstate);
  return result;
}