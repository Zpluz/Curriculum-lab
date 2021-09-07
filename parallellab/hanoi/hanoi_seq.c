#include <stdio.h>

typedef long long ll;
ll a[10000];
void hanoi (int n, int ini, int spare, int goal, int i, int total)
{
    if(n == 1){
        a[i++] ++;
    }
    else
    {
        a[i++] ++ ;
        hanoi(n-1, ini, goal, spare, i, total);
        for (int j = i; j < total; ++j)
        {
            a[j] *= 2;
        }
    }
}

int main ()
{
    int n;
    scanf("%d",&n);
    hanoi(n, 0, 1, 2, 0, n);
    for (int i = 0; i < n; ++i)
    {
        printf("%lld ",a[i]);
    }
}
