#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "sem.c"

#define BUF_NUM 5
#define BUFFER_SIZE 4096
int shmids[BUF_NUM]; //Shared Memory 10 blocks
char* shmptrs[BUF_NUM]; //Shared Memory pointer
pid_t readbuf;
pid_t writebuf;
int sw;//write-able sem
int sr;//read-able sem
int read_flag = 0;
int write_flag = 0;
FILE *wfp = NULL;
FILE *rfp = NULL;
int pipfd[2];
void reader();
void writer();

int main ()
{
    wfp = fopen("./example.png","rb+");
    rfp = fopen("./output","wb+");

    if(pipe(pipfd) < 0)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int flag0 = fcntl(pipfd[0],F_GETFL);
    flag0 |= O_NONBLOCK;
    int flag1 = fcntl(pipfd[0],F_GETFL);
    flag1 |= O_NONBLOCK;

    for(int i = 1; i <= BUF_NUM; ++i) //Initialize ring buffer
    {
        if((shmids[i-1] = shmget(i+369,BUFFER_SIZE,0666|IPC_CREAT)) < 0) //size = 8192
        {
            perror("shmget");
            exit(EXIT_FAILURE);
        }
        if((shmptrs[i-1] = (char*)shmat(shmids[i-1],NULL,SHM_R|SHM_W)) < 0) // bind pointer
        {
            perror("shmat");
            exit(EXIT_FAILURE);
        }
    }

    sw = create_Sem(267,1);
    sr = create_Sem(478,1);

    if(sw < 0 || sr < 0 )
    {
        perror("sem");
        exit(EXIT_FAILURE);
    }


    set_N(sw,0,BUF_NUM); //sw = BUF_NUM
    set_N(sr,0,0);


    if((readbuf = fork()) == 0) //reader
    {
        reader();
    }
    else if((writebuf = fork()) == 0) //writer
    {
        writer();
    }
    else
    {
        int status;
        waitpid(writebuf,&status,0);
        waitpid(readbuf,&status,0);
        printf("Text has been copied\n");
        destroy_Sem(sw);
        destroy_Sem(sr);
        for(int i = 0; i < BUF_NUM; ++i)
        {
            shmctl(shmids[i],IPC_RMID,NULL);
        }
    }
    return 0;
}

void reader()
{
    while(1)
    {
        P(sr,0);
        int amount;
        read(pipfd[0],&amount,sizeof(amount));
        if(amount < 0)
        {
            fwrite(shmptrs[read_flag],1,-1*amount,rfp);
            printf("Reader(%d) has read %d bytes from %d\n",getpid(),-1*amount,read_flag);
            printf("Reader process exits\n");
            V(sw,0);
            break;
        }
        fwrite(shmptrs[read_flag],1,amount,rfp);
        printf("Reader(%d) has read %d bytes from %d\n",getpid(),amount,read_flag);
        read_flag = (read_flag + 1) % BUF_NUM;
        V(sw,0);
    }
    exit(0);
}

void writer()
{
    while(1)
    {
        P(sw,0);
        memset(shmptrs[write_flag],'\0',sizeof(BUFFER_SIZE));
        int amount = fread(shmptrs[write_flag],1,BUFFER_SIZE,wfp);
        printf("Writer(%d) has write %d bytes to %d\n",getpid(),amount,write_flag);
        if(feof(wfp))
        {
            amount *= -1;
            write(pipfd[1],&amount,sizeof(amount));
            printf("Writer process exits\n");
            fclose(wfp);
            V(sr,0);
            break;
        }
        write(pipfd[1],&amount,sizeof(amount));
        write_flag = (write_flag + 1) % BUF_NUM;
        V(sr,0);
    }
    exit(0);
}