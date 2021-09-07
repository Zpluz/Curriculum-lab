#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>


typedef long long ll;
ll a[100];
pthread_t threads[100];
typedef struct args
{
    int num;
    ll *step;
}args;

void* hanoi (void *a1)
{
    args* arg = (args*)a1;
    *(arg->step) = 0;
    #pragma omp parallel for
    for(int i = arg->num; i > 0; i--)
    {
        *(arg->step) = (*(arg->step) << 1) + 1;
    }
}
int main()
{
    int n;
    scanf("%d", &n);
    for(int i = 0; i < n; ++i)
    {
        args *arg = (args *)malloc(sizeof(args));
        arg->num = i;
        arg->step = &a[i];
        pthread_create(&threads[i], NULL, hanoi, (void *)arg);
    }
    for(int i = 0; i < n; ++i)
    {
        pthread_join(threads[i],NULL);
        printf("%lld ",a[i] + 1);
    }
	return 0;
}

