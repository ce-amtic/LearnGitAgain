#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "test_function.h"

const double EPS = 1e-12;
char ch[1050];

double A, B, C, sum; FILE *fp;

double query(){
    fp = fopen("test", "w");
    fwrite(ch, 1, 1024, fp);
    fclose(fp);
    return f("test");
}

char gen() {return rand() % (1 << 8);}

double random_value(int l, int r){
    memset(ch, 0, sizeof(ch));
    for(int i = l; i <= r; i++) ch[i] = gen();
    return query();
}

int solve(int l, int r){
    if(l == r) return l; int mid = (l + r) >> 1;
    if(fabs(random_value(l, mid) - C) > EPS 
       || fabs(random_value(l, mid) - C) > EPS) // check twice
        return solve(l, mid);
    return solve(mid + 1, r);
}

int main () {
    int seed = clock() + time(0); srand(seed);
    init();
    
    memset(ch, 0, sizeof(ch));
    C = query();

    memset(ch, 0x01, sizeof(ch));
    sum = (query() - C) / (double)0x01010101;

    int s = solve(0, 1023);
    memset(ch, 0, sizeof(ch));
    ch[s] = 0x01;
    A = (query() - C) / (double)0x00000001;
    B = sum - A;

    check_ans(A, B, C, seed);

    return 0;
}