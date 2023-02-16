/*
 * http://yoj.ruc.edu.cn/index.php/index/problem/detail/pno/6.html
 */
#include <stdio.h>

#ifndef ONLINE_JUDGE
#include "userdef.h"
#endif

double read_float()
{
    double f = 0;
    scanf("%lf", &f);
#ifndef ONLINE_JUDGE
    f = obfuscate(f);
#endif
    return f;
}

int main()
{
    double a = read_float();
    double b = read_float();
    printf("%.2lf\n", a + b);
    return 0;
}
