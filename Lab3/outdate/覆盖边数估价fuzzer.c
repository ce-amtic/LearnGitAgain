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
  int edge_cnt;
  int cover_cnt;
  int bonus;
  struct NodeInfo *fa;
  struct NodeInfo *next;
} NodeInfo;
typedef struct{
  NodeInfo *front, *rear;
} QueueInfo;

QueueInfo info; // 名为 info 的队列
NodeInfo *tarinfo;
NodeInfo *newinfo;

void QI_Save(QueueInfo *info, NodeInfo *newinfo)
{
  newinfo->next = info->rear->next;
  info->rear->next = newinfo;
  info->rear = newinfo;
}
void QI_Init(QueueInfo *info)
{
  NodeInfo *root;
  
  info->front = info->rear = (NodeInfo *)(malloc(sizeof(NodeInfo)));
  
  root = (NodeInfo *)(malloc(sizeof(NodeInfo)));
  root->cover_cnt = 0;
  root->edge_cnt = 0;
  root->bonus = 0;
  root->fa = NULL;
  root->next = NULL;
  
  QI_Save(info, root);
}

int CaseCount = 0, CurMode = 0;
// CurMode = 0 : 0x01000000 模式
// CurMode = 1 : 0xffff0000 模式
// CurMode = 2 : 随机字节随机值模式

/*参数列表*/
#define DE_RATE 0.96114514
#define OUTDATING 0.93114514
#define EDGE_RATIO 1.14514
#define COVER_RATIO 0.114514

int main()  
{
  srand(1919810);

  int result = sqrt(1);
  Init(&queue);  	//初始化测试用例队列
  QI_Init(&info);

  while (1) {
    if(CurMode < 2) CaseCount++;
    if(CaseCount > 886) CurMode++, CaseCount = 0;
    target = Select(&queue);	//筛选目标测试用例
    newone = Mutate(target);	//变异目标测试用例
    Execute(newone);		//执行变异后的测试用例
    result = Observe();                 //观测执行结果
    if (result&1 == 1 || result&2 == 2)	//新覆盖或新状态
      Save(&queue, newone);	//加入测试用例队列
  }
}

/* 随机选取target并定位tarinfo */
int value[10000001];
LinkNode * Select(LinkQueue *queue)
{
  int n = queue->cnt, pos, tot;
  NodeInfo *cur = info.front->next;
  for(int i = 0; i < n; i++){
    value[i] = cur->edge_cnt + cur->cover_cnt + cur->bonus;
    cur = cur->next;
  }

  pos = rand()%n, tot = rand()%n;
  for(int i = 0; i < tot; i++){
    int newpos = rand()%n;
    if(value[newpos] > value[pos]) pos = newpos;
  }

  LinkNode *target = queue->front->next;
  tarinfo = info.front->next;
  for(int i = 0; i < pos; i++){
    target = target->next;
    tarinfo = tarinfo->next;
  }
  tarinfo->bonus = tarinfo->bonus * OUTDATING;

  return target;
}

/* 随机选择四个字节变成0x01000000 */
LinkNode * Mutate(LinkNode *target) 
{
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
  
  if(!CurMode){
    index = rand()%(size-3);
    buffer[index] = 1;
    buffer[index+1] = 0;
    buffer[index+2] = 0;
    buffer[index+3] = 0;
  }
  else if(CurMode == 1){
    index = rand()%(size-3);
    buffer[index] = 0xff;
    buffer[index+1] = 0xff;
    buffer[index+2] = 0;
    buffer[index+3] = 0;    
  }
  else{
    tot = rand()%(size / 10);
    if(!tot) tot = 1;
    while(tot--){     
      index = rand()%size;
      buffer[index] = rand()%256;
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

/* 获取newinfo并于tarinfo进行比较 **/
int Observe(void)
{
  FILE *fcoverage = fopen("coverage", "rb");
  FILE *fstate = fopen("state", "rb");
  int result = 0;
  
  unsigned char *buffer; unsigned int size;
  fseek(fcoverage, 0, SEEK_END);
  size = ftell(fcoverage);
  buffer = (unsigned char *)(malloc(size));
  rewind(fcoverage);
  fread(buffer, 1, 65536, fcoverage);

  newinfo = (NodeInfo *)(malloc(sizeof(NodeInfo)));
  newinfo->cover_cnt = newinfo->edge_cnt = newinfo->bonus = 0;
  newinfo->next = newinfo->fa = NULL;
  for(int i = 0; i < 65536; i++){
    if(buffer[i] > 0){
      newinfo->cover_cnt += (int)buffer[i];
      newinfo->edge_cnt++;
    }
  }

  int delta = (newinfo->edge_cnt - tarinfo->edge_cnt) * EDGE_RATIO
             +(newinfo->cover_cnt - tarinfo->cover_cnt) * COVER_RATIO;
  if(delta > 0){
    result = 1;
    newinfo->fa = tarinfo;
    QI_Save(&info, newinfo);
    while(tarinfo){
      tarinfo->bonus += delta;
      delta = delta * DE_RATE;
      tarinfo = tarinfo->fa;
    }
  }
  
  fclose(fcoverage);
  fclose(fstate);
  return result;
}