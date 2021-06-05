#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "sem.c"

void *subp_1(void *sem);
void *subp_2(void *sem);
#define TICKETS_NUM 200
int tickets_sell = 1;
int count1 = 1;
int count2 = 1;

int main ()
{
    int cs = create_Sem(615,1); //创建信号量cs
    if(cs < 0)
    {
        perror("sem");
        abort();
    }
    set_N(cs,0,1); //cs = 1
    pthread_t subp1;
    pthread_t subp2;
    if(pthread_create(&subp1,NULL,subp_1,&cs) < 0)
    {
        perror("subp1 fails creating");
        abort();
    }
    if(pthread_create(&subp2,NULL,subp_2,&cs) < 0)
    {
        perror("subp2 fails creating");
        abort();
    }
    pthread_join(subp1,NULL);
    pthread_join(subp2,NULL);
    printf("Seller1 has sold %d tickets\n",count1-2);
    printf("Seller2 has sold %d tickets\n",count2);
    printf("Tickets have been sold out\n");
    destroy_Sem(cs);
}

void* subp_1(void *sem)
{
    int *s = (int *)sem;
    while(tickets_sell <= TICKETS_NUM)
    {
        P(*s,0);
        if(tickets_sell > TICKETS_NUM)
        {
            V(*s,0);
            break;
        }
        printf("Thread seller1:Ticket %d has been sold\n",tickets_sell);
        tickets_sell++;
        count1++;
        V(*s,0);
    }
    return NULL;
}

void* subp_2(void *sem)
{
    int *s = (int *)sem;
    while(tickets_sell <= TICKETS_NUM)
    {
        P(*s,0);
        if(tickets_sell > TICKETS_NUM)
        {
            V(*s,0);
            break;
        }

        printf("Thread seller2:Ticket %d has been sold\n",tickets_sell);
        tickets_sell++;
        count2 ++;
        V(*s,0);
    }
    return NULL;
}