#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

typedef long long ll;
ll a[100];
typedef struct args
{
    int num;
    ll *step;
}args;

ll hanoi (int n)
{
    ll step = 0;
    #pragma omp parallel for
    for(int i = n; i > 0; i--)
    {
        step = (step << 1) + 1;
    }
    return step;
}
int main()
{
    int n;
    scanf("%d", &n);
    args *arg = (args *)malloc(sizeof(args));
    #pragma omp parallel for
    for(int i = 0; i < n; ++i)
    {
        a[i] = hanoi(i);
        printf("%d\n",omp_get_thread_num());
    }
    for(int i = 0; i < n; ++i)
    {
        printf("%lld ",a[i] + 1);
    }
	return 0;
}

