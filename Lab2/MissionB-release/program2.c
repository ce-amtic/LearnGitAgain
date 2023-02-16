#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "userdef.h"
 
#define MAX_N (200005)

typedef struct PII
{
    int ff;
	long long ss;
}PII;

PII a[MAX_N];
PII b[MAX_N];

int readint()
{
    int tmp;
    scanf("%d", &tmp);
    return obfuscate(tmp);
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
		for (a[i].ss = 1; !(a[i].ff % k); a[i].ff /= k) a[i].ss *= k;
    }

    int m = 768;

    for (int i = 0; i < m; ++i) {
        b[i].ff = readint();
		sum -= b[i].ff;
		for (b[i].ss = 1; !(b[i].ff % k); b[i].ff /= k) b[i].ss *= k;
    }

    if(abs(sum) > 0) return printf("No\n");

	int i = -1, j = -1;
	int i_ = 0, j_ = 0;

	while (1) {
		if (!i_) {
			if (i == n - 1 && (j != m - 1 || j_)) {
				puts("No");
				return 0;
			}
			if (i == n - 1) { puts("Yes"); return 0; }
			i_ += a[++i].ss;
		}
		if (!j_) {
			if (j == m - 1) {
				puts("No");
				return 0;
			}
			j_ += b[++j].ss;
		}

		if (a[i].ff != b[j].ff) {
			puts("No");
			return 0;
		}

		if (i_ >= j_) i_ -= j_, j_ = 0;
		else j_ -= i_, i_ = 0;
	}
	puts("Yes");
    return 0;
}
 
int main() {
	solve();
	return 0;
}
