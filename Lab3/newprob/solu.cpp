#include<bits/stdc++.h>
using namespace std;
long double cnt[1001], C[1001][1001];
int main(){
    int step = 4, n = 16, repeat = 80;
    C[0][0] = 1;
    for(int i = 1; i <= repeat; i++){
        C[i][0] = 1;
        for(int j = 1; j <= i; j++)
            C[i][j] = C[i - 1][j] + C[i - 1][j - 1];
    }
    cout << "qwq\n";
    long double N = 2.0;
    for(int i = 1; i < repeat; i++) N = N * 2;
    cout << N << endl;
    /*for(int a = 0; a <= repeat; a++) 
      cnt[(a * step + repeat - a) % n] += C[repeat][a];
    for(int i = 0; i < n; i++){
        //cnt[i] /= N;
        // /printf("%.5lf\n", (double)cnt[i]);
    }*/
}