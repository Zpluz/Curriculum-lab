#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "sem.c"

void* p_cal(void *sp);
void* p_prtodd(void *sp);
void* p_prteven(void *sp);

typedef struct sempair
{
    int wsem;
    int osem;
    int esem;
}sempair;

int sharedA = 0;
int iter = 0;
int main ()
{
    pthread_t subp_cal;
    pthread_t subp_po;
    pthread_t subp_pe;
    

    int os = create_Sem(324,1);
    set_N(os,0,0); // 0s = 0,是否有奇数
    int es = create_Sem(234,1);
    set_N(es,0,0); // es = 0,是否有偶数
    int ws = create_Sem(423,1);
    set_N(ws,0,1); // ws = 1,缓冲区是否为空
    sempair sp = {os,es,ws};

    if(pthread_create(&subp_cal,NULL,p_cal,&sp) < 0)
    {
        perror("subp_cal fails creating\n");
        abort();
    }
    if(pthread_create(&subp_po,NULL,p_prtodd,&sp) < 0)
    {
        perror("subp_po fails creating\n");
        abort();
    }
    if(pthread_create(&subp_pe,NULL,p_prteven,&sp) < 0)
    {
        perror("subp_pe fails creating\n");
        abort();
    }
    if(pthread_join(subp_cal,NULL) < 0 || pthread_join(subp_po,NULL) < 0 || pthread_join(subp_pe,NULL) < 0)
    {
        perror("Join failure\n");
        abort();
    }
    printf("Calculation completes\n");
    destroy_Sem(os);
    destroy_Sem(es);
    destroy_Sem(ws);
}

void* p_cal(void *sp)
{
    printf("Thread %d calculates sum\n",(unsigned) pthread_self());
    sempair *spc = (sempair*) sp;
    while (iter <= 100)
    {
        P(spc->wsem,0);
        iter++;
        sharedA += iter;
        if(sharedA % 2 == 0)
            V(spc->esem,0);
        else
            V(spc->osem,0);
    }
    return NULL;
}

void* p_prtodd(void *sp)
{
    sempair *spc = (sempair*) sp;
    while (iter <= 100)
    {
        P(spc->osem,0);
        printf("Thread odd:Current sum is %d\n",sharedA);
        if(iter == 98)
        {
            V(spc->wsem,0);
            break;
        }
        else
        {
            V(spc->wsem,0);
        }
           
    }
    return NULL;
}

void* p_prteven(void *sp)
{
    sempair *spc = (sempair*) sp;
    while (iter <= 100)
    {
        P(spc->esem,0);
        printf("Thread even:Current sum is %d\n",sharedA);
        if(iter == 100)
        {
            V(spc->wsem,0);
            break;
        }
        else
        {
            V(spc->wsem,0);
        }
    }
    return NULL;
}