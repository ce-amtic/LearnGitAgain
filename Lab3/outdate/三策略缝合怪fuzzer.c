/*三策略缝合怪*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fuzzer.h"    //实现了Init、Execute、Save函数

LinkQueue queue;  //测试用例队列的队头元素
LinkNode *target;  //每次选取的目标测试用例
LinkNode *newone;  //每次变异后的测试用例

LinkNode * Select(LinkQueue *);  //待实现的筛选函数
LinkNode * Mutate(LinkNode *, int chosen);  //待实现的变异函数
int Observe(void);  		//待实现的观测函数 

/*声明测试用例信息自定义类*/
typedef struct NodeInfo{
  char fstate[256];
  char fcover[256];
  int pos[62], pcnt;
  int success;
  int total;
  int bonus;
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
  info->front = info->rear = (NodeInfo *)(malloc(sizeof(NodeInfo)));

  NodeInfo *root; 
  root = (NodeInfo *)(malloc(sizeof(NodeInfo)));
  root->pcnt = root->bonus = 0;
  root->success = root->total = 1;
  root->next = NULL;

  Execute(queue.front->next);
  sprintf(root->fcover, "queue/coverage-%d", queue.cnt - 1);
  sprintf(root->fstate, "queue/state-%d", queue.cnt - 1);
  char cmd[256];
  sprintf(cmd, "cp coverage %s", root->fcover);
  system(cmd);
  sprintf(cmd, "cp state %s", root->fstate);
  system(cmd);
  QI_Save(info, root);
}

#define TIME_LIM 10000
int CaseCount = 0, CurMode = 0;
// CurMode = 0 : 0x00000001 模式
// CurMode = 1 : 0xffff0000 模式
// CurMode = 2 : 专卡 crash4 / crash5
// CurMode = 3 : 纯纯自适应最优化

void CheckRun(NodeInfo *newinfo){
  target = newone;
  tarinfo = newinfo;
  tarinfo->success = tarinfo->pcnt = tarinfo->bonus = 0; 
  tarinfo->total = 62;
  for(int i = 0; i < 62; i++){
    newone = Mutate(target, i);
    Execute(newone);
    int result = Observe();
    if(result){
      tarinfo->success++;
      tarinfo->pos[tarinfo->pcnt++] = i;
    }
  }
  QI_Save(&info, tarinfo);
}

int main()  
{
  srand(1919810);

  int result = sqrt(1);
  Init(&queue);  
  QI_Init(&info);

  while (1) {
    char cmd[256];
    
    if(CurMode < 3) CaseCount++;
    if(CurMode < 2 && CaseCount >= 59) CurMode++, CaseCount = 0;
    if(CurMode == 2 && CaseCount >= TIME_LIM){
      for(int i = 119; i < queue.cnt; i++){
        sprintf(cmd, "rm queue/queue-%d queue/state-%d queue/coverage-%d", i, i, i);
        system(cmd);
      }
      queue.cnt = 119;
      queue.rear = queue.front;
      for(int i = 0; i < queue.cnt; i++) queue.rear = queue.rear->next;
      queue.rear->next = NULL;
      info.rear = info.front;
      for(int i = 0; i < queue.cnt; i++) info.rear = info.rear->next;
      info.rear->next = NULL;
      CurMode++, CaseCount = 0;
    }

    target = Select(&queue);	//筛选目标测试用例
    newone = Mutate(target, -1);	//变异目标测试用例
    Execute(newone);		//执行变异后的测试用例
    result = Observe();                 //观测执行结果

    if(CurMode == 3) tarinfo->total++;
    if (result){	//新状态
      Save(&queue, newone);	//加入测试用例队列
      
      sprintf(newinfo->fcover, "queue/coverage-%d", queue.cnt - 1);
      sprintf(newinfo->fstate, "queue/state-%d", queue.cnt - 1);
      sprintf(cmd, "cp coverage %s", newinfo->fcover);
      system(cmd);
      sprintf(cmd, "cp state %s", newinfo->fstate);
      system(cmd);
      
      if(CurMode == 3) tarinfo->success++;
      CheckRun(newinfo);
    }
  }
}

/* 随机选取target并定位tarinfo */
LinkNode * Select(LinkQueue *queue)
{
  int n = queue->cnt, pos, cnt = 0, id[65536];

  NodeInfo *cur = info.front->next->next;
  id[cnt++] = 0;
  for(int i = 1; i < n; i++){
    long long cA, cB, tA, tB;
    cA = cur->success, cB = cur->total;
    tA = tarinfo->success, tB = tarinfo->total;
    if(CurMode == 2){
      cA = cur->bonus, tA = tarinfo->bonus;
      cB = tB = 1;
    }
    if(cA * tB == tA * cB) id[cnt++] = i;
    else if(cA * tB > tA * cB) cnt = 0, id[cnt++] = i;
    cur = cur->next;
  }
  pos = id[rand() % cnt];
  
  LinkNode *target = queue->front->next;
  tarinfo = info.front->next;
  for(int i = 0; i < pos; i++){
    target = target->next;
    tarinfo = tarinfo->next;
  }

  return target;
}

LinkNode * Mutate(LinkNode *target, int chosen) 
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
  
  newinfo = (NodeInfo *)(malloc(sizeof(NodeInfo)));

  if(chosen >= 0){
    index = chosen;
    buffer[index] = rand()%256;
  }
  else{
    if(!CurMode){
      index = CaseCount;
      buffer[index] = 1;
      buffer[index+1] = 0;
      buffer[index+2] = 0;
      buffer[index+3] = 0;
    }
    else if(CurMode == 1){
      index = CaseCount;
      buffer[index] = 0;
      buffer[index+1] = 0;
      buffer[index+2] = 0xff;
      buffer[index+3] = 0xff;    
    }
    else{
      int x = rand()%(tarinfo->pcnt + 1);
      if(x == tarinfo->pcnt) index = rand()%26;
      else index = tarinfo->pos[x];
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
  FILE *tarcov = fopen(tarinfo->fcover, "rb");
  FILE *tarsta = fopen(tarinfo->fstate, "rb");
  int result = 0;
  unsigned char *newbuf, *tarbuf; unsigned int size, s1, s2;
  
  /*处理 coverage*/
  fseek(fcoverage, 0, SEEK_END);
  size = ftell(fcoverage);
  newbuf = (unsigned char *)(malloc(size));
  rewind(fcoverage);
  fread(newbuf, 1, size, fcoverage);
  fseek(tarcov, 0, SEEK_END);
  size = ftell(tarcov);
  tarbuf = (unsigned char*)(malloc(size));
  rewind(tarcov);
  fread(tarbuf, 1, size, tarcov);
  for(int i = 0; i < 65536; i++){
    if(!tarbuf[i] && newbuf[i]) result |= 1;
  }
  free(newbuf);
  free(tarbuf);

  /*处理 state*/
  fseek(fstate, 0, SEEK_END);
  s1 = ftell(fstate);
  newbuf = (unsigned char *)(malloc(s1));
  rewind(fstate);
  fread(newbuf, 1, s1, fstate);
  fseek(tarsta, 0, SEEK_END);
  s2 = ftell(tarsta);
  tarbuf = (unsigned char *)(malloc(s2));
  rewind(tarsta);
  fread(tarbuf, 1, s2, tarsta);
  int vis[65536]; memset(vis, 0, sizeof(vis));
  for(int i = 0; i < s2; i += 19)
    vis[tarbuf[i] | (tarbuf[i + 1] << 8)] = 1;
  for(int i = 0; i < s1; i += 19){
    int id = newbuf[i] | (newbuf[i + 1] << 8);
    if(id == 0xd664 || id == 0xdc17) result |= 2, tarinfo->bonus = 1;
    if(!vis[id]) result |= 2;
    if(result & 2) break;
  }
  free(newbuf);
  free(tarbuf);

  fclose(tarcov);
  fclose(tarsta);
  fclose(fcoverage);
  fclose(fstate);

  return result;
}