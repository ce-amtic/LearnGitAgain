/*
定点更改 v1.5 dev04 效率不高

限制变异次数

disable reference system          [o]
enable slight-adjustment method   [o]
disable SuccessRate system        [o]
improve Observe() efficiency      [o]
improve Select() efficiency       [o]
improve Mode 0/1 efficiency       [o]
enable CrashBlockAimed system     [dev]
  1. visited marking system                          [passed]
  2. KeyBit SlightAjustment system                   [disappointed]
  3. confining size of queue                         [working]
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fuzzer.h"    //实现了Init、Execute、Save函数

int FuckUCrash45[62];
int FuckUCrash123[62];
int FuckUPos;

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
  int pos, mark, used;
  struct NodeInfo *next;
} NodeInfo;
typedef struct{
  NodeInfo *front, *rear;
} QueueInfo;

QueueInfo info; // 名为 info 的队列
NodeInfo *tarinfo;
NodeInfo *newinfo;

void QI_Save(QueueInfo *info, NodeInfo *newinfo){
  sprintf(newinfo->fcover, "queue/coverage-%d", queue.cnt - 1);
  sprintf(newinfo->fstate, "queue/state-%d", queue.cnt - 1);

  char cmd[256];
  sprintf(cmd, "cp coverage %s", newinfo->fcover);
  system(cmd);
  sprintf(cmd, "cp state %s", newinfo->fstate);
  system(cmd);
  
  newinfo->next = info->rear->next;
  info->rear->next = newinfo;
  info->rear = newinfo;
}
void QI_Init(QueueInfo *info){
  info->front = info->rear = (NodeInfo *)(malloc(sizeof(NodeInfo)));

  NodeInfo *root; 
  root = (NodeInfo *)(malloc(sizeof(NodeInfo)));
  root->pos = -1, root->mark = 0, root->next = NULL;

  Execute(queue.front->next);
  QI_Save(info, root);
}

int CaseCount = 0, CurMode = 0;
// CurMode = 0 : 0x00000001 模式
// CurMode = 1 : 0xffff0000 模式
// CurMode = 2 : 随机字节随机值模式

int main(){
  srand(19260817);

  int result = sqrt(1);
  Init(&queue);  	//初始化测试用例队列
  QI_Init(&info);

  while (1) {
    CaseCount++;
    if(CurMode < 2 && CaseCount >= 59) CurMode++, CaseCount = 0;

    target = Select(&queue);	//筛选目标测试用例
    newone = Mutate(target);	//变异目标测试用例
    Execute(newone);		//执行变异后的测试用例
    FuckUPos = newinfo->pos;
    result = Observe();                 //观测执行结果
    // printf("res : %d\n", result);
    if (result){	//新覆盖或新状态
      Save(&queue, newone);	//加入测试用例队列
      QI_Save(&info, newinfo);
    }
  }
}

int id[1 << 21];
LinkNode * Select(LinkQueue *queue){
  LinkNode *target = queue->front->next;
  int n = 0, index = 0, tot = 0;
  while(target) n++, target = target->next;
  target = queue->front->next;
  tarinfo = info.front->next;

  if(CurMode < 2) return target;
  printf("%d\n", n);

  for(int i = 0; i < n; i++){
    if(tarinfo->mark) id[tot++] = i;
    tarinfo = tarinfo->next;
  }
  tarinfo = info.front->next;

  if(tot && rand() % 2) index = id[rand() % tot];
  else index = rand() % n;
  for(int i = 0; i < index; i++){
    target = target->next;
    tarinfo = tarinfo->next;
  }

  tarinfo->used++;
  if(index && tarinfo->used >= 4){
    LinkNode *prv = queue->front->next;
    for(int i = 0; i < index - 1; i++) prv = prv->next;
    prv->next = target->next, target->next = NULL;
    NodeInfo *prvinfo = info.front->next;
    for(int i = 0; i < index - 1; i++) prvinfo = prvinfo->next;
    prvinfo->next = tarinfo->next, tarinfo->next = NULL;
  }

  return target;
}

/* 
Crash4/Crash5 KeyBit 
0xd664 : 38(lv) / 43(rv)
0xdc17 : 19(lv) / 56(lv)
*/
int KeyBit[29] = {0, 1, 10, 11, 12,  13, 14, 15, 16, 17,
                  18, 19, 20, 21, 22,  23, 24, 25, 28, 29,
                  30, 32, 35, 36, 37,  38, 40, 41, 43};
void Adjust(unsigned char *buffer, int index, int stable){
  if(stable){
    switch(rand() % 3){
      case 0:
        buffer[index] = (buffer[index] + 1) % 256;
        break;
      case 1:
        buffer[index] = (buffer[index] + 16) % 256;
        break;
      case 2:
        break;
    }
    return;
  }
  switch(rand() % 2){
    case 0:
      buffer[index] = (buffer[index] + 1) % 256;
      break;
    case 1:
      buffer[index] = (buffer[index] + 16) % 256;
      break;
  }
}
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
  newinfo->mark = 0;

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
  else{
    if(tarinfo->mark){ // 对关键字段进行微调
      for(int i = 0; i < 29; i++) Adjust(buffer, KeyBit[i], 1);
    }
    // 对指定字段进行等概率的微调或随机重构
    if(tarinfo->pos < 0 || !(rand() % 2)) index = rand()%size;
    else index = tarinfo->pos;
    newinfo->pos = index;
    Adjust(buffer, index, 0);
  }

  newone = (LinkNode *)(malloc(sizeof(LinkNode)));
  sprintf(newone->fname, "newone");

  fout = fopen(newone->fname, "wb+");
  fwrite(buffer, 1, size, fout);
  fclose(fout);
  free(buffer);

  return newone;
}

int Observe(void){
  FILE *fcoverage = fopen("coverage", "r+");
  FILE *fstate = fopen("state", "r+");
  FILE *tarcov = fopen(tarinfo->fcover, "r+");
  FILE *tarsta = fopen(tarinfo->fstate, "r+");
  int result = 0, vis[65536] = {0};
  unsigned char newbuf[65536 * 19 + 1], tarbuf[65536 * 19 + 1]; 
  unsigned int size1, size2;
  unsigned long long LV[65536] = {0}, RV[65536] = {0};
  
  fread(newbuf, 1, 65536, fcoverage);
  fread(tarbuf, 1, 65536, tarcov);
  for(int i = 0; i < 65536; i++){
    if(!tarbuf[i] && newbuf[i]) result |= 1;
    tarbuf[i] |= newbuf[i];
  }
  // rewind(tarcov);
  // fwrite(tarbuf, 1, 65536, tarcov);

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
    for(int j = 0; j < 8; j++) LV[id] |= tarbuf[i + 3 + j] << (j * 8);
    for(int j = 0; j < 8; j++) RV[id] |= tarbuf[i + 11 + j] << (j * 8);
  }
  for(int i = 0; i < size1; i += 19){
    int id = newbuf[i] | (newbuf[i + 1] << 8), flag = 0; 
    unsigned long long lv = 0, rv = 0;
    for(int j = 0; j < 8; j++) lv |= newbuf[i + 3 + j] << (j * 8);
    for(int j = 0; j < 8; j++) rv |= newbuf[i + 11 + j] << (j * 8);
    if(id == 0xd664
    && (!vis[id] || lv == rv)){
      // printf("0xd664 %d %d %llu %llu\n", tarinfo->mark, newinfo->pos, lv, rv);
      newinfo->mark |= 1;
      FuckUCrash45[newinfo->pos]++;
    }
    if(id == 0xdc17
    && (!vis[id] || lv == rv)){
      // printf("0xdc17 %d %d %llu %llu\n", tarinfo->mark, newinfo->pos, lv, rv);
      newinfo->mark |= 2;
      FuckUCrash45[newinfo->pos]++;
    }
    if(!vis[id]){
      result |= 2;
      // fwrite(newbuf + i, 1, 19, tarsta);
    }
  }

  //if(!newinfo->mark && tarinfo->mark){
  //  result = 0;
  //}

  fclose(tarcov);
  fclose(tarsta);
  fclose(fcoverage);
  fclose(fstate);
  return result;
}