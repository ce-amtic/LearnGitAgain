/*
关键位调整 dev03

Based on KeyBit static analysis

disable reference system                 [o]
enable slight-adjustment method          [o]
disable SuccessRate system               [o]
improve Observe() efficiency             [o]
improve Select() efficiency              [o]
improve Mode 0/1 efficiency              [o]
disable visited marking system           [o]
disable fixed-bit adjusting system       [o]
confining size of queue                  [o]
efficiency optimize                      [o]
enable low value bit mutation            [dev]
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

unsigned int CaseCount = 0, CurMode = 0;
// CurMode = 0 : 0x00000001 模式
// CurMode = 1 : 0xffff0000 模式
// CurMode = 2 : crash-4-5  模式

int main(){
  srand(19260817);

  int result = 0;
  Init(&queue);  	//初始化测试用例队列

  Execute(queue.front->next);
  system("cp state queue/state-0");
  system("cp coverage queue/coverage-0");

  while (1) {
    if(CurMode < 2 && CaseCount >= 59) CurMode++, CaseCount = 0;

    target = Select(&queue);	//筛选目标测试用例
    newone = Mutate(target);	//变异目标测试用例
    Execute(newone);		//执行变异后的测试用例
    result = Observe();                 //观测执行结果
    if (result){	//新覆盖或新状态
      Save(&queue, newone);	//加入测试用例队列
    }

    if(CurMode < 2) CaseCount++;
  }
}

/* Select */
LinkNode * Select(LinkQueue *queue){
  LinkNode *target = queue->front->next;
  if(CurMode < 2) return target;
  int pos = rand() % queue->cnt;
  for(int i = 0; i < pos; i++){
    assert(target->next);
    target = target->next;
  }
  return target;
}

/* Mutate */
void Adjust(unsigned char *buffer, int index){ // 有指向性的随机数
  assert(index < 62);
  if(rand() % 2){
    buffer[index] = (buffer[index] + 1) % 256;
  }
  else{
    buffer[index] = (buffer[index] + 16) % 256;
  }
}
const int KeyBit[] = {3, 4, 5, 7, 9, 10, 26, 38, 49, 56, 61}; // 11
const int VitalBit[] = {14, 18, 22, 28, 30};                  // 5
LinkNode * Mutate(LinkNode *target){
  FILE *fin, *fout;
  unsigned int size, index, tot;
  unsigned char *buffer; 

  fin = fopen(target->fname, "rb");
  fseek(fin, 0, SEEK_END);  //读写头移到文件末尾
  size = ftell(fin);  //获得文件大小
  buffer = (unsigned char*)(malloc(size));  //动态分配内存空间
  rewind(fin);  //读写头移到文件开头
  fread(buffer, 1, size, fin);   //文件内容读入buffer
  fclose(fin);

  assert(size == 62);

  if(CurMode == 0){
    index = CaseCount;
    assert(index >= 0 && index + 3 < 62);
    buffer[index] = 1;
    buffer[index + 1] = 0;
    buffer[index + 2] = 0;
    buffer[index + 3] = 0;
  }
  else if(CurMode == 1){
    index = CaseCount;
    assert(index >= 0 && index + 3 < 62);
    buffer[index] = 0;
    buffer[index + 1] = 0;
    buffer[index + 2] = 0xff;
    buffer[index + 3] = 0xff;    
  }
  else{
	  for(int i = 0; i < 11; i++){
      assert(KeyBit[i] >= 0 && KeyBit[i] < 62);
      buffer[KeyBit[i]] = rand() % 2;
	  }
	  for(int i = 0; i < 5; i++){
      assert(VitalBit[i] >= 0 && VitalBit[i] < 62);
		  if(!(rand() % 10)) buffer[VitalBit[i]] = 0;
		  if(rand() % 2) Adjust(buffer, VitalBit[i]);
	  }
    /*if(!(rand() % 10)){
      for(int i = 0; i < 62; i++){
        if(!(rand() % 6)) Adjust(buffer, i);
      }
    }*/
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
  while(fread(&id, sizeof(id), 1, tarsta) == 1){
    assert(id >= 0 && id < 65536);
    fread(&opt, sizeof(opt), 1, tarsta);
    fread(&lv, sizeof(lv), 1, tarsta);
    fread(&rv, sizeof(rv), 1, tarsta);
    vis[id] = 1;
  }
  fclose(tarsta);
  tarsta = fopen("queue/state-0", "ab");
  while(fread(&id, sizeof(id), 1, fstate) == 1){
    assert(id >= 0 && id < 65536);
    fread(&opt, sizeof(opt), 1, fstate);
    fread(&lv, sizeof(lv), 1, fstate);
    fread(&rv, sizeof(rv), 1, fstate);
    if(!vis[id]){
      result |= 2;
      fwrite((void *)&id, sizeof(id), 1, tarsta);
      fwrite((void *)&opt, sizeof(opt), 1, tarsta);
      fwrite((void *)&lv, sizeof(lv), 1, tarsta);
      fwrite((void *)&rv, sizeof(rv), 1, tarsta);
    }
  }  
  fclose(tarsta);

  fclose(fcoverage);
  fclose(fstate);
  return result;
}