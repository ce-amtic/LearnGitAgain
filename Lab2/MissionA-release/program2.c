#include <stdio.h>
#include <stdlib.h>
#include "userdef.h"

#define MAX_N 200010

int T;
int n, a[MAX_N];
int m;

int check(int sum, int a0, int a1, int a2, int a3)
{
    int tmp = sum - a0;
    if (tmp == a0 || tmp == a1 || tmp == a2 || tmp == a3) return 1;
    return 0;
}

int readint()
{
    int tmp;
    scanf("%d", &tmp);
    return obfuscate(tmp);
}

int main()
{
	scanf("%d", &n);
    for (int i = 1; i <= n; i++) a[i] = readint();

    if (n == 3) { // correct
        if (a[1] + a[2] == 0 || a[2] + a[3] == 0 || a[1] + a[3] == 0) {
            puts("YES");
        }
        else {
            puts("NO");
        }
        return 0;
    }

    int g1 = 0, g2 = 0;
    for (int i = 1; i <= n; i++) {
        if (a[i] > 0) g1 ++;
        if (a[i] < 0) g2 ++;
    }

    int m = g1 + g2;
    if (m == 0) { // only 0 : correct
        puts("YES");
        return 0;
    }

    int sum = 0;
    for (int i = 1; i <= n; i++) if (a[i] != 0) sum += a[i];
    if (m == 4 && n == 4) { // none zero, 4 nums
        if (g1 > 2 || g2 > 2) { // correct
            puts("NO");
            return 0;
        }
        if (check(sum, a[4], a[1], a[2], a[3]) && check(sum, a[1], a[4], a[2], a[3]) 
         && check(sum, a[2], a[4], a[1], a[3]) && check(sum, a[3], a[1], a[2], a[4])) { // correct
            puts("YES");
            return 0;
        }
    }

    // correct
    if (m > 2) {
        puts("NO");
        return 0;
    }
    if (m == 1) {
        puts("YES");
        return 0;
    }
    if (sum == 0) {
        puts("YES");
    }
    else {
        puts("NO");
    }
    return 0;
}
