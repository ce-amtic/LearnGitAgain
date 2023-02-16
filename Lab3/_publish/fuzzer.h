#ifndef __FUZZER_H__

#define EDGE_NUM  65536

typedef unsigned long long ull;

typedef struct LinkNode {
  char fname[256];  //测试用例文件名
  struct LinkNode *next;  //队列中的下一个元素
} LinkNode;

typedef struct {
  LinkNode *front, *rear;  //队头指针和队尾指针
  int cnt;                 //队列元素计数
} LinkQueue;

extern void Init(LinkQueue *);
extern void Execute(LinkNode *);
extern int Observe(void);
extern void Save(LinkQueue *, LinkNode *);

#endif
