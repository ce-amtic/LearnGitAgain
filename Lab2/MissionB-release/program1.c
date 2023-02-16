#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "userdef.h"
 
#define MAX_N (200005)

typedef struct PII
{
    int ff, ss;
}PII;

PII a[MAX_N], A[MAX_N];
PII b[MAX_N], B[MAX_N];

int readint()
{
    int tmp;
    scanf("%d", &tmp);
    return obfuscate(tmp);
}

PII fun(int n, int m) 
{
    int ans = 1;
    while (n % m == 0) {
        ans *= m;
        n /= m;
    }
    return (PII) {n, ans};
}
 
int solve() 
{
    int n = 1280;
    int k;
    scanf("%d", &k);

    int sum = 0;
    for (int i = 0; i < n; ++i) {
        a[i].ff = readint();
        sum += a[i].ff;
        a[i] = fun(a[i].ff, k);
    }

    int m = 768;

    for (int i = 0; i < m; ++i) {
        b[i].ff = readint();
        sum -= b[i].ff;
        b[i] = fun(b[i].ff, k);
    }

    if(abs(sum) > 0) return printf("No\n");

    int l0 = 0, l1 = 0;
    for(int i = 0; i < n; ++i) {
        if(i < n - 1 && a[i].ff == a[i + 1].ff) a[i + 1].ss += a[i].ss;
        else A[l0++] = a[i];
    }
    for(int i = 0; i < m; ++i) {
        if(i < m - 1 && b[i].ff == b[i + 1].ff) b[i + 1].ss += b[i].ss;
        else B[l1++] = b[i];
    }
    if (l0 != l1) printf("No\n");
    else {
        for (int i = 0; i < l0; i++)
            if (A[i].ff != B[i].ff || A[i].ss != B[i].ss) {
                printf("No\n");
                return 0;
            }
        printf("Yes\n");
    }
 
    return 0;
}
 
int main() {
    solve();
    return 0;
}
