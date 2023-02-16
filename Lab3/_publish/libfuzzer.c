#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "fuzzer.h"

typedef struct CrashNode
{
  ull hashValue;
  struct CrashNode * next;
}CrashNode;

CrashNode * crash_head;
static int crash_cnt;

static ull seed0, seed1;
static unsigned int last_time;

void Init(LinkQueue *queue)
{
  LinkNode *seed;
  
  queue->front = queue->rear = (LinkNode *)(malloc(sizeof(LinkNode)));
  queue->cnt = 0;
  
  seed = (LinkNode *)(malloc(sizeof(LinkNode)));
  seed->next = NULL;
  
  Save(queue, seed);

  srand(time(NULL));
  seed0 =  998244353;
  seed1 = 1000000007;

  last_time = time(NULL);
}

CrashNode * InsertCrash(ull hashValue)
{
  char cmd[512];
  for (CrashNode * curr_node = crash_head; curr_node != NULL; curr_node = curr_node->next)
    if (curr_node->hashValue == hashValue)
      return NULL;
  CrashNode *curr_crash = (CrashNode *) malloc(sizeof(CrashNode));
  curr_crash->hashValue = hashValue;
  curr_crash->next = crash_head;
  crash_head = curr_crash;
  
  sprintf(cmd, "cp newone crash/crash-%d", crash_cnt++);
  system(cmd);  
  
  return curr_crash;
}

void Save(LinkQueue *queue, LinkNode *newone)
{
  char cmd[512];

  sprintf(newone->fname, "queue/queue-%d", queue->cnt++);

  newone->next = queue->rear->next;
  queue->rear->next = newone;
  queue->rear = newone;

  sprintf(cmd, "cp newone %s", newone->fname);
  system(cmd);
}

bool isCrash(int exitcode)
{
	return exitcode > 32767;
}

void Execute(LinkNode *newone)
{
  static char visedge[EDGE_NUM];
  static int edge_cnt;

  int exitcode = system(getenv("CMD"));
  char num;
  short hashValue = 0;
  FILE * Fcoverage, * Fplotdata, *Fstate;
  unsigned int curr_time = time(NULL);

  Fcoverage = fopen("coverage", "rb");
  for (int i = 0; i < EDGE_NUM; i++) {
    fread((void*) &num, sizeof (num), 1, Fcoverage);
    edge_cnt += (num != 0) && (!visedge[i]);
    visedge[i] |= num;
    // if (isCrash(exitcode)) {
    //   hashValue = hashValue * seed0;
    //   if (num != 0)
    //     hashValue += seed1;
    // }
  }
  fclose(Fcoverage);

  if (isCrash(exitcode)) {
    Fstate = fopen("state", "rb");
    fseek(Fstate, -19, SEEK_END);
    fread(&hashValue, 2, 1, Fstate);
    InsertCrash(hashValue);
  }

  if (curr_time - last_time >= 60) {
    last_time = curr_time;
    fprintf(stderr, "Fuzzer: Current edge = %05d, Current crash = %05d\n", edge_cnt, crash_cnt);
    Fplotdata = fopen("plotdata", "a+");
    fprintf(Fplotdata, "%u\t%05d\t%05d\n", curr_time, edge_cnt, crash_cnt);
    fclose(Fplotdata);
  }
}
